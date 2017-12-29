
#ifndef __DEMO
#define __DEMO

#include "poopie/defs.h"
#include "poopie/gfx/Renderer.h"
#include "poopie/sfx/Music.h"

using namespace poopie;
using namespace poopie::gfx;
using namespace poopie::sfx;


class Demo {
public:

	Demo( Renderer * renderer ) : m_renderer(renderer) {}

	void init();
	void render();

	void setTime( unsigned int time );
	unsigned int time();
	float timef();

private:

	unsigned int m_startTime;

	Music * m_song;
	Renderer * m_renderer;

	Mesh * m_mesh_meteo;
	Material * m_mat_meteo;
	Shader * m_shader_vertexLit;

	DepthStencil * m_rt_depth;

};

#endif

