
#ifndef __POOPIE_GFX_DX11_DEPTHSTENCIL
#define __POOPIE_GFX_DX11_DEPTHSTENCIL

#include <d3d11.h>

#include "poopie/gfx/DepthStencil.h"

namespace poopie {
namespace gfx {
namespace dx11 {


	class DxDepthStencil : public DepthStencil {
	public:

		DxDepthStencil( ID3D11DepthStencilView * dxView, Texture * bufResource ) : m_dxView(dxView), m_tex(bufResource) {}

		ID3D11DepthStencilView * m_dxView;
		Texture * m_tex;

	};

}
}
}


#endif
