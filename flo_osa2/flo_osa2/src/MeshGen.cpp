
#include "poopie/gfx/Mesh.h"
#include "poopie/gfx/MeshData.h"
#include "poopie/base/Array.h"
#include "poopie/math/Matrix.h"
#include "MeshGen.h"

using namespace poopie::gfx;
using namespace poopie;

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

	/*
	//Array<int> indices( 6*6 );
	indices[0] = 1; 	indices[1] = 0; 	indices[2] = 2;
	indices[3] = 3; 	indices[4] = 2; 	indices[5] = 0;

	indices[6] = 4; 	indices[7] = 5; 	indices[8] = 6;
	indices[9] = 6; 	indices[10] = 7;	indices[11] = 4;

	indices[12] = 0; 	indices[13] = 1; 	indices[14] = 5;
	indices[15] = 5; 	indices[16] = 4;	indices[17] = 0;

	indices[18] = 2; 	indices[19] = 3; 	indices[20] = 6;
	indices[21] = 7; 	indices[22] = 6;	indices[23] = 3;

	indices[24] = 3; 	indices[25] = 0; 	indices[26] = 7;
	indices[27] = 4; 	indices[28] = 7;	indices[29] = 0;

	indices[30] = 1; 	indices[31] = 2; 	indices[32] = 6;
	indices[33] = 6; 	indices[34] = 5;	indices[35] = 1;
	*/

	MeshData * m = new MeshData();
	//m->indices = indices;
	m->faces = faces;
	m->vertices = vertices;	
	m->computeNormals();

	return m;
}

MeshData * MeshGen::SplineCone( Spline & spline, float r0, float r1, int radQual, int lenQual ) {

	int vertCount = radQual * (lenQual+1);

	Array<Vertex> verts( vertCount );
	//Array<int> inds( 3 * 2 * radQual * lenQual );
	Array<Face> faces( radQual * lenQual );

	float rotA = 2.0f * 3.141593f / radQual;

	Vector3 arb = Vector3(1,1,1);

	for( int x = 0; x <= lenQual; x++ ) {
		float p = (float)x / lenQual;
		float r = r0 + (r1-r0) * p;
		Vector3 tangent = spline.getTangent( p );
		Vector3 cp = spline.getPoint( p );
		Matrix4 rot = Matrix4::rotateAxis( tangent, rotA );

		Vector3 p0 = tangent.cross( arb ).normalized() * r;
		for( int a = 0; a < radQual; a++ ) {
			verts[x * radQual + a].position = cp + p0;
			p0 = rot.mulVec( p0 );
		}
	}


	for( int x = 0; x < lenQual; x++ ) {
		for( int a = 0; a < radQual; a++ ) {
			int I = (x*radQual+a);
			faces[I].v[0].i = (x+0) * radQual + (a+1)%radQual;
			faces[I].v[1].i = (x+0) * radQual + (a+0);
			faces[I].v[2].i = (x+1) * radQual + (a+0);
			faces[I].v[3].i = (x+1) * radQual + (a+1)%radQual;
		}
	}

	MeshData * m = new MeshData();
	m->faces = faces;
	m->vertices = verts;	
	m->computeNormals();

	return m;

}

