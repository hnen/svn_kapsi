
#ifndef __SPACE_SHIP
#define __SPACE_SHIP

#include "poopie_gfx.h"
#include "SpaceObject.h"

namespace space {

	class SpaceInput;
	class SpaceEngine;
	class SpaceResources;

	class SpaceShip : public SpaceObject {
	public:

		SpaceShip( SpaceEngine * engine );
		void update(); 
		void render();

		Vector2 getPosition() const { return m_pos; }
		Vector2 getDir() const { return m_dir; }

	private:

		void step( Vector2 pos, Vector2 velDir, float velM, float tdamp );
		void fire();

		SpaceEngine * m_engine;
		SpaceResources * m_res;
		SpaceInput * m_input;
		Renderer * m_renderer;

		Vector2 m_dir;
		Vector2 m_vel;
		Vector2 m_pos;

		float m_lastFired;

	};


}


#endif