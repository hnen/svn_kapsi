
#include "poopie_base.h"
#include "poopie_gfx.h"

#include "SpaceEngine.h"
#include "SpaceShip.h"
#include "SpaceInput.h"
#include "SpaceTerrain.h"

namespace space {

	void SpaceEngine::init( poopie::ui::win::Window * window ) {
		POOP_LOG( "Engaging engine!" );
		m_input = new SpaceInput( window );
		m_res = new SpaceResources( m_renderer );
		m_terrain = new SpaceTerrain( this );
		m_ship = new SpaceShip( this );

		m_objects.add( m_ship, m_ship );

	}

	bool pressed = false;

	void SpaceEngine::update( unsigned int dt ) {

		bool eDown = m_input->keyboardCharacterDown('e');
		if ( eDown && !pressed ) {
			m_terrain->exportTerrain();
		}
		eDown = pressed;

		m_time += dt;

		Vector2 tgtPos = m_ship->getPosition();
		
		m_cameraPos = m_cameraPos * 0.95f + tgtPos * 0.05f;
		//m_cameraPos = tgtPos + Vector2( sin(t()), cos(t()) ) * 5;

		m_objects.resetIteration();
		HashMap<SpaceObject*,SpaceObject*>::KeyValuePair iter;
		while ( m_objects.iterateNext( &iter ) ) {
			SpaceObject * obj = *iter.value;
			obj->update();
		}

	}

	void SpaceEngine::render() {
		Vector2 tgtPos = m_ship->getPosition();

		Matrix4 v = Matrix4::LookAt( Vector3( -m_cameraPos.x, -m_cameraPos.y, 5.0f), Vector3( -tgtPos.x, -tgtPos.y, 0), Vector3(0,1,0) );
		Matrix4 p = Matrix4::perspective( 45.0f / 180.0f * 3.1416f, 1.78f, 0.1f, 50.0f );
		m_v = v;
		m_vp = v*p;

		//m_ship->render();

		m_objects.resetIteration();
		HashMap<SpaceObject*,SpaceObject*>::KeyValuePair iter;
		while ( m_objects.iterateNext( &iter ) ) {
			SpaceObject * obj = *iter.value;
			obj->render();
		}

		m_terrain->render();
	}

	void SpaceEngine::cleanup() {
		delete m_ship;
		delete m_res;
	}

	void SpaceEngine::addObject( SpaceObject * obj ) {
		m_objects.add( obj,obj );
	}

	void SpaceEngine::removeObject( SpaceObject * obj ) {
		m_objects.remove( obj );
	}

}

