#include "poopie/gfx/VertexLayout.h"
#include "poopie/gfx/Format.h"
#include "poopie/gfx/Renderer.h"
#include "poopie/gfx/Material.h"
#include "poopie/gfx/ImageData.h"
#include "poopie/gfx/Mesh.h"
#include "poopie/defs.h"
#include "poopie/base/ByteStream.h"
#include "poopie/base/DynamicArray.h"
#include "poopie/base/HashMap.h"
#include "poopie/math/Vector.h"
#include "poopie/gfx/ImageImporter.h"

#include "MeshData.h"

namespace poopie {
namespace gfx {

enum EType {
	POSITION,
	NORMAL,
	TANGENT,
	TEX0,
	LODPOS,
	LODNORMAL,
	COLOR,
};

void MeshData::flipFaces() {
	for( uint i = 0; i < faces.getLength(); i++ ) {
		Face &f = faces[i];
		if( f.v[3].i == -1 ) {
			Face::V tmpi = f.v[0];
			f.v[0] = f.v[1];
			f.v[1] = tmpi;
		} else {
			Face::V A = f.v[0];
			Face::V B = f.v[1];
			Face::V C = f.v[2];
			Face::V D = f.v[3];
			f.v[0] = B;
			f.v[1] = A;
			f.v[2] = D;
			f.v[3] = C;
		}
	}
}


void MeshData::scale( float scale )
{
	POOP_ASSERT( vertices.getLength() > 0 );
	for( int i = 0; i < vertices.getLength(); i++ ) 
		vertices[i].position = vertices[i].position * scale;
}


void MeshData::mirrorZ() {

	Array<int> mirroredVerts(vertices.getLength());

	DynamicArray<Vertex> nverts( vertices );

	for ( int i = 0; i < vertices.getLength(); i++ ) {
		Vertex &v = vertices[i];
		if ( fabsf(v.position.z) <= 0.1f ) {
			mirroredVerts[i] = i;
		} else {
			Vertex nvert = v;
			nvert.position.z = -v.position.z;
			mirroredVerts[i] = nverts.getLength();
			nverts.append( nvert );
		}
	}

	DynamicArray<Face> nfaces( faces );
	for ( int i = 0; i < faces.getLength(); i++ ) {
		Face & f = faces[i];
		Face nface = f;
		int vc = f.v[3].i == -1 ? 3 : 4;
		for ( int v = 0; v < 4 && f.v[v].i != -1; v++ ) {
			nface.v[v].i = mirroredVerts[f.v[vc-v-1].i];
		}
		nfaces.append(nface);
	}

	this->vertices = nverts.ToArray();
	this->faces = nfaces.ToArray();

}

void MeshData::computeFaceTangents() {
	for ( int i = 0; i < faces.getLength(); i++ ) {
		for ( int v = 0; v < 4; v++ ) {
			faces[i].v[v].tangent0 = Vector3(0,0,0);
			faces[i].v[v].tangent1 = Vector3(0,0,0);
		}
	}
	for ( int i = 0; i < faces.getLength(); i++ ) {
		Face & f = faces[i];

		Vertex & v0 = vertices[faces[i].v[0].i];
		Vertex & v1 = vertices[faces[i].v[1].i];
		Vertex & v2 = vertices[faces[i].v[2].i];

		Vector3 e0 = v1.position - v0.position;
		Vector3 e1 = v2.position - v0.position;

		//Vector2 t0 = v1.uv - v0.uv;
		//Vector2 t1 = v2.uv - v0.uv;
		Vector2 t0 = faces[i].v[1].uv - faces[i].v[0].uv;
		Vector2 t1 = faces[i].v[2].uv - faces[i].v[0].uv;

		float invdet = (t0.x*t1.y - t0.y*t1.x);
		Vector2 it0 = Vector2( t0.y, -t0.x ) / invdet;
		Vector2 it1 = Vector2( t1.y, -t1.x ) / invdet;

		Vector3 T = Vector3( Vector2( e0.x, e1.x ).dot(it0), Vector2( e0.y, e1.y ).dot(it0), Vector2( e0.z, e1.z ).dot(it0) );
		Vector3 B = Vector3( Vector2( e0.x, e1.x ).dot(it1), Vector2( e0.y, e1.y ).dot(it1), Vector2( e0.z, e1.z ).dot(it1) );

		f.v[0].tangent0 += T;
		f.v[1].tangent0 += T;
		f.v[2].tangent0 += T;
		f.v[3].tangent0 += T;

		f.v[0].tangent1 += B;
		f.v[1].tangent1 += B;
		f.v[2].tangent1 += B;
		f.v[3].tangent1 += B;

		/*
		if ( faces[i].v[3].i != -1 ) {
			Vertex & v3 = vertices[faces[i].v[3].i];

			Vector3 e0 = v1.position - v3.position;
			Vector3 e1 = v2.position - v3.position;

			//Vector2 t0 = v1.uv - v3.uv;
			//Vector2 t1 = v2.uv - v3.uv;
			Vector2 t0 = faces[i].v[1].uv - faces[i].v[3].uv;
			Vector2 t1 = faces[i].v[2].uv - faces[i].v[3].uv;

			float invdet = (t0.x*t1.y - t0.y*t1.x);
			Vector2 it0 = Vector2( t0.y, -t0.x ) / invdet;
			Vector2 it1 = Vector2( t0.y, -t0.x ) / invdet;

			Vector3 T = Vector3( Vector2( e0.x, e1.x ).dot(it0), Vector2( e0.y, e1.y ).dot(it0), Vector2( e0.z, e1.z ).dot(it0) );
			Vector3 B = Vector3( Vector2( e0.x, e1.x ).dot(it1), Vector2( e0.y, e1.y ).dot(it1), Vector2( e0.z, e1.z ).dot(it1) );
			f.v[3].tangent0 += T;
			f.v[3].tangent1 += B;
			f.v[1].tangent0 += T;
			f.v[1].tangent1 += B;
			f.v[2].tangent0 += T;
			f.v[2].tangent1 += B;
		}
		*/
	}
}

void MeshData::append( Array<MeshData *> & another ) {
	int nverts = 0;
	int nfaces = 0;
	for ( int i = 0; i < another.getLength(); i++ ) {
		if ( !another[i] ) continue;
		nverts += another[i]->vertices.getLength();
		nfaces += another[i]->faces.getLength();
	}

	int voff = vertices.getLength();
	int foff = faces.getLength();

	vertices.resize( vertices.getLength() + nverts );
	faces.resize( faces.getLength() + nfaces );

	for ( int i = 0; i < another.getLength(); i++ ) {
		if ( !another[i] ) continue;
		for ( int v = 0; v < another[i]->vertices.getLength(); v++ ) {
			vertices[voff+v] = another[i]->vertices[v];
		}
		for ( int f = 0; f < another[i]->faces.getLength(); f++ ) {
			Face f0 = another[i]->faces[f];
			f0.v[0].i += voff;
			f0.v[1].i += voff;
			f0.v[2].i += voff;
			if ( f0.v[3].i != -1 ) {
				f0.v[3].i += voff;
			}
			faces[foff+f] = f0;
		}

		foff += another[i]->faces.getLength();
		voff += another[i]->vertices.getLength();
	}


}

void MeshData::append( MeshData * another ) {

	int offset = vertices.getLength();
	vertices.resize( vertices.getLength() + another->vertices.getLength() );
	for ( int i = 0; i < another->vertices.getLength(); i++ ) {
		vertices[i+offset] = another->vertices[i];
	}

	int foffset = faces.getLength();
	faces.resize( faces.getLength() + another->faces.getLength() );
	for ( int i = 0; i < another->faces.getLength(); i++ ) {
		Face f = another->faces[i];
		f.v[0].i += offset;
		f.v[1].i += offset;
		f.v[2].i += offset;
		if ( f.v[3].i != -1 ) {
			f.v[3].i += offset;
		}
		faces[i+foffset] = f;
	}

}

void MeshData::computeNormals() {
	for( uint v = 0; v < vertices.getLength(); v++ ) {
		vertices[v].normal = Vector3(0,0,0);
		vertices[v].lodNormal = Vector3(0,0,0);
	}
	for( uint i = 0; i < faces.getLength(); i++ ) {
		Face & f = faces[i];
		Vertex & v0 = vertices[f.v[0].i];
		Vertex & v1 = vertices[f.v[1].i];
		Vertex & v2 = vertices[f.v[2].i];
		Vector3 e0 = v0.position - v1.position;
		Vector3 e1 = v0.position - v2.position;
		Vector3 e0_0 = v0.lodPosition - v1.lodPosition;
		Vector3 e1_0 = v0.lodPosition - v2.lodPosition;
		Vector3 n = e0.cross(e1).normalized();
		Vector3 n_0 = e0_0.cross(e1_0).normalized();
		v0.normal += n;
		v1.normal += n;
		v2.normal += n;
		v0.lodNormal += n_0;
		v1.lodNormal += n_0;
		v2.lodNormal += n_0;
		if ( f.v[3].i != -1 ) {
			Vertex & w0 = vertices[f.v[2].i];
			Vertex & w1 = vertices[f.v[3].i];
			Vertex & w2 = vertices[f.v[0].i];
			Vector3 e2 = w0.position - w1.position;
			Vector3 e3 = w0.position - w2.position;
			Vector3 e2_0 = w0.lodPosition - w1.lodPosition;
			Vector3 e3_0 = w0.lodPosition - w2.lodPosition;
			Vector3 n = e2.cross(e3).normalized();
			Vector3 n_0 = e2_0.cross(e3_0).normalized();
			w0.normal += n;
			w1.normal += n;
			w2.normal += n;
			w0.lodNormal += n_0;
			w1.lodNormal += n_0;
			w2.lodNormal += n_0;
		}
	}
	for( uint v = 0; v < vertices.getLength(); v++ ) {
		vertices[v].normal.normalize();
		vertices[v].lodNormal.normalize();
	}
}

void MeshData::outputVerticesToByteStream( ByteStream & bs, VertexLayout * vertexLayout ) {
	int J = vertexLayout->getFormatListElementCount();
	const Format * F = vertexLayout->getFormatList();

	EType * t = new EType[F->getSize()];
	for ( int j = 0; j < J; j++ ) {
		if ( F[j].name.toLowerCase().equals( "position" ) ) {
			t[j] = POSITION;
		} else if ( F[j].name.toLowerCase().equals( "normal" ) ) {
			t[j] = NORMAL;
		} else if ( F[j].name.toLowerCase().equals( "texcoord" ) ) {
			t[j] = TEX0;
		} else if ( F[j].name.toLowerCase().equals( "tangent" ) ) {
			t[j] = TANGENT;
		} else if ( F[j].name.toLowerCase().equals( "lodpos" ) ) {
			t[j] = LODPOS;
		} else if ( F[j].name.toLowerCase().equals( "lodnormal" ) ) {
			t[j] = LODNORMAL;
		} else if ( F[j].name.toLowerCase().equals( "color" ) ) {
			t[j] = COLOR;
		} else {
			POOP_ERROR( "unrecognized vertex layout type" );
		}
	}

	for ( unsigned int i = 0; i < vertices.getLength(); i++ ) {
		for ( int j = 0; j < J; j++ ) {
			const Format & f = F[j];
			float * data = 0;
			int inputSize = 3;
			if ( t[j] == POSITION ) {
				data = (float*)&vertices[i].position;
			} else if ( t[j] == NORMAL ) {
				data = (float*)&vertices[i].normal;
			} else if ( t[j] == TEX0 ) {
				data = (float*)&vertices[i].uv;
			} else if ( t[j] == TANGENT ) {
				data = (float*)&vertices[i].tangent;
			} else if ( t[j] == LODPOS ) {
				data = (float*)&vertices[i].lodPosition;
			} else if ( t[j] == LODNORMAL ) {
				data = (float*)&vertices[i].lodNormal;
			} else if ( t[j] == COLOR ) {
				data = (float*)&vertices[i].color;
			} else {
				POOP_WARNING( "unknown vertex attribute - skipping it" );
				continue;
			}

			FORMAT_TYPE outFmt = f.fmt;
			int outputSize = f.num;
			if ( outFmt == FORMAT_TYPE_FLOAT ) {
				for ( int k = 0; k < outputSize; k++ ) {
					if ( k < inputSize ) {
						bs.append( data[k] );
					} else {
						if ( k < 3 ) bs.append( 0.0f );
						else bs.append( 1.0f );
					}
				}
			} else if ( outFmt == FORMAT_TYPE_UBYTE ) {
				for ( int k = 0; k < outputSize; k++ ) {
					if ( k < inputSize ) {
						if ( f.normalized ) {
							bs.append( (char)(data[k] * 255.0f) );
						} else {
							bs.append( (char)data[k] );
						}
					} else {
						if ( k < 3 ) bs.append( (char)0 );
						else bs.append( (char)0xff );
					}
				}
			}
		}
	}
}


void MeshData::triangulate() {
	DynamicArray<Face> triangles(faces.getLength()*2);

	for( unsigned int i = 0; i < faces.getLength(); i++ ) {
		Face & n = faces[i];
		if ( n.v[3].i == -1 ) {
			triangles.append( n );
		} else {
			Face a = n, b = n;
			a.v[0] = n.v[0];
			a.v[1] = n.v[1];
			a.v[2] = n.v[2];
			a.v[3].i = -1;
			b.v[0] = n.v[2];
			b.v[1] = n.v[3];
			b.v[2] = n.v[0];
			b.v[3].i = -1;

			triangles.append( a );
			triangles.append( b );
		}
	}

	faces = triangles.ToArray();
}

Array<int> MeshData::createTriangleIndices() {
	DynamicArray<int> triangles(faces.getLength()*2);

	for( unsigned int i = 0; i < faces.getLength(); i++ ) {
		Face & n = faces[i];
		if ( n.v[3].i == -1 ) {
			triangles.append( n.v[0].i );
			triangles.append( n.v[1].i );
			triangles.append( n.v[2].i );
		} else {
			Face a = n, b = n;
			triangles.append( n.v[0].i );
			triangles.append( n.v[1].i );
			triangles.append( n.v[2].i );
			triangles.append( n.v[2].i );
			triangles.append( n.v[3].i );
			triangles.append( n.v[0].i );
		}
	}

	return triangles.ToArray();
}

void MeshData::computeVertexTangents() {
	for ( unsigned int i = 0; i < vertices.getLength(); i++ ) { 
		vertices[i].tangent = Vector4(0,0,0,0);
	}
	Array<Vector3> ts( vertices.getLength() );
	Array<Vector3> bs( vertices.getLength() );
	for ( unsigned int i = 0; i < faces.getLength(); i++ ) {
		for( int v = 0; v < 4 && faces[i].v[v].i != -1; v++ ) {
			Vector3 t0 = faces[i].v[v].tangent0;
			Vector3 t1 = faces[i].v[v].tangent1;
			ts[faces[i].v[v].i] += t0;
			bs[faces[i].v[v].i] += t1;
		}
	}
	for ( unsigned int i = 0; i < vertices.getLength(); i++ ) { 
		// orthogonalize
		Vector3 & N = vertices[i].normal;
		Vector3 & T = ts[i];
		Vector3 & B = bs[i];
		Vector3 oT = T - N * N.dot(T);
		Vector3 oB = B - N * N.dot(B) - oT * oT.dot(B) / T.dot(T);
		//vertices[i].tangent = Vector4( oT.x, oT.y, oT.z, N.cross(oT).dot( oB ) );
		vertices[i].tangent = Vector4( T.x, T.y, T.z, N.cross(oT).dot( oB ) );
	}
}

void MeshData::processForRender( VertexLayout * vertexLayout ) {
	if ( vertexLayout->hasUVs() ) {
		generateUniqueVertices();
		if ( vertexLayout->hasTangents() ) {
			computeVertexTangents();
		}
	}
}

Mesh * MeshData::createMesh( Renderer * renderer, VertexLayout * vertexLayout ) {
	int vertDataSize = vertexLayout->getStride() * vertices.getLength();

	ByteStream bs;

	outputVerticesToByteStream( bs, vertexLayout );

	Mesh * ret = new Mesh();
	ret->m_vertices = renderer->createVertexBufferStatic( vertexLayout->getStride(), vertices.getLength(), bs.getDataPtr() );

	Array<int> indices = createTriangleIndices();
	ret->m_indices = renderer->createIndexBufferStatic( indices.getLength(), indices.getData() );

	ret->boundingSphere = computeBoundingSphere();

	return ret;
}

void MeshData::updateMesh( Renderer * renderer, VertexLayout * vertexLayout, Mesh * dstMesh ) {
	int vertDataSize = vertexLayout->getStride() * vertices.getLength();
	int destSize = dstMesh->m_vertices->GetSize() * dstMesh->m_vertices->GetStride();
	POOP_ASSERT( destSize >= vertDataSize );

	ByteStream bs( dstMesh->m_vertices->lock(), destSize );
	outputVerticesToByteStream( bs, vertexLayout );
	dstMesh->m_vertices->unlock();
	dstMesh->m_vertices->setActiveElements( vertices.getLength() );

	Array<int> indices = createTriangleIndices();

	POOP_ASSERT( dstMesh->m_indices->GetStride() * dstMesh->m_indices->GetSize() >= sizeof(int)*indices.getLength() );

	void * inds = dstMesh->m_indices->lock();
	memcpy( inds, indices.getDataConst(), sizeof(int) * indices.getLength() );
	dstMesh->m_indices->unlock();
	dstMesh->m_indices->setActiveElements( indices.getLength() );
}

void MeshData::generateUniqueVertices() {
	HashMap<Face::V, int> uniqueVertices( 4 * (4 * faces.getLength()) / 3 );

	int vertId = 0;
	for( unsigned int i = 0; i < faces.getLength(); i++ ) {
		Face & face = faces[i];
		for( int v = 0; v < 4; v++ ) {
			if ( face.v[v].i == -1 ) continue;
			if ( !uniqueVertices.contains( face.v[v] ) ) {
				uniqueVertices.add( face.v[v], vertId );
				vertId++;
			}
		}
	}

	Array<Vertex> nVertices(vertId);
	HashMap<Face::V, int>::KeyValuePair kvp;
	uniqueVertices.resetIteration();
	while( uniqueVertices.iterateNext( &kvp ) ) {
		Face::V & v = *kvp.key;
		int nvid = *kvp.value;
		nVertices[nvid] = vertices[v.i];
		nVertices[nvid].uv = v.uv;
	}

	// Map old vertex ids to new ids
	Array<Face> nFaces(faces.getLength());
	for( unsigned int i = 0; i < faces.getLength(); i++ ) {
		Face & face = faces[i];
		for( int v = 0; v < 4; v++ ) {
			int nid = uniqueVertices.get(face.v[v]);
			face.v[v].i = nid;
		}
	}

	vertices = nVertices;
}

enum MaterialDataType {
	END = 0,
	VECTOR4 = 1,
	FLOAT = 2,
	IMG = 3
};

MaterialData MaterialData::fromStream( ByteStream & bs ) {

	MaterialData nmat;

	String prop;
	MaterialDataType type;
	do {
		type = (MaterialDataType)bs.read<int>();
		if ( type != END ) {
			prop = bs.readString();
			int len = bs.read<int>();
			if ( type == IMG ) {
				String fileName = bs.readString();
				ImageData * img = ImageImporter::importPng( fileName );
				img->generateMips();
				nmat.images.add( prop, img ); 
			} else if ( type == VECTOR4 ) {
				nmat.vecs.add( prop, bs.read<Vector4>() );
			} else if ( type == FLOAT ) {
				nmat.values.add( prop, bs.read<float>() );
			} else {
				POOP_ERROR( String( "Unknown material entry type: " ) + String(type) );
			}
		}
	} while( type != END );

	return nmat;
}

Material * MaterialData::createMaterial( Renderer * renderer, Shader * shader ) {

	Material * ret = new Material( shader );

	ret->vecs = this->vecs;
	ret->values = this->values;

	this->images.resetIteration();
	
	HashMap<String, ImageData*>::KeyValuePair it;
	while( this->images.iterateNext( &it ) ) {
		ret->textures.add( *it.key, renderer->createTexture( *it.value ) );
	}

	return ret;

}

void MeshData::displace( ImageData * imgData, float midLevel, float strength ) {
	//for ( int i = 0; i < vertices.getLength(); i++ ) {
	for ( int f = 0; f < faces.getLength(); f++ ) {
		for ( int i = 0; i < 4 && faces[f].v[i].i != -1; i++ ) {
			Face::V & V = faces[f].v[i];
			Vertex & v = vertices[V.i];
			//Vector2 uv = V.uv;
			Vector2 uv = Vector2( atan2f( v.position.z, v.position.x ) / 6.28f, atan2f( v.position.x, v.position.y ) / 6.28f );
			float px = fmodf( uv.x * imgData->getWidth(), (float)imgData->getWidth() );
			float py = fmodf( uv.y * imgData->getHeight(), (float)imgData->getHeight() );
			if ( px < 0 ) px += imgData->getWidth();
			if ( py < 0 ) py += imgData->getHeight();
			float d = (((float)imgData->getData(0)[((int)py * imgData->getHeight() + (int)px) * imgData->getDepth() + 1]) / 255 - midLevel)  * strength;
			//float d = 
			v.position += v.normal * d;
		}
	}
	this->computeNormals();
}

void MeshData::computeLodPositions( MeshData * lowerLod ) {
	for ( int i = 0; i < vertices.getLength(); i++ ) {
		Vertex & v = vertices[i];
		if ( v.lodElement < lowerLod->vertices.getLength() ) {
			v.lodPosition = lowerLod->vertices[v.lodElement].position;
			v.lodNormal = lowerLod->vertices[v.lodElement].normal;
		} else if ( v.lodElement < lowerLod->vertices.getLength() + lowerLod->faces.getLength() ) {
			int fi = v.lodElement - lowerLod->vertices.getLength();
			Face & f = lowerLod->faces[fi];
			Vector3 & p0 = lowerLod->vertices[f.v[0].i].position;
			Vector3 & p1 = lowerLod->vertices[f.v[1].i].position;
			Vector3 & p2 = lowerLod->vertices[f.v[2].i].position;
			Vector3 & n0 = lowerLod->vertices[f.v[0].i].normal;
			Vector3 & n1 = lowerLod->vertices[f.v[1].i].normal;
			Vector3 & n2 = lowerLod->vertices[f.v[2].i].normal;
			if ( f.v[3].i != -1 ) {
				Vector3 & p3 = lowerLod->vertices[f.v[3].i].position;
				Vector3 & n3 = lowerLod->vertices[f.v[3].i].normal;
				v.lodPosition = (p0 + p1 + p2 + p3) * 0.25f;
				v.lodNormal = (n0 + n1 + n2 + n3) * 0.25f;
			} else {
				v.lodPosition = (p0 + p1 + p2) / 3.0f;
				v.lodNormal = (n0 + n1 + n2) / 3.0f;
			}
		} else {
			int E = v.lodElement - lowerLod->vertices.getLength() - lowerLod->faces.getLength();
			int f = E/4;
			int e = E%4;
			int i0 = lowerLod->faces[f].v[e].i;
			int i1 = lowerLod->faces[f].v[(e+1)%4].i;
			if ( i1 == -1 ) i1 = lowerLod->faces[f].v[0].i;
			Vector3 & p0 = lowerLod->vertices[i0].position;
			Vector3 & p1 = lowerLod->vertices[i1].position;
			Vector3 & n0 = lowerLod->vertices[i0].normal;
			Vector3 & n1 = lowerLod->vertices[i1].normal;
			v.lodPosition = (p0 + p1) * 0.5f;
			v.lodNormal = (n0 + n1) * 0.5f;
		}
	}
}

Sphere MeshData::computeBoundingSphere() {
	int minx = 0, miny = 0, minz = 0;
	int maxx = 0, maxy = 0, maxz = 0;
	for ( int i = 1; i < vertices.getLength(); i++ ) {
		Vector3 & p = vertices[i].position;
		if ( p.x < vertices[minx].position.x ) minx = i;
		if ( p.y < vertices[miny].position.y ) miny = i;
		if ( p.z < vertices[minz].position.z ) minz = i;
		if ( p.x > vertices[minx].position.x ) maxx = i;
		if ( p.y > vertices[miny].position.y ) maxy = i;
		if ( p.z > vertices[minz].position.z ) maxz = i;
	}
	float dx = (vertices[minx].position - vertices[maxx].position).lengthSq();
	float dy = (vertices[miny].position - vertices[maxy].position).lengthSq();
	float dz = (vertices[minz].position - vertices[maxz].position).lengthSq();
	int min = minx;
	int max = maxx;
	if ( dy > dx && dy > dz ) {
		min = miny;
		max = maxy;
	}
	if ( dz > dy && dz > dx ) {
		min = miny;
		max = maxy;
	}

	Sphere s(vertices[min].position, vertices[max].position);
	for ( int i = 0; i < vertices.getLength(); i++ ) {
		s = s.add( vertices[i].position );
	}
	return s;
}


}

template<> unsigned int poopie_hash<gfx::Face::V>( const gfx::Face::V & s ) {
	gfx::Face::V r = s;
	r.tangent0 = Vector3(0,0,0);
	r.tangent1 = Vector3(0,0,0);
	return Hash::superFastHash( &r, sizeof(r) );
}

}




