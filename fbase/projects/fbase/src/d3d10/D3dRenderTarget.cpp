
#include "D3dRenderTarget.h"


D3dRenderTarget::D3dRenderTarget( int width, int height, D3dFormat format, D3d10 * d3d10 ) :
					m_renderTargetView(0),
					m_tex(new D3dTexture( width, height, format, D3dTexture::NoMips | D3dTexture::RenderTarget, d3d10 ) ),
					m_d3d10(d3d10), m_width(width), m_height(height) {
	p_init( width, height, format );
}


D3dRenderTarget::~D3dRenderTarget() {
	if ( m_tex ) {
		delete m_tex;
		m_tex = NULL;
	}
	if ( m_renderTargetView ) {
		m_renderTargetView->Release();
		m_renderTargetView = NULL;
	}
}

D3D10_RENDER_TARGET_VIEW_DESC D3dRenderTarget::p_createDesc( D3dFormat format ) {
	D3D10_RENDER_TARGET_VIEW_DESC rtDesc;
	rtDesc.Format = format.dxgiFormat;
	rtDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
	rtDesc.Texture2D.MipSlice = 0;
	return rtDesc;
}

void D3dRenderTarget::p_init( int width, int height, D3dFormat format ) {
	D3D10_RENDER_TARGET_VIEW_DESC rtDesc = p_createDesc( format );
	m_d3d10->getDevice()->CreateRenderTargetView( m_tex->getD3dTexture(), &rtDesc, &m_renderTargetView );
}

D3dRenderTarget::D3dRenderTarget( ID3D10RenderTargetView * d3dRenderTargetView, int width, int height, D3d10 * d3d10 ) :
		m_renderTargetView(d3dRenderTargetView), m_tex( NULL ), m_d3d10(d3d10), m_width(width), m_height(height) {
}

void D3dRenderTarget::clear( const Color4 & clearColor ) {
	m_d3d10->getDevice()->ClearRenderTargetView( m_renderTargetView, (float*)&clearColor );
}

int D3dRenderTarget::getWidth() const {
	return m_width;
}

int D3dRenderTarget::getHeight() const {
	return m_height;
}


D3D10_VIEWPORT D3dRenderTarget::p_createViewport( int width, int height ) const {
	D3D10_VIEWPORT vp;

	vp.Width = width;
	vp.Height = height;

	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;

	vp.TopLeftX = (this->getWidth() - vp.Width) / 2;
	vp.TopLeftY = (this->getHeight() - vp.Height) / 2;

	return vp;

}
