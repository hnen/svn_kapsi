
#include "poopie.h"
#include "Demo.h"

namespace Gfx {

	void drawTex( Texture * tex, float x, float y, int align, float scale ) {
		Renderer * r = Demo::get().getRenderer();
		float scrWidth = r->getPrimaryRenderTarget()->getWidth();
		float scrHeight = r->getPrimaryRenderTarget()->getHeight();
		float texWidth = tex->getWidth();
		float texHeight = tex->getHeight();

		float ax = x / 1280.0f;
		float ay = y / 720.0f;

		float aw = (texWidth / 1920.0f) * scale;
		float ah = (texHeight / 1080.0f) * scale;

		if ( (align & ALIGN_SCR_H) == ALIGN_SCR_R ) ax = 1.0f - ax;
		if ( (align & ALIGN_SCR_H) == ALIGN_SCR_HC ) ax = 0.5f + ax;
		if ( (align & ALIGN_SCR_V) == ALIGN_SCR_T ) ay = 1.0f - ay;
		if ( (align & ALIGN_SCR_V) == ALIGN_SCR_VC ) ay = 0.5f + ay;

		if ( (align & ALIGN_IMG_H) == ALIGN_IMG_R ) ax -= aw;
		if ( (align & ALIGN_IMG_H) == ALIGN_IMG_HC ) ax -= aw * 0.5f;
		if ( (align & ALIGN_IMG_V) == ALIGN_IMG_T ) ay -= ah;
		if ( (align & ALIGN_IMG_V) == ALIGN_IMG_VC ) ay -= ah * 0.5f;

		ax *= 2.0f;
		ax -= 1.0f;
		ay *= 2.0f;
		ay -= 1.0f;

		Shader * s = DemoResources::get().shader_overlay;
		s->setVariable( "posSize", Vector4( ax, ay, aw*2.0f, ah*2.0f ) );
		s->setVariable( "tex_image", tex );
		r->setActiveShader( s );
		r->draw( 4, RT_TRIANGLESTRIP );

	}


}

