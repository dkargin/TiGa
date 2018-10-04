#pragma once

#include <list>
#include <functional>
#include <memory>

#include "math3/math.h"
#include "fx/spritedata.h"
#include "fx/rect.h"
#include "fx/rendercontext.h"

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
	 * Please no std::enable_shared_from_this<Object>
	 */
	class Object
	{
	public:
		typedef math3::vec2f uiVec;
		typedef std::shared_ptr<Object> Pointer;

		Object(const Fx::Rect & rc);
		virtual ~Object();

		void detach();			/// detach from parent
		bool isRoot() const;

		// manual align
		void insertWidget(Object* ptr);

		void removeWidget(Object* object);

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
		virtual bool sendSignalUp(Signal &msg);
		/// send signal to children
		virtual bool sendSignalDown(Signal &msg);
		/// recieved signal from parent
		virtual bool onSignalUp(Signal & msg) { return false;}
		/// recieved signal from children
		virtual bool onSignalDown(Signal & msg) { return false;}
		virtual void onRender(Fx::RenderContext* rc) {}
		virtual void onUpdate(float dt) {}
		virtual void onSize(float width, float height) {}
		virtual bool IsDone() { return true; }
	
		virtual bool sizeFixed() const { return false; }
		virtual void SetColor(Fx::FxRawColor color) { this->color=color; }

		virtual void show(bool flag) {visible = flag;}
	
		Fx::Rect windowRect;
		Fx::FxRawColor color;

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
		virtual void callRender(Fx::RenderContext* context, const Fx::Rect& clipRect);
		virtual void callUpdate(float dt);

		void enableLayouting(bool flag );
		void updateLayout();

		typedef bool ObjectIterator(Object* object);
		void findObject(const uiVec& vec, bool forceAll, std::function<ObjectIterator> fn);

	protected:
		// calculate layout for specific object
		void calculateLayout(Object* object);
		//virtual Fx::Rect calculateChildrenRect() const;
		virtual Fx::Rect calculateContentsRect() const;
		float desiredX, desiredY, desiredWidth, desiredHeight;	// desired control size
		AlignType alignHor, alignVer;							// alignment to parent
		float borderSize, margin;

		typedef std::list<Object*> Children;
		Children children;
		// Parent object
		Object* parent = nullptr;
		// Modal child. All input is redirected to this object
		Object* modal = nullptr;
		// Cell parameters for grid layout
		int cellX = 0, cellY = 0, cellWidth = 1, cellHeight = 1;
		bool clipChildren;
		// Check if object is visible
		bool visible;
		// Check if object responds to events
		bool enabled;
		// Auto expand to contents size
		bool contentsWidth, contentsHeight;

	private:
		bool layoutChanged;	
		bool layoutIsActive;
		static size_t globalControlLastId;
		size_t globalControlId;

		// do not use it
		Object(const Object& go);
		Object& operator=(const Object& go);
	};

} // namespace GUI

