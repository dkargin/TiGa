#include "world.h"

#include <simengine/fx/fxmanager.h>
#include "sim/device.h"
#include "sim/projectile.h"
#include "sim/unit.h"
#include "simengine/sim/debugDraw.h"
#include "game.h"
///////////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
// World class
///////////////////////////////////////////////////////////////
World::World(const char *name, Game * core)
	:dynamics(b2Vec2(0,0))
	,level(*this)
	,core(core)
{
	//LogFunction(*g_logger);
	if(name)
		this->name = name;

	dynamics.SetContactFilter(this);
	dynamics.SetContactListener(this);
	dynamics.SetContinuousPhysics(true);

	collisionsReset();	// enable all collisions

	level.cellSize = 10.0f;

	updatePeriod = 1;
	updateLeft = updatePeriod;

	server = false;
	useNet = true;
}

World::~World()
{
//	LogFunction(*g_logger);
	level.clear();

#ifdef FUCK_THIS
	if(visionLayer)
		delete visionLayer;

	if(visionPass)
		draw->hge->Target_Free(visionPass);
#endif

	if(draw)
		delete draw;

	while(!attachedEffects.empty())
	{
		auto it = attachedEffects.begin();
		delete it->second.get();
		attachedEffects.erase(it);
	}

	if(gameObjects)
	{
		gameObjects->removeListener(this);
		delete gameObjects;
		gameObjects = NULL;
	}
}

void World::initRenderer(HGE * hge)
{
	assert(!draw);
	draw.reset(new sim::Draw);
	draw->init(hge);
	dynamics.SetDebugDraw(draw);
	draw->SetFlags(b2Draw::e_shapeBit/* | b2DebugDraw::e_aabbBit*/);
	int screenWidth = core->getScreenWidth();
	int screenHeight = core->getScreenHeight();
	visionPass = hge->Target_Create(screenWidth,screenHeight,false);
	visionLayer=new hgeSprite(hge->Target_GetTexture(visionPass),0,0,screenWidth,screenHeight);
	visionLayer->SetBlendMode(BLEND_MUL);//BLEND_MUL | BLEND_NOZWRITE | BLEND_ALPHAADD /*| BLEND_NOZWRITE*/);
}

void World::saveLevel(const char * file)
{
	sim::IOBuffer buffer;
	sim::StreamOut stream(&buffer);
	saveState(stream);
	FILE * fp = fopen(file, "wb");
	if(file)
	{
		size_t bufferSize = buffer.size();
		fwrite(&bufferSize,sizeof(bufferSize), 1, fp);
		fwrite(buffer.data(), bufferSize, 1, fp);
		fclose(fp);
	}
}

void World::loadLevel(const char * file)
{
	FILE * fp = fopen(file, "rb");

	if(fp)
	{
		size_t bufferSize = 0;
		fread((char*)&bufferSize,sizeof(bufferSize),1,fp);
		if(bufferSize)
		{
			sim::IOBuffer buffer;
			buffer.resize(bufferSize);
			fread((char*)buffer.data(), bufferSize,1,fp);
			loadState(sim::StreamIn(&buffer));
		}
		fclose(fp);
	}
}

Scripter * World::getScripter()
{
	return core->getScripter();
}

void World::saveState(StreamOut & stream)
{
	level.saveState(stream);
	gameObjects->saveState(stream);
	getScripter()->call("WorldSaveState",&stream);
}

bool World::loadState(StreamIn & stream)
{
	level.loadState(stream);
	gameObjects->loadState(stream);
	getScripter()->call("WorldLoadState",&stream);
	return true;
}

void World::setCursor(Fx::EntityPtr effect)
{
	core->cursor[0].effect = effect;
}

void World::collisionsReset(bool val)
{
	int size = sizeof(collisionMap);
	for(int i = 0; i < cgMax; i++)
		for(int j = 0; j < cgMax; j++)
			collisionMap[i][j] = val;
}

