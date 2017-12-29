
#ifndef __BYTESTREAM
#define __BYTESTREAM

#include "Array.h"

class ByteStream {
	Array<char> m_data;
public:

	ByteStream() {};
	ByteStream( const ByteStream & a ) : m_data(a.m_data) {};

	void add( void * dataPtr, int dataSize ) {
		char * bytePtr = (char*)dataPtr;
		for ( int l = 0; l < dataSize; l++ ) {
			m_data.add( bytePtr[l] );
		}
	}

	const char * getDataPtr() const {
		return m_data.getDataPtr();
	}

};


#endif
