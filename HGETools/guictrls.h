#ifndef HGEGUICTRLS_H
#define HGEGUICTRLS_H

#include <guibase.h>
#include <functional>


#include "hge.h"
#include "basetypes.h"
#include "spritedata.h"
#include "hgefont.h"
#include "rect.h"
#include "fxObjects.h"

namespace Fx
{

#define hgeButtonGetState(gui,id)		((hgeGUIButton*)gui->GetCtrl(id))->GetState()
#define hgeButtonSetState(gui,id,b)		((hgeGUIButton*)gui->GetCtrl(id))->SetState(b)
#define hgeSliderGetValue(gui,id)		((hgeGUISlider*)gui->GetCtrl(id))->GetValue()
#define hgeSliderSetValue(gui,id,f)		((hgeGUISlider*)gui->GetCtrl(id))->SetValue(f)
#define hgeGetTextCtrl(gui,id)			((hgeGUIText*)gui->GetCtrl(id))
#define hgeGetListboxCtrl(gui,id)		((hgeGUIListbox*)gui->GetCtrl(id))

namespace GUI
{
	typedef std::shared_ptr<hgeFont> FontPtr;
	/**
	 * Text widget
	 */
	class Text : public Object
	{
	public:
		Text();
		Text(const Rect & rect, FontPtr fnt);

		void SetMode(int _align);
		void SetText(const char *_text);
		void SetFont(FontPtr font);
		void printf(const char *format, ...);

		virtual void onRender();

	private:
		FontPtr font;
		// Text alignment
		int align;
		// Text contents
		char text[256];
	};

	class Frame: public Object
	{
	public:
		Frame();
		Frame(const Rect & rc);

		virtual void onRender();
		void setOffsetHor(float offset);
		void setOffsetVer(float offset);

		virtual bool	onMouseMove(int mouseId, const uiVec & vec, MoveState state);
		virtual bool	onMouse(int mouseId, int key, int state, const uiVec & vec);

		bool drawFrame;
		bool drawBackground;
		FxRawColor clrFrame;

		bool slideHor;
		bool slideVer;
	protected:
		int slideMouseId;			//< if control is sliding right now
		uiVec slideStart;			// 
		float startOffsetHor, startOffsetVer;
		float offsetHor, offsetVer;
		bool blockMouse;
	};
	///*
	//** Button
	//*/
	//class Button : public Object
	//{
	//public:
	//	Button(int id, const Rect & rect, HTEXTURE tex, float tx, float ty);
	//	virtual			~Button();
	//
	//	void			SetMode(bool _bTrigger) { bTrigger=_bTrigger; }
	//	void			SetState(bool _bPressed) { bPressed=_bPressed; }
	//	bool			GetState() const { return bPressed; }
	//
	//	virtual void	onRender();
	//	virtual bool	onMouse(int key, int state, const uiVec & vec);
	//
	//	virtual void	executeOnStateChange(bool down) {}
	//private:
	//	bool			bTrigger;
	//	bool			bPressed;
	//	bool			bOldState;
	//	hgeSprite		*sprUp, *sprDown;
	//};

	/*
	 * GUI Button2
	 */
	class Button : public Object
	{
	public:
		Button();
		virtual ~Button();

		//void SetImages(const hgeSprite & up, const hgeSprite & down);

		void setText(const char * text, FontPtr fnt);
		void setMode(bool _bTrigger) { bTrigger=_bTrigger; }
		void setState(bool _bPressed) { bPressed=_bPressed; }
		bool setState() const { return bPressed; }

		virtual void onRender();
		virtual bool onMouse(int mouseId, int key, int state, const uiVec & vec);

		typedef void EventOnStateChange(bool down);
		std::function<EventOnStateChange> onStateChange;
		std::function<void ()> onPressed;
		virtual void executeOnStateChange(bool down) {}

		EffectPtr sprite;
	private:
		FontPtr font;
		bool useSprites;
		Instance<Text> text;
		//Frame		frame;
		bool useText;

		bool bTrigger;
		bool bPressed;
		bool bOldState;
	};

	/*
	 * Image container widget
	 */
	class Image : public Object
	{
	public:
		Image();
		FxEffect::Pointer sprite;
		virtual void onRender();
	};

	/*
	 * Slider widget
	 */
	#define HGESLIDER_BAR			0
	#define HGESLIDER_BARRELATIVE	1
	#define HGESLIDER_SLIDER		2

	class Slider : public Object
	{
	public:
		Slider();
		virtual ~Slider();

		void init( bool vertical, int min, int max );
		void setMode(float _fMin, float _fMax, int _mode) { fMin=_fMin; fMax=_fMax; mode=_mode; }
		void setValue(float _fVal);
		float setValue() const { return fVal; }

		virtual void onRender();
		virtual bool onMouseMove(int mouseId, const uiVec & vec, MoveState state);
		virtual bool onMouse(int mouseId, int key, int state, const uiVec & vec);

		typedef void EventOnStateChange(Slider * slider, float value, float min, float max);
		std::function<EventOnStateChange> onStateChange;

		FxSpritePtr sprite;
	private:
		bool			bPressed;
		bool			bVertical;
		int				mode;
		float			fMin, fMax, fVal;		
	};
	/*
	** Listbox
	*/
	struct ListboxItem
	{
		char				text[64];
		ListboxItem	*next;
	};

	class Listbox : public Object
	{
	public:
		Listbox(int id, const Rect & rect, hgeFont *fnt, DWORD tColor, DWORD thColor, DWORD hColor);
		virtual ~Listbox();

		int AddItem(char *item);
		void DeleteItem(int n);
		int GetSelectedItem() { return nSelectedItem; }
		void SetSelectedItem(int n) { if(n>=0 && n<GetNumItems()) nSelectedItem=n; }
		int GetTopItem() { return nTopItem; }
		void SetTopItem(int n) { if(n>=0 && n<=GetNumItems()-GetNumRows()) nTopItem=n; }

		char* GetItemText(int n);
		int GetNumItems() { return nItems; }
		int GetNumRows() { return int((windowRect.y2-windowRect.y1)/font->GetHeight()); }
		void Clear();

		virtual void onRender();
		virtual bool MouseMove(float x, float y) { mx=x; my=y; return false; }
		virtual bool MouseLButton(bool bDown);
		virtual bool MouseWheel(int nNotches);
		virtual bool KeyClick(int key, int chr);

		virtual void executeOnRClick(int element) {}
		virtual void executeOnLClick(int element) {}
		virtual void executeOnSelChange(int element) {}
	private:
		SpriteData *sprHighlight;
		hgeFont *font;
		FxColorRaw textColor, texthilColor;

		int nItems, nSelectedItem, nTopItem;
		float mx, my;
		ListboxItem* pItems;
	};

	class Tabs : public Frame
	{
	public:
		Tabs();
		~Tabs();
		struct TabData
		{
			Button button;
			Frame frame;
		};
		Frame * addTab(const char * name);
		void removeTab(const char * name);

		std::map<std::string, TabData *> tabs;
	};
} // namespace Gui

} // namespace Fx
