
#ifndef POOPIE_STRING_H_
#define POOPIE_STRING_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "poopie/base/Hash.h"

namespace poopie {

class String {

	char * m_data;

	void p_free() {
		if ( m_data ) {
			delete [] m_data;
		}
		m_data = 0;
	}

	void p_setCStr( const char * cstr ) {
		p_free();
		m_data = new char [strlen(cstr) + 1];
		strcpy( m_data, cstr );
	}

	void p_setFromFloat( float f ) {
		char fstr[13];
		sprintf( fstr, "%.2f", f );
		p_setCStr( fstr );
	}

	void p_setFromInteger( int i ) {
		char intstr[13];
		sprintf( intstr, "%d", i );
		p_setCStr( intstr );
	}

	void p_copy( const String & str ) {
		p_setCStr( str.getCStr() );
	}

	void p_add( const String & a ) {
		char * n = new char[a.getLength() + this->getLength() + 1];
		strcpy( n, getCStr() );
		strcpy( &n[this->getLength()], a.getCStr() );
		p_free();
		m_data = n;
	}


public:

	String() : m_data(NULL) { p_setCStr( "" ); };
	String( int integer ) : m_data(NULL) { p_setFromInteger(integer); };
	String( unsigned int integer ) : m_data(NULL) { p_setFromInteger(integer); };
	String( float floating ) : m_data(NULL) { p_setFromFloat(floating); };
	String( const char * cstr ) : m_data(NULL) { p_setCStr( cstr ); }
	String( const String & str ) : m_data(NULL) { p_copy( str ); }
	~String() { p_free(); }

	const char * getCStr() const {
		return m_data ? m_data : "(NULL)";
	}

	operator const char * () const {
		return getCStr();
	}
	
	String & operator = ( const char * cstr ) {
		p_setCStr( cstr );
		return *this;
	}

	String & operator = ( const String & str ) {
		p_setCStr( str.getCStr() );
		return *this;
	}

	String lowerCase() const {
		String ret = *this;
		for ( int i = 0; i < this->getLength(); i++ ) {
			char c = ret.m_data[i];
			if ( c >= 'A' && c <= 'Z' ) {
				c = c - 'A' + 'a';
			}
			ret.m_data[i] = c;
		}
		return ret;
	}

	//bool operator == ( const String & a ) {
	bool equals( const char * a ) {
		return strcmp( getCStr(), a ) == 0;
	}

	bool equals( const String & a ) {
		return strcmp( getCStr(), a.getCStr() ) == 0;
	}

	int getLength() const {
		return strlen( m_data );
	}

	String toLowerCase() const {
		String n = *this;
		for( int i = 0; i < n.getLength(); i++ ) {
			char c = n.m_data[i];
			if ( c >= 'A' && c <= 'Z' ) {
				n.m_data[i] = c - 'A' + 'a';
			}
		}
		return n;
	}

	String & operator += ( const String & a ) {
		p_add( a );
		return *this;
	};

	String operator + ( const String & a ) const {
		String ret(*this);
		ret += a;
		return ret;
	};

	String & operator += ( int a ) {
		p_add( String( a ) );
		return *this;
	};

	String & operator += ( unsigned int a ) {
		p_add( String( a ) );
		return *this;
	};

	String operator + ( int a ) const {
		String ret(*this);
		ret += a;
		return ret;
	};

	String operator + ( unsigned int a ) const {
		String ret(*this);
		ret += a;
		return ret;
	};

	String & operator += ( float a ) {
		p_add( String( a ) );
		return *this;
	};

	String operator + ( float a ) const {
		String ret(*this);
		ret += a;
		return ret;
	};
	
	friend class String;

};

	template<> unsigned int poopie_hash<String>( const String & s );
}

#endif
