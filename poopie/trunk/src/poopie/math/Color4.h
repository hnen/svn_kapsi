
#ifndef COLOR_H_
#define COLOR_H_

#include "poopie/defs.h"

namespace poopie {

class Color4 {
	float m_r, m_g, m_b, m_a;

	void p_setFromARGB8 ( uint32 ARGB ) {
		m_b = (float)(ARGB & 0xFF) / 255.f;
		ARGB >>= 8;
		m_g = (float)(ARGB & 0xFF) / 255.f;
		ARGB >>= 8;
		m_r = (float)(ARGB & 0xFF) / 255.f;
		ARGB >>= 8;
		m_a = (float)(ARGB & 0xFF) / 255.f;
	}

public:

	Color4( float r, float g, float b, float a ) : m_r(r), m_g(g), m_b(b), m_a(a) {}
	Color4( uint32 ARGB ) {
		p_setFromARGB8( ARGB );
	}
	Color4() : m_r(0.0f), m_g(0.0f), m_b(0.0f), m_a(1.0f) {}

	Color4 operator + ( const Color4 & a ) const { return Color4( m_r + a.m_r, m_g + a.m_g, m_b + a.m_b, m_a + a.m_a ); }
	Color4 operator - ( const Color4 & a ) const { return Color4( m_r - a.m_r, m_g - a.m_g, m_b - a.m_b, m_a - a.m_a ); }
	Color4 operator * ( float a ) const { return Color4( m_r * a, m_g * a, m_b * a, m_a * a ); }
	Color4 operator / ( float a ) const { return Color4( m_r / a, m_g / a, m_b / a, m_a / a ); }

	Color4 getNormalized() {
		float max = MAX( MAX(m_r, m_g), m_b );
		Color4 ret = *this;
		if ( max > 1.0f ) {
			ret.m_r /= max;
			ret.m_g /= max;
			ret.m_b /= max;
		}
		return ret;
	}

	uint32 getARGB8() const {
		uint8 a = (uint8)(CLAMP(m_a, 0.0f, 1.0f) * 255.f);
		uint8 r = (uint8)(CLAMP(m_r, 0.0f, 1.0f) * 255.f);
		uint8 g = (uint8)(CLAMP(m_g, 0.0f, 1.0f) * 255.f);
		uint8 b = (uint8)(CLAMP(m_b, 0.0f, 1.0f) * 255.f);
		return (a << 24) | (r << 16) | (g << 8) | b;
	}

};

}

#endif /* COLOR_H_ */



