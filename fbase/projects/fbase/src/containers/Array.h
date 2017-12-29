/*
 * Array.h
 *
 *  Created on: 4.7.2010
 *      Author: hth
 *
 * Smart array implementation.
 *
 */

#ifndef ARRAY_H_
#define ARRAY_H_

#include <stdio.h>
#include <string.h>
//#include "Map.h"

#include <map>
#include <vector>

#include "../DebugTools.h"

#define P_ARRAY_INITIAL_SIZE 1

class TArray;

typedef std::vector<void*> PtrList;
typedef std::vector<void*>::iterator PtrListIter;
typedef std::map<void*, PtrList> AllocMap;

extern AllocMap p_arrayAllocs;

//template <class T> class Array<T>;

//template <class T>
class TArray {
protected:
	bool m_lockPointer;
	int m_allocSize;
	int m_length;
	int m_elementSize;

	void * m_data;

	void p_addToAllocMap() {
		PtrList &l = p_arrayAllocs[m_data];
		l.push_back( this );
	}

	void p_removeFromAllocMap() {
		PtrList &l = p_arrayAllocs[m_data];
		for( unsigned int n = 0; n < l.size(); n++ ) {
			if ( l[n] == this ) {
				l.erase( l.begin() + n );
				return;
			}
		}
		// Only allowed if m_lockPointer == true
		FASSERT( m_lockPointer );
	}

	void p_free() {
		if ( m_data ) {
			p_removeFromAllocMap();
			if ( p_arrayAllocs[m_data].empty() ) {
				if ( !m_lockPointer ) {
					//delete [] m_data;
					free( m_data );
				}
			}
			m_data = 0;
			m_allocSize = m_length = 0;
		}
	}
	
	void p_copyFrom( const TArray & another ) {
		p_free();
		FASSERT( another.m_elementSize == 0 || m_elementSize == 0 || m_elementSize == another.m_elementSize );
		m_data = another.m_data;
		m_lockPointer = another.m_lockPointer;
		//m_elementSize = another.m_elementSize;
		m_allocSize = another.m_allocSize;
		m_length = another.m_length;

//		p_arrayAllocs[m_data]++;
		p_addToAllocMap();
	}

	void p_initFromOutsidePointer( void * data, int length ) {
		m_data = data;
		m_length = m_allocSize = length;
		m_lockPointer = true;
	}

	void p_init( int allocSize, int length ) {
		if ( m_data ) {
			p_free();
		}
		if ( allocSize > 0 ) {
			//m_data = (T*)malloc( allocSize * sizeof(T) );
			m_data = malloc( allocSize * m_elementSize );
			p_addToAllocMap();
		}
		m_allocSize = allocSize;
		m_length = length;
		m_lockPointer = false;

		//p_arrayAllocs[m_data] = 1;
	}

	void p_dataPointerChanged( void * newDataPtr ) {
		p_removeFromAllocMap();
		m_data = newDataPtr;
		p_addToAllocMap();
	}

	void p_doubleSize() {
		//T * newData = (T*)malloc( sizeof(T) * m_allocSize * 2);
		void * newData = malloc( m_elementSize * m_allocSize * 2);
		memcpy( newData, m_data, m_length * m_elementSize );

		PtrList &l = p_arrayAllocs[m_data];
		for ( unsigned int n = 0; n < l.size(); n++ ) {
			//Array<T> * a = (Array<T>*)l[n];
			TArray * a = (TArray*)l[n];
			a->p_dataPointerChanged( newData );
		}

		m_data = newData;
		m_allocSize *= 2;
	}

public:

	TArray( const TArray & a ) : m_elementSize(0), m_data(0) { p_copyFrom(a); };
	TArray( int elementSize ) : m_elementSize(elementSize), m_data(0) { p_init(0, 0); };
	TArray( int elementSize, int length ) : m_elementSize(elementSize), m_data(0) { p_init(length, length); }
	TArray( int elementSize, void * pointer, int size ) : m_elementSize(elementSize) { p_initFromOutsidePointer( pointer, size ); }
	~TArray() { p_free(); }

	TArray cloneRaw() {
		TArray ret( m_elementSize, m_length );
		memcpy( ret.getRawDataPtr(), m_data, m_elementSize * m_length );
		ret.m_length = m_length;
		return ret;
	}

	int getLength() const { return m_length; }

	void * getRawDataPtr() { return m_data; }

	TArray & operator = ( const TArray & a ) {
		p_copyFrom( a );
		return *this;
	}										

};

template <class T>
class Array : public TArray {

public:
	Array( const Array & a ) : TArray( a ) {};
	Array( const TArray & a ) : TArray( a ) {};
	Array( ) : TArray(sizeof(T)) {};
	Array( int length ) : TArray(sizeof(T), length) {};
	Array( void * pointer, int size ) : TArray( sizeof(T), pointer, size ) {};
	~Array() { p_free(); }

	T & operator[](int i) { return ((T*)m_data)[i]; }

	void add( const T & element ) {
		FASSERT( !m_lockPointer );
		if ( !m_lockPointer ) {
			if ( m_allocSize == 0 ) {
				p_init( P_ARRAY_INITIAL_SIZE, 0 );
			}
			if ( m_allocSize == m_length ) {
				p_doubleSize();
			}
			((T*)m_data)[m_length] = element;
			m_length++;
		}
	}

	void copy( const Array<T> & src ) {
		copy( 0, src.getDataPtr(), src.getLength() );
	}

	void copy( int dest, const void * ptr, int numElements ) {
		int end = dest + numElements * sizeof(T);
		while( end >= m_allocSize ) {
			p_doubleSize();
		}
		memcpy( &((T*)m_data)[dest], ptr, numElements * sizeof(T) );
		if ( end > m_length ) {
			m_length = end;
		}
	}

	int getLength() const { return m_length; }

	Array & operator = ( const Array & a ) {
		p_copyFrom( a );
		return *this;
	}										

	Array & operator = ( const TArray & a ) {
		p_copyFrom( a );
		return *this;
	}

	const T * getDataPtr() const { return (T*)m_data; }

	Array<T> clone() {
		Array<T> ret( m_length );
		memcpy( ret.getRawDataPtr(), m_data, m_elementSize * m_length );
		ret.m_length = m_length;
		return ret;
	}

};

#endif /* ARRAY_H_ */
