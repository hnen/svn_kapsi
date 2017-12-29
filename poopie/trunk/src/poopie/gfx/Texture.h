
#ifndef __POOPIE_GFX_TEXTURE
#define __POOPIE_GFX_TEXTURE

namespace poopie {
namespace gfx {

	class Texture {
	public:

		virtual int getWidth() = 0;
		virtual int getHeight() = 0;
		virtual void setDebugName( const char * name ) = 0;

	};

}
}


#endif

