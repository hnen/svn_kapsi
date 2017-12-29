
#ifndef __SPACE_TERRAIN_RENDERER
#define __SPACE_TERRAIN_RENDERER

namespace space {

	class TerrainMesh;
	class SpaceTerrain;
	class SpaceEngine;


	class TerrainRenderer {
	public:

		TerrainRenderer( SpaceTerrain * terrain, SpaceEngine * engine );
		void render();
		void updateMesh();

		void markRemoved( int tx, int ty );

		void exportTerrain();

	private:

		TerrainMesh * m_mesh;

		SpaceTerrain * m_terrain;
		SpaceEngine * m_engine;




	};

}


#endif
