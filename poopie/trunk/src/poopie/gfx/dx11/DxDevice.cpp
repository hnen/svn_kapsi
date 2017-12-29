
#include <Windows.h>
#include <DxErr.h>

#include "poopie/defs.h"
#include "poopie/ui/win32/WinUIElement.h"
#include "poopie/base/win/WinDebug.h"

#include "poopie/gfx/dx11/DxTexture.h"
#include "poopie/gfx/dx11/DxDevice.h"


namespace poopie {

	using ui::win::WinUIElement;
	using win::WinDebug;

namespace gfx {
namespace dx11 {

	DxTexture * DxDevice::getBackbufferTexture() {

		ID3D11Texture2D * ret = NULL;

		HRESULT hr;

		hr = m_swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**)&ret );
		WinDebug::handleResult( hr, "Error retrieving back buffer texture" );

		return new DxTexture( ret, 0 );
	}

	DxDevice::DxDevice( WinUIElement * el ) : m_device(0), m_swapChain(0), m_deviceContext(0) {

		UINT createDeviceFlags = 0;

		DXGI_SWAP_CHAIN_DESC sd = p_createSwapchainDesc( el );

		HRESULT hr = D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, NULL, 0, D3D11_SDK_VERSION,
						&sd, &m_swapChain, &m_device, &m_activeFeatureLevel, &m_deviceContext );

		if ( !SUCCEEDED(hr) ) {
			
			POOP_WARNING( String("Paska kone: ") + String(DXGetErrorString(hr)) + ", " + String(DXGetErrorDescription(hr)) );
#ifndef DEBUG
			POOP_WARNING( "Fallbacking to reference device." );
			hr = D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_REFERENCE, NULL, createDeviceFlags, NULL, 0, D3D11_SDK_VERSION,
						&sd, &m_swapChain, &m_device, &m_activeFeatureLevel, &m_deviceContext  );
#endif
			win::WinDebug::handleResult( hr, "Couldn't create D3D11 device" );
		}

		D3D11_VIEWPORT vp;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.Width = (FLOAT)el->getXSize();
		vp.Height = (FLOAT)el->getYSize();
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		getDeviceContext()->RSSetViewports( 1, &vp );
	}

	DxDevice::~DxDevice() {
		if ( m_device ) {
			m_deviceContext->Release();
			m_device->Release();
		}
	}

	DXGI_SWAP_CHAIN_DESC DxDevice::p_createSwapchainDesc( WinUIElement * el ) {
		DXGI_SWAP_CHAIN_DESC sd;
		memset( &sd, 0, sizeof( sd ) );
		sd.BufferCount = 2;
		sd.BufferDesc.Width = el->getXSize();
		sd.BufferDesc.Height = el->getYSize();
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = el->getWindowHandle();
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = el->isFullscreen() ? FALSE : TRUE;

		return sd;
	}


}
}
}




