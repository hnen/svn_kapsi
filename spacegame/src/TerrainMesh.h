
#ifndef __SPACE_TERRAIN_MESH_H
#define __SPACE_TERRAIN_MESH_H

#define BLOCK_SIZE 16

#include "poopie_base.h"
#include "poopie_gfx.h"


#if 1

namespace space {

	class SpaceTerrain;
	class SpaceEngine;

	struct TerrainFace;
	struct TerrainVertex;
	struct VertexMergeGroup;

	struct TerrainEdge { 
		TerrainEdge() {}
		TerrainEdge( TerrainVertex * a, TerrainVertex * b ) {
			unsigned int ia = (unsigned int)a;
			unsigned int ib = (unsigned int)b;
			if ( ia <= ib ) {
				this->a = a;
				this->b = b;
			} else {
				this->a = b;
				this->b = a;
			}
		}

		TerrainVertex * a, * b;

		bool operator == ( const TerrainEdge & a ) const {
			return a.a == this->a && b == a.b;
		}
	};

	class TerrainMesh {
	public:
		TerrainMesh( SpaceTerrain * terrain, SpaceEngine * engine );
		
		void updateMesh();
		void markRemoved( int tx, int ty );
		void createMeshBlock( int bx, int by );

		int getMeshCount() { return m_meshes.getLength(); }
		Mesh * getMesh( int i ) { return m_meshes[i]; }
		Sphere getBounds( int i );
		
		void exportTerrain();

	private:
		void appendFace( TerrainFace * f, TerrainVertex * a, TerrainVertex * b, TerrainVertex * c, TerrainVertex * d, DynamicArray<Vertex> & outVerts, DynamicArray<Face> & outFaces, HashMap<TerrainVertex*, int> & vertexIndex, HashMap<TerrainVertex*, int> & closingVertex );

		void createMesh();
		void merge( TerrainVertex * a, TerrainVertex * b );
		TerrainVertex * getRealVertex( TerrainVertex * v );
		void detectAndMergeCorner( int x, int y );
		void removeMergeGroup( VertexMergeGroup * vmg );
		void generateDistanceField();
		bool isBorderVertex( int cx, int cy );
		IntVec2 worldToBlock( Vector2 w );
		Vector2 blockToWorld( int x, int y );
		float GetVertZ( int cx, int cy );
		void setDepths();
		void computeNormals();

		int m_blocksX, m_blocksY;
		int tw, th;

		Array<Mesh*> m_meshes;
		Array<MeshData*> m_meshdatas;
		Array<MeshData*> m_meshdatasNonMerged;

		Array<float> m_distanceField;

		SpaceTerrain * m_terrain;
		SpaceEngine * m_engine;

		void addFace( TerrainFace * nface, int tx, int ty );
		void removeFace( TerrainFace * face );
		void removeVertex( TerrainVertex * v );
		HashSet<TerrainFace*> faces;
		//HashMap<IntVec2, TerrainFace*> voxelToFace;
		Array<TerrainFace*> voxelToFace;
		Array< HashSet<TerrainFace*> > blockFaces;

		HashSet<TerrainVertex*> verts;
		//HashMap<IntVec2, TerrainVertex*> cornerToVert;
		Array<TerrainVertex*> cornerToVert;
		HashMap<TerrainVertex*, int> faceCounts;

		HashSet<IntVec2> potentialCorner;
		HashSet<IntVec2> removed;
		HashSet<VertexMergeGroup*> mergeGroupCollection;
		HashMap<TerrainVertex*,VertexMergeGroup*> vertexToMergeGroup;
		HashMap<TerrainEdge,int> edgeFaceCount;
		bool isOpenEdge( TerrainVertex * a, TerrainVertex * b );

	};

}


#else


namespace space {

	struct TerrainCorner;
	class SpaceTerrain;
	class SpaceEngine;

	struct VoxelCorner {
		VoxelCorner() {}
		VoxelCorner ( int x, int y ) : x(x), y(y) {}
		int x, y;
		bool operator == ( const VoxelCorner & a ) const {
			return x == a.x && y == a.y;
		}

	};

	struct VoxelEntry {
		VoxelEntry() : merged(false), closingVertex(-1) {}
		VoxelEntry( int id ) : index(id), merged(false), closingVertex(-1) {}
		int index;
		int closingVertex;
		bool merged;
	};

	struct TerrainVertex {
		TerrainVertex() {}
		TerrainVertex( Vector3 pos, VoxelCorner vc ) : pos(pos), vc(vc) {}
		Vector3 pos;
		Vector3 normal;
		VoxelCorner vc;
	};

	class TerrainMesh {
	public:

		TerrainMesh( SpaceTerrain * terrain, SpaceEngine * engine );
		void updateMesh( int minX, int minY, int maxX, int maxY );

		int getMeshCount() { return m_meshes.getLength(); }
		Mesh * getMesh( int i ) { return m_meshes[i]; }

	private:

		void computeDistanceField();
		void generateMesh( int mx, int my );
		void generateClosingFaces();
		void computeNormals();

		void genMesh( bool );
		bool matchesCorner( const TerrainCorner * tc );
		TerrainCorner * matchCorner( int X, int Y );

		MeshData * m_terrainMeshData;
		
		DynamicArray<TerrainVertex> verts;
		DynamicArray<Face> faces;

		HashMap<VoxelCorner, VoxelEntry> map;

		//Mesh * m_terrainMesh;
		Array<Mesh*> m_meshes;

		Array<float> m_distField;
		DynamicArray<TerrainCorner> corners;

		SpaceTerrain * m_terrain;
		SpaceEngine * m_engine;

	};



}


#endif

#endif

