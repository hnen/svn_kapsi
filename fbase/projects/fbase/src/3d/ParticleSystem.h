
#ifndef __PARTICLESYSTEM
#define __PARTICLESYSTEM

#include "../math/Vector.h"

class ParticleSystem {

	class Particle {
		Vector3 pos;
		float tmpDepth;

		operator float() { return tmpDepth; };
	};

	Array<Particle> m_prts;

public:

	ParticleSystem() {};
	void sort( const Vector3 & dir );

};



#endif


