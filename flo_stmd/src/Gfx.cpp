
#include "poopie.h"
#include "Demo.h"

namespace Gfx {

	void drawTexFS( Texture * tex ) {
		drawTex( tex, Vector2(0, 0), ALIGN_IMG_HCVC | ALIGN_SCR_HCVC, Vector2(1, 1), DemoResources::get().shader_overlay );
	}
	void drawTexFS( Texture * tex, Shader * shader_override ) {
		drawTex( tex, Vector2(0, 0), ALIGN_IMG_HCVC | ALIGN_SCR_HCVC, Vector2(1, 1), shader_override );
	}

	void drawTex( Texture * tex, float x, float y, int align, float scale ) {

		int texWidth = tex->getWidth();
		int texHeight = tex->getHeight();

		drawTex( tex, Vector2(x / 1920.0f, y / 1080.0f), align, Vector2( (((float)texWidth) / 1280.0f) * scale, (((float)texHeight) / 720.0f) * scale ), DemoResources::get().shader_overlay );
	}

	void drawTex( Texture * tex, Vector2 apos, int align, Vector2 asize, Shader * s ) {
		Renderer * r = Demo::get().getRenderer();

		float ax = apos.x;
		float ay = apos.y;
		float aw = asize.x;
		float ah = asize.y;

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

		//Shader * s = DemoResources::get().shader_overlay;

		s->setVariable( "posSize", Vector4( ax, ay, aw*2.0f, ah*2.0f ) );
		s->setVariable( "tex_image", tex );
		r->setActiveShader( s );
		r->draw( 4, RT_TRIANGLESTRIP );
		s->setVariable( "tex_image", (Texture*)NULL );

	}


}

