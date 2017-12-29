
#ifndef _FADERI
#define _FADERI

#include <Windows.h>

#include "Demo.h"
#include "poopie.h"
#include <poopie\ui\win32\Window.h>

using namespace poopie;
using namespace poopie::gfx;

class Faderi
{
public:
	Faderi()
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
			File file( "data/faderi.fxo" );
			shader_scene = render->createShader( "Taustakuva", 0, file.getData(), file.getDataSize() );
		}

		{
			File file( "data/color.png");
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
		s->setVariable( "tex", tex );
		s->setVariable( "volume", volume );
		r->setActiveShader( s );
		r->draw( 4, RT_TRIANGLESTRIP );
		s->setVariable( "tex", (Texture*)NULL );
	}

	void Render( )
	{
		Demo& demo = Demo::get();

		float eim = demo.time();

		float alkk = DemoResources::musicSyncTime[4];
		float lopp = DemoResources::musicSyncTime[5];
		float lenn = lopp-alkk;

		alkk += lenn / 2.0f;
		lenn = lopp-alkk;

		if ( eim < alkk || eim >= lopp )
			return;


		volume = (eim-alkk)/lenn;

		Renderer* render = demo.getRenderer();
		DemoResources& res = DemoResources::get();


		if ( poopie::ui::win::Window::keyPressed( VK_RETURN ) )
			LoadShaders();

		render->clear( res.rt_depth );
		drawTexFS( tex, shader_scene );
	}

	poopie::gfx::Shader* shader_scene;
	Texture* tex;

};


#endif
