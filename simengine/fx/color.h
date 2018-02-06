#pragma once

namespace Fx
{

inline void ColorClamp(float &x) { if(x<0.0f) x=0.0f; if(x>1.0f) x=1.0f; }

class ColorRGB
{
public:
	float		r,g,b,a;

	ColorRGB(float _r, float _g, float _b, float _a) { r=_r; g=_g; b=_b; a=_a; }
	ColorRGB() { r=g=b=a=0; }

	ColorRGB		operator-  (const ColorRGB &c) const { return ColorRGB(r-c.r, g-c.g, b-c.b, a-c.a); }
	ColorRGB		operator+  (const ColorRGB &c) const { return ColorRGB(r+c.r, g+c.g, b+c.b, a+c.a); }
	ColorRGB		operator*  (const ColorRGB &c) const { return ColorRGB(r*c.r, g*c.g, b*c.b, a*c.a); }
	ColorRGB&	operator-= (const ColorRGB &c)		{ r-=c.r; g-=c.g; b-=c.b; a-=c.a; return *this;   }
	ColorRGB&	operator+= (const ColorRGB &c)		{ r+=c.r; g+=c.g; b+=c.b; a+=c.a; return *this;   }
	bool			operator== (const ColorRGB &c) const { return (r==c.r && g==c.g && b==c.b && a==c.a);  }
	bool			operator!= (const ColorRGB &c) const { return (r!=c.r || g!=c.g || b!=c.b || a!=c.a);  }

	ColorRGB		operator/  (const float scalar) const { return ColorRGB(r/scalar, g/scalar, b/scalar, a/scalar); }
	ColorRGB		operator*  (const float scalar) const { return ColorRGB(r*scalar, g*scalar, b*scalar, a*scalar); }
	ColorRGB&		operator*= (const float scalar)		  { r*=scalar; g*=scalar; b*=scalar; a*=scalar; return *this;   }

	void			Clamp() { ColorClamp(r); ColorClamp(g); ColorClamp(b); ColorClamp(a); }
#ifdef FUCK_THIS
	ColorRGB(DWORD col) { SetHWColor(col); }
	void			SetHWColor(DWORD col) {	a = (col>>24)/255.0f; r = ((col>>16) & 0xFF)/255.0f; g = ((col>>8) & 0xFF)/255.0f; b = (col & 0xFF)/255.0f;	}
	DWORD			GetHWColor() const { return (DWORD(a*255.0f)<<24) + (DWORD(r*255.0f)<<16) + (DWORD(g*255.0f)<<8) + DWORD(b*255.0f);	}
#endif
};

inline ColorRGB operator* (const float sc, const ColorRGB &c) { return c*sc; }


class ColorHSV
{
public:
	float		h,s,v,a;

	ColorHSV(float _h, float _s, float _v, float _a) { h=_h; s=_s; v=_v; a=_a; }

	ColorHSV() { h=s=v=a=0; }

	ColorHSV		operator-  (const ColorHSV &c) const { return ColorHSV(h-c.h, s-c.s, v-c.v, a-c.a); }
	ColorHSV		operator+  (const ColorHSV &c) const { return ColorHSV(h+c.h, s+c.s, v+c.v, a+c.a); }
	ColorHSV		operator*  (const ColorHSV &c) const { return ColorHSV(h*c.h, s*c.s, v*c.v, a*c.a); }
	ColorHSV&	operator-= (const ColorHSV &c)		{ h-=c.h; s-=c.s; v-=c.v; a-=c.a; return *this;   }
	ColorHSV&	operator+= (const ColorHSV &c)		{ h+=c.h; s+=c.s; v+=c.v; a+=c.a; return *this;   }
	bool			operator== (const ColorHSV &c) const { return (h==c.h && s==c.s && v==c.v && a==c.a);  }
	bool			operator!= (const ColorHSV &c) const { return (h!=c.h || s!=c.s || v!=c.v || a!=c.a);  }

	ColorHSV		operator/  (const float scalar) const { return ColorHSV(h/scalar, s/scalar, v/scalar, a/scalar); }
	ColorHSV		operator*  (const float scalar) const { return ColorHSV(h*scalar, s*scalar, v*scalar, a*scalar); }
	ColorHSV&	operator*= (const float scalar)		  { h*=scalar; s*=scalar; v*=scalar; a*=scalar; return *this;   }

	void			Clamp() { ColorClamp(h); ColorClamp(s); ColorClamp(v); ColorClamp(a); }
#ifdef FUCK_THIS
	ColorHSV(int32_t col) { SetHWColor(col); }
	void			SetHWColor(DWORD col);
	DWORD			GetHWColor() const;
#endif
};

inline ColorHSV operator* (const float sc, const ColorHSV &c) { return c*sc; }

}
