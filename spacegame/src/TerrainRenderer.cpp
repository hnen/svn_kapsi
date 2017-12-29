
#include "TerrainMesh.h"


#include "poopie_base.h"
#include "poopie_gfx.h"

#include "SpaceTerrain.h"
#include "SpaceEngine.h"
#include "TerrainRenderer.h"

namespace space {


	TerrainRenderer::TerrainRenderer( SpaceTerrain * terrain, SpaceEngine * engine ) : m_terrain(terrain), m_engine(engine) {
		m_mesh = new TerrainMesh( terrain, engine );
	}

	void TerrainRenderer::updateMesh() {
		m_mesh->updateMesh();
	}

	void TerrainRenderer::markRemoved( int tx, int ty ) {
		m_mesh->markRemoved( tx, ty );
	}

	void TerrainRenderer::exportTerrain() {
		m_mesh->exportTerrain();
	}

	void TerrainRenderer::render() {

		SpaceResources * res = m_engine->getResources();
		Renderer * renderer = m_engine->getRenderer();

		/*
		Frustum f = Frustum::fromMatrix( m_engine->getViewProj() );

		int width = m_terrain->getWidth();
		int height = m_terrain->getHeight();
		float blockWidth = m_terrain->getBlockWidth();

		Shader * shader = res->shader_basic;
		renderer->setActiveVertexLayout( res->vlayout_basic );
		for( int y = 0; y < height; y++ ) {
			for ( int x = 0; x < width; x++ ) {
				if ( m_terrain->getValByMapPos(x,y) ) {
					Vector2 wpos = m_terrain->mapToWorld(x,y);
					if ( f.intersects( Sphere( Vector3(wpos.x,wpos.y,0), blockWidth ) ) ) {
						Matrix4 w = Matrix4::scale( Vector3(blockWidth, blockWidth, blockWidth) * 0.5f ) * Matrix4::translate( Vector3(wpos.x,wpos.y,0) );

						shader->setVariable( "wv", w*m_engine->getView() );
						shader->setVariable( "wvp", w*m_engine->getViewProj() );
						renderer->setActiveShader( shader );

						renderer->drawMesh( res->mesh_cube );
					}
				}	
			}
		}
		*/

		Shader * shader = res->shader_basic;
		renderer->setActiveVertexLayout( res->vlayout_basic );
		shader->setVariable( "v", m_engine->getView() );
		shader->setVariable( "wv", m_engine->getView() );
		shader->setVariable( "wvp", m_engine->getViewProj() );
		renderer->setActiveShader( shader );

		Frustum f = Frustum::fromMatrix( m_engine->getViewProj() );

		for ( int i = 0; i < m_mesh->getMeshCount(); i++ ) {
			Mesh * m;
			if ( m = m_mesh->getMesh(i) ) {
				Sphere s = m_mesh->getBounds(i);
				if ( f.intersects( s ) ) {
					renderer->drawMesh( m );
				}
			}
		}

		/*
		for ( int i = 0; i < corners.getLength(); i++ ) {
			renderer->setActiveVertexLayout( res->vlayout_basic );
			Vector2 pos = m_terrain->mapToWorld( (float)corners[i].cX() - 0.5f, (float)corners[i].cY() - 0.5f );
			Matrix4 w = Matrix4::scale( Vector3(1,1,1) * 0.05f ) * Matrix4::translate( Vector3( pos.x, pos.y, 0.0f ) );
			shader->setVariable( "v", m_engine->getView() );
			shader->setVariable( "wv", w*m_engine->getView() );
			shader->setVariable( "wvp", w*m_engine->getViewProj() );
			renderer->setActiveShader( shader );
			renderer->drawMesh( m_engine->getResources()->mesh_cube );

			w = Matrix4::scale( Vector3(1,1,1) * 0.025f ) * Matrix4::translate( Vector3( pos.x + corners[i].sX * 0.1f, pos.y + corners[i].sY * 0.1f, 0.0f ) );
			shader->setVariable( "v", m_engine->getView() );
			shader->setVariable( "wv", w*m_engine->getView() );
			shader->setVariable( "wvp", w*m_engine->getViewProj() );
			renderer->setActiveShader( shader );
			renderer->drawMesh( m_engine->getResources()->mesh_cube );

			w = Matrix4::scale( Vector3(1,1,1) * 0.025f ) * Matrix4::translate( Vector3( pos.x + corners[i].lX * 0.1f, pos.y + corners[i].lY * 0.1f, 0.0f ) );
			shader->setVariable( "v", m_engine->getView() );
			shader->setVariable( "wv", w*m_engine->getView() );
			shader->setVariable( "wvp", w*m_engine->getViewProj() );
			renderer->setActiveShader( shader );
			renderer->drawMesh( m_engine->getResources()->mesh_cube );

			if ( corners[i].pattern == 0 ) {
				w = Matrix4::scale( Vector3(1,1,1) * 0.025f ) * Matrix4::translate( Vector3( pos.x + corners[i].lX * 0.2f, pos.y + corners[i].lY * 0.2f, 0.0f ) );
				shader->setVariable( "v", m_engine->getView() );
				shader->setVariable( "wv", w*m_engine->getView() );
				shader->setVariable( "wvp", w*m_engine->getViewProj() );
				renderer->setActiveShader( shader );
				renderer->drawMesh( m_engine->getResources()->mesh_cube );
			}

		}
		*/

	}




}

