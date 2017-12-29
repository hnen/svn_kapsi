
#include <string.h>
#include "Spline.h"
#include "poopie/defs.h"

namespace poopie {

inline float catmull( float t, float tsq, float tcb, float p0, float p1, float p2, float p3 ) {
	return 0.5f * ( (2.0f * p1) + (-p0 + p2) * t + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * tsq + (-p0 + 3*p1 - 3*p2 + p3) * tcb );
}
inline float dcatmull( float t, float tsq, float p0, float p1, float p2, float p3 ) {
	return (-p0 + p2) + 2.0f * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t + 3.0f * (-p0 + 3*p1 - 3*p2 + p3) * tsq;
}

Spline::Spline( Vector3 * points, int cnt ) {
	m_pts = new Vector3[cnt];
	m_ptCount = cnt;
	for( int i = 0; i < cnt; i++ ) {
		m_pts[i] = points[i];
	}
	//memcpy( m_pts, points, cnt * sizeof(Vector3) );
}

Vector3 Spline::getPoint( float pnt ) {
	int maxPt = m_ptCount-1;
	pnt *= maxPt;
	if ( pnt > maxPt ) pnt = maxPt;
	if ( pnt < 0 ) pnt = 0;
	
	int ipnt = (int)pnt;
	float fpnt = pnt - (float)ipnt;

	int p0 = CLAMP( ipnt - 1, 0, m_ptCount-1 );
	int p1 = CLAMP( ipnt + 0, 0, m_ptCount-1 );
	int p2 = CLAMP( ipnt + 1, 0, m_ptCount-1 );
	int p3 = CLAMP( ipnt + 2, 0, m_ptCount-1 );

	float t = fpnt;
	float tsq = t*t;
	float tcb = t*tsq;

	return Vector3( 
		catmull( t, tsq, tcb, m_pts[p0].x, m_pts[p1].x, m_pts[p2].x, m_pts[p3].x ),
		catmull( t, tsq, tcb, m_pts[p0].y, m_pts[p1].y, m_pts[p2].y, m_pts[p3].y ),
		catmull( t, tsq, tcb, m_pts[p0].z, m_pts[p1].z, m_pts[p2].z, m_pts[p3].z ) );
}

Vector3 Spline::getTangent( float pnt ) {
	int maxPt = m_ptCount-1;
	pnt *= maxPt;
	if ( pnt > maxPt ) pnt = maxPt;
	if ( pnt < 0 ) pnt = 0;
	
	int ipnt = (int)pnt;
	float fpnt = pnt - (float)ipnt;

	int p0 = CLAMP( ipnt - 1, 0, m_ptCount-1 );
	int p1 = CLAMP( ipnt + 0, 0, m_ptCount-1 );
	int p2 = CLAMP( ipnt + 1, 0, m_ptCount-1 );
	int p3 = CLAMP( ipnt + 2, 0, m_ptCount-1 );

	float t = fpnt;
	float tsq = t*t;
	float tcb = t*tsq;

	return Vector3( 
		dcatmull( t, tsq, m_pts[p0].x, m_pts[p1].x, m_pts[p2].x, m_pts[p3].x ),
		dcatmull( t, tsq, m_pts[p0].y, m_pts[p1].y, m_pts[p2].y, m_pts[p3].y ),
		dcatmull( t, tsq, m_pts[p0].z, m_pts[p1].z, m_pts[p2].z, m_pts[p3].z ) ).normalized();
}

	Vector3 slerp( const Vector3 & a, const Vector3 & b, float t ) {
		float omega = acosf( a.dot(b) / (a.length() * b.length()) );
		return (a * sin((1-t)*omega) + b * sin(t*omega)) / sin(omega);
	}

	/*
	// linear
	float lerp( float a, float b, float t ) {
		return a*(1-t) + b*t;
	} 

	float qerp( float a, float b, float t ) {
		float tsq = t*t;
		return a*(tsq - 2.0f*t + 1) + b*(2.0f*t-tsq);
	} // quadratic

	template<class A>
	A cerp( A a, A b, A t ) {
		float tsq = t*t;
		float tcb = tsq*t;
		return a*(2.0f*tcb - 3.0f*tsq + 1.0f) + b*(3.0f*tsq - 2.0f*tcb);
	} // cubic
	*/
}