
#ifndef __POOPIE_GFX_DX_DEVICE
#define __POOPIE_GFX_DX_DEVICE

#include <d3d11.h>


namespace poopie {
	namespace ui {
		namespace win {
			class WinUIElement;
		}
	}

namespace gfx {
namespace dx11 {

	class DxTexture;

	class DxDevice {
	public:

		DxDevice( ui::win::WinUIElement * uiElement );
		~DxDevice();

		ID3D11Device * getDevice() { return m_device; }
		ID3D11DeviceContext * getDeviceContext() { return m_deviceContext; }

		DxTexture * getBackbufferTexture();
		IDXGISwapChain * getSwapChain() { return m_swapChain; }

	private:

		DXGI_SWAP_CHAIN_DESC p_createSwapchainDesc( ui::win::WinUIElement * el );

	private:
		
		D3D_FEATURE_LEVEL m_activeFeatureLevel;
		ID3D11Device * m_device;
		ID3D11DeviceContext * m_deviceContext;
		IDXGISwapChain * m_swapChain;

	};


}
}
}



#endif

