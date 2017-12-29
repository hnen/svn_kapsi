
#ifndef __POOPIE_BASE_FRUSTUM
#define __POOPIE_BASE_FRUSTUM

#include "poopie/math/Matrix.h"
#include "poopie/math/Vector.h"
#include "poopie/math/Plane.h"


namespace poopie {

	class Frustum {
	public:

		Plane p[6];

		static Frustum fromMatrix( Matrix4 & M ) {
			Frustum r;
			r.p[0] = Plane( M._03 + M._00, M._13 + M._10, M._23 + M._20, M._33 + M._30 );
			r.p[1] = Plane( M._03 - M._00, M._13 - M._10, M._23 - M._20, M._33 - M._30 );
			r.p[2] = Plane( M._03 + M._01, M._13 + M._11, M._23 + M._21, M._33 + M._31 );
			r.p[3] = Plane( M._03 - M._01, M._13 - M._11, M._23 - M._21, M._33 - M._31 );
			r.p[4] = Plane( M._02, M._12, M._22, M._32 );
			r.p[5] = Plane( M._03 - M._02, M._13 - M._12, M._23 - M._22, M._33 - M._32 );
			return r;
		}

		bool intersects( const Sphere & sphere ) {
			for( int i = 0; i < 6; i++ ) {
				if ( p[i].distance( sphere ) < -sphere.radius * 2.0f ) {
					return false;
				}
			}
			return true;
		}

		bool pointInside( Vector3 & v ) {
			for( int i = 0; i < 6; i++ ) {
				if ( p[i].dot( v ) < 0 ) {
					return false;
				}
			}
			return true;
		}


	};

}


#endif