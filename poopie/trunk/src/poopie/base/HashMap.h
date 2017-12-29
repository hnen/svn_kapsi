
#ifndef __POOPIE_HASHMAP
#define __POOPIE_HASHMAP

#include <string.h>
#include "poopie/defs.h"
#include "poopie/base/Hash.h"

#define POOPIE_HASHMAP_EMPTY 0
#define POOPIE_HASHMAP_DELETED 1
#define POOPIE_HASHMAP_OCCUPIED 2

namespace poopie {

	template<class K, class V>
	class HashMap {
	public:

		struct KeyValuePair {
			K * key;
			V * value;
		};

		// protip: Initialize with 4 * expectedElementCount / 3 !
		HashMap<K,V>( int initialSize = 1024 ) {

			POOP_ASSERT( sizeof(K) % 4 == 0, "Key must be aligned by 4 bytes!" );

			m_values = new V[initialSize];
			m_keys = new K[initialSize];
			m_occupied = new uint8[initialSize];

			memset( m_occupied, POOPIE_HASHMAP_EMPTY, initialSize );

			m_cntSize = initialSize;
			m_elCount = m_occupiedOrDeletedCount = 0;
			m_managePtr = true;
		}

		HashMap<K,V>( const HashMap<K,V> & another ) : m_values(0), m_keys(0), m_occupied(0) {
			copyFrom( another );
			m_managePtr = true;
		}
		HashMap<K,V> & operator = ( const HashMap<K,V> & another ) {
			copyFrom( another );
			return *this;
		}

		~HashMap<K,V>() {
			if ( m_managePtr ) {
				delete [] m_values;
				delete [] m_keys;
				delete [] m_occupied;
			}
		}

		bool contains( const K & key ) const {
			return findValueSlotByKey( key ) != -1;
		}

		bool add( const K & key, const V & element ) {
			if ( contains(key) ) {
				//POOP_ERROR( "Already contains key!" );
				return false;
			}

			// Load factor is >75% => resize
			if ( 4 * m_occupiedOrDeletedCount > 3 * m_cntSize ) {
				//resize( 2 * 4 * m_elCount / 3 ); // Rehash to a hashmap with load factor of ~37,5%
				resize( m_cntSize * 2 );
			}

			int slot = getUnoccupiedSlot( key );
			m_keys[slot] = key;
			m_values[slot] = element;
			m_occupied[slot] = POOPIE_HASHMAP_OCCUPIED;
			m_elCount++;
			m_occupiedOrDeletedCount++;
			return true;
		}

		V & operator [] ( const K & key ) {
			if ( !contains(key) ) {
				add( key, V() );
			} 
			return get(key);
			
		}

		bool remove( const K & key ) {
			int slot = findValueSlotByKey( key );
			if ( slot == -1 ) {
				return false;
			}
			m_occupied[slot] = POOPIE_HASHMAP_DELETED;
			m_elCount--;
			return true;
		}

		V & get( const K & key ) {
			int v = findValueSlotByKey( key );
			if ( v != -1 ) {
				return m_values[v];
			}
			POOP_ERROR( "Didn't contain key!" );
			return *((V*)0);
		}

		void resetIteration() {
			m_iter = 0;
		}
		bool iterateNext( KeyValuePair * next ) {
			while( m_iter < m_cntSize && m_occupied[m_iter] != POOPIE_HASHMAP_OCCUPIED ) {
				m_iter++;
			}
			if ( m_iter < m_cntSize ) {
				KeyValuePair N;
				N.key = &m_keys[m_iter];
				N.value = &m_values[m_iter];
				*next = N;
				m_iter++;
				return true;
			} else {
				m_iter++;
				return false;
			}
		}

		void clear() {
			for ( int i = 0; i < m_cntSize; i++ ) {
				m_occupied[i] = POOPIE_HASHMAP_EMPTY;
			}
		}

		int getLength() {
			return m_elCount;
		}

	private:

		void copyFrom( const HashMap<K,V> & another ) {
			if ( m_managePtr ) {
				delete [] m_values;
				delete [] m_keys;
				delete [] m_occupied;
			}
			m_managePtr = true;

			m_cntSize = another.m_cntSize;
			m_occupied = new uint8[m_cntSize];
			m_keys = new K[m_cntSize];
			m_values = new V[m_cntSize];

			memcpy( m_occupied, another.m_occupied, m_cntSize );
			for( unsigned int i = 0; i < m_cntSize; i++ ) {
				m_keys[i] = another.m_keys[i];
				m_values[i] = another.m_values[i];
			}

			m_elCount = another.m_elCount;
			m_occupiedOrDeletedCount = another.m_occupiedOrDeletedCount;
		}

		void resize( int nsize ) {
			HashMap<K,V> n( nsize );
			n.m_managePtr = false;
			for( uint i = 0; i < m_cntSize; i++ ) {
				if ( m_occupied[i] == POOPIE_HASHMAP_OCCUPIED ) {
					n.add( m_keys[i], m_values[i] );
				}
			}

			if ( m_managePtr ) {
				delete [] m_values;
				delete [] m_keys;
				delete [] m_occupied;
			}

			m_values = n.m_values;
			m_keys = n.m_keys;
			m_occupied = n.m_occupied;
			m_occupiedOrDeletedCount = n.m_occupiedOrDeletedCount;
			m_cntSize = nsize;			
		}

		int getUnoccupiedSlot( const K & key ) {
			uint h = poopie_hash(key) % m_cntSize;
			while( m_occupied[h%m_cntSize] == POOPIE_HASHMAP_OCCUPIED ) {
				h++;
				h %= m_cntSize;
			}
			return h;
		}
		int findValueSlotByKey( const K & key ) const {
			uint h = poopie_hash(key) % m_cntSize;
			bool found;
			int irs = m_cntSize;;
			while( irs > 0 && (found = (m_occupied[h] != POOPIE_HASHMAP_EMPTY)) && !(m_occupied[h] != POOPIE_HASHMAP_DELETED && m_keys[h] == key) ) {
				h++;
				irs--;
				h %= m_cntSize;
			}
			if ( irs == 0 ) return -1;
			if ( !found ) return -1;
			return h;
		}


		bool m_managePtr;
		uint8 * m_occupied;
		K * m_keys;
		V * m_values;
		uint m_cntSize;
		uint m_elCount;
		uint m_occupiedOrDeletedCount;
		
		uint m_iter;
	};

	template<class A>
	class HashSet {
	public:

		HashSet() {}
		HashSet( int size ) : m_container(size) {}

		void add( const A & a ) {
			m_container[a] = 1;
		}
		bool contains( const A & a ) const {
			return m_container.contains( a );
		}
		void remove( const A & a ) {
			m_container.remove( a );
		}

		void resetIteration() {
			m_container.resetIteration();
		}

		int getLength() {
			return m_container.getLength();
		}

		bool iterateNext( A * next ) {
			HashMap<A,char>::KeyValuePair kvp;
			if ( m_container.iterateNext( &kvp ) ) {
				*next = *kvp.key;
				return true;
			} else {
				return false;
			}
		}

		void clear() {
			m_container.clear();
		}

	private:

		HashMap<A,char> m_container;

	};


}

#endif

