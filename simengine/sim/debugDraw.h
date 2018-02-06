#pragma once

#include "gameObject.h"
#include "vision.h"

namespace sim
{

/*
 * Helper class to draw simplified box2d objects
 */
class Draw: public b2Draw
{
public:
	Fx::RenderContext* hge;
	Fx::RenderQueue* queue;
	//HTARGET* passVision;
	Fx::FxView2 globalView;

	bool drawDynamics;
	bool drawDevices;
	bool drawTasks;

	//Mt3x3 getTransform();
	//void translate(const vec2f &pos);
	Draw();
	~Draw();

	void init(Fx::RenderContext* rc);
	void drawObject(GameObject* gameObject);
	void draw(const VisionManager::VisionDesc& vision);
	//void draw(const Mover *mover);
	//void draw(Perception *perception);
	//void draw(Item *item);

	void draw(Unit *unit);
	void drawSprite(const Pose &pose, const Fx::SpriteData *sprite);

	/// from b2DebugRender
	virtual void DrawTransform(const b2Transform &);
	virtual void DrawPolygon (const b2Vec2 *vertices, int32 vertexCount, const b2Color &color);
	virtual void DrawSolidPolygon (const b2Vec2 *vertices, int32 vertexCount, const b2Color &color);
	virtual void DrawCircle (const b2Vec2 &center, float32 radius, const b2Color &color);
	virtual void DrawSolidCircle (const b2Vec2 &center, float32 radius, const b2Vec2 &axis, const b2Color &color);
	virtual void DrawSegment (const b2Vec2 &p1, const b2Vec2 &p2, const b2Color &color);
};

void drawLine(Fx::RenderContext* rc, const vec2f &from, const vec2f &to, Fx::FxRawColor color);

}