void World::collisionsSet(CollisionGroup a,CollisionGroup b,bool val)
{
	collisionMap[a][b] = val;
	collisionMap[b][a] = val;
}

bool World::collisionsGet(CollisionGroup a,CollisionGroup b)
{
	return collisionMap[a][b];
}

int World::initSimulation(bool server)
{
//	LogFunction(*g_logger);
	assert(!gameObjects);
	int result = 1;
	this->server = server;
	// create unit manager
	try
	{
		gameObjects = new ObjectManager(getScripter(), core->fxManager);
		gameObjects->initSimulation( &dynamics);
		gameObjects->initManagers();
		gameObjects->addListener(this);
		gameObjects->role = server ? ObjectManager::Master : ObjectManager::Slave;
	}
	catch(...)
	{
		result = 0;
	}
	return result;
}
const ObjectManager * World::getObjectManager() const
{
	return gameObjects;
}

void World::restore()
{
#ifdef FIX_THIS
	if(visionLayer && visionPass)
		visionLayer->SetTexture(draw->hge->Target_GetTexture(visionPass));
#endif
}

#ifdef WORLD_USE_NETWORK
//
//void World::readCmd(IO::StreamIn &buffer,int client)
//{
//	LogFunction(*g_logger);
//	PNetCmd msg=NULL;
//	buffer>>msg;
//	if(msg)
//	{
//		std::string fnName=cmdMap[msg->type()];
//		_Scripter::Object world = scripter.getGlobals("Net");
//		//world.call("exec",client,_Scripter::UserBind("NetCmdBase",msg));
//		world.call("exec",client,msg);
//		delete msg;
//	}
//}
//
//void World::sendCmd(int client,NetCmd::Base *cmd)
//{
//	LogFunction(*g_logger);
//	commands.push_front(cmd);
//}
//
//void World::writeCmd(IO::StreamOut &buffer,int client)
//{
//	if(!server && !helloSent)
//	{
//		sendCmd(client,new NetCmd::Hello());
//		helloSent=true;
//	}
//	for(std::list<PNetCmd>::iterator it=commands.begin();it!=commands.end();++it)
//	{
//		NetCmd::Base *cmd=*it;
//		buffer.write((int)cmd->type());
//		cmd->write(buffer);
//	}
//	for(std::list<PNetCmd>::iterator it=commands.begin();it!=commands.end();++it)
//		delete *it;
//	commands.clear();
//}
//
//struct WorldMsg
//{
//	int type;
//	int length;
//};
//
//void World::readMessages(IO::StreamIn &buffer,int client)
//{
//	while(!buffer.eof())
//	{
//		WorldMsg msg;
//		buffer.read(msg);
//		//switch(msg.type)
//		{
//		//case sourceDynamics:
//			//dynamicsScene->readMessages(buffer.readBuffer(msg.length),client);
//		//	break;
//		//case sourceObjectManager:
//		//	gameObjects->readMessages(buffer.readBuffer(msg.length),client);
//		//	break;
//		//case sourceWorld:
//		//	readCmd(buffer.readBuffer(msg.length),client);
//		//	break;
//		};
//	}
//}
//
//void World::msgBegin(IO::StreamOut &buffer,int type)
//{
//	assert(false);
//	// TODO: reimplement
//	/*
//	msgPos = buffer.pos();
//	WorldMsg msg = {type,0};
//	buffer.write(msg);
//	*/
//}
//
//void World::msgEnd(IO::StreamOut &buffer)
//{
//	assert(false);
//	// TODO: reimplement
//	/*
//	int len = buffer.pos()-msgPos-sizeof(WorldMsg);
//	if(len)
//		buffer.getAfter<WorldMsg>(msgPos).length=len;
//	else
//		buffer.pos(msgPos);
//	*/
//}
//
//void World::writeMessages(IO::StreamOut &buffer,int client)
//{
//	LogFunction(*g_logger);
//	// write messages from dynamics
//	//msgBegin(buffer,sourceDynamics);
//	//	dynamicsScene->writeMessages(buffer,client);
//	//msgEnd(buffer);
//	// write messages from ObjectManager
//	//msgBegin(buffer,sourceObjectManager);
//	//	gameObjects->writeMessages(buffer,client);
//	//msgEnd(buffer);
//	// write messages from World
//	//msgBegin(buffer,sourceWorld);
//	//	writeCmd(buffer,client);
//	//msgEnd(buffer);
//}
#endif

