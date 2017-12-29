
#ifndef __D3DRENDERTARGET
#define __D3DRENDERTARGET

#include "../math/Color.h"
#include "D3dTexture.h"

class D3dRenderTarget {

	D3d10 * m_d3d10;
	ID3D10RenderTargetView * m_renderTargetView;
	D3dTexture * m_tex;

	int m_width, m_height;

	void p_init( int width, int height, D3dFormat format );

	D3D10_RENDER_TARGET_VIEW_DESC p_createDesc( D3dFormat format );

	D3D10_VIEWPORT p_createViewport( int width, int height ) const;

	void p_setViewport();

public:

	D3dRenderTarget( int width, int height, D3dFormat format, D3d10 * d3d10 );
	D3dRenderTarget( ID3D10RenderTargetView * d3dRenderTargetView, int width, int height, D3d10 * d3d10  );
	~D3dRenderTarget();
	
	ID3D10RenderTargetView * const getD3dRenderTargetView() { return m_renderTargetView; }
	void clear( const Color4 & clearColor = Color4() );

	int getWidth() const;
	int getHeight() const;

};

#endif

