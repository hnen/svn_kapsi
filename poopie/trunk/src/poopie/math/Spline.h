
#ifndef __POOPIE_SPLINE
#define __POOPIE_SPLINE

#include "Vector.h"

namespace poopie {

	class Spline {
	public:

		Spline( Vector3 * points, int pointCount );
		Vector3 getPoint( float normalizedPoint );
		Vector3 getTangent( float normalizedPoint );

	private:

		Vector3 * m_pts;
		int m_ptCount;

	};

	// linear
	template<class A>
	A lerp( const A & a, const A & b, float t ) {
		return a*(1-t) + b*t;
	} 

	template<class A>
	A qerp( const A & a, const A & b, float t ) {
		float tsq = t*t;
		return a*(tsq - 2.0f*t + 1) + b*(2.0f*t-tsq);
	} // quadratic

	template<class A>
	A cerp( const A & a, const A & b, float t ) {
		float tsq = t*t;
		float tcb = tsq*t;
		return a*(2.0f*tcb - 3.0f*tsq + 1.0f) + b*(3.0f*tsq - 2.0f*tcb);
	} // cubic

	Vector3 slerp( const Vector3 & a, const Vector3 & b, float t );

}


#endif

