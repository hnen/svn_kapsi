/*
 * Map.h
 *
 *  Created on: 5.7.2010
 *      Author: hth
 *
 */

#ifndef MAP_H_
#define MAP_H_

#include <stdlib.h>

//#define P_MAP_PRIME 16063
#define P_MAP_PRIME 7

typedef void* voidptr;

template<class A>
void p_objAllocate() { return new A(); };

class PMapImpl {
	int m_keySize;
	int m_dataSize;
//	p_objAllocatorPtr m_objAllocator;

	struct HashSlot {
		HashSlot() : elements(0), allocSize(0), data(0) {}

		struct MapEntry {
			MapEntry() : key(0), value(0) {}
			~MapEntry() {
				if ( key ) {
					free( key );
				}
				if ( value ) {
					free( value );
				}
			}

			void * key;
			void * value;
		};
		short elements;
		short allocSize;
		MapEntry * data;
	};

	HashSlot m_hashData[P_MAP_PRIME];

	HashSlot * p_getHashSlot( const void * key );
	HashSlot::MapEntry * p_getMapEntry( const void * key );

public:
	PMapImpl( int keySize, int dataSize );
	bool doesKeyExist( const void * key );
	void setElement( const void * key, const void * data );
	void * getElement( const void * key );
};

template<class A, class B>
class Map
{
public:
	inline Map () : impl(sizeof(A), sizeof(B) ) { };

	inline B & operator [] ( const A & key ) {
		return *reinterpret_cast<B*>( impl.getElement( &key ) );
	};

	bool exists ( const A & a ) { return impl.doesKeyExist( &a ); }

private:
	PMapImpl impl;
};



#endif /* MAP_H_ */
