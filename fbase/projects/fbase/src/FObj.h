
#ifndef __FOBJ
#define __FOBJ

#include <map>

extern std::map<void*, int> g_FObjRefCount;

template<class A>
class FObjPtr {
	A * m_ptr;

	static void addRef( void * ptr ) {
		g_FObjRefCount[ptr]++;
	}
	static void remRef( void * ptr ) {
		g_FObjRefCount[ptr]--;
	}

public:

	inline FObjPtr<A> & setPtr( A * a ) {
		if ( m_ptr ) {
			remRef( m_ptr );
		}
		m_ptr = a;
		addRef( m_ptr );
		return *this;
	}
	inline A * getPtr() { return m_ptr; }

	FObjPtr() : m_ptr(0) {}
	FObjPtr( A * a ) : m_ptr(0) { setPtr(a); }

	inline A * operator -> () { return getPtr(); }
	inline FObjPtr<A> & operator = ( A * a ) { return setPtr( a ); }

};

#endif

