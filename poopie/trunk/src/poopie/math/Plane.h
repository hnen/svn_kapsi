
#ifndef __POOPIE_BASE_PLANE
#define __POOPIE_BASE_PLANE

#include "poopie/math/Vector.h"
#include "poopie/math/Sphere.h"

namespace poopie {

	class Plane {
	public:
		float a, b, c, d;
		
		Plane() {}
		Plane( float a, float b, float c, float d ) : a(a), b(b), c(c), d(d) {}

		Vector3 getOrigin() { return c != 0 ? Vector3( 0, 0, -d/c ) : Vector3( 0, -d/b, 0 ); }
		Vector3 getNormal() { return Vector3(a,b,c); }
		
		float dot( const Vector3 & v ) {
			return (v-getOrigin()).dot( getNormal() );
		}
		float distance( const Vector3 & p ) {
			return dot(p) / getNormal().length();
		}
		float distance( const Sphere & s ) {
			return distance(s.center) - s.radius;
		}

		Vector4 asVec4() {
			return Vector4(a,b,c,d);
		}
	};

}


#endif


