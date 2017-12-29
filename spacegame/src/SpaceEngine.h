
#ifndef __SPACE_SPACEENGINE_H
#define __SPACE_SPACEENGINE_H

#include "poopie_base.h"
#include "poopie_gfx.h"
#include "SpaceResources.h"
#include "SpaceObject.h"

using namespace poopie::gfx;

// TODO: Remove this when platform dependency successfully removed!
namespace poopie {
	namespace ui {
		namespace win {
			class Window;
		}
	}
}


namespace space {

	class SpaceShip;
	class SpaceInput;
	class SpaceTerrain;

	class SpaceEngine {
	public:

		SpaceEngine( Renderer * renderer ) : m_renderer(renderer), m_time(0) {}

		void init( poopie::ui::win::Window * window );
		void update( unsigned int dt );
		void render();
		void cleanup();

		SpaceResources * getResources() { return m_res; }
		Renderer * getRenderer() { return m_renderer; } 

		float t() const { return (float)m_time / 1000; }

		Matrix4 & getViewProj() { return m_vp; }
		Matrix4 & getView() { return m_v; }

		SpaceInput * getInput( int player ) { return m_input; }
		SpaceTerrain * getTerrain() { return m_terrain; }

		void addObject( SpaceObject * obj );
		void removeObject( SpaceObject * obj );

	private:

		//DynamicArray<SpaceObject*> m_objects;
		HashMap<SpaceObject*, SpaceObject*> m_objects;

		Vector2 m_cameraPos;
		
		Matrix4 m_vp, m_v;

		SpaceResources * m_res;
		SpaceShip * m_ship;
		SpaceTerrain * m_terrain;

		Renderer * m_renderer;
		unsigned int m_time;

		SpaceInput * m_input;


	};


}

#endif
