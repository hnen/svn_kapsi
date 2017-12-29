
#ifndef __POOPIE_MATH_SPHERE
#define __POOPIE_MATH_SPHERE

#include "poopie/math/Vector.h"

namespace poopie {

	class Sphere {
	public:
		Vector3 center;
		float radius;

		Sphere() {}
		Sphere( Vector3 center, float radius ) : center(center), radius(radius) {}
		
		// Minimal sphere containing p0 and p1
		Sphere( Vector3 p0, Vector3 p1 ) {
			center = (p0+p1) * 0.5f;
			radius = (center-p0).length();
		}

		bool intersects( const Sphere & s ) {
			return (s.center - center).lengthSq() < radius * radius + s.radius * s.radius + 2.0f * radius * s.radius;
		}

		Sphere add( const Vector3 & point ) {
			Vector3 d = point - center;
			float dist2 = d.lengthSq();
			if ( dist2 > radius * radius ) {
				float dist = sqrtf( dist2 );
				float nR = (radius + dist) * 0.5f;
				float k = (nR - radius) / dist;
				return Sphere( center + d * k, nR );
			} else {
				// Point contains already sphere; union is existing sphere
				return *this;
			}
		}

	};


}


#endif