const bool useVision = false;
void World::renderVision()
{
	const float unseenOpacity = 0.4;
	if(!useVision)
		return;
#ifdef FIX_VISION
	HGE* hge = draw->hge;
	hge->Gfx_BeginScene(visionPass);
	hge->Gfx_Clear(ARGB(255*unseenOpacity,255*unseenOpacity,255*unseenOpacity,255*unseenOpacity));
	hge->Gfx_SetTransform();

	gameObjects->fxManager->setView( draw->globalView );
	for(auto it = gameObjects->vision.begin();it != gameObjects->vision.end();++it)
		draw->draw(*it);

	hge->Gfx_EndScene();

#endif
}

void World::renderUI()
{
}

#ifdef USE_PATHPROJECT
void renderNode(pathProject::MapBuilderQuad::QuadNode *n)
{
	if(n==NULL)
		return;
	if(n->children[0] != NULL)
	{
		renderNode(n->children[0]);
		renderNode(n->children[1]);
		renderNode(n->children[2]);
		renderNode(n->children[3]);
	}
	else
	{
		RECT &rc=n->rc;
		//GLParam param[] = {	GLParam(GL_LINE_WIDTH,2),};

		//glColor3fv(colorQuad);
		glColor3f(0.3f, 0.8f, 0.1f);
		glBegin(GL_LINE_LOOP);
		glVertex2f(rc.left,rc.top);
		glVertex2f(rc.right,rc.top);
		glVertex2f(rc.right,rc.bottom);
		glVertex2f(rc.left,rc.bottom);
		glEnd();
	}
	/*vec3 start=n->getCenter();
	for(std::set<QuadNode*>::iterator it=n->links.begin();it!=n->links.end();it++)
	{
		vec3 end=0.5*((*it)->getCenter()+start);
		geom->setColor(0.3,0.1,0.9);
		geom->setLineWidth(2);
		geom->drawLine(start[0],start[1],end[0],end[1]);

		vec3 s=((*it)->getCenter());
		geom->setColor(0.9,0.1,0.3);
		geom->setLineWidth(1);
		geom->drawLine(s[0],s[1],end[0],end[1]);
	}*/
}

void drawQuadMap()
{

}
#endif
void World::renderObjects()
{
	gameObjects->fxManager->setView( draw->globalView );

	Fx::FxManagerPtr manager = gameObjects->fxManager;

	if(level.background != NULL)
		level.background->query(manager, Pose(0,0,0));
#ifdef USE_PATHPROJECT
	if(false)
	{
		glDisable(GL_DEPTH_TEST);
		pathProject::MapManager * mapmanager = pathCore.getMapManager();
		auto nodeDrawer = [&](pathProject::Node * node)->int
		{
			const vec3 offset(0,0,10.0f);

			pathProject::Node * parent = mapmanager->getMap(node)->getNodeParent(node);


			/*pathProject::MapManager * manager = mapmanager;

			auto adjacentDrawer = [to,offset,manager](pathProject::Node * node)->int
			{
				glBegin(GL_LINES);
				glVertex3fv(manager->getNodePos(node) + offset);
				glVertex3fv(to + offset);
				glEnd();
				return 1;
			};*/

			if(node->back)
			{
				vec3 to = mapmanager->getNodePos(node);
				glBegin(GL_LINES);
				glVertex3fv(mapmanager->getNodePos(node->back)+offset);
				glVertex3fv(to + offset);
				glEnd();
			}

			//manager->enumAdjacentNodes(process, node, adjacentDrawer);
			return 1;
		};
		glColor3f(0,1,0);
		if(level.pathing)
		{
			//GLParam param[] = {	GLParam(GL_LINE_WIDTH,1),};
			level.pathing->getTarget()->enumNodes(nodeDrawer);
		}
		glColor3f(1,0,0);
		if(level.quadder)
		{
			//GLParam param[] = {	GLParam(GL_LINE_WIDTH,1),};
			level.quadder->getTarget()->enumNodes(nodeDrawer);
			if(level.quadder->root)
			{
				glPushMatrix();
				glTranslatef(0,0,4);
				glScalef(level.cellSize, level.cellSize, 1.f);
				renderNode(level.quadder->root);
				glPopMatrix();
			}
		}
		glEnable(GL_DEPTH_TEST);
	}
#endif

	for(GameObjectPtr it: gameObjects->objects)
		draw->drawObject(it);

	for(auto it = attachedEffects.begin(); it != attachedEffects.end(); it++)
	{
		if(it->first != NULL)
			it->second->render(manager, it->first->getPose());
	}

	if(draw->drawDynamics)
		dynamics.DrawDebugData();

	getScripter()->call("onRenderObjects");

	gameObjects->fxManager->pyro.render(manager, Pose(),1);

	// execute render queue
	gameObjects->fxManager->renderQueue.render(manager, Pose(0,0,0));
	/*
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	*/
}

void World::render()
{
	if(!draw)
		return;

	HGE* hge = draw->hge;
	//int i;
	//bool visionRendered = false;
	// Render graphics to the texture
	//if(/*!visionRendered && */useVision)
	//{
	//	renderVision();
	//}

	//hge->Gfx_BeginScene();
	//hge->Gfx_Clear(0);
	//hge->Gfx_SetTransform();
	renderObjects();


	//world->renderVision();
	hge->Gfx_SetTransform();
	if(useVision)
	{
		visionLayer->SetTexture(hge->Target_GetTexture(visionPass));
		visionLayer->Render(0,0);
		//visionRendered = true;
	}
	//gui->Render();
	renderUI();
	//hge->Gfx_EndScene();
}

FxPointer World::attachEffect(GameObjectPtr object, FxPointer effect)
{
	FxPointer result = effect->clone();
	attachedEffects.insert(std::make_pair(object,result));
	return result;
}

void World::clearEffects(GameObjectPtr object)
{
	for( auto it = attachedEffects.lower_bound(object); it != attachedEffects.upper_bound(object); )
	{
		if(it->second != NULL)
			delete it->second.get();
		auto toDelete = it;
		it++;
		attachedEffects.erase(toDelete);
	}
}

void World::onDie(GameObject *object)
{
	Unit *unit = toUnit(object);
	if(unit)
		getScripter()->call("onUnitDie",object);
}

void World::onDelete(GameObject *object)
{
	Unit *unit = toUnit(object);
	if(unit)
		getScripter()->call("onUnitDelete",object);
}

void World::update(float dt)
{
//	LogFunction(*g_logger);
	//gui.Update(dt);
	const int updateSteps = 16;
	const int client = 0;

	if(gameObjects)gameObjects->onFrameStart(dt);

	//for(int i = 0; i < updateSteps; i++)
	dynamics.Step(dt, updateSteps, updateSteps);

	if(gameObjects)gameObjects->onFrameFinish(dt);

	if(level.background)
		level.background->update(dt);

	// update effects that are attached to some objects
	for(auto it = attachedEffects.begin(); it != attachedEffects.end();)
	{
		if(it->first == NULL)
		{
			auto toDelete = it;
			it++;
			delete toDelete->second;
			attachedEffects.erase(toDelete);
		}
		else
		{
			it->second->update(dt);
			it++;
		}
	}
	// update scripted part
	getScripter()->call("onUpdateWorld",dt);
}

Solid * World::createWall(const vec3f &from, const vec3f &to, float width)
{
	//vec3 dir=normalise(to-from);
	//float length=(to-from).length();
	//Shape::Box box;
	//box.dimensions=vec3(length,width,100)*0.5;
	//box.setCenter(vec3(length/2,0,0));
	//Mt4x4 org(Mt4x4::identity());
	//org.axisX(dir);
	//org.axisY(vec3(0,0,1)^dir);
	//org.origin(from);
	//Solid *res=Shape::create(dynamicsScene,box,org);
	//res->enable(solidFrozen);
	//res->setGroup(typeBlock);
	//return res;
	return NULL;
}
// calculate transform from world to screen
void World::screen2world(int screen[2],float world[2])
{
//	LogFunction(*g_logger);
	float cs=cosf(draw->globalView.pose.orientation);
	float sn=sinf(draw->globalView.pose.orientation);
	int screenWidth = core->getScreenWidth();
	int screenHeight = core->getScreenHeight();
	vec2f &v=*(vec2f*)world;
	v = draw->globalView.pose.coords((screen[0] - screenWidth/2) / draw->globalView.scale,(screen[1]-screenHeight/2) / draw->globalView.scale);
}

void World::initMap(const char * map,float size,bool rigid,const Pose &pose)
{
	level.init(map,size,rigid,pose);
}

void World::world2map(float worldCoords[2],int mapCoords[2])
{
//	LogFunction(*g_logger);
	vec2f pos=vec2f(worldCoords)/level.cellSize+vec2f(level.blocks.size_x(),level.blocks.size_y())*0.5;
	mapCoords[0]=pos[0];
	mapCoords[1]=pos[1];
}

void World::onControl(int key,KeyEventType type,int x,int y,float wheel)
{
	int screen[] = {x,y};

	cursor.screenPos = vec2f(x,y);
	screen2world(screen,cursor.worldPos);

	cursor.object = gameObjects->objectAtPoint(cursor.worldPos);

	getScripter()->call("onControl", key, (int)type, x, y, wheel);
}

int World::processHit(GameObject *a, ObjectType typea,GameObject *b,ObjectType typeb,const vec2f &normal,const vec2f &tangent)
{
//	LogFunction(*g_logger);
	if(a && typea==typeProjectile)
	{
		Projectile *proj=(Projectile*)a;
		if(typeb==typeBlock)
		{
//			(*g_logger)(2,"wrong collision pair");
			assert(!b);
		}
		else if(typeb=typeUnit){}
		proj->onHit(b);
	}
	return 1;
}

bool World::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
	GameObject * object0=(GameObject*)fixtureA->GetBody()->GetUserData();
	GameObject * object1=(GameObject*)fixtureB->GetBody()->GetUserData();
	if(object1 && object0)
	{
		CollisionGroup type0=object0->getCollisionGroup();
		CollisionGroup type1=object1->getCollisionGroup();
		if(type0 == cgProjectile && type1 == cgUnit)
		{
			if(dynamic_cast<Projectile*>(object0)->Source == object1)
				return false;
		}
		if(type1 == cgProjectile && type0 == cgUnit)
		{
			if(dynamic_cast<Projectile*>(object1)->Source == object0)
				return false;
		}
		return collisionMap[type0][type1];
	}
	return true;
}

void World::BeginContact(b2Contact* contact)
{
	GameObject * object0=(GameObject*)contact->GetFixtureA()->GetBody()->GetUserData();
	GameObject * object1=(GameObject*)contact->GetFixtureB()->GetBody()->GetUserData();
	if(object1 && object0)
	{
		ObjectType type0=object0->getType();
		ObjectType type1=object1->getType();
		processHit(object0,object0->getType(),object1,object1->getType(), conv(contact->GetManifold()->localNormal),vec2f(0,0));
		processHit(object1,object1->getType(),object0,object0->getType(),-conv(contact->GetManifold()->localNormal),vec2f(0,0));
	}
}

void World::setView(float x, float y, float angle, float scale)
{
	draw->globalView.scale = scale;
	draw->globalView.pose = Pose(x, y, angle*3.1415/180);
}

void testGameObject(GameObject * object)
{
//	LogFunction(*g_logger);
//	g_logger->line(0,"some test on object");
}
