
#ifndef __POOPIE_GFX_DX_TEXTURE
#define __POOPIE_GFX_DX_TEXTURE

#include <d3d11.h>
#include "poopie/gfx/Texture.h"

namespace poopie {
namespace gfx {
namespace dx11 {

	class DxTexture : public Texture {
	public:

		// Init from D3d11 Buffer
		DxTexture( ID3D11Texture2D * dxTexture, ID3D11ShaderResourceView * dxResView  );
		~DxTexture();

		ID3D11Texture2D * getDxTexture() { return m_dxTexture; }

		ID3D11ShaderResourceView * getDxShaderResourceView() {
			return m_dxResourceView;
		}

		int getWidth() { return m_width; }
		int getHeight() { return m_height; }

		void setDebugName ( const char * name );

	private:

		int m_width;
		int m_height;

		ID3D11Texture2D * m_dxTexture;
		ID3D11ShaderResourceView * m_dxResourceView;

	};

}
}
}




#endif
