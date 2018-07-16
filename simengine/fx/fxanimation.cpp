#include "fxobjects.h"
#include "rendercontext.h"

namespace Fx
{

///////////////////////////////////////////////////////
// Common drawing tools

///////////////////////////////////////////////////////

/*
void drawPoints(RenderContext* rc,const vec2f *p,int n)
{
	if(n>1)
		rc->Gfx_RenderLine(p[n-1][0],p[n-1][1],p[0][0],p[0][1]);
	for(int i=1;i<n;i++)
		rc->Gfx_RenderLine(p[i-1][0],p[i-1][1],p[i][0],p[i][1]);
}


void drawPoints(RenderContext* rc, const vec3 *p, int n)
{
	if(n>1)
		rc->Gfx_RenderLine(p[n-1][0],p[n-1][1],p[0][0],p[0][1]);
	for(int i=1;i<n;i++)
		rc->Gfx_RenderLine(p[i-1][0],p[i-1][1],p[i][0],p[i][1]);
}*/

void boxCorners(const Pose &p,float width,float height, Pose::vec *result)
{
	result[0] = p.coords(-width, height);
	result[1] = p.coords( width, height);
	result[2] = p.coords( width,-height);
	result[3] = p.coords(-width,-height);
}

FxAnimation2::FxAnimation2(FxManager *manager, FxTextureId tex, float x, float y, float w, float h)
	:Entity()
	,fps(1.0f)
	,mode(AnimationRepeat)
	,drawRect(false)
	,sprite(tex,0,0,x,y,w,h)
	,manager(manager)
{
}

FxAnimation2::FxAnimation2(const FxAnimation2 &effect)
	:Entity(effect)
	,fps(effect.fps)
	,frames(effect.frames.begin(),effect.frames.end())
	,mode(effect.mode)
	,current(effect.current)
	,run(effect.run)
	,width(effect.width),height(effect.height)
	,cropWidth(effect.cropWidth),cropHeight(effect.cropHeight),crop(effect.crop),drawRect(false),tiled(effect.tiled)
	,sprite(effect.sprite)
{
	manager = effect.manager;
	tiledWidth = effect.tiledWidth;
	tiledHeight = effect.tiledHeight;
}

FxAnimation2::~FxAnimation2()
{
	// TODO: Release texture data
}

int FxAnimation2::addFrame(const Rect &rect)
{
	frames.push_back(rect);
	return frames.size();
}

bool FxAnimation2::valid()const
{
	return !frames.empty();
}

void FxAnimation2::start(AnimationMode mode)
{	
	if( mode != AnimationNoChange )
		this->mode = mode;
	run = true;
}

void FxAnimation2::rewind()
{
	current=0;
}

void FxAnimation2::stop(bool immediate)
{
	current = 0;
	run = false;
}

bool FxAnimation2::isTiled() const
{
	return tiled;
}

void FxAnimation2::xTile(float length)
{
	if(length != width && length > 0)
	{
		tiledWidth = length;
		tiledHeight = height;
		tiled = true;
	}
}

void FxAnimation2::addBlendMode(int mode)
{
	if(!valid())return;
	sprite.setBlendMode(mode|sprite.getBlendMode());
}

void FxAnimation2::setBlendMode(int mode)
{
	if(!valid())
		return;
	sprite.setBlendMode(mode);
}

float FxAnimation2::duration()const
{
	return fps>0?frames.size()/fps:0;
}

void FxAnimation2::setSize(float w,float h,bool mid)
{
	width=w;
	height=h;
	if(!valid())
		return;
	if(mid)
		sprite.setHotSpot(w/2,h/2);
	else
		sprite.setHotSpot(0,0);
}
// here we just advance current frame and change state
void FxAnimation2::update(float dt)
{	
	if(!run)return;
	if(!valid())return;

	dt=fps*dt;
	switch(mode)
	{
	case AnimationHold:break;
	case AnimationOnce:
		if(run && (current+=dt)>=frames.size())
			stop(true);
	break;
	case AnimationRepeatGroup:
	case AnimationRepeat:
		if(run && (current+=dt)>=(frames.size()-1))
			current=0;
	break;

//	case Bounce:break;
	}
}

float FxAnimation2::getWidth()
{
	return scale * width;
}

float FxAnimation2::getHeight()
{
	return scale * height;
}

void FxAnimation2::render(RenderContext* context, const Pose& base)
{
	if(!valid() || !visible)
		return;

	if (current >= frames.size())
		current = frames.size() - 1;

	const Rect& rc = frames[current];

	Pose p = base*getPose();
	if(!tiled)
	{
		//if(crop)
		{
			sprite.setTextureRect(rc.x1,rc.y1, rc.width()*cropWidth,rc.height()*cropHeight);
			sprite.setHotSpot(rc.width()*cropWidth,rc.height()*cropHeight);
			drawSprite(context, sprite, p,
					cropWidth*width*0.5*scale, cropHeight*height*0.5*scale);
		}
		//else
		//{			
		//	sprite.SetTextureRect(rc.x,rc.y,rc.w,rc.h);			
		//	sprite.SetHotSpot(rc.w/2,rc.h/2);
		//	drawSprite(rc,&sprite,p,width*0.5*scale,height*0.5*scale,drawRect);
		//}
	}
	else
	{
		float xSegment = getWidth();	
		float ySegment = getHeight();

		for(float y = 0; y < tiledHeight; y += ySegment)
		{
			for(float x = 0; x < tiledWidth; x += xSegment)
			{			
				//parts[1]->crop=true;
			
				float cropWidth=(tiledWidth - x < xSegment)?(tiledWidth - x) / xSegment : 1.0;
				float cropHeight=(tiledHeight - x < xSegment)?(tiledHeight - x) / ySegment : 1.0;
				//parts[1]->setPose(p*Pose(l,0,0));
				//Pose bp=p*Pose(l+segment/2,0,0);
				Pose bpose=p * Pose( x + cropWidth * xSegment/2, y + cropHeight * ySegment/2,0);
				//parts[1]->render(bp);

				//bool crop=(tiledWidth - x < xSegment);

				//if(crop)
				{
					sprite.setTextureRect(rc.x1,rc.y1,rc.width()*cropWidth,rc.height()*cropHeight);
					sprite.setHotSpot(rc.width()*cropWidth,rc.height()*cropHeight);
					drawSprite(context, sprite, bpose, cropWidth*width*0.5*scale,cropHeight*height*0.5*scale);
				}
				//else
				//{			
				//	sprite.SetTextureRect(rc.x,rc.y,rc.w,rc.h);			
				//	sprite.SetHotSpot(rc.w/2,rc.h/2);
				//	drawSprite(context,&sprite,bp,width*0.5*scale,height*0.5*scale,drawRect);
				//}
			}
		}
	}
}

}