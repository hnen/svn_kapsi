
#include "poopie/gfx/Shader.h"
#include "poopie/base/String.h"
#include "poopie/gfx/Material.h"
#include "poopie/gfx/ImageData.h"
#include "poopie/gfx/ImageImporter.h"

namespace poopie {
namespace gfx {

	void Material::applyShaderConstants() {
		
		// Textures
		textures.resetIteration();
		HashMap<String, Texture*>::KeyValuePair texI;
		while( textures.iterateNext( &texI ) ) {
			shader->setVariable( *texI.key, *texI.value );
		}

		// vectors
		vecs.resetIteration();
		HashMap<String, Vector4>::KeyValuePair vecI;
		while( vecs.iterateNext( &vecI ) ) {
			shader->setVariable( *vecI.key, *vecI.value );
		}

		// values
		values.resetIteration();
		HashMap<String, float>::KeyValuePair valI;
		while( values.iterateNext( &valI ) ) {
			shader->setVariable( *valI.key, *valI.value );
		}

	}

}
}

