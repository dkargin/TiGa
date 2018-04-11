#include "world.h"
#include <cmath>
#include <fx/fxmanager.h>

//using namespace pathProject;

/*
int gsystem(const char *format,...)
{
	static char fileName[MAX_PATH];
	va_list v;
	va_start (v, format);
	vsprintf(fileName,format,v);
	va_end(v);
	return system(fileName);
}
*/

Level::Level(World & world)
:body(NULL), world(world), wallMode(Transparent)
{
	//pathing(NULL),quadder(NULL),
}

Level::~Level()
{
	clear();
}

void Level::clear()
{
	if(background)
		background = nullptr;

	for(int i=0; i < size_x() * size_y(); i++)
	{
		Tile &tile=blocks[i];
		if(tile.solid)
		{
			tile.solid->GetWorld()->DestroyBody(tile.solid);
			tile.solid = nullptr;
			//blocks[i]->release();
			//blocks[i]=NULL;
		}

		if(tile.effects)
		{
			tile.effects.reset();;
		}
	}

	if(body)
	{
		body->GetWorld()->DestroyBody(body);
		body = NULL;
	}

	mapName = "";
}

void Level::update()
{
	updateArea(0,0, size_x()-1, size_y()-1);
}

void Level::updateArea(int x0,int y0,int x1,int y1)
{
	if(!(x1-x0) || !(y1-y0))
		return;
	if(x1>x0)
	{
		int tmp=x1;x1=x0;x0=tmp;
	}

	if(y1>y0)
	{
		int tmp=y1;y1=y0;y0=tmp;
	}

#ifdef FIX_PATHING
	std::vector<unsigned char> data(size_x()*size_y());
	//for(int i=0;i<data.size();i++)
	//	data[i]=blocks[i]?1:0;
	if(pathing)
	{
		pathing->setBitmap(Builder::targetCost0,blocks.size_x(),blocks.size_y(),Builder::sourceBitmap8,&data[0]);
		pathing->build();
	}
#endif
}

void Level::saveState(sim::StreamOut & stream)
{
	stream.write(cellSize);
	stream.write(mapName);
	stream.write(wallMode);
}

bool Level::loadState(sim::StreamIn & stream)
{
	float size = 0;
	std::string file;

	stream.read(size);
	stream.read(file);
	stream.read(wallMode);

	init(file.c_str(), size, wallMode != Transparent, Pose(0,0,0,-10));
	return true;
}

void Level::initBlocks(int width, int height)
{
	blocks.resize(width*height);
	blocks_w = width;
	blocks_h = height;
}

