
#ifndef __DEMO
#define __DEMO

#include "DemoResources.h"

extern class Dualism;
extern class Mosho;

class Demo {
public:

	Demo( Renderer * renderer ) : m_renderer(renderer) 
	{
		lastt = 0;
	}

	void init();
	void render();

	void setTime( unsigned int time );
	unsigned int time();
	float timef();

	void drawMesh( Mesh * m, Shader * shader, Matrix4 & w );

	void demo_opaque( float t, Shader * shader_override = 0 );
	void demo_glowgeom( float t );

	void tex_blur( Shader ** blur, Texture * tex_src, RenderTarget * rt_pass, RenderTarget * rt_dst );
	void floaters_prepass( Texture * tex_perlin, Texture * tex_geom, RenderTarget * rt_fieldpass, RenderTarget * rt_vectors,  Texture * tex_intensity, Texture * tex_intensityMul, RenderTarget * rt_intensity );

	void floaters_draw( Texture * tex_field, Texture * tex_intensity, float intensity );

	static Demo & get( Renderer * renderer = NULL ) {
		static Demo instance(renderer);
		return instance;
	}

	Renderer * getRenderer() { return m_renderer; };

	Vector3 camPos, camView;

private:
	Matrix4 v, p, vp;

	unsigned int m_startTime;
	float lastt;

	Renderer * m_renderer;

	Mosho* m_mosho;
	Dualism* m_dualism;

};

#include "Gfx.h"

#endif

