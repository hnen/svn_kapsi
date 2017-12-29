/*
 * Map.cpp
 *
 *  Created on: 5.7.2010
 *      Author: hth
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Map.h"

inline bool valuesEqual( const void * a, const void * b, int dataSize ) {
	char * ab = (char*)a;
	char * bb = (char*)b;
	int h = 0;
	for( int l = 0 ; l < dataSize; l++ ) {
		if ( ab[l] != bb[l] ) {
			return false;
		}
	}
	return true;
}

inline int computeHashValue( const void * valData, int dataSize ) {
	char * byteData = (char*)valData;
	int h = 0;
	for( int l = 0 ; l < dataSize; l++ ) {
		h += byteData[l];
		h %= P_MAP_PRIME;
	}
	return h;
}
PMapImpl::PMapImpl( int keySize, int dataSize ) :
		m_keySize(keySize), m_dataSize(dataSize) {
}

PMapImpl::HashSlot * PMapImpl::p_getHashSlot( const void * key ) {
	int v = computeHashValue( key, m_keySize );
	return &m_hashData[v];
}

PMapImpl::HashSlot::MapEntry * PMapImpl::p_getMapEntry( const void * key ) {
	HashSlot * h = p_getHashSlot( key );
	if ( !h->data ) {
		return 0;
	}
	for( int l = 0 ; l < h->elements; l++ ) {
		if ( valuesEqual( h->data[l].key, key, m_keySize ) ) {
			return &h->data[l];
		}
	}
	return 0;
}

void PMapImpl::setElement( const void * key, const void * data ) {
	HashSlot::MapEntry * me = p_getMapEntry( key );

	if ( !me ) {
		HashSlot * slot = p_getHashSlot( key );
		if ( slot->elements >= slot->allocSize ) {
			int osize = slot->allocSize;
			int nsize = slot->allocSize == 0 ? 1 : slot->allocSize * 2;
			HashSlot::MapEntry * nData = new HashSlot::MapEntry[nsize];
			if( slot->data ) {
				for( int l = 0; l < osize; l++ ) {
					nData[l] = slot->data[l];
				}
				delete [] slot->data;
			}
			slot->data = nData;
		}
		me = &slot->data[slot->elements];
		slot->elements++;
	}

	if ( me->value ) {
		free( me->value );
		me->value = 0;
	}
	if ( !me->key ) {
		me->key = malloc( m_keySize );
		memcpy( me->key, key, m_keySize );
	}

	me->value = malloc( m_dataSize );
	memcpy( me->value, data, m_dataSize );
}

bool PMapImpl::doesKeyExist( const void * key ) {
	return p_getMapEntry(key) != 0;
}

void * PMapImpl::getElement( const void * key ) {
	HashSlot::MapEntry * me = p_getMapEntry(key);

	if ( !me ) {
		void * data = malloc( m_dataSize );
		setElement( key, data );
		free(data);
		me = p_getMapEntry(key);
  	}
	return me->value;

}



