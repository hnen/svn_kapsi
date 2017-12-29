
#include <string.h>
#include "RadixSort.h"
#include "../../DebugTools.h"

//NOTE: Most optimal when N > 65024, BITS=16, and when N > 224, BITS=8

#define BITS 8

#define MASK ((1<<BITS)-1)
#define RSIZE (1<<BITS)
#define RCOUNT (32/BITS)

void p_RadixSortImpl::p_sort( void * data, uint numElements, bool floatFix ) {
	byte * elements = (byte*)data;
	byte * aelements = new byte[numElements * m_elementSize];

	uint c[RSIZE];
	uint offset[RSIZE];

	uint * vals = new uint[numElements];
	uint * avals = new uint[numElements];
	for( uint l = 0; l < numElements; l++ ) {
		vals[l] = m_valueGetter( (void*)(&elements[l*m_elementSize]) );
		if ( floatFix ) {
			vals[l] ^= -((int)(vals[l]>>31)) | 0x80000000;
		}
	}

	uint shift = 0;
	for( int r = 0; r < RCOUNT; r++ ) {
		uint lptr;
		
		for( uint l = 0; l < RSIZE; l++ ) {
			c[l] = 0;
		}

		lptr = 0;
		for( uint l = 0; l < numElements; l++ ) {
			uint value = vals[l];
			lptr += m_elementSize;
			uint radix = (value>>shift) & MASK;
			c[radix]++;
		}

		offset[0] = 0;
		for( uint l = 1; l < RSIZE; l++ ) {
			offset[l] = offset[l-1] + c[l-1];
			FASSERT( offset[l] <= numElements );
		}

		lptr = 0;
		for( uint l = 0; l < numElements; l++ ) {
			uint value = vals[l];
			lptr += m_elementSize;
			uint radix = (value>>shift) & MASK;
			FASSERT( offset[radix]*m_elementSize < numElements * m_elementSize );
			FASSERT( l*m_elementSize < numElements * m_elementSize );
			memcpy( &aelements[offset[radix]*m_elementSize], &elements[l*m_elementSize], m_elementSize );
			avals[offset[radix]] = vals[l];
			offset[radix] ++;
		}

		byte * t = aelements;
		aelements = elements;
		elements = t;

		uint * p = avals;
		avals = vals;
		vals = p;

		shift += BITS;
	}

	delete [] aelements;
	delete [] avals;
	delete [] vals;

}

