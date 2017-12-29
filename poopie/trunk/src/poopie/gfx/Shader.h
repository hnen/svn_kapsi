
#ifndef __POOPIE_GFX_SHADER
#define __POOPIE_GFX_SHADER

#include "poopie/defs.h"
#include "poopie/math/Matrix.h"
#include "poopie/math/Vector.h"

namespace poopie {
namespace gfx {

	class Buffer;
	class Texture;
	class RenderTarget;
	class VertexLayout;

	class Shader {
	public:

		virtual void apply() = 0;
		
		virtual void setVariable( const char * name, const Matrix4 & m ) = 0;
		virtual void setVariable( const char * name, Texture * tex ) = 0;
		virtual void setVariable( const char * name, const Vector4 & v ) = 0;
		//virtual void setVariable( const char * name, const Vector3 & v ) = 0;
		virtual void setVariable( const char * name, float f ) = 0;

		virtual void setConstants( const char * name, Buffer * data ) = 0;

		virtual VertexLayout * getVertexLayout() = 0;

	};

}
}


#endif