void Level::generateWallBlocks()
{
	int width = size_x();
	int height = size_y();

	b2BodyDef bodyDef;
	bodyDef.type = b2_kinematicBody;
	bodyDef.position.Set(0.0f, 0.0f);
	b2PolygonShape shape;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = 0.f;
	fixtureDef.friction = 0.0f;
	int objects = 0;
	auto SolidLine = [&](float x0, float x1, float y)
	{

		shape.SetAsBox(	cellSize/2 * (x1 - x0),
						cellSize/2,
						b2Vec2( (x1 + x0) * 0.5f * cellSize - width * cellSize/2,
								height * cellSize/2 - y * cellSize), 0);


		//level.blocks(x,y).solid = NULL;
		//level.body = dynamics.CreateBody(&bodyDef);
		body->CreateFixture(&fixtureDef);
	};
	//Raster2D<char> directions(header.width,header.height);
	// create solid bodies
	for(int y = 0; y < height; y++)
	{
		int lineStart = -1;
		for(int x = 0; x <= width; x++ )
		{
			unsigned int flag = (x < width) ? blocks[x + y*blocks_w].flags : 0;
			if(flag && lineStart == -1)
			{
				lineStart = x;
			}
			else if(!flag && lineStart != -1)
			{
				if(!body)
					body = world.dynamics.CreateBody(&bodyDef);
				float hsize = cellSize / 2;
				SolidLine(lineStart,x - 1,y);
				objects++;
				lineStart = -1;
			}
		}
	}
	wallMode = WallBlocks;
}
// Instead of generating solid boxes i propose another approach
// This algorithm generates edges for single cell and tries to merge
// edges from adjacent cells.
void Level::generateWallEdges()
{
//	LogFunction(*g_logger);

	wallMode = Transparent;

	int width = size_x();
	int height = size_y();

	int maxWalls = 4 * ((width*height +1)/2);
	Wall * walls = new Wall[maxWalls];

	if(!walls)
	{
//		g_logger->line(3,"Not enough memory");
		return;
	}

	int wallsCount = 0;
	// create new wall and add it to container
	auto addWall = [&](int x0, int y0, int dx, int dy)->Wall*
	{
		assert(wallsCount < maxWalls);
		Wall *wall = &walls[wallsCount++];
		wall->x0 = x0;
		wall->y0 = y0;
		wall->x1 = x0 + dx;
		wall->y1 = y0 + dy;
		return wall;
	};

	struct CellWalls
	{
		Wall * walls[4]; //{left, top, right, bottom }
	};

	//Raster2D<CellWalls> cells(width,height);
	std::vector<CellWalls> cells(width*height);
	// try to prolong existing wall found in cell(x,y) to direction (dx,dy)
	auto mergeWall = [&](int w, int x0, int y0, int dx, int dy)->Wall*
	{
		Wall * wall = cells[x0 + y0*width].walls[w];
		if(wall)
		{
			wall->x1 += dx;
			wall->y1 += dy;
		}
		return wall;
	};

	// 1. Analyse walls
	for(int y = 0; y < height; y++)
		for(int x = 0; x < width; x++ )
		{
			int i = x + y*blocks_w;
			auto value = blocks[i].flags;
			// skip free cells
			if( value == 0 ) continue;

			auto cell = cells[i].walls;

			auto hasBorder = [&](int dx,int dy)->bool
			{
				if(	( dx < 0 && x < -dx ) ||
					( dy < 0 && y < -dy ) ||
					( dx > 0 && x + dx >= width) ||
					( dy > 0 && y + dy >= height))
					return true;
				return value != blocks[x + dx + (y + dy)*width].flags;
			};

			bool borders[] =
			{
				hasBorder(-1, 0),	// left
				hasBorder( 0,-1),	// top
				hasBorder( 1, 0),	// right
				hasBorder( 0, 1)	// bottom
			};
			// wall corner - cx,cy, wall cell - x,y
			auto build =  [&](int w, int cx, int cy, int x, int y)
			{
				// determine wall direction
				int dy = ~w & 1;
				int dx = w & 1;

				if(borders[w])
				{
					// create new wall
					if(borders[dy])
						cell[w] = addWall(cx, cy, dx, dy);
					// try to prolong existing wall
					else if( ! (cell[w] = mergeWall(w, x - dx, y - dy, dx, dy )))
						cell[w] = addWall(cx, cy, dx, dy);
				}
				else
					cell[w] = 0;
			};

			build(0,x,y,x,y);	// build left
			build(1,x,y,x,y);	// build top
			build(2,x+1,y,x,y);	// build right
			build(3,x,y+1,x,y);	// build bottom
		}

	// 2. Generate solids for each wall
	if(wallsCount)
	{
		// 2.1 Generate root body
		b2BodyDef bodyDef;
		bodyDef.type = b2_kinematicBody;
		bodyDef.position.Set( -width * cellSize * 0.5, -height * cellSize * 0.5f);
		body = world.dynamics.CreateBody(&bodyDef);

		b2PolygonShape shape;
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;
		fixtureDef.density = 0.f;
		fixtureDef.friction = 0.0f;
		// 2.2 Add wall shapes
		std::for_each(walls, walls + wallsCount,[&](const Wall &wall)
		{
			shape.m_vertexCount = 2;
			shape.m_type = b2PolygonShape::e_edge;
			shape.m_vertices[0] = b2Vec2(wall.x0 * cellSize, (height - wall.y0) * cellSize);
			shape.m_vertices[1] = b2Vec2( wall.x1 * cellSize, (height - wall.y1) * cellSize);
			body->CreateFixture(&fixtureDef);
		});
		wallMode = WallEdges;
	}

	delete[] walls;
}

bool Level::isSolid() const
{
	return wallMode != Transparent;
}

int Level::init(const char * map, float size, bool rigid, const Pose &pose)
{
	clear();
	if(map)
	{
		struct RawHeader
		{
			enum ChannelType
			{
				UByte,
				Byte,
				UShort,
				Short,
				UInt,
				Int,
				Float,
			};
			char type;
			size_t width;
			size_t height;
		}header;
#ifdef FIX_THIS
		// convert to raw form
		gsystem("shTools img2raw \"%s\" \"%s.raw\"", map, map);
		// open image
		char tempName[255];
		sprintf(tempName,"%s.raw",map);
		FILE * in = fopen(tempName,"r");
		if(!in)
			return 0;
		// read raw image header
		fread(&header,sizeof(header),1,in);
		cellSize = size;
#endif

#ifdef FIX_PATHING
		// create map builders
		MapManager *manager = world.pathCore.getMapManager();
		pathing = new pathProject::Builder::MapBuilderImage(*manager,"map2D");
		//quadder = new pathProject::MapBuilderQuad(*manager,"quad");

		assert(pathing);
		{
			pathing->setCellSize(cellSize, cellSize);
			// set map orientation
			Mt4x4 pose = Mt4x4::translate(- 0.5 * header.width * size ,0.5 * header.height * size,0);
			pose.axisY(0,-1,0);
			pathing->setMapOrientation(pose);
			// send data to map builder
			auto buffer = new char[header.width * header.height];
			fread(buffer,1,header.width*header.height,in);
			initBlocks(header.width,header.height);
			const float robotSize = 0.6;
			pathing->setSizeOffset(robotSize);
			pathing->setBitmap(pathProject::Builder::targetCost0,header.width,header.height,pathProject::Builder::sourceBitmap8,buffer);
			pathProject::DefMap2D def;
			// set this to enable hierarchy
			def.layerFlags.layerRegionID = true;
			pathing->setLayerDef(def);
			pathing->build();

			initBlocks(header.width, header.height);
			size_t i = 0;
			for(int y = 0; y < header.height; y++)
				for(int x = 0; x < header.width; x++, i++)
				{
					blocks[i].flags = buffer[i];
				}

			if(quadder)
			{
				quadder->setSource(pathing->getTarget());
				quadder->build();
			}
			//int mode = heuristicNone;
			//pathCore.setParam(ppHeuristicMode,&mode);
			delete []buffer;
		}
#endif`
		if(rigid)
		{
			//generateWallBlocks();
			generateWallEdges();
		}
		fclose(in);
		// init level sprite
		Fx::FxSpritePtr ptr = world.gameObjects->fxManager->fxSprite(map,0,0,0,0);
		float w = ptr->sprite.GetWidth();
		float h = ptr->sprite.GetHeight();
		ptr->setBlendMode(COLORMUL | ALPHAADD);
		float scale = header.width / w;
		ptr->setScale(scale*size);
		ptr->setPose(pose);
		background = ptr;
		mapName = map;
	}
	return 1;
}

/*
void World::setWall(int x,int y,int z,int type)
{
	if(level.blocks.validPos(x,y))
	{
		if(type && !level.blocks(x,y)) // create new block
		{
			vec3 pos=vec3(x*cellSize,y*cellSize,cellSize*0.5)-vec3(	level.blocks.size_x(),
																	level.blocks.size_y(),0)*cellSize*0.5;
			//level.blocks(x,y)=Shape::create(dynamicsScene,
			//								Shape::Box(cellSize/2,cellSize/2,cellSize/2),
			//								Mt4x4::translate(pos));
			//level.blocks(x,y)->enable(solidFrozen);
			//level.blocks(x,y)->setGroup(typeBlock);
		}
		else if(!type && level.blocks(x,y))	// delete block
		{
			//level.blocks(x,y)->release();
			//level.blocks(x,y)=NULL;
		}
	}
}
*/
int Level::getWall(int x,int y,int z)
{
	if(blocks.validPos(x,y))
	{
		return blocks(x,y).flags;
	}
	return -1;
}

void World::updateMap()
{
	level.update();
}
