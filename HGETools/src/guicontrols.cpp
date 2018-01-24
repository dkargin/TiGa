/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeGUI default controls implementation
*/

#include <guictrls.h>

#include "stdafx.h"

namespace GUI
{
/*
** hgeGUIText
*/
Text::Text() : Object(hgeRect())
{
	//static=true;
	visible = true;
	enabled = true;

	font = NULL;
	align = 0;
	text[0]=0;
}

Text::Text(const hgeRect & rect, FontPtr fnt)
	:Object(rect)
{
	//static=true;
	visible = true;
	enabled = true;

	font = fnt;
	align = 0;
	text[0]=0;
}

void Text::SetMode(int _align)
{
	align=_align;
}

void Text::SetFont(FontPtr fnt)
{
	font = fnt;
}

void Text::SetText(const char *_text)
{
	strcpy(text, _text);
}

void Text::printf(const char *format, ...)
{
	vsprintf(text, format, (char *)&format+sizeof(format));
}

void Text::onRender()
{
	if(font)
	{
		float tx, ty;
		if(align==HGETEXT_RIGHT) tx = windowRect.x2;
		else if(align==HGETEXT_CENTER) tx = (windowRect.x1 + windowRect.x2)/2.0f;
		else tx=windowRect.x1;

		ty = windowRect.y1+(windowRect.y2 - windowRect.y1 - font->GetHeight())/2.0f;

		font->SetColor(color);
		font->Render(tx,ty,align,text);
	}
}
/*
** Button2
*/
Button::Button()
	:Object(hgeRect(0,0,0,0)), font(NULL)
{
	text->setAlign(AlignExpand, AlignExpand);
	insert(text);

	visible = true;
	enabled = true;

	bPressed = false;
	bTrigger = false;	
}

Button::~Button(){}

void Button::onRender()
{
	if( sprite != NULL )
	{
		hgeRect rect = getRect();
		float width = sprite->getWidth();
		float height = sprite->getHeight();
		Pose2z pose(0.5 * (rect.x2 + rect.x1), 0.5 * (rect.y2 + rect.y1), 0, 0);
		sprite->render(sprite->getManager(), pose);
	}
	if(bPressed)
		drawRectSolid(getHGE(), getRect(), ARGB(255,65,65,65));
}

void Button::setText(const char * msg, FontPtr fnt)
{
	text->SetFont(fnt);
	text->SetText(msg);
	text->SetMode(HGETEXT_CENTER);
}

bool Button::onMouse(int mouseId, int key, int state, const Button::uiVec & pos)
{
	if(state)
	{
		bOldState = bPressed; bPressed = true;

		if(onStateChange)
			onStateChange(bPressed);

		executeOnStateChange(bPressed);
		return false;
	}
	else
	{
		if(bTrigger) 
			bPressed = !bOldState;
		else 
		{
			if(bPressed && onPressed)
				onPressed();
			bPressed = false;
		}

		if( onStateChange )
			onStateChange(bPressed);

		executeOnStateChange(bPressed);
		return true; 
	}
}

/*
** Frame
*/
Frame::Frame():Object(hgeRect(0,0,0,0)), drawFrame(true), drawBackground(true)
{
	offsetHor = 0;
	offsetVer = 0;
	slideMouseId = -1;
	slideHor = false;
	slideVer = false;
	blockMouse = false;
}

Frame::Frame(const hgeRect & rect)
	:Object(rect),drawFrame(true), drawBackground(true)
{
	offsetHor = 0;
	offsetVer = 0;
	slideMouseId = false;
	slideHor = false;
	slideVer = false;
	blockMouse = false;
}

void Frame::onRender()
{
	if(drawBackground)
		drawRectSolid(getHGE(), windowRect, color);
	if(drawFrame)
		drawRect(getHGE(), windowRect, clrFrame);	
}

void Frame::setOffsetHor( float offset )
{
	if( offset != offsetHor )
	{
		float delta = offset - offsetHor;
		for( Children::iterator it = children.begin(); it != children.end(); it++)
		{
			Object * object = *it;
			hgeRect rect = object->getRect();			
			object->setDesiredPos(rect.x1 + delta, rect.y1);
			calculateLayout(*it);
		}
		offsetHor = offset;
	}
}

void Frame::setOffsetVer( float offset )
{
	if( offset != offsetVer )
	{
		float delta = offset - offsetVer;
		for( Children::iterator it = children.begin(); it != children.end(); it++)
		{
			Object * object = *it;
			hgeRect rect = object->getRect();			
			
			object->setDesiredPos(rect.x1, rect.y1 + delta);
			calculateLayout(*it);
		}
		offsetVer = offset;
		//runLayout();
	}
}

bool	Frame::onMouseMove(int mouseId, const uiVec & vec, Frame::MoveState state)
{	
	if( slideMouseId != mouseId )
		return false;
	
	
	uiVec delta = vec - slideStart;
	const float minOffset = 2;
	if( state == MoveLeave)
	{
		slideMouseId = -1;
		blockMouse = false;
	}
	else if( !blockMouse && (fabs(delta[0]) > minOffset || fabs(delta[1]) > minOffset))
	{
		blockMouse = true;
	}

	if( slideHor )
	{
		setOffsetHor(offsetHor + delta[0]);
	}
	if( slideVer )
	{
		setOffsetVer(offsetVer + delta[1]);
	}	
	slideStart = vec;
	return blockMouse;
}

bool	Frame::onMouse(int mouseId, int key, int state, const uiVec & vec)
{
	if( state && (slideHor || slideVer) && slideMouseId == -1)
	{
		slideMouseId = mouseId;
		slideStart = vec;
		return false;
	}
	else if( mouseId == slideMouseId )
	{
		blockMouse = false;
		slideMouseId = -1;
	}
	return false;
}
/*
** Image
*/
Image::Image()
	:Object(hgeRect(0,0,0,0))
{}

void Image::onRender()
{
	if( sprite != NULL )
	{
		hgeRect rect = getRect();
		float width = sprite->getWidth();
		float height = sprite->getHeight();
		Pose2z pose(0.5 * (rect.x2 + rect.x1), 0.5 * (rect.y2 + rect.y1), 0, 0);
		sprite->renderAll(sprite->getManager(), pose);
	}
}

/*
** Slider
*/
Slider::Slider()
	:Object(hgeRect())
{
	bPressed=false;
	bVertical = true;
	

	mode = HGESLIDER_BAR;
	fMin=0; fMax=100; fVal=50;
}

Slider::~Slider()
{}

void Slider::setValue(float _fVal)
{
	if(_fVal<fMin) fVal=fMin;
	else if(_fVal>fMax) fVal=fMax;
	else fVal=_fVal;
}

void Slider::onRender()
{
	float xx, yy;
	float x1,y1,x2,y2;
	hgeRect rect = getRect();
	

	float sl_w = 0, sl_h = 0;
	if(sprite != NULL)
	{
		sl_w = sprite->getWidth();
		sl_h = sprite->getHeight();
	}	

	xx=rect.x1+(rect.x2-rect.x1 - sl_w)*(fVal-fMin)/(fMax-fMin);
	yy=rect.y1+(rect.y2-rect.y1 - sl_h)*(fVal-fMin)/(fMax-fMin);
	

	if(bVertical)
		switch(mode)
		{
			case HGESLIDER_BAR: 
				x1=rect.x1; y1=rect.y1; x2=rect.x2; y2=yy; 
				break;
			case HGESLIDER_BARRELATIVE: 
				x1=rect.x1; y1=(rect.y1+rect.y2)/2; x2=rect.x2; y2=yy; 
				break;
			case HGESLIDER_SLIDER: 
				x1=(rect.x1+rect.x2-sl_w)/2; y1=yy-sl_h/2; x2=(rect.x1+rect.x2+sl_w)/2; y2=yy+sl_h/2; 
				break;
		}
	else
		switch(mode)
		{
			case HGESLIDER_BAR: x1=rect.x1; y1=rect.y1; x2=xx; y2=rect.y2; break;
			case HGESLIDER_BARRELATIVE: x1=(rect.x1+rect.x2)/2; y1=rect.y1; x2=xx; y2=rect.y2; break;
			case HGESLIDER_SLIDER: x1=xx-sl_w/2; y1=(rect.y1+rect.y2-sl_h)/2; x2=xx+sl_w/2; y2=(rect.y1+rect.y2+sl_h)/2; break;
		}
	if( sprite )
		sprite->render( sprite->getManager(), FxEffect::Pose((x1 + x2)/2, yy + sl_h/2) );
	//sprSlider->RenderStretch(x1, y1, x2, y2);
}

bool Slider::onMouse(int mouseId, int key, int state, const uiVec & vec)
{
	bPressed = state;
	return false;
}

bool Slider::onMouseMove(int mouseId, const uiVec & vec, Slider::MoveState state)
{
	float x = vec[0];
	float y = vec[1];
	if(bPressed)
	{
		float value = fVal;
		hgeRect rect = getRect();
		if(bVertical)
		{
			if(y > rect.height()) 
				y = rect.height();
			if(y < 0) y = 0;
			value = fMin+(fMax-fMin)*y/rect.height();
		}
		else
		{			
			if(x > rect.width()) 
				x = rect.width();
			if(x < 0) x = 0;
			value = fMin+(fMax-fMin)*x/rect.width();
		}
		if( value != fVal )
		{
			fVal = value;
			if( onStateChange )
				onStateChange(this, value, fMin, fMax);
		}
		return true;
	}

	return false;
}


/*
** Listbox
*/

Listbox::Listbox(int _id, const hgeRect & rect, hgeFont *fnt, DWORD tColor, DWORD thColor, DWORD hColor)
	:Object(rect)
{
	//bStatic = false;
	//bVisible = true;
	//bEnabled = true;
	font=fnt;
	sprHighlight = new hgeSprite(0, 0, 0, (rect.x2 - rect.x1), fnt->GetHeight());
	sprHighlight->SetColor(hColor);
	textColor=tColor;
	texthilColor=thColor;
	pItems=0;
	nItems=0;

	nSelectedItem=0;
	nTopItem=0;
	mx=0; my=0;
}

Listbox::~Listbox()
{
	Clear();
	if(sprHighlight) delete sprHighlight;
}


int Listbox::AddItem(char *item)
{
	ListboxItem *pItem=pItems, *pPrev=0, *pNew;

	pNew = new ListboxItem;
	memcpy(pNew->text, item, std::min(sizeof(pNew->text), strlen(item)+1));
	pNew->text[sizeof(pNew->text)-1]='\0';
	pNew->next=0;

	while(pItem) { pPrev=pItem;	pItem=pItem->next; }

	if(pPrev) pPrev->next=pNew;
	else pItems=pNew;
	nItems++;

	return nItems-1;
}

void Listbox::DeleteItem(int n)
{
	int i;
	ListboxItem *pItem=pItems, *pPrev=0;

	if(n<0 || n>=GetNumItems()) return;

	for(i=0;i<n;i++) { pPrev=pItem;	pItem=pItem->next; }

	if(pPrev) pPrev->next=pItem->next;
	else pItems=pItem->next;

	delete pItem;
	nItems--;
}

char *Listbox::GetItemText(int n)
{
	int i;
	ListboxItem *pItem=pItems;

	if(n<0 || n>=GetNumItems()) return 0;

	for(i=0;i<n;i++) pItem=pItem->next;

	return pItem->text;
}

void Listbox::Clear()
{
	ListboxItem *pItem=pItems, *pNext;

	while(pItem)
	{
		pNext=pItem->next;
		delete pItem;
		pItem=pNext;
	}
	
	pItems=0;
	nItems=0;
}

void Listbox::onRender()
{
	int i;
	ListboxItem *pItem=pItems;

	for(i=0;i<nTopItem;i++) pItem=pItem->next;
	for(i=0;i<GetNumRows();i++)
	{
		if(i>=nItems) return;

		if(nTopItem+i == nSelectedItem)
		{
			sprHighlight->Render(windowRect.x1,windowRect.y1+i*font->GetHeight());
			font->SetColor(texthilColor);
		}
		else
			font->SetColor(textColor);

		font->Render(windowRect.x1+3, windowRect.y1+i*font->GetHeight(), HGETEXT_LEFT, pItem->text);
		pItem=pItem->next;
	}
}

bool Listbox::MouseLButton(bool bDown)
{
	int nItem;

	if(bDown)
	{
		nItem=nTopItem+int(my)/int(font->GetHeight());
		if(nItem<nItems)
		{
			if(nSelectedItem != nItem)
			{
				nSelectedItem=nItem;
				executeOnLClick(nItem);
				executeOnSelChange(nItem);
			}			
			return true;
		}
	}
	return false;
}


bool Listbox::MouseWheel(int nNotches)
{
	nTopItem-=nNotches;
	if(nTopItem<0) nTopItem=0;
	if(nTopItem>GetNumItems()-GetNumRows()) nTopItem=GetNumItems()-GetNumRows();

	return true;
}

bool Listbox::KeyClick(int key, int chr)
{
	switch(key)
	{
		case HGEK_DOWN:
			if(nSelectedItem < nItems-1)
			{
				nSelectedItem++;
				if(nSelectedItem > nTopItem+GetNumRows()-1) nTopItem=nSelectedItem-GetNumRows()+1;
				return true;
			}
			break;

		case HGEK_UP:
			if(nSelectedItem > 0)
			{
				nSelectedItem--;
				if(nSelectedItem < nTopItem) nTopItem=nSelectedItem;
				return true;
			}
			break;
	}
	return false;
}

} // namespace GUI