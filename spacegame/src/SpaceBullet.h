
#ifndef __SPACE_BULLET
#define __SPACE_BULLET

#include "poopie_base.h"

#include "SpaceShip.h"
#include "SpaceObject.h"
#include "SpaceEngine.h"

namespace space {

	class SpaceBullet : SpaceObject {
	public:

		static SpaceBullet * spawn( SpaceEngine * engine, SpaceShip * ship );

		void update();
		void render();

	private:

		void explode();

		Vector2 m_pos;
		Vector2 m_dir;

		SpaceEngine * m_engine;
		SpaceShip * m_ownerShip;

	};

}


#endif

