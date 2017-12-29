
#ifndef POOPIE_DYNAMICARRAY_H_
#define POOPIE_DYNAMICARRAY_H_

#include "poopie/defs.h"
#include "poopie/base/Array.h"

namespace poopie {

	template<class A>
	class DynamicArray {
	public:

		DynamicArray( unsigned int allocatedSize = 16 ) : m_container(allocatedSize), m_elCount(0) {}
		
		DynamicArray( const Array<A> & initData ) : m_container( initData ) {
			m_elCount = m_container.getLength();
		}

		void append( const A & el ) {
			if ( m_elCount >= m_container.getLength() ) {
				m_container.resize( m_container.getLength() * 2 );
			}
			m_container[m_elCount] = el;
			m_elCount++;
		}

		void remove( int i ) {
			m_elCount--;
			for( int j = i; j < m_elCount; j++ ) {
				m_container[j] = m_container[j+1];
			}
			
		}

		int find( const A & el ) {
			for ( int i = 0; i < m_elCount; i++ ) {
				if ( m_container[i] == el ) {
					return i;
				}
			}
			return -1;
		}

		void clear() {
			m_elCount = 0;
		}

		A & operator [] ( unsigned int id ) {
			POOP_ASSERT( id >= 0 && id < m_elCount );
			return m_container[id];
		};

		Array<A> ToArray() {
			Array<A> ret( m_elCount );
			for( unsigned int i = 0; i < m_elCount; i++ ) {
				ret[i] = m_container[i];
			}
			return ret;
		}

		A * getData() {
			return m_container.getData();
		}
		
		int getLength() {
			return m_elCount;
		}

	private:

		unsigned int m_elCount;
		Array<A> m_container;

	};


}

#endif

