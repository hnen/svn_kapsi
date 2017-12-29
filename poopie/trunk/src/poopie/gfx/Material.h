
#ifndef __POOPIE_GFX_MATERIAL
#define __POOPIE_GFX_MATERIAL

#include "poopie/base/ByteStream.h"
#include "poopie/base/HashMap.h"
#include "poopie/math/Vector.h"

namespace poopie {

namespace gfx {

	class Texture;
	class Shader;

	class Material {
	public:

		Material( Shader * shader ) : shader(shader) {};

		HashMap<String, Texture*> textures;
		HashMap<String, Vector4> vecs;
		HashMap<String, float> values;

		void applyShaderConstants();

	private:
		
		Shader * shader;


	};

}
}

#endif

