
#include "poopie/base/DynamicArray.h"
#include "poopie/base/HashMap.h"
#include "MeshData.h"


namespace poopie {
namespace gfx {

	struct Edge {
		int i0, i1;

		Edge() {}
		Edge( int i0, int i1 ) {
			if ( i0 > i1 ) {
				this->i0 = i1;
				this->i1 = i0;
			} else {
				this->i0 = i0;
				this->i1 = i1;
			}
		}

		bool operator == ( const Edge & e ) const {
			return i0 == e.i0 && i1 == e.i1;
		}

	};

	void MeshData::subdivide( bool smooth ) {

		DynamicArray<Vertex> nverts( vertices );

		HashMap<Edge, DynamicArray<int>> edgeToFacePoint( 2 * faces.getLength() );
		Array<DynamicArray<int>> vertIdToFacePoints(vertices.getLength());
		Array<DynamicArray<int>> vertIdToEdgePoints(vertices.getLength());
		HashMap<Edge, int> edgeToEdgePoint( 2 * faces.getLength() );

		Array<DynamicArray<Edge>> vertIdToEdges(vertices.getLength());

		int numVertsOrig = vertices.getLength();

		for( int i = 0; i < nverts.getLength(); i++ ) {
			nverts[i].lodElement = i;
		}

		Array<int> V(vertices.getLength());
		for( int i = 0; i < vertices.getLength(); i++ ) {
			V[i] = 0;
		}

		// face centroids
		for( int i = 0; i < faces.getLength(); i++ ) {
			Face::V & v0 = faces[i].v[0];
			Face::V & v1 = faces[i].v[1];
			Face::V & v2 = faces[i].v[2];
			Face::V & v3 = faces[i].v[3];
			int nid = nverts.getLength();
			if ( v3.i == -1 ) {
				nverts.append( (vertices[v0.i] + vertices[v1.i] + vertices[v2.i]) * (1.0f/3.0f) );
				edgeToFacePoint[Edge( v0.i, v1.i )].append(nid);
				edgeToFacePoint[Edge( v1.i, v2.i )].append(nid);
				edgeToFacePoint[Edge( v2.i, v0.i )].append(nid);
				vertIdToFacePoints[v0.i].append(nid);
				vertIdToFacePoints[v1.i].append(nid);
				vertIdToFacePoints[v2.i].append(nid);

				V[v0.i]++;
				V[v1.i]++;
				V[v2.i]++;

				vertIdToEdges[v0.i].append( Edge(v0.i, v1.i) );
				vertIdToEdges[v0.i].append( Edge(v2.i, v0.i) );
				vertIdToEdges[v1.i].append( Edge(v0.i, v1.i) );
				vertIdToEdges[v1.i].append( Edge(v1.i, v2.i) );
				vertIdToEdges[v2.i].append( Edge(v1.i, v2.i) );
				vertIdToEdges[v2.i].append( Edge(v2.i, v0.i) );

			} else {
				nverts.append( (vertices[v0.i] + vertices[v1.i] + vertices[v2.i] + vertices[v3.i]) * 0.25f );
				edgeToFacePoint[Edge( v0.i, v1.i )].append(nid);
				edgeToFacePoint[Edge( v1.i, v2.i )].append(nid);
				edgeToFacePoint[Edge( v2.i, v3.i )].append(nid);
				edgeToFacePoint[Edge( v3.i, v0.i )].append(nid);
				vertIdToFacePoints[v0.i].append(nid);
				vertIdToFacePoints[v1.i].append(nid);
				vertIdToFacePoints[v2.i].append(nid);
				vertIdToFacePoints[v3.i].append(nid);

				V[v0.i]++;
				V[v1.i]++;
				V[v2.i]++;
				V[v3.i]++;

				vertIdToEdges[v0.i].append( Edge(v0.i, v1.i) );
				vertIdToEdges[v0.i].append( Edge(v3.i, v0.i) );
				vertIdToEdges[v1.i].append( Edge(v0.i, v1.i) );
				vertIdToEdges[v1.i].append( Edge(v1.i, v2.i) );
				vertIdToEdges[v2.i].append( Edge(v1.i, v2.i) );
				vertIdToEdges[v2.i].append( Edge(v2.i, v3.i) );
				vertIdToEdges[v3.i].append( Edge(v2.i, v3.i) );
				vertIdToEdges[v3.i].append( Edge(v3.i, v0.i) );
			}
			nverts[nverts.getLength() - 1].lodElement = vertices.getLength() + i;
		}
		
		Array<DynamicArray<Vector3>> edgePoints( vertices.getLength() );
		Array<DynamicArray<Vector3>> edgePoints2d( vertices.getLength() );
		HashMap<Edge,bool> addedEdges( 2 * faces.getLength() );
		
		Array<bool> verts2d( vertices.getLength() );
		for( int i = 0; i < vertices.getLength(); i++ ) {
			verts2d[i] = false;
		}

		for( int i = 0; i < faces.getLength(); i++ ) {
			int N = faces[i].v[3].i == -1 ? 3 : 4;
			for( int v = 0; v < N; v++ ) {
				int v0 = faces[i].v[v].i, v1 = faces[i].v[(v+1)%N].i;
				Edge e = Edge( v0, v1 );
				if ( !addedEdges.contains( e ) ) {
					addedEdges.add( e, true );
					Vertex F_a;
					int numFacePoints = edgeToFacePoint[e].getLength();
					for( int ei = 0; ei < numFacePoints; ei++ ) {
						F_a = F_a + nverts[edgeToFacePoint[e][ei]];
					}
					F_a = F_a * (1.0f / (float)numFacePoints);
										
					//F_a = (((vertices[v0] + vertices[v1]) * 0.5f));
					if ( numFacePoints > 1 ) {
						F_a = (F_a + ((vertices[v0] + vertices[v1]) * 0.5f)) * 0.5f;
						edgePoints[v0].append(F_a.position);
						edgePoints[v1].append(F_a.position);
					} else {
						verts2d[v0] = true;
						verts2d[v1] = true;
						F_a = (vertices[v0] + vertices[v1]) * 0.5f;
						edgePoints2d[v0].append(F_a.position);
						edgePoints2d[v1].append(F_a.position);
					}

					//F_a = (F_a * (float)(numFacePoints-1) + ((vertices[v0] + vertices[v1]) * 0.5f)) * (1.0f / numFacePoints);

					//F_a = (F_a * 2.0f + ((nverts[v0] + nverts[v1]) * 0.5f)) * (1.0f / 3.0f);
					//F_a = (F_a * (numFacePoints-1) + ((nverts[v0] + nverts[v1]) * 0.5f)) * (1.0f / ( numFacePoints ));
					//F_a = (F_a * (numFacePoints) + ((nverts[v0] + nverts[v1]) * 0.5f)) * (1.0f / ( numFacePoints + 1));


					//F_a = (F_a * 2.0f + ((nverts[v0] + nverts[v1]) * 0.5f)) * (1.0f / 3.0f);
					F_a.lodElement = vertices.getLength() + faces.getLength() + i*4+v;
					nverts.append( F_a );
					edgeToEdgePoint[e] = nverts.getLength() - 1;
					vertIdToEdgePoints[v0].append( nverts.getLength() - 1 );
					vertIdToEdgePoints[v1].append( nverts.getLength() - 1 );
				}
			}
		}

		for( int i = 0; i < vertices.getLength(); i++ ) {
			Vertex F;
			int numFacePoints = vertIdToFacePoints[i].getLength();
			for ( int j = 0; j < numFacePoints; j++ ) {
				Vertex fp = nverts[vertIdToFacePoints[i][j]];
				F = F + fp;
			}
			F = F * (1.0f / (float)numFacePoints);

			Vertex R;
			//int numEdgePoints = vertIdToEdges[i].getLength();
			int numEdgePoints;
			if ( !verts2d[i] ) {
				numEdgePoints = edgePoints[i].getLength();
				for ( int j = 0; j < numEdgePoints; j++ ) {
					Vertex ep;
					ep.position = edgePoints[i][j];
					R = R + ep;
				}
			} else {
				numEdgePoints = edgePoints2d[i].getLength();
				for ( int j = 0; j < numEdgePoints; j++ ) {
					Vertex ep;
					ep.position = edgePoints2d[i][j];
					R = R + ep;
				}
			}
			R = R * (1.0f / (float)numEdgePoints);

			//nverts[i] = (F + R * 2.0f + vertices[i]) * (1.0f / 4.0f);
			//nverts[i] = (F + R * 2.0f + nverts[i] * (V[i]-1)) * (1.0f / (3+V[i]-1));
			//nverts[i] = (F + R * 2.0f + vertices[i] * (numFacePoints)) * (1.0f / (3.0f + numFacePoints));
			if ( verts2d[i] ) {
				nverts[i] = (R * (1.0f/numEdgePoints) + vertices[i] * (((float)(numEdgePoints-1))/(numEdgePoints)) );
			} else {
				nverts[i] = (F * (1.0f / numFacePoints) + R * (2.0f/numFacePoints) + vertices[i] * (((float)(numFacePoints-3))/(numFacePoints)) );
			}
			//nverts[i] = vertices[i];
			nverts[i].lodElement = i;
		}

		DynamicArray<Face> nfaces;

		for ( int i = 0; i < faces.getLength(); i++ ) {
			Face & f = faces[i];
			Face::V & v0 = f.v[0];
			Face::V & v1 = f.v[1];
			Face::V & v2 = f.v[2];
			Face::V & v3 = f.v[3];

			Face::V fp(numVertsOrig + i);
			fp.uv = (v0.uv + v1.uv + v2.uv + v3.uv) * 0.25f;
			Face::V e01(edgeToEdgePoint[Edge(v0.i, v1.i)]);
			e01.uv = (v0.uv + v1.uv) * 0.5f;
			Face::V e12(edgeToEdgePoint[Edge(v1.i, v2.i)]);
			e12.uv = (v1.uv + v2.uv) * 0.5f;

			if ( f.v[3].i != -1 ) {

				// quad
				Face::V e23(edgeToEdgePoint[Edge(v2.i, v3.i)]);
				e23.uv = (v2.uv + v3.uv) * 0.5f;
				Face::V e30(edgeToEdgePoint[Edge(v3.i, v0.i)]);
				e30.uv = (v3.uv + v0.uv) * 0.5f;

				nfaces.append( Face(v0, e01, fp, e30) );
				nfaces.append( Face(v1, e12, fp, e01) );
				nfaces.append( Face(v2, e23, fp, e12) );
				nfaces.append( Face(v3, e30, fp, e23) );

			} else {
				Face::V e20(edgeToEdgePoint[Edge(v2.i, v0.i)]);
				e20.uv = (v2.uv + v0.uv) * 0.5f;

				nfaces.append( Face(v0, e01, fp, e20) );
				nfaces.append( Face(v1, e12, fp, e01) );
				nfaces.append( Face(v2, e20, fp, e12) );
			}
			

		}

		vertices = nverts.ToArray();
		faces = nfaces.ToArray();

	}

}
}
