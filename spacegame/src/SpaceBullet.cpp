
#include "SpaceBullet.h"
#include "SpaceTerrain.h"

namespace space {

	class SpaceBullet;

	SpaceBullet * SpaceBullet::spawn( SpaceEngine * engine, SpaceShip * ship ) {
		SpaceBullet * bullet = new SpaceBullet();
		bullet->m_ownerShip = ship;
		bullet->m_engine = engine;
		bullet->m_pos = ship->getPosition();
		bullet->m_dir = ship->getDir().normalized();
		engine->addObject( bullet );
		return bullet;
	}

	void SpaceBullet::render() {
		SpaceResources * res = m_engine->getResources();
		Renderer * renderer = m_engine->getRenderer();

		Shader * shader = res->shader_basic;

		Matrix4 w = Matrix4::rotX( PI * 0.5f ) * Matrix4( Vector3(-m_dir.y, m_dir.x, 0), Vector3(-m_dir.x, -m_dir.y, 0), Vector3(0,0,1) ) * Matrix4::translate( Vector3( m_pos.x, m_pos.y, 0 ) );

		shader->setVariable( "v", m_engine->getView() );
		shader->setVariable( "wv", w*m_engine->getView() );
		shader->setVariable( "wvp", w*m_engine->getViewProj() );
		renderer->setActiveShader( shader );
		renderer->setActiveVertexLayout( res->vlayout_basic );

		renderer->drawMesh( res->mesh_bullet );
	}

	void SpaceBullet::explode() {
		float radius = 1.5f;
		SpaceTerrain * terrain = m_engine->getTerrain();
		Vector2 min = terrain->worldToMap( Vector2( m_pos.x - radius, m_pos.y - radius ) );
		Vector2 max = terrain->worldToMap( Vector2( m_pos.x + radius, m_pos.y + radius ) );
		for ( int x = (int)min.x; x <= max.x; x++ ) {
			for ( int y = (int)min.y; y <= max.y; y++ ) {
				Vector2 wpos = terrain->mapToWorld( (float)x + 0.5f, (float)y + 0.5f );
				float dSq = (wpos-m_pos).lengthSq();
				if ( dSq < radius*radius ) {
					terrain->setBlock( x, y, 0 );
				}
			}
		}
		terrain->updateGraphics();

		m_engine->removeObject( this );
		delete this;
	}

	void SpaceBullet::update() {

		SpaceTerrain * terrain = m_engine->getTerrain();

		Vector2 vel = m_dir * 0.3f;
		Vector2 hit;
		if ( terrain->raycast( m_pos, vel.normalized(), vel.length(), &hit, NULL ) ) {
			m_pos = hit;
			explode();
			return;
		} else {
			m_pos = m_pos + vel;
		}

		

		if ( m_pos.x < m_engine->getTerrain()->getMinX() || m_pos.x > m_engine->getTerrain()->getMaxX() || 
				m_pos.y < m_engine->getTerrain()->getMinY() || m_pos.y > m_engine->getTerrain()->getMaxY() ) {
			m_engine->removeObject( this );
			delete this;
		}
	}





}

