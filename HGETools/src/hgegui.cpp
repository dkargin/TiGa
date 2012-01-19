/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeGUI helper class implementation
*/

#include "stdafx.h"
#include "hgegui.h"
namespace GUI
{
	size_t Object::globalControlLastId = 0;
	Object::Object(const hgeRect & rc)
	:visible(true)
	,enabled(true)
	,alignHor(AlignManual)
	,alignVer(AlignManual)
	,borderSize(0)
	,windowRect(rc)
	,desiredX(rc.x1)
	,desiredY(rc.y1)
	,desiredWidth(rc.x2 - rc.x1)
	,desiredHeight(rc.y2 - rc.y1)
	{ 
		color = 0xFFFFFFFF; 
		hge=hgeCreate(HGE_VERSION);
		clipChildren = false;
		contentsWidth = false;
		contentsHeight = false;
		globalControlId = ++globalControlLastId;
		/// maybe this can happen one day
		if( globalControlLastId == ~(size_t)0 )
			std::_Xoverflow_error("globalControlLastId");
	}

	Object::~Object()
	{
		if(parent != NULL)
		{
			parent->detach(this);
			int w = 0;
			parent = NULL;
		}
		hge->Release();
	}

	bool Object::isRoot() const
	{
		return parent == NULL;
	}

	void Object::insert(Object *object)
	{
		assert(object != this);
		assert(object->parent != this);
		children.push_back(object);
		hgeRect newRect(object->desiredX, 
						object->desiredY, 
						object->desiredX + object->desiredWidth, 
						object->desiredY + object->desiredHeight);
		object->alignHor = AlignManual;
		object->alignVer = AlignManual;
		object->setRect(newRect);
		object->parent = this;
		calculateLayout(object);
	}

	void Object::insert(Object *object, AlignType hor, AlignType ver)
	{
		assert(object != this);
		assert(object->parent != this);
		children.push_back(object);
		object->alignHor = hor;
		object->alignVer = ver;	
		object->parent = this;
		calculateLayout(object);	
	}

	void Object::insert(Object * object, int cellX, int cellY, int cellWidth, int cellHeight)
	{
		assert(object != this);
		assert(object->parent != this);
		children.push_back(object);
		object->alignHor = AlignCell;
		object->alignVer = AlignCell;
		object->parent = this;
		calculateLayout(object);	
	}

	void Object::detach(const Object::Pointer & object)
	{
		Children::iterator it = std::find(children.begin(), children.end(), object);
		if(it != children.end())
			children.erase(it);
	}

	void Object::callUpdate(float dt)
	{	
		onUpdate(dt);

		for(Children::iterator it = children.begin(); it != children.end(); ++it)
		{
			Object * object = it->get();
			if(object != NULL)
				object->callUpdate(dt);
		}
	}
	/*
	hgeRect Object::calculateChildrenRect() const
	{	
		// TODO:
		if( children.empty() )
			return hgeRect();
		hgeRect result;
		for(Children::const_iterator it = children.begin(); it != children.end(); ++it)
		{
			Object * object = it->get();
			if(object != NULL)
			{
				//resultobject->calculateChildrenRect();
			}
		}
		return result;
	}
	
	void Object::runLayout()
	{

	}
	*/
	hgeRect Object::calculateContentsRect() const
	{		
		hgeRect result;		
		bool first = true;

		for(Children::const_iterator it = children.begin(); it != children.end(); ++it)
		{
			Object * child = it->get();
			hgeRect childRect = child->getRect();

			if( child->contentsWidth || child->contentsHeight )
			{
				hgeRect childContentsRect = child->calculateContentsRect();

				if( child->contentsWidth )
				{
					childRect.x1 = childContentsRect.x1;
					childRect.x2 = childContentsRect.x2;
				}
				if( child->contentsHeight )
				{
					childRect.y1 = childContentsRect.y1;
					childRect.y2 = childContentsRect.y2;
				}
			}
			if( first )
			{
				result = childRect;
				first = false;
			}
			else
			{
				result = hgeRect::Merge(result, childRect);
			}
		}
		return result;
	}

	void Object::enableLayouting( bool flag )
	{
		if( layouting != flag )
		{
			layouting = flag;
			//if( layouting )
			//	runLayout();
		}
	}

	void Object::callRender(const hgeRect & clipRect)
	{
		if(!visible)
			return;
		HGE * hge = getHGE();
		
		hgeRect clip = hgeRect::Intersect(getRect(),clipRect);
		// if avialable area is zero - return
		if(clip.IsClean())
			return;
		// craw self
		const bool uiDebug = false;
		// draw children
		if( clipChildren )
			hge->Gfx_SetClipping(clip.x1, clip.y1, clip.x2 - clip.x1, clip.y2 - clip.y1);
		if(uiDebug)
			drawRect(hge, getRect(), ARGB(255,64,255,64));
		onRender();
		// turn off clipping
		if( clipChildren )
			hge->Gfx_SetClipping();
				
		//clip = hgeRect::Intersect(getClientRect(),clipRect);
		// render children
		for(Children::iterator it = children.begin(); it != children.end(); ++it)
		{
			Object * object = it->get();
			if(object)
				object->callRender(clip);
		}		
	}

	void Object::findObject( const vec2f & vec, bool forceAll, std::function<ObjectIterator> fn)
	{
		if(!forceAll && (!visible || !enabled))
			return;
		hgeRect rect = getRect();
		if(!rect.TestPoint(vec[0], vec[1]))
			return;
		if(fn && fn(this))
			return;
		for(Children::iterator it = children.begin(); it != children.end(); ++it)
		{
			Object * object = it->get();			
			object->findObject(vec, forceAll, fn);
		}
	}

	bool Object::callMouseMove( int mouseId, const uiVec & vec, Object::MoveState state )
	{
		if(!visible || !enabled)
			return false;
		hgeRect rect = getRect();
		// test if point is outside
		if(!rect.TestPoint(vec[0], vec[1]))
			return false;
		// if we have any handler - use it, or update children instead
		if(onMouseMove(mouseId, vec, state))
			return true;
		
		// update children
		for(Children::iterator it = children.begin(); it != children.end(); ++it)
		{
			Object * object = it->get();						
			if( object->callMouseMove(mouseId, vec, state) )
				return true;
		}
		return false;
	}

	bool Object::callMouse( int mouseId, int key, int state, const uiVec & vec )
	{
		if(!visible || !enabled)
			return false;
		hgeRect rect = getRect();
		// test if point is outside
		if(!rect.TestPoint(vec[0], vec[1]))
			return false;

		// if we have any handler - use it, or update children instead
		if(onMouse(mouseId, key, state, vec))
			return true;
		// update children
		bool res = false;
		for(Children::iterator it = children.begin(); it != children.end(); ++it)
		{
			Object * object = it->get();
			if(object != NULL)
				res = res || object->callMouse(mouseId, key, state, vec);
			if( res )
				break;
		}
		return res;
	}

	void Object::callKeyClick(int key, int state)
	{
	}

	void Object::calculateLayout(const Object::Pointer & object)
	{
		hgeRect rect = getClientRect();
		hgeRect newRect = object->getRect();
		switch(object->alignHor)
		{
		case AlignManual:
			newRect.x1 = object->desiredX;
			newRect.x2 = object->desiredX + object->desiredWidth;
			break;
		case AlignCenter:		
			newRect.x1 = (rect.x1 + rect.x2 - object->desiredWidth)/2;
			newRect.x2 = (rect.x1 + rect.x2 + object->desiredWidth)/2;
			break;
		case AlignExpand:
			newRect.x1 = rect.x1;
			newRect.x2 = rect.x2;
			break;
		case AlignMin:		// left
			newRect.x1 = rect.x1;
			newRect.x2 = rect.x1 + object->desiredWidth;
			break;
		case AlignMax:		// right
			newRect.x1 = rect.x2 - object->desiredWidth;
			newRect.x2 = rect.x2;
		case AlignCell:		// TODO: implement
			break;
		}

		switch(object->alignVer)
		{
		case AlignManual:
			newRect.y1 = object->desiredY;
			newRect.y2 = object->desiredY + object->desiredHeight;
			break;
		case AlignCenter:
			newRect.y1 = (rect.y1 + rect.y2 - object->desiredHeight)/2;
			newRect.y2 = (rect.y1 + rect.y2 + object->desiredHeight)/2;
			break;
		case AlignExpand:
			newRect.y1 = rect.y1;
			newRect.y2 = rect.y2;
			break;
		case AlignMin:	// top
			newRect.y1 = rect.y1;
			newRect.y2 = rect.y1 + object->desiredHeight;
			break;			
		case AlignMax:	// bottom
			newRect.y1 = rect.y2 - object->desiredHeight;
			newRect.y2 = rect.y2;
			break;			
		case AlignCell:	// TODO: implement
			break;
		}
		object->setRect(newRect);
	}

	bool Object::sendSignalUp(Object::Signal & msg)
	{
		if( parent )
		{
			if(!parent->onSignalUp(msg))
				return parent->sendSignalUp(msg);
			else
				return true;
		}
		return false;
	}

	bool Object::sendSignalDown(Object::Signal & msg)
	{
		/// 1. notify immediate children
		for( Children::iterator it = children.begin(); it != children.end(); ++it )
		{
			Object * object = *it;
			if( object && object->onSignalDown( msg ))	/// object can be occasionly dead
				return true;
		}
		/// 2. propagate signal further to children
		for( Children::iterator it = children.begin(); it != children.end(); ++it )
		{
			Object * object = *it;
			if( object && object->sendSignalDown(msg))
				return  true;
		}		
		return false;
	}

	void Object::setDesiredPos(float x, float y)
	{
		desiredX = x;
		desiredY = y;
	}

	void Object::setDesiredSize(float width, float height)
	{
		desiredWidth = width;
		desiredHeight = height;
	}

	void Object::setRect(const hgeRect & rect)
	{
		hgeRect oldRect = getRect();
		windowRect = rect;
		for(Children::iterator it = children.begin(); it != children.end(); ++it)
			calculateLayout(*it);
		onSize(rect.width(), rect.height());
	}

	hgeRect Object::getRect() const
	{
		return windowRect;
	}

	hgeRect Object::getClientRect() const
	{
		return hgeRect(windowRect.x1 + borderSize, windowRect.y1 + borderSize, windowRect.x2 - borderSize, windowRect.y2 - borderSize);
	}

	HGE * Object::getHGE()
	{
		return hge;
	}
} // namespace GUI

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void drawRect(HGE * hge, const hgeRect & rect, DWORD color)
{
	hge->Gfx_RenderLine(rect.x1, rect.y1,
						rect.x2, rect.y1, color);
	hge->Gfx_RenderLine(rect.x2, rect.y1,
						rect.x2, rect.y2, color);
	hge->Gfx_RenderLine(rect.x2, rect.y2,
						rect.x1, rect.y2, color);
	hge->Gfx_RenderLine(rect.x1, rect.y2,
						rect.x1, rect.y1, color);
}

void drawRectSolid(HGE * hge, const hgeRect & rect, DWORD color)
{
	float z = 0;

	auto assign = [&](hgeVertex & target, float x, float y)
	{
		target.x = x;
		target.y = y;
		target.z = z;
		target.tx = 0;
		target.ty = 0;
		target.col = color;
	};
	
	hgeQuad background;
	background.tex = 0;
	background.blend = 0;
	
	assign(background.v[0],rect.x1, rect.y1);
	assign(background.v[1],rect.x2, rect.y1);
	assign(background.v[2],rect.x2, rect.y2);
	assign(background.v[3],rect.x1, rect.y2);

	hge->Gfx_RenderQuad(&background);
}