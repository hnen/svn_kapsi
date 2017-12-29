
#ifndef __SPACE_TERRAIN
#define __SPACE_TERRAIN

#include "poopie_base.h"

namespace space {

	class TerrainRenderer;
	class SpaceEngine;

	class SpaceTerrain {
	public:

		SpaceTerrain( SpaceEngine * engine );
		void render();

		bool raycast( Vector2 origin, Vector2 dir, float length, Vector2 * hitPos, Vector2 * hitNormal );
		char getValByWorldPos( Vector2 worldPos );
		char getValByMapPos( int ix, int iy );

		float getMinX() const { return -m_width*m_blockWidth*0.5f; }
		float getMaxX() const { return m_width*m_blockWidth*0.5f; }
		float getMinY() const { return -m_height*m_blockHeight*0.5f; }
		float getMaxY() const { return m_height*m_blockHeight*0.5f; }

		int getWidth() const { return m_width; }
		int getHeight() const { return m_height; }
		float getBlockWidth() const { return m_blockWidth; }

		void setBlock( int x, int y, char c );

		Vector2 mapToWorld( float x, float y );
		Vector2 worldToMap( Vector2 & pos );

		void updateGraphics();

		void exportTerrain();

	private:

		
		char getValByMapPos( Vector2 mapPos );

		void p_init();

		int m_width, m_height;
		Array<char> m_map;

		float m_blockWidth, m_blockHeight;

		SpaceEngine * m_engine;
		TerrainRenderer * m_renderer;
	};

}


#endif

