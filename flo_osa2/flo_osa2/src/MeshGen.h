
#ifndef __MESHGEN
#define __MESHGEN

#include "poopie/gfx/MeshData.h"
#include "poopie/math/Spline.h"

using namespace poopie::gfx;
using namespace poopie;

class MeshGen {
public:

	static MeshData * Cube();
	static MeshData * Tree();

	static MeshData * SplineCone( Spline & spline, float r0, float r1, int radQual, int lenQual );

};

#endif

