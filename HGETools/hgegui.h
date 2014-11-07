/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeGUI helper classes header
*/


#ifndef HGEGUI_H
#define HGEGUI_H

#include "../frostTools/autoPtr.hpp"
#include <list>
#include <functional>
#include "../frostTools/3dmath.h"

#include "hge.h"
#include "hgesprite.h"
#include "hgerect.h"

#define HGEGUI_NONAVKEYS		0
#define HGEGUI_LEFTRIGHT		1
#define HGEGUI_UPDOWN			2
#define HGEGUI_CYCLED			4

namespace GUI
{
	enum AlignType
	{
		AlignManual,	// desired pos and size
		AlignCenter,	// desired width/height + center position
		AlignExpand,	// fixed position by parent
		AlignMin,		// left or top, desired width/height
		AlignMax,		// right or bottom, desired width/height
		AlignCell,
	};
	/*
	** hgeGUIObject
	*/
	class Object : public Referenced
	{
	public:
		typedef vec2f uiVec;
		typedef SharedPtr<Object> Pointer;

		Object(const hgeRect & rc);
		virtual			~Object();

		void detach();			/// detach from parent
		bool isRoot() const;
		// manual align
		Object * insert(Object * ptr);
		Object * insert(Object * ptr, int cellX, int cellY, int cellWidth, int cellHeight);

		void removeChild(const Pointer & object);

		void setAlign( AlignType hor, AlignType ver );
		// set rect
		virtual void	setRect(const hgeRect & rc);
		// get window rect (screen)
		virtual hgeRect getRect() const;
		// get client rect (screen)
		virtual hgeRect getClientRect() const;

		void setDesiredPos(float x, float y);
		void setDesiredSize(float width, float height);

		struct Signal
		{
			size_t msg;
			int arg0;
			int arg1;
			int handled;
		};
		/// send signal to parents
		virtual bool	sendSignalUp(Signal &msg);
		/// send signal to children
		virtual bool	sendSignalDown(Signal &msg);
		/// recieved signal from parent
		virtual bool	onSignalUp(Signal & msg) { return false;}
		/// recieved signal from children
		virtual bool	onSignalDown(Signal & msg) { return false;}
		virtual void	onRender() {}
		virtual void	onUpdate(float dt) {} 
		virtual void	onSize(float width, float height) {}
		virtual bool	IsDone() { return true; }
	
		virtual bool	sizeFixed() const { return false; }
		virtual void	SetColor(DWORD _color) { color=_color; }

		virtual void show(bool flag) {visible = flag;}
	
		hgeRect			windowRect;
		DWORD			color;

		enum MoveState
		{
			MoveEnter,
			MoveRemain,
			MoveLeave
		};
		// event handlers
		virtual bool onMouse(int mouseId, int key, int state, const uiVec & vec) {return false;}	
		virtual bool onMouseMove(int mouseId, const uiVec & vec, MoveState state = MoveRemain) { return false;}
		// event callers
		virtual bool callMouseMove(int mouseId, const uiVec & vec, MoveState state = MoveRemain);
		virtual bool callMouse(int mouseId, int key, int state, const uiVec & vec);
		virtual void callKeyClick(int key, int chr);
		virtual void callRender(const hgeRect & clipRect);
		virtual void callUpdate(float dt);
		HGE * getHGE();

		void enableLayouting( bool flag );	
		void updateLayout();
		typedef bool ObjectIterator( Object * object);
		void findObject(const uiVec & vec, bool forceAll, std::function<ObjectIterator> fn);
	protected:
		//virtual void runLayout();
		void calculateLayout(const Pointer & object);			// calculate layout for specific object
		//virtual hgeRect calculateChildrenRect() const;
		virtual hgeRect calculateContentsRect() const;
		float desiredX, desiredY, desiredWidth, desiredHeight;	// desired control size
		AlignType alignHor, alignVer;							// alignment to parent
		float borderSize, margin;
		
		typedef std::list<Pointer> Children;
		Children children;
		WeakPtr<Object> parent;									// parent object
		WeakPtr<Object> modal;									// modal children
		int cellX, cellY, cellWidth, cellHeight;
		bool clipChildren;
		bool visible;											// if object is visible
		bool enabled;											// if object responds to events	
		bool contentsWidth, contentsHeight;						// auto expand to contents size
		HGE * hge;
	private:
		bool layoutChanged;	
		bool layouting;
 		static size_t globalControlLastId;
		size_t globalControlId;
		
		// do not use it
		Object(const Object &go);
		Object&	operator= (const Object &go);
	};
}

void drawRectSolid(HGE * hge, const hgeRect & rect, DWORD color);
void drawRect(HGE * hge, const hgeRect & rect, DWORD color);
#endif
