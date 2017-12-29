
#ifndef POOPIE_ARRAY_H_
#define POOPIE_ARRAY_H_

#include "poopie/defs.h"

#include <string.h>

namespace poopie {

	template<class A>
	class Array {
	public:

		Array() : m_ptr(0), m_length(0), m_managePtr(true) {
		};
		
		Array( A * d, uint length ) : m_ptr(d), m_length(length), m_managePtr(false) {}

		Array( uint length ) : m_ptr(0), m_managePtr(true) {
			resize( length );
		}

		Array( const Array<A> & another ) : m_ptr(0), m_length(0), m_managePtr(true) {
			copyFrom( another );
		}

		~Array() {
			if ( m_managePtr ) {
				delete [] m_ptr;
			}
		}

		Array<A> & operator = ( const Array<A> & another ) {
			copyFrom( another );
			return *this;
		}


		A & operator [] ( unsigned int id ) {
			POOP_ASSERT( m_ptr && id >= 0 && id < m_length );
			return m_ptr[id];
		};

		void fill( A element ) {
			for ( int i = 0; i < getLength(); i++ ) {
				(*this)[i] = element;
			}
		}

		void resize( uint nsize ) {
			A * nptr = (nsize>0)? nptr = new A[nsize] : NULL;
			if ( m_ptr ) {
				int s = getSizeInBytes();
				int d = sizeof(A) * nsize;
				if ( s>d ) s = d;
				memcpy( nptr, m_ptr, s );
				//memset( nptr, 0, m_length * sizeof(A) );
				delete [] m_ptr;
			}
			m_ptr = nptr;
			m_length = nsize;
		}


		uint getSizeInBytes() const {
			//POOP_ASSERT( m_ptr );
			return getLength() * sizeof(A);
		}
		uint getLength() const {
			//POOP_ASSERT( m_ptr );
			return m_length;
		}

		void unalloc() {
			if ( m_ptr ) {
				delete [] m_ptr;
				m_ptr = 0;
				m_length = 0;
			}
		}

		int find( A & el ) {
			for ( int i = 0; i < m_length; i++ ) {
				if ( m_ptr[i] == el ) {
					return i;
				}
			}
			return -1;
		}

		A * getData() {
			return m_ptr;
		}

		const A * getDataConst() const {
			return m_ptr;
		}
		
	protected:

		void copyFrom( const Array<A> & from ) {
			delete [] m_ptr;
			m_ptr = new A[from.m_length];
			for ( uint i = 0; i < from.m_length; i++ ) {
				m_ptr[i] = from.m_ptr[i];
			}
			m_length = from.m_length;
		}
		
		A * m_ptr;
		uint m_length;
		bool m_managePtr;

	};

	
	//template<class T> unsigned int poopie_hash<Array<T>>( const Array<T> & arr );

}

#endif

