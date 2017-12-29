#ifndef __POOPIE_MATH_AABB
#define __POOPIE_MATH_AABB

#include "poopie/math/Vector.h"

namespace poopie {

	class AABB {
	public:
		Vector3 min, max;

		AABB() {}
		AABB( const Vector3 & min, const Vector3 & max ) : min(min), max(max) {}

	};

}

#endif 