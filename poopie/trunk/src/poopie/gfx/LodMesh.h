
#ifndef __POOPIE_GFX_LODMESH
#define __POOPIE_GFX_LODMESH

#include "poopie/base/Array.h"
#include "poopie/gfx/Mesh.h"

namespace poopie {
namespace gfx {


	class LodMesh {
	public:

		Array<Mesh*> meshes;
		Array<float> distances;

		Mesh * selectMesh( float z );
		int selectMeshI( float z );
		
		//static LodMesh * Create( const Array<Mesh*> & meshes, float closestDist );
		LodMesh( const Array<Mesh*> & meshes, float closestDist );

	};

}
}


#endif
