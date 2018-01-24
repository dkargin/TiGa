#pragma once;

class Draw: public b2Draw
{	
	Mt3x3 local;
	
public:
	HGE *hge;	
	HTARGET *passVision;
	FxView2 globalView;

	bool drawDynamics;
	bool drawDevices;
	bool drawTasks;

	//Mt3x3 getTransform();
	//void translate(const vec2f &pos);
	Draw();
	~Draw();

	void init(HGE *hge);
	void drawObject(GameObject *gameObject);
	void draw(const VisionManager::Vision &vision);
	//void draw(const Mover *mover);
	//void draw(Perception *perception);
	//void draw(Item *item);

	void draw(Unit *unit);
	void drawSprite(const Pose &pose, hgeSprite *sprite);

	/// from b2DebugRender
	virtual void DrawTransform(const b2Transform &);
	virtual void DrawPolygon (const b2Vec2 *vertices, int32 vertexCount, const b2Color &color);
	virtual void DrawSolidPolygon (const b2Vec2 *vertices, int32 vertexCount, const b2Color &color);
	virtual void DrawCircle (const b2Vec2 &center, float32 radius, const b2Color &color);
	virtual void DrawSolidCircle (const b2Vec2 &center, float32 radius, const b2Vec2 &axis, const b2Color &color);
	virtual void DrawSegment (const b2Vec2 &p1, const b2Vec2 &p2, const b2Color &color);
}; 

void drawLine(HGE * hge, const vec2f &from, const vec2f &to, DWORD color);
