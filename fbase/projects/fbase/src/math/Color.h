/*
 * Color.h
 *
 *  Created on: 13.5.2010
 *      Author: hth
 */

#ifndef COLOR_H_
#define COLOR_H_

#include "misc.h"
#include "../DebugTools.h"


class Color4 {
	float m_r, m_g, m_b, m_a;

	void p_setFromARGB8 ( unsigned int ARGB ) {
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
	Color4( unsigned int ARGB ) {
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

	unsigned int getARGB8() const {
		unsigned char a = (unsigned char)(CLAMP(m_a, 0.0f, 1.0f) * 255.f);
		unsigned char r = (unsigned char)(CLAMP(m_r, 0.0f, 1.0f) * 255.f);
		unsigned char g = (unsigned char)(CLAMP(m_g, 0.0f, 1.0f) * 255.f);
		unsigned char b = (unsigned char)(CLAMP(m_b, 0.0f, 1.0f) * 255.f);
		return (a << 24) | (r << 16) | (g << 8) | b;
	}

};


#endif /* COLOR_H_ */
