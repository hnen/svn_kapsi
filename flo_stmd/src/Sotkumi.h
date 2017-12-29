
#ifndef _SOTKUMI
#define _SOTKUMI

#include <Windows.h>

#include "Demo.h"
#include "poopie.h"
#include <poopie\ui\win32\Window.h>

using namespace poopie;
using namespace poopie::gfx;

class Sotkumi
{
public:
	Sotkumi()
	{
	}


	float volume;

	void Init( )
	{
		Demo demo = Demo::get();
		DemoResources & res = DemoResources::get();
		Renderer* render = demo.getRenderer();

		LoadShaders();

		volume = 1.0f;

	}


	void LoadShaders()
	{
		Demo demo = Demo::get();
		DemoResources & res = DemoResources::get();
		Renderer* render = demo.getRenderer();
		{
			File file( "data/taustakuva.fxo" );
			shader_scene = render->createShader( "Taustakuva", 0, file.getData(), file.getDataSize() );
		}

		{
			File file( "data/nailusator.png");
			ImageData * img = ImageImporter::importPng( file.getData(), file.getDataSize() );
			img->generateMips();
			tex = render->createTexture( img );
			delete img;
		}
	}

	void drawTexFS( Texture * tex, Shader * shader_override ) {
		drawTex( tex, Vector2(0, 0), Gfx::Align::ALIGN_IMG_HCVC | Gfx::Align::ALIGN_SCR_HCVC, Vector2(1, 1), shader_override );
	}

	void drawTex( Texture * tex, Vector2 apos, int align, Vector2 asize, Shader * s ) {
		Renderer * r = Demo::get().getRenderer();
		DemoResources & res = DemoResources::get();


		float ax = apos.x;
		float ay = apos.y;
		float aw = asize.x;
		float ah = asize.y;

		if ( (align & Gfx::Align::ALIGN_SCR_H) == Gfx::Align::ALIGN_SCR_R ) ax = 1.0f - ax;
		if ( (align & Gfx::Align::ALIGN_SCR_H) == Gfx::Align::ALIGN_SCR_HC ) ax = 0.5f + ax;
		if ( (align & Gfx::Align::ALIGN_SCR_V) == Gfx::Align::ALIGN_SCR_T ) ay = 1.0f - ay;
		if ( (align & Gfx::Align::ALIGN_SCR_V) == Gfx::Align::ALIGN_SCR_VC ) ay = 0.5f + ay;

		if ( (align & Gfx::Align::ALIGN_IMG_H) == Gfx::Align::ALIGN_IMG_R ) ax -= aw;
		if ( (align & Gfx::Align::ALIGN_IMG_H) == Gfx::Align::ALIGN_IMG_HC ) ax -= aw * 0.5f;
		if ( (align & Gfx::Align::ALIGN_IMG_V) == Gfx::Align::ALIGN_IMG_T ) ay -= ah;
		if ( (align & Gfx::Align::ALIGN_IMG_V) == Gfx::Align::ALIGN_IMG_VC ) ay -= ah * 0.5f;

		ax *= 2.0f;
		ax -= 1.0f;
		ay *= 2.0f;
		ay -= 1.0f;

		//Shader * s = DemoResources::get().shader_overlay;

		s->setVariable( "time", Demo::get().timef() );
		s->setVariable( "tex", res.tex_envmap );
		s->setVariable( "volume", volume );
		r->setActiveShader( s );
		r->draw( 4, RT_TRIANGLESTRIP );
		s->setVariable( "tex", (Texture*)NULL );
	}

	void Render( )
	{
		Demo& demo = Demo::get();
		Renderer* render = demo.getRenderer();
		DemoResources& res = DemoResources::get();


		float eim = Demo::get().time();


		if ( eim < DemoResources::musicSyncTime[5] )
			volume = 0.5f;
		else if ( eim < DemoResources::musicSyncTime[9] )
		{
			volume = 0;
		}
		else if ( eim < DemoResources::musicSyncTime[10] )
			volume = 2;
		else if ( eim < DemoResources::musicSyncTime[12] )
		{
			float* spec = res.music->getSpectrum(64);
			float s = 0.05f;
			for ( int a=31; a<63; a++ ) s += spec[a] * 1.0f;
			s *= (eim-DemoResources::musicSyncTime[12]) / ((float)DemoResources::musicSyncTime[16]-DemoResources::musicSyncTime[12]);
			volume = s * 10.0f;
		} 
		else if ( eim < DemoResources::musicSyncTime[13] )
			volume = 2;
		else 
		{
			float* spec = res.music->getSpectrum(64);
			float s = 0.05f;
			for ( int a=0; a<63; a++ ) s += spec[a] * 1.0f;
			volume = min(s,1);
		}
		
		if ( poopie::ui::win::Window::keyPressed( VK_RETURN ) )

			LoadShaders();

//		drawTex( tex, shader_scene );
//		drawTex( tex, Vector2(0,0), Gfx::Align::ALIGN_IMG_HC || Gfx::Align::ALIGN_IMG_VC, Vector2(1,1), shader_scene );

//		render->setActiveRenderTarget( render->getBackbufferRenderTarget(), res.rt_depth );
//		render->clear( render->getBackbufferRenderTarget(), Color4(0,0,0.3f,1.0f) );
		render->clear( res.rt_depth );

//		Gfx::drawTexFS( tex );
//		Gfx::drawTexFS( tex, shader_scene );
		drawTexFS( tex, shader_scene );

	}

	poopie::gfx::Shader* shader_scene;
	Texture* tex;

};


#endif
