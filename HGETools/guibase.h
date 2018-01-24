#pragma once

#include <list>
#include <functional>
#include <memory>

#include "math3/math.h"
#include "spritedata.h"
#include "rect.h"

#define HGEGUI_NONAVKEYS		0
#define HGEGUI_LEFTRIGHT		1
#define HGEGUI_UPDOWN			2
#define HGEGUI_CYCLED			4

namespace GUI
{
	enum AlignType
	{
		AlignManual,	//< desired pos and size
		AlignCenter,	//< desired width/height + center position
		AlignExpand,	//< fixed position by parent
		AlignMin,			//< left or top, desired width/height
		AlignMax,			//< right or bottom, desired width/height
		AlignCell,
	};
	/*
	 * Object
	 * Base GUI class and layout container as well
	 */
	class Object : public Referenced
	{
	public:
		typedef math3::vec2f uiVec;

		Object(const Fx::Rect & rc);
		virtual ~Object();

		void detach();			/// detach from parent
		bool isRoot() const;
		// manual align
		Object* insert(Object* ptr);
		Object* insert(Object* ptr, int cellX, int cellY, int cellWidth, int cellHeight);

		void removeChild(const Pointer & object);

		void setAlign( AlignType hor, AlignType ver );
		// set rect
		virtual void setRect(const Fx::Rect & rc);
		// get window rect (screen)
		virtual Fx::Rect getRect() const;
		// get client rect (screen)
		virtual Fx::Rect getClientRect() const;

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
	
		Rect			windowRect;
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
		virtual void callRender(const Fx::Rect& clipRect);
		virtual void callUpdate(float dt);
		HGE * getHGE();

		void enableLayouting(bool flag );
		void updateLayout();
		typedef bool ObjectIterator(Object * object);
		void findObject(const uiVec & vec, bool forceAll, std::function<ObjectIterator> fn);
	protected:
		//virtual void runLayout();
		void calculateLayout(const Pointer & object);			// calculate layout for specific object
		//virtual Fx::Rect calculateChildrenRect() const;
		virtual Fx::Rect calculateContentsRect() const;
		float desiredX, desiredY, desiredWidth, desiredHeight;	// desired control size
		AlignType alignHor, alignVer;							// alignment to parent
		float borderSize, margin;
		
		typedef std::list<Pointer> Children;
		Children children;
		WeakPtr<Object> parent;									//< parent object
		WeakPtr<Object> modal;									//< modal child
		int cellX, cellY, cellWidth, cellHeight;
		bool clipChildren;
		bool visible;											//< if object is visible
		bool enabled;											//< if object responds to events
		bool contentsWidth, contentsHeight;						//< auto expand to contents size
		HGE * hge;
	private:
		bool layoutChanged;	
		bool layouting;
 		static size_t globalControlLastId;
		size_t globalControlId;
		
		// do not use it
		Object(const Object& go);
		Object& operator=(const Object& go);
	};

} // namespace GUI

void drawRectSolid(HGE* hge, const Fx::Rect& rect, DWORD color);
void drawRect(HGE* hge, const Fx::Rect& rect, DWORD color);


