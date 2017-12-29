
#ifndef _MOSHO
#define _MOSHO

#include <Windows.h>

#include "Demo.h"
#include "poopie.h"
#include <poopie\ui\win32\Window.h>

using namespace poopie;
using namespace poopie::gfx;

class Mosho
{
public:
	Mosho()
	{
	}

	float gtime;

	void Init( )
	{
		
		im = 0;
		Demo demo = Demo::get();
		DemoResources & res = DemoResources::get();
		Renderer* render = demo.getRenderer();

		loadShaders();

		Format fmt[] = {
			Format( "position", FORMAT_TYPE_FLOAT, 4 ),
			Format( "normal", FORMAT_TYPE_FLOAT, 4 )
		};
		vlayout = render->createVertexLayout( shaderScene, fmt, sizeof(fmt)/sizeof(Format) );
		
		Format mf( FORMAT_TYPE_UBYTE, 4 );
		int w = render->getBackbufferRenderTarget()->getWidth();
		int h = render->getBackbufferRenderTarget()->getHeight();

		kuvaT = render->createRenderTargetTexture( w, h, mf );
		kuva = render->createRenderTarget( kuvaT );
		render->clear( kuva, Color4(0,0,0.01f,1.0f) );
		mixedT = render->createRenderTargetTexture( w, h, mf );
		mixed = render->createRenderTarget( mixedT );
		render->clear( mixed, Color4(0,0,0.01f,1.0f) );

	}

	void drawTex( Texture* tex, Texture* scene, Shader* s = NULL, float amount = 1.0f ) {
		Renderer * r = Demo::get().getRenderer();
		DemoResources & res = DemoResources::get();

		if ( s == NULL )
			s = this->shaderOut;

		Vector2 apos = Vector2(0,0);
		int align = Gfx::Align::ALIGN_IMG_HCVC|Gfx::Align::ALIGN_SCR_HCVC;
		Vector2 asize = Vector2(1,1);
		
		float ax = apos.x;		float ay = apos.y;
		float aw = asize.x;		float ah = asize.y;

		if ( (align & Gfx::Align::ALIGN_SCR_H) == Gfx::Align::ALIGN_SCR_R ) ax = 1.0f - ax;		if ( (align & Gfx::Align::ALIGN_SCR_H) == Gfx::Align::ALIGN_SCR_HC ) ax = 0.5f + ax;
		if ( (align & Gfx::Align::ALIGN_SCR_V) == Gfx::Align::ALIGN_SCR_T ) ay = 1.0f - ay;		if ( (align & Gfx::Align::ALIGN_SCR_V) == Gfx::Align::ALIGN_SCR_VC ) ay = 0.5f + ay;

		if ( (align & Gfx::Align::ALIGN_IMG_H) == Gfx::Align::ALIGN_IMG_R ) ax -= aw;		if ( (align & Gfx::Align::ALIGN_IMG_H) == Gfx::Align::ALIGN_IMG_HC ) ax -= aw * 0.5f;
		if ( (align & Gfx::Align::ALIGN_IMG_V) == Gfx::Align::ALIGN_IMG_T ) ay -= ah;		if ( (align & Gfx::Align::ALIGN_IMG_V) == Gfx::Align::ALIGN_IMG_VC ) ay -= ah * 0.5f;

		ax = ax*2.0f-1.0f;		ay = ay*2.0f-1.0f;

		s->setVariable( "posSize", Vector4( ax, ay, aw*2.0f, ah*2.0f ) );
		s->setVariable( "tex", tex );
		s->setVariable( "scene", scene );
		s->setVariable( "envmap", res.tex_envmap );
		s->setVariable( "amount", amount );
		r->setActiveShader( s );
		r->draw( 4, RT_TRIANGLESTRIP );
		s->setVariable( "tex", (Texture*)NULL );
		s->setVariable( "scene", (Texture*)NULL );
		s->setVariable( "envmap", (Texture*)NULL );

	}


	void copy( Texture* source, RenderTarget* target )
	{
		Demo& demo = Demo::get();
		Renderer* render = demo.getRenderer();
		DemoResources& res = DemoResources::get();
		render->setActiveRenderTarget( target, res.rt_depth );
		render->clear( res.rt_depth );
		Gfx::drawTexFS( source );
	}


	int im ;
	void Render( )
	{

		if ( poopie::ui::win::Window::keyPressed( VK_RETURN ) )
			loadShaders();

		Demo& demo = Demo::get();
		Renderer* render = demo.getRenderer();
		DemoResources& res = DemoResources::get();

					render->setActiveRenderTarget( render->getBackbufferRenderTarget(), res.rt_depth );
			render->clear( render->getBackbufferRenderTarget(), Color4(0,0,0.3f,1.0f) );
			Gfx::drawTexFS( res.tex_perlin );


			if ( poopie::ui::win::Window::keyPressed( VK_DELETE ) )
			{
				render->clear( kuva, Color4(0,0,0,0) );
				render->clear( mixed, Color4(0,0,0,0) );
			}
		if ( im++>100 ||poopie::ui::win::Window::keyPressed( VK_DELETE ) )
		{
			gtime = rand();
			im=0;
		}
		float t = demo.timef() + gtime;

		float* spec = res.music->getSpectrum(64);
		float s = 0.05f;
		for ( int a=0; a<16; a++ ) s += spec[a+8] * 1.0f;
		for ( int a=-4; a<5; a++ )
			RenderForReal( cos(t + a * s) * (10.0f+s), demo.timef()  );


		if ( !poopie::ui::win::Window::isKeyDown( 'Y' ) )
		{
			render->setActiveRenderTarget( render->getBackbufferRenderTarget(), res.rt_depth );
//			render->clear( render->getBackbufferRenderTarget(), Color4(0,0,0.3f,1.0f) );
			render->clear( res.rt_depth );
	//		drawTex( NULL, res.tex_rt_scene );
//			drawTex( NULL, kuvaT);

//			Gfx::drawTexFS( res.tex_perlin );
			render->clear( res.rt_depth );
			drawTex( NULL, kuvaT);
		}
		else
			copy( kuvaT, render->getBackbufferRenderTarget() );

	}

	void RenderForReal( float time, float realTime )
	{
		Demo& demo = Demo::get();
		Renderer* render = demo.getRenderer();
		DemoResources& res = DemoResources::get();

		float* spec = res.music->getSpectrum(64);
		float s = 0.05f;
		for ( int a=0; a<16; a++ ) s += spec[a+8] * 1.0f;

		float a = time;
//		for ( int Z=0; Z<1; Z++ )
		int Z = 0;
		{
			if ( Z==1 )
				a -= 0.1f;
			Vector3 eye = Vector3( 0,0,-0.5f );
			Vector3 tgt = Vector3( sinf(a/10.0f), 0.3f * cosf(a/10.0f) ,cosf(a/10.0f) );
			Matrix4 v = Matrix4::LookAt( tgt, tgt + Vector3(0,0,1), Vector3(s,1,0) );
			Matrix4 p = Matrix4::perspective( 3.14f * 35.0f / 180.0f, 1280.0f / 720.0f, 0.01f, 100.0f );

//			Vector3 camView = Vector3(0,0,0);
			Vector3 camView = Vector3(  cos(a/3.0f + sin(a/7.0f+time)) * 3.5f, 0,0 );
			Vector3 camPos = tgt * 5.0f;
		
			Matrix4 proj = Matrix4::perspective( 3.14f * 35.0f / 180.0f, render->getBackbufferRenderTarget()->getWidth() / (float)render->getBackbufferRenderTarget()->getHeight(), 0.01f, 100.0f );

			////////////////////////////////// SCENE
			if ( Z==1 )
			{
				render->setActiveRenderTarget( kuva, res.rt_depth );
				render->clear( kuva, Color4(0,0,1,0) );
			}
			else
			{
				render->setActiveRenderTarget( res.rt_scene, res.rt_depth );
				render->clear( res.rt_scene, Color4(0,0,1.0f,1.0f) );
			}
			render->clear( res.rt_depth);

			v = Matrix4::LookAt( camPos, camView, Vector3(0,1,0) );// * Matrix4::rotZ( timef() * 0.1f );
			render->setActiveShader( shaderScene );
			shaderScene->setVariable( "v", v );
			shaderScene->setVariable( "wv", v );
			shaderScene->setVariable( "wvp", v*proj );
			shaderScene->setVariable( "p", proj );
		float* spec = res.music->getSpectrum(64);
		float s = 0.0f;
		for ( int a=0; a<4; a++ ) s += spec[a*8 + 4] * 3.0f;
			shaderScene->setVariable( "mur", s );
			shaderMix->setVariable( "mur", s );


			float aikuus = 62.5f;
			float ygg = realTime-aikuus;
			float m = 50-min( max( ygg, 0 ), 50 );
			if ( realTime < aikuus || fmodf(realTime,1) < m/50.0f )
				demo.getRenderer()->drawMesh( res.mesh_pallo );
			else
				demo.getRenderer()->drawMesh( res.mesh_ship );

		}

//		if ( false )	
		if ( poopie::ui::win::Window::isKeyDown( 'T' ) )
		{
			// -----------  show above step
			render->setActiveShader( res.shader_overlay );
			copy( res.tex_rt_scene, render->getBackbufferRenderTarget() );
			return;
		}

		////////////////////////////////// MIX

//		if ( false )
		{
//			render->setActiveRenderTarget( render->getBackbufferRenderTarget(), res.rt_depth );
//			render->clear( render->getBackbufferRenderTarget(), Color4(0,0,0.3f,1.0f) );
//			render->clear( res.rt_depth );

			render->setActiveRenderTarget( mixed, res.rt_depth );
//			render->clear( kuva, Color4(0,0,0.3f,1.0f) );
			render->clear( res.rt_depth );

			drawTex( kuvaT, res.tex_rt_scene, shaderMix, 1-s );
//			drawTex( mixedT, res.tex_rt_scene, shaderMix, 0.09f );
//		copy( res.tex_rt_scene, kuva);

			copy( mixedT, kuva);
		}

		if ( false )
		{
			//  ------------ show above step
			copy( kuvaT, render->getBackbufferRenderTarget() );
		}
		
		////////////////////////////////// OUT

		if ( false )
		{
			render->setActiveRenderTarget( render->getBackbufferRenderTarget(), res.rt_depth );
//			render->clear( render->getBackbufferRenderTarget(), Color4(0,0,0.3f,1.0f) );
			render->clear( res.rt_depth );
	//		drawTex( NULL, res.tex_rt_scene );
//			drawTex( NULL, kuvaT);
			drawTex( NULL, mixedT);
		}

//		Gfx::drawTexFS( res.tex_rt_scene);

		/*
		render->setActiveVertexLayout( res.vlayout_basic );

//		shaderOut->setVariable( "tex", envmap );
		shaderOut->setVariable( "tex", res.tex_rt_scene );
//		shaderOut->setVariable( "scene", res.tex_rt_scene );
		render->setActiveShader(shaderOut);
		
		render->draw( 4, RT_TRIANGLESTRIP );
		shaderOut->setVariable( "tex", (Texture*)NULL );
		shaderOut->setVariable( "scene", (Texture*)NULL );
		*/
		
//		Gfx::drawTexFS( res.tex_rt_scene);
	}

protected:
	void loadShaders()
	{
		Demo demo = Demo::get();
		Renderer* render = demo.getRenderer();
		{
			File file( "data/mosho.fxo" );
			shaderScene = render->createShader( "Liima", 0, file.getData(), file.getDataSize() );
		}
		{
			File file( "data/mosho2.fxo" );
			shaderOut = render->createShader( "Out", 0, file.getData(), file.getDataSize() );
		}
		{
			File file( "data/mosho2.fxo" );
			shaderMix = render->createShader( "Mix", 0, file.getData(), file.getDataSize() );
		}
	}

	VertexLayout* vlayout;

	poopie::gfx::Mesh* mesh;

	poopie::gfx::Shader* shaderScene;
	poopie::gfx::Shader* shaderOut;
	poopie::gfx::Shader* shaderMix;

	Texture* kuvaT;
	RenderTarget* kuva;
	Texture* mixedT;
	RenderTarget* mixed;

};


#endif
