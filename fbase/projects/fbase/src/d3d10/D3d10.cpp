
#define WINAPI_INLINE WINAPI

#include "../Window.h"
#include "D3d10.h"
#include "../DebugTools.h"
#include "../WinDebug.h"
#include "D3dRenderTarget.h"

void D3d10::p_init() {
	m_viewSize = p_calcBufferSize();
	p_initDevice();
	p_createView();
}

D3d10::D3d10( Window * windowPtr, Fraction aspect ) :
		m_device(NULL),
		m_windowPtr(windowPtr),
		m_renderTarget( NULL ) {
	m_aspect = aspect;
	p_init();
}

void D3d10::p_initDevice() {

	UINT createDeviceFlags = 0;
#ifndef FINAL
	createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif

	DXGI_SWAP_CHAIN_DESC sd = p_createSwapchain();


	HRESULT hr = D3D10CreateDeviceAndSwapChain( NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, D3D10_SDK_VERSION,
			&sd, &m_swapChain, &m_device );

	if ( !SUCCEEDED(hr) ) {
#ifndef DEBUG
		FWARNING( "Fallbacking to reference device." );
		hr = D3D10CreateDeviceAndSwapChain( NULL, D3D10_DRIVER_TYPE_REFERENCE, NULL, createDeviceFlags, D3D10_SDK_VERSION,
					&sd, &m_swapChain, &m_device );
#endif
		WinDebug::handleResult( hr, "Couldn't create D3D10 device" );
	}

}

void D3d10::p_createView() {
	ID3D10Texture2D * viewTexture = NULL;
	ID3D10RenderTargetView * renderTargetView = NULL;

	HRESULT hr;

	hr = m_swapChain->GetBuffer( 0, __uuidof( ID3D10Texture2D ), (void**)&viewTexture );
	if ( !SUCCEEDED(hr) ) {
		FERROR( "Error creating view." );
	}

	hr = m_device->CreateRenderTargetView( viewTexture, NULL, &renderTargetView );
	viewTexture->Release();
	if ( !SUCCEEDED(hr) ) {
		FERROR( "Error creating main rendertarget." );
	}

	m_renderTarget = new D3dRenderTarget( renderTargetView, m_windowPtr->getXSize(), m_windowPtr->getYSize(), this );
	ID3D10RenderTargetView * const v = m_renderTarget->getD3dRenderTargetView();
	m_device->OMSetRenderTargets( 1, &v, NULL );

	D3D10_VIEWPORT vp = p_createViewport();
	m_device->RSSetViewports( 1, &vp );
}

D3D10_VIEWPORT D3d10::p_createViewport( int width, int height ) const {
	D3D10_VIEWPORT vp;

	vp.Width = width;
	vp.Height = height;

	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;

	vp.TopLeftX = (m_windowPtr->getXSize() - vp.Width) / 2;
	vp.TopLeftY = (m_windowPtr->getYSize() - vp.Height) / 2;

	return vp;
}

D3D10_VIEWPORT D3d10::p_createViewport() const {
	return p_createViewport( m_viewSize.getWidth(), m_viewSize.getHeight() );
}
D3D10_VIEWPORT D3d10::p_createFullSizeViewport() const {
	return p_createViewport( m_windowPtr->getXSize(), m_windowPtr->getYSize() );
}

RectSize D3d10::p_calcBufferSize() const {
	return RectSize( m_windowPtr->getXSize(), m_windowPtr->getYSize() ).fitRect( m_aspect );
}

DXGI_SWAP_CHAIN_DESC D3d10::p_createSwapchain() {
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount = 1;
    sd.BufferDesc.Width = m_windowPtr->getXSize();
    sd.BufferDesc.Height = m_windowPtr->getYSize();
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = m_windowPtr->getWindowHandle();
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = m_windowPtr->isFullscreen() ? FALSE : TRUE;

    return sd;
}

D3d10::~D3d10() {
	p_release();
}

void D3d10::p_release() {
	if ( m_renderTarget ) {
		delete m_renderTarget;
		m_renderTarget = NULL;
	}

	if ( m_swapChain ) {
		m_swapChain->Release();
		m_swapChain = NULL;
	}

	if ( m_device ) {
		m_device->Release();
		m_device = NULL;
	}
}


void D3d10::clear( const Color4 & clearColor ) {
	m_renderTarget->clear( clearColor );
}

void D3d10::present() {
	m_swapChain->Present( 0, 0 );
}



