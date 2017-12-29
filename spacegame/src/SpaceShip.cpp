
#include "poopie_base.h"
#include "SpaceEngine.h"
#include "SpaceShip.h"
#include "SpaceInput.h"
#include "SpaceTerrain.h"
#include "SpaceBullet.h"

namespace space {

	SpaceShip::SpaceShip( SpaceEngine * engine ) {
		m_engine = engine;
		m_res = m_engine->getResources();
		m_renderer = m_engine->getRenderer();
		m_input = engine->getInput( 0 );
		
		SpaceTerrain * terr = m_engine->getTerrain();
		while ( terr->getValByWorldPos( m_pos ) ) {
			m_pos = Vector2( RND( terr->getMinX(), terr->getMaxX() ), RND( terr->getMinY(), terr->getMaxY() ) );
		}

		m_lastFired = 0;
		
		m_dir = Vector2(0,1);
	}

	void SpaceShip::step( Vector2 pos, Vector2 velDir, float velM, float tdamp ) {
		SpaceTerrain * terr = m_engine->getTerrain();
		Vector2 hit, norm;
		if ( terr->raycast( pos, velDir, velM, &hit, &norm ) ) {
		//if ( false ) {
			if( norm == Vector2(0,0) ) {
				m_vel = velDir * velM;
				m_pos = m_pos + m_vel;
				return;
			} else {
				float damp = 1.0f;
				velDir = velDir - norm * norm.dot( m_vel.normalized() ) * 1.4f;
				pos = hit;
				velM = (velM - (hit-pos).length()) * damp;
				step( pos, velDir, velM, tdamp * damp );
			}
		} else {
			m_vel = velDir * tdamp;
			m_pos = m_pos + velDir * velM;
		}
	}

	void SpaceShip::update() {
		float rot = m_input->getDirection().x;
		m_dir = m_dir.rotate( rot * 0.1f ).normalized();

		float thrust = MAX( 0, m_input->getDirection().y ) * 80.0f;
		float drag = m_vel.lengthSq() * 2500.0f;
		//float drag = 0;

		if ( m_input->fireDown() ) {
			fire();
		}

		m_vel = m_vel + (Vector2( 0, -7.0f ) + m_dir * thrust - m_vel * drag) * 0.0001f;

		step( m_pos, m_vel.normalized(), m_vel.length(), m_vel.length() );

	}

	void SpaceShip::fire() {
		if ( m_engine->t() - m_lastFired > 0.15f ) {
			m_lastFired = m_engine->t();
			SpaceBullet::spawn( m_engine, this );
		}
	}



	void SpaceShip::render() {

		Shader * shader = m_res->shader_basic;

		Matrix4 w = Matrix4::rotX( PI * 0.5f ) * Matrix4( Vector3(-m_dir.y, m_dir.x, 0), Vector3(-m_dir.x, -m_dir.y, 0), Vector3(0,0,1) ) * Matrix4::translate( Vector3( m_pos.x, m_pos.y, 0 ) );

		shader->setVariable( "v", m_engine->getView() );
		shader->setVariable( "wv", w*m_engine->getView() );
		shader->setVariable( "wvp", w*m_engine->getViewProj() );
		m_renderer->setActiveShader( shader );
		m_renderer->setActiveVertexLayout( m_res->vlayout_basic );

		m_renderer->drawMesh( m_res->mesh_alus );

	}

}

