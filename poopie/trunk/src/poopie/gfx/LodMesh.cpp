
#include "LodMesh.h"

namespace poopie {
namespace gfx {

	int LodMesh::selectMeshI( float z ) {
		return 6;

		int i = 0;
		while ( i < distances.getLength() - 1 && z > distances[i] ) {
			i++;
		}
		return i;
	}

	Mesh * LodMesh::selectMesh( float z ) {
		return meshes[selectMeshI(z)];
	}

	LodMesh::LodMesh( const Array<Mesh*> & meshes, float closestDist ) {
		this->meshes = meshes;
		this->distances.resize( this->meshes.getLength() - 1 );
		this->distances[0] = closestDist;
		for ( int i = 1; i < this->distances.getLength(); i++ ) {
			this->distances[i] = 2.0f * this->distances[i-1];
		}
	}

}
}


