
#ifndef __POOPIE_GFX_DX_RENDERTARGET
#define __POOPIE_GFX_DX_RENDERTARGET

#include <d3d11.h>
#include "poopie/gfx/RenderTarget.h"
#include "poopie/gfx/Texture.h"

namespace poopie {
namespace gfx {
namespace dx11 {

	class DxTexture;
	class DxRenderer;

	class DxRenderTarget : public RenderTarget {
	public:

		ID3D11RenderTargetView * getRenderTargetView() { return m_renderTargetView; }

		DxRenderTarget( ID3D11RenderTargetView * renderTargetView, Texture * targetTexture ) : m_renderTargetView(renderTargetView) {
			m_renderTargetView->AddRef();
			m_texture = targetTexture;

			m_width = m_texture->getWidth();
			m_height = m_texture->getHeight();
			

		}
		~DxRenderTarget() {
			m_renderTargetView->Release();
		};

		Texture * getTexture() {
			return m_texture;
		}

		int getWidth() { return m_width; }
		int getHeight() { return m_height; }

	private:

		int m_width;
		int m_height;

		Texture * m_texture;
		ID3D11RenderTargetView * m_renderTargetView;

	};

}
}
}


#endif

