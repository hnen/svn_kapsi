
#include "poopie_gfx.h"
#include "TerrainGen.h"
#include "SpaceTerrain.h"
#include "SpaceEngine.h"
#include "TerrainRenderer.h"

namespace space {

	SpaceTerrain::SpaceTerrain( SpaceEngine * engine ) : m_engine(engine) {
		p_init();
	}


	void SpaceTerrain::p_init() {

		m_map = TerrainGen::generate( 32, 32, 3, 20 );
		m_width = 32 << 3;
		m_height = 32 << 3;

		//m_map = TerrainGen::generate( 32, 32, 1, 20 );
		//m_width = 32 << 1;
		//m_height = 32 << 1;


		m_blockWidth = 0.2f;
		m_blockHeight = 0.2f;

		m_renderer = new TerrainRenderer( this, m_engine );

	}

	void SpaceTerrain::exportTerrain() {
		m_renderer->exportTerrain();
	}

	Vector2 SpaceTerrain::mapToWorld( float x, float y ) {
		float wwidth = (float)m_width * m_blockWidth;
		float wheight = (float)m_height * m_blockHeight;

		float fx = x*m_blockWidth - wwidth * 0.5f + m_blockWidth * 0.5f;
		float fy = y*m_blockHeight - wheight * 0.5f + m_blockHeight * 0.5f;

		return Vector2(fx,fy);
	}

	void SpaceTerrain::setBlock( int x, int y, char c ) {
		if ( x >= 0 && y >= 0 && x < m_width && y < m_height ) {
			char & ov = m_map[y*m_width+x];
			if ( ov && !c ) {
				m_renderer->markRemoved( x, y );
			}
			ov = c;
		}
	}

	void SpaceTerrain::updateGraphics() {
		m_renderer->updateMesh();
	}

	Vector2 SpaceTerrain::worldToMap( Vector2 & pos ) {
		float wwidth = (float)m_width * m_blockWidth;
		float wheight = (float)m_height * m_blockHeight;

		float x = (pos.x + wwidth * 0.5f - m_blockWidth * 0.5f)/m_blockWidth;
		float y = (pos.y + wheight * 0.5f - m_blockHeight * 0.5f)/m_blockHeight;
		return Vector2(x,y);
	}
	char SpaceTerrain::getValByMapPos( int ix, int iy ) {
		return m_map[iy * m_width + ix];
	}

	char SpaceTerrain::getValByMapPos( Vector2 mapPos ) {
		int ix = (int)mapPos.x;
		int iy = (int)mapPos.y;
		return getValByMapPos(ix,iy);
	}

	char SpaceTerrain::getValByWorldPos( Vector2 worldPos ) {
		return getValByMapPos( worldToMap( worldPos ) );
	}

	bool SpaceTerrain::raycast( Vector2 origin, Vector2 dir, float length, Vector2 * hitPos, Vector2 * hitNormal ) {
		dir = dir.normalized();

		Vector2 mapPos = worldToMap( origin );
		
		//mapPos = mapPos - dir * 0.001f;

		int ix = (int)mapPos.x;
		int iy = (int)mapPos.y;

		float totalL = 0;
		Vector2 normal;
		while( !getValByMapPos( ix,iy ) ) {
			float L = FLT_MAX;
			if ( dir.x > 0 ) {
				float destx = ix + 1;
				float l = (destx - mapPos.x) / dir.x;
				if ( l < L ) {
					L = l;
					normal = Vector2(-1,0);
				}
			} else if ( dir.x < 0 ) {
				float destx = ix - 1;
				float l = (destx - mapPos.x) / dir.x;
				if ( l < L ) {
					L = l;
					normal = Vector2(1,0);
				}
			}
			if ( dir.y > 0 ) {
				float desty = iy + 1;
				float l = (desty - mapPos.y) / dir.y;
				if ( l < L ) {
					L = l;
					normal = Vector2(0,-1);
				}
			} else if ( dir.y < 0 ) {
				float desty = iy - 1;
				float l = (desty - mapPos.y) / dir.y;
				if ( l < L ) {
					L = l;
					normal = Vector2(0,1);
				}
			}

			totalL += L * m_blockWidth;
			mapPos = mapPos + dir * L;

			ix += -(int)normal.x;
			iy += -(int)normal.y;

			if ( totalL > length ) {
				return false;
			}
		}

		if( hitPos ) {
			*hitPos = mapToWorld(mapPos.x, mapPos.y);
		}

		Vector2 blockCenter = Vector2((float)ix + 0.5f, (float)iy + 0.5f);
		Vector2 D = mapPos - blockCenter;
		float l = dir.x > 0 ? -D.x : 0;
		float r = dir.x < 0 ? D.x : 0;
		float d = dir.y > 0 ? -D.y : 0;
		float u = dir.y < 0 ? D.y : 0;
		Vector2 norm;
		if ( l > r && l > u && l > d ) {
			norm = Vector2(-1,0);
		}  else if ( r > u && r > d ) {
			norm = Vector2(1,0);
		} else if ( u > d ) {
			norm = Vector2(0,1);
		} else {
			norm = Vector2(0,-1);
		}

		if ( norm.dot( dir ) > 0 ) {
			return false;
		}

		if ( hitNormal ) {
			*hitNormal = norm;
		}

		return true;
	}
	
	void SpaceTerrain::render() {
		m_renderer->render();
	}


}

