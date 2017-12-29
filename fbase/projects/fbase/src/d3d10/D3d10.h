
#ifndef D3D10_H_
#define D3D10_H_

#include "../math/Fraction.h"
#include "../math/RectSize.h"
#include "../math/Color.h"
#include "d3d10_mingw.h"
#include "D3dEnums.h"
#include "../Window.h"

class D3dRenderTarget;

class D3d10 {

	ID3D10Device * m_device;
	IDXGISwapChain * m_swapChain;

	D3dRenderTarget * m_renderTarget;

	Fraction m_aspect;
	RectSize m_viewSize;

	Window * m_windowPtr;

	DXGI_SWAP_CHAIN_DESC p_createSwapchain();

	void p_initDevice();
	void p_init();
	void p_release();

	void p_createView();

	D3D10_VIEWPORT p_createViewport( int width, int height ) const;
	D3D10_VIEWPORT p_createViewport() const;
	D3D10_VIEWPORT p_createFullSizeViewport() const;

	RectSize p_calcBufferSize() const;

public:

	D3d10( Window * assignWindow, Fraction aspect );
	~D3d10();

	void clear( const Color4 & clearColor = Color4() );
	void present();

	void setRenderTarget();

	ID3D10Device * const getDevice() { return m_device; }

};

#endif
