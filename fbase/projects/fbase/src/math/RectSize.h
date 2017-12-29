/*
 * RectSize.h
 *
 *  Created on: 8.5.2010
 *      Author: hth
 */

#ifndef RECTSIZE_H_
#define RECTSIZE_H_

#include <math.h>

/**
 * Width/height pair.
 */
class RectSize {

	int m_w, m_h;

public:

	RectSize() : m_w(0), m_h(0) {}
	RectSize( int w, int h ) : m_w(w), m_h(h) {}

	int getWidth() const { return m_w; }
	int getHeight() const { return m_h; }

	RectSize fitRect( RectSize rect ) const {
		return fitRect( Fraction(rect.getWidth(), rect.getHeight()) );
	}

	RectSize fitRect( Fraction aspect ) const {
		Fraction thisAspect = Fraction( m_w, m_h );
		if ( thisAspect < aspect ) {
			// Wider than this
			return RectSize( m_w, (int)floor( (float)(Fraction(m_w) / aspect) ) );
		} else if ( thisAspect > aspect ) {
			// Narrower than this
			return RectSize( (int)floor ( (float)(Fraction(m_h) * aspect) ), m_h );
		} else {
			// Fits perfectly
			return *this;
		}
	}

};

#endif /* RECTSIZE_H_ */
