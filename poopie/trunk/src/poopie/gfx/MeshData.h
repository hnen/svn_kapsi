#ifndef __POOPIE_GFX_MESHDATA
#define __POOPIE_GFX_MESHDATA

#include "poopie/gfx/Buffer.h"
#include "poopie/gfx/VertexLayout.h"
#include "poopie/math/Vector.h"
#include "poopie/base/Array.h"
#include "poopie/base/ByteStream.h"
#include "poopie/math/Vector.h"
#include "poopie/math/Sphere.h"
#include "poopie/base/HashMap.h"
#include "poopie/base/Hash.h"

namespace poopie {
namespace gfx {

struct Mesh;
class Renderer;
class VertexLayout;
class Material;
class ImageData;
class Shader;

struct Vertex {
	Vector3 position;
	Vector3 normal;
	Vector2 uv;
	Vector4 tangent;

	// Determines where, from lower lod mesh, the vertex was subdivided from.
	// If lodElement < vertexCount, lodElement points to vertex lodElement
	// If lodElement >= vertexCount, lodElement is a face centroid for face index lodElement - vertexCount
	// If lodElement >= vertexCount + faceCount, lodElement points to an edge like when N = lodElement - (vertexCount + faceCount), face is N/4, and edge id is N%4
	int lodElement;
	Vector3 lodPosition;
	Vector3 lodNormal;
	Vector4 color;

	Vertex operator + ( const Vertex & a ) const {
		Vertex ret(*this);
		ret.position = position + a.position;
		return ret;
	}

	Vertex operator - ( const Vertex & a ) const {
		Vertex ret(*this);
		ret.position = position - a.position;
		return ret;
	}

	Vertex operator * ( const float a ) const {
		Vertex ret(*this);
		ret.position = position * a;
		return ret;
	}

	Vertex( Vector3 pos ) : position(pos) {}
	Vertex( Vector3 pos, Vector3 normal ) : position(pos), normal(normal) {}

	Vertex() {}

};

struct Face {
	struct V {
		int i;
		Vector2 uv;
		Vector3 tangent0;
		Vector3 tangent1;
		inline bool operator == ( const V & a ) const { return i == a.i && uv == a.uv; }
		V() : i(-1) {}
		V( int i ) : i(i) { POOP_ASSERT( i >= -1 ); }
	};
	V v[4];

	Face() {}

	Face( int i0, int i1, int i2, int i3 ) {
		v[0] = V(i0);
		v[1] = V(i1);
		v[2] = V(i2);
		v[3] = V(i3);
	}
	Face( int i0, int i1, int i2 ) {
		v[0] = V(i0);
		v[1] = V(i1);
		v[2] = V(i2);
	}

	Face( V & v0, V & v1, V & v2, V & v3 ) {
		v[0] = v0;
		v[1] = v1;
		v[2] = v2;
		v[3] = v3;
	}
	Face( V & v0, V & v1, V & v2 ) {
		v[0] = v0;
		v[1] = v1;
		v[2] = v2;
		v[3].i = -1;
	}

	bool operator == ( const Face & a ) const { return v[0].i == a.v[0].i && v[1].i == a.v[1].i && v[2].i == a.v[2].i && v[3].i == a.v[3].i; }

};


class MaterialData {
public:
	static MaterialData fromStream( ByteStream & bs );

	// Probably should have this property..
	//Shader * shader;

	HashMap<String, ImageData*> images;
	HashMap<String, Vector4> vecs;
	HashMap<String, float> values;

	Material * createMaterial( Renderer * renderer, Shader * shader );

};

class MeshData {
public:

	Array<Vertex> vertices;
	Array<Face> faces;
	Array<MaterialData> materials;

	void computeNormals();
	void computeFaceTangents();
	void flipFaces();
	
	// Create triangles
	Mesh * createMesh( Renderer * renderer, VertexLayout * vertexLayout );
	void updateMesh( Renderer * renderer, VertexLayout * vertexLayout, Mesh * dstMesh );

	void processForRender( VertexLayout * vertexLayout );

	Sphere computeBoundingSphere();

	void subdivide( bool smooth );
	void displace( ImageData * imgData, float midLevel, float strength );
	void scale( float scale );

	void computeLodPositions( MeshData * lowerLod );

	void mirrorZ();

	void exportOBJ( char * filename );

	void append( Array<MeshData *> & another );
	void append( MeshData * another );

private:

	// Duplicate vertices with differrent UVs etc
	void generateUniqueVertices();
	void computeVertexTangents();
	void triangulate();

	Array<int> createTriangleIndices();

	void outputVerticesToByteStream( ByteStream & bs, VertexLayout * vertexLayout );

};

}

template<> unsigned int poopie_hash<gfx::Face::V>( const gfx::Face::V & s );

}

#endif
