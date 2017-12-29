
#ifndef __RADIXSORT
#define __RADIXSORT

#include "../../const.h"

template<class A, class B>
uint p_GetValue( void * objPtr ) {
	B vB = (B(*static_cast<A*>(objPtr)));
	return (uint&)vB;
};

typedef unsigned int (*Ptr_GetValue)( void* );


class p_RadixSortImpl {

	const int m_elementSize;
	Ptr_GetValue m_valueGetter;

public:

	void p_sort( void * data, uint numElements, bool floatFix );
	p_RadixSortImpl( int elementSize, Ptr_GetValue valueGetter ) : m_elementSize(elementSize), m_valueGetter(valueGetter) {};

};

template<class A>
class RadixSortUInt {

	p_RadixSortImpl impl;

public:

	RadixSortUInt() : impl( sizeof(A), p_GetValue<A, uint> ) {};
	void sort( A * data, int numElements ) { impl.p_sort( (void*)data, numElements, false ); }

};

template<class A>
class RadixSortFloat {

	p_RadixSortImpl impl;
	bool m_floats;

public:

	RadixSortFloat() : impl( sizeof(A), p_GetValue<A, float> ) {};
	void sort( A * data, int numElements ) { impl.p_sort( (void*)data, numElements, true ); }

};


#endif

