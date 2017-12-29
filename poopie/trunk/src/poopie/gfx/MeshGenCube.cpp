
#include "MeshData.h"
#include "MeshGen.h"

namespace poopie {
	namespace gfx {

		MeshData * MeshGen::Cube() {
			Array<Vertex> vertices( 8 );

			vertices[0].position = Vector3(-1,-1,-1 );
			vertices[1].position = Vector3( 1,-1,-1 );
			vertices[2].position = Vector3( 1,-1, 1 );
			vertices[3].position = Vector3(-1,-1, 1 );

			vertices[4].position = Vector3(-1, 1,-1 );
			vertices[5].position = Vector3( 1, 1,-1 );
			vertices[6].position = Vector3( 1, 1, 1 );
			vertices[7].position = Vector3(-1, 1, 1 );

			Array<Face> faces( 6 );
			faces[0].v[0].i = 1;
			faces[0].v[1].i = 0;
			faces[0].v[2].i = 3;
			faces[0].v[3].i = 2;

			faces[1].v[0].i = 4;
			faces[1].v[1].i = 5;
			faces[1].v[2].i = 6;
			faces[1].v[3].i = 7;

			faces[2].v[0].i = 0;
			faces[2].v[1].i = 1;
			faces[2].v[2].i = 5;
			faces[2].v[3].i = 4;

			faces[3].v[0].i = 2;
			faces[3].v[1].i = 3;
			faces[3].v[2].i = 7;
			faces[3].v[3].i = 6;

			faces[4].v[0].i = 3;
			faces[4].v[1].i = 0;
			faces[4].v[2].i = 4;
			faces[4].v[3].i = 7;

			faces[5].v[0].i = 1;
			faces[5].v[1].i = 2;
			faces[5].v[2].i = 6;
			faces[5].v[3].i = 5;

			MeshData * m = new MeshData();
			m->faces = faces;
			m->vertices = vertices;	
			m->computeNormals();

			return m;
		}

	}
}

