
#include <Windows.h>

#include "Demo.h"
#include "poopie.h"
#include "Dualism.h"
#include "Mosho.h"

using namespace ::Gfx;

void Demo::init() {
	DemoResources & res = DemoResources::get();
 
	res.load( m_renderer );
	res.music->play();

	m_dualism = new Dualism();
	m_dualism->Init( );
//	m_mosho = new Mosho();
//	m_mosho->Init();
}
bool onoff =false;

float sun_y;
void Demo::render() {
	DemoResources & res = DemoResources::get();

	float t = timef();
	unsigned int it = time();
	float dt = t-lastt;
	lastt = t;

	p = Matrix4::perspective( 3.14f * 55.0f / 180.0f, 1280.0f / 720.0f, 0.01f, 1000.0f );

/*
	sun_y = 0;
	if ( it > 62438-3000 ) {
		int it0 = it-(62438-3000);
		sun_y = cerp( 0.0f, 3.5f, SAT( (float)it0 / 6500 ) );
	}

	camView = Vector3(0, sun_y,-3);	
	camPos = Vector3( 0.0f, 0.0f, qerp( 40.0f, 2.0f, MIN(1,t / 60.0f) ) );
	Vector3 camUp = Vector3(0,1,0);

	float floaters_intensity = 1.0f;

	if ( it > 62438 + 3000 ) {
		// kauemmas
		float _t = (it-(62438+3000));
		camPos = Vector3( 0.0f, 0.0f, cerp( 2.0f, 7.0f, MIN(1,_t / 8000.0f) ) );
	}
	if ( it > 62438 + 8000 ) {
		// tärinä
		float _t = (it-(62438+8000));
		camView = camView + Vector3( sin(_t*1.5f) + cos(_t*4), cos(_t*3) + sin(_t*2), 0 ) * cerp( 0.0f, 1.5f, SAT(_t / 8000.0f) ) ;	
	}
	if ( it > 76134 ) {
		// kameran siirto
		float _t = (float)(it-76134) / 1000;
		camPos = slerp( camPos, Vector3( 0, 6, 0 ), SAT( _t / 2.5f ) );
		camView = cerp( camView, Vector3(), SAT(_t/2.5f) );	
		camUp = slerp( Vector3(0,1,0), Vector3(-1,0,0), SAT(_t/2.5f) );
	}



	v = Matrix4::LookAt( camPos, camView, camUp );// * Matrix4::rotZ( timef() * 0.1f );
	vp = v*p;

	m_renderer->clear( res.rt_scene_glowGeom, Color4(0,0,0,0) );
	m_renderer->clear( res.rt_scene, Color4(0,0,0,0) );
	m_renderer->clear( res.rt_mask, Color4(0,0,0,0) );
	m_renderer->clear( res.rt_depth );
	m_renderer->setActiveRenderTarget( res.rt_scene, res.rt_mask, res.rt_depth );

	demo_opaque( t );

	m_renderer->setActiveRenderTarget( res.rt_scene_glowGeom, res.rt_mask, res.rt_depth );
	demo_glowgeom( t );
	demo_opaque( t, res.shader_blockglow );

	tex_blur( res.shader_blur, res.tex_rt_scene_glowGeom, res.rt_scene_glowGeom_tmp, res.rt_scene_glowGeom_blurred );


	if ( it > 62438 ) {
		int it0 = it-62438;
		floaters_intensity = qerp( 1.0f, 0.0f, SAT( (float)it0 / 250 ) );
	}
	if ( it > 76134 ) {
		float _t = (float)(it-76134) / 1000;
		floaters_intensity = cerp( 0.0f, 1.0f, SAT(_t/3.0f) );
	}

	if ( floaters_intensity > 0 ) {
		tex_blur( res.shader_blur_expand, res.tex_rt_mask, res.rt_mask_tmp, res.rt_mask_fieldMul );
		tex_blur( res.shader_blur_expandMore, res.tex_rt_mask_fieldMul, res.rt_mask_tmp, res.rt_mask_intensity );
		tex_blur( res.shader_blur_shrink, res.tex_rt_mask, res.rt_mask_tmp, res.rt_mask_intensityMul );

		floaters_prepass( res.tex_perlin, res.tex_rt_mask_fieldMul, res.rt_floaters_field, res.rt_floaters_vectors, res.tex_rt_mask_intensity, res.tex_rt_mask_intensityMul, res.rt_floaters_intensity );

		m_renderer->setActiveRenderTarget( m_renderer->getBackbufferRenderTarget(), res.rt_depth );
		m_renderer->clear( m_renderer->getBackbufferRenderTarget(), Color4(0,0,0,0) );
		m_renderer->clear( res.rt_depth );

		m_renderer->setActiveRenderTarget( m_renderer->getBackbufferRenderTarget(), NULL );
		Gfx::drawTexFS( res.tex_rt_scene );
		floaters_draw( res.tex_rt_floaters_vectors, res.tex_rt_floaters_intensity, floaters_intensity );
	} else {
		m_renderer->clear( m_renderer->getBackbufferRenderTarget(), Color4(0,0,0,0) );
		m_renderer->clear( res.rt_depth );
		m_renderer->setActiveRenderTarget( m_renderer->getBackbufferRenderTarget(), NULL );
		Gfx::drawTexFS( res.tex_rt_scene );
	}
	m_renderer->setActiveRenderTarget( m_renderer->getBackbufferRenderTarget(), NULL );
	Gfx::drawTexFS( res.tex_rt_scene_glowGeom );
	Gfx::drawTexFS( res.tex_rt_scene_glowGeom_blurred, res.shader_overlay_add );
	*/
	m_dualism->Render();
//	m_mosho->Render();
}

void Demo::demo_glowgeom( float t ) {
	DemoResources & res = DemoResources::get();
	drawMesh( res.mesh_pallo, res.shader_sun, Matrix4::scale( Vector3(1,1,1) * 4.0f ) * Matrix4::translate( Vector3(0,sun_y,-16) ) );
}

float rotZ = -0.1f;
void Demo::demo_opaque( float t, Shader * shader_override ) {
	DemoResources & res = DemoResources::get();

	static float lt = t;
	float dt = t - lt;
	lt = t;

	#define SHADER(__s) (shader_override ? shader_override : (__s))

		//rotZ = t*0.02f - 0.1f;
		if ( t < 62.438f + 8.0f ) { 
			rotZ += dt  * 0.02f;
		} else {
			float _t = (float)(t - (62.438f + 8.0f));
			rotZ += dt  * (0.02f + cerp(0.0f, 1.0f, SAT(_t/3.0f) ) );
		}

		drawMesh( res.mesh_ship, SHADER(res.shader_alus),  Matrix4::rotZ( rotZ ) );

	#undef SHADER
}

float Demo::timef() {
	return (float)time() / 1000.0f;
}

unsigned int Demo::time() {
#ifndef FMOD_DISABLE
	return DemoResources::get().music->getTime();
#else
	return timeGetTime();
#endif
}

void Demo::setTime( unsigned int time ) {
#ifndef FMOD_DISABLE
	DemoResources::get().music->setTime( time );
#else
#endif
}

void Demo::floaters_prepass( Texture * tex_perlin, Texture * tex_geom, RenderTarget * rt_fieldpass, RenderTarget * rt_vectors,  Texture * tex_intensity, Texture * tex_intensityMul, RenderTarget * rt_intensity ) {
	DemoResources & res = DemoResources::get();

	m_renderer->setActiveRenderTarget( rt_fieldpass, NULL );
	res.shader_floaters_field->setVariable( "tex_field_perlin", tex_perlin );
	res.shader_floaters_field->setVariable( "tex_field_geom", tex_geom );
	res.shader_floaters_field->setVariable( "t", timef() );
	m_renderer->setActiveShader( res.shader_floaters_field );
	m_renderer->draw( 4, RT_TRIANGLESTRIP );

	m_renderer->setActiveRenderTarget( rt_vectors, NULL );
	res.shader_floaters_vectors->setVariable( "tex_vectors_geom", rt_fieldpass->getTexture() );
	m_renderer->setActiveShader( res.shader_floaters_vectors );
	m_renderer->draw( 4, RT_TRIANGLESTRIP );

	m_renderer->setActiveRenderTarget( rt_intensity, NULL );
	res.shader_floaters_intensity->setVariable( "tex_intensity_base", tex_intensity );
	res.shader_floaters_intensity->setVariable( "tex_intensity_mul", tex_intensityMul );
	m_renderer->setActiveShader( res.shader_floaters_intensity );
	m_renderer->draw( 4, RT_TRIANGLESTRIP );

}


void Demo::tex_blur( Shader ** shader_blur, Texture * tex_src, RenderTarget * rt_pass, RenderTarget * rt_dst ) {
	DemoResources & res = DemoResources::get();

	m_renderer->setActiveRenderTarget( rt_pass, NULL );
	shader_blur[0]->setVariable( "tex_input", tex_src );
	m_renderer->setActiveShader( shader_blur[0] );
	m_renderer->draw( 4, RT_TRIANGLESTRIP );

	m_renderer->setActiveRenderTarget( rt_dst, NULL );
	shader_blur[1]->setVariable( "tex_input", rt_pass->getTexture() );
	m_renderer->setActiveShader( shader_blur[1] );
	m_renderer->draw( 4, RT_TRIANGLESTRIP );

}

void Demo::drawMesh( Mesh * m, Shader * shader, Matrix4 & w ) {
	DemoResources & res = DemoResources::get();

	shader->setVariable( "v", v );
	shader->setVariable( "wv", w*v );
	shader->setVariable( "wvp", w*vp );
	shader->setVariable( "color", Vector4(1,1,1,1) );

	m_renderer->setActiveShader( shader );
	m_renderer->setActiveVertexLayout( res.vlayout_basic );

	m_renderer->drawMesh( m );
}


void Demo::floaters_draw( Texture * tex_field, Texture * tex_intensity, float intensity ) {
	DemoResources & res = DemoResources::get();

	res.shader_floaters->setVariable( "g_intensity", intensity );
	res.shader_floaters->setVariable( "tex_field", tex_field );
	res.shader_floaters->setVariable( "tex_intensity", tex_intensity );
	res.shader_floaters->setVariable( "tex_noise", res.tex_noise );
	res.shader_floaters->setVariable( "tex_floater", res.tex_blob );
	res.shader_floaters->setVariable( "tex_intensityPixel", res.tex_rt_mask_intensityMul );
	res.shader_floaters->setVariable( "t", timef() );

	m_renderer->setActiveShader( res.shader_floaters );
	//m_renderer->draw( 7500, RT_TRIANGLESTRIP );
	m_renderer->draw( 7500, RT_TRIANGLESTRIP );

}

