#ifndef __POOPIE_BASE_BYTESTREAM
#define __POOPIE_BASE_BYTESTREAM

#include <string.h>
#include "poopie/defs.h"
#include "poopie/base/DynamicArray.h"
#include "poopie/base/String.h"

namespace poopie {

	class ByteStream {
	public:

		ByteStream( void * dstPtr, int dataSize ) {
			m_alloced = 0;
			m_ptr = 0;
			m_managePtr = false;
			m_data = dstPtr;
			m_dataSize = dataSize;
		}

		ByteStream() {
			m_alloced = 16;
			m_data = new char[m_alloced];
			m_ptr = 0;
			m_managePtr = true;
		}

		~ByteStream() {
			if ( m_managePtr ) {
				delete [] m_data;
			}
		}

		template<class T>
		void append( T data ) {
			if ( m_managePtr && m_alloced < m_ptr + sizeof(T) ) {
				void * ndata = new char[m_alloced<<1];
				memcpy( ndata, m_data, m_alloced );
				m_alloced *= 2;
				delete [] m_data;
				m_data = ndata;
			}
			*(T*)((char*)m_data + m_ptr) = data;
			m_ptr += sizeof(data);
			if ( m_ptr >= m_dataSize ) m_dataSize = m_ptr+1;
		}

		template<class T>
		T & read() {
			if ( m_ptr + sizeof(T) > m_dataSize ) {
				POOP_ERROR( "End of stream." );
				//return T();
			}
			T & ret = *((T*)((char*)m_data + m_ptr));
			m_ptr += sizeof(T);
			return ret;
		}

		char * readChars( int num ) {
			char * buf = new char[num];
			for( int i = 0; i < num; i++ ) {
				buf[i] = read<char>();
			}
			return buf;
		}

		void readBytesTo( int num, void * dst ) {
			char * buf = new char[num];
			for( int i = 0; i < num; i++ ) {
				((unsigned char*)dst)[i] = read<unsigned char>();
			}
		}

		String readString() {
			DynamicArray<char> bf(16);
			char c;
			do {
				c = read<char>();
				bf.append(c);
			} while ( c != 0 );
			return String( bf.getData() );
		}

		bool endOfStream() {
			return m_ptr == m_dataSize;
		}

		void resetPtr() {
			m_ptr = 0;
		}

		void skip( int numBytes ) {
			m_ptr += numBytes;
		}

		int getDataSize() {
			return m_ptr;
		}

		unsigned int getPosition() {
			return m_ptr;
		}

		const void * getDataPtr() {
			return m_data;
		}

	private:

		bool m_managePtr;
		void * m_data;
		unsigned int m_alloced;

		unsigned int m_ptr;
		unsigned int m_dataSize;

	};

}

#endif
