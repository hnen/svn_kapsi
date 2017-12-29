#ifndef __POOPIE_BASE_TUPLE
#define __POOPIE_BASE_TUPLE


namespace poopie {

	template<class A, class B>
	struct Tuple {
		A a;
		B b;

		Tuple() {}
		Tuple( const A & a, const B & b ) : a(a), b(b) {}

	};


}


#endif