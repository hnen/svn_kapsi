/*
 * Fraction.h
 *
 *  Created on: 8.5.2010
 *      Author: hth
 */

#ifndef FRACTION_H_
#define FRACTION_H_

#include "../DebugTools.h"

/**
 * Represents a fraction a/b
 */
class Fraction {

	int m_a, m_b;

	int p_gcd() const {
		return gcd(m_a, m_b);
	}

	void p_reduce() {
		int gcd = p_gcd();
		m_a /= gcd;
		m_b /= gcd;
	}

public:

	Fraction() : m_a(0), m_b(1) {}
	Fraction( int n ) : m_a(n), m_b(1) {}
	Fraction( int a, int b ) : m_a(a), m_b(b) { FASSERT( m_b != 0 ); p_reduce(); }

	operator float() { return (float)m_a / m_b; }
	operator double() { return (double)m_a / m_b; }
//	operator int() { if( m_b != 1 ) FWARNING( "Converting Fraction to int with precision loss." ); return m_a / m_b; }

	Fraction operator + ( const Fraction & a ) const { return Fraction( m_a * a.m_b + m_b * a.m_a, m_b * a.m_b ); }
	Fraction operator - ( const Fraction & a ) const { return Fraction( m_a * a.m_b - m_b * a.m_a, m_b * a.m_b ); }
	Fraction operator * ( const Fraction & a ) const { return Fraction( m_a * a.m_a, m_b * a.m_b ); }
	Fraction operator / ( const Fraction & a ) const { return Fraction( m_a * a.m_b, m_b * a.m_a ); }

	bool operator < ( const Fraction & a ) const { return ( (m_b > 0 && a.m_b > 0) || (m_b < 0 && a.m_b < 0) ) ? m_a * a.m_b < a.m_a * m_b : m_a * a.m_b > a.m_a * m_b;   }
	bool operator > ( const Fraction & a ) const { return ( (m_b > 0 && a.m_b > 0) || (m_b < 0 && a.m_b < 0) ) ? m_a * a.m_b > a.m_a * m_b : m_a * a.m_b < a.m_a * m_b;   }
	bool operator <= ( const Fraction & a ) const { return !(*this > a); }
	bool operator >= ( const Fraction & a ) const { return !(*this < a); }

	static int gcd( int a, int b ) {
		int t;
		while ( b ) {
			t = b;
			b = a % b;
			a = t;
		}
		return a;
	}

};

#endif /* FRACTION_H_ */
