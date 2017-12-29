
#ifndef __POOPIE_GFX_RENDERTARGET
#define __POOPIE_GFX_RENDERTARGET

namespace poopie {
namespace gfx {

	class RenderTarget {
	public:
		
		virtual Texture * getTexture() = 0;

		virtual int getWidth() = 0;
		virtual int getHeight() = 0;
		
	};

}
}


#endif


