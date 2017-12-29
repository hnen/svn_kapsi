
#include "../WinDebug.h"
#include "D3dDepthStencil.h"

void D3dDepthStencil::p_init( D3dFormat format ) {

	D3D10_DEPTH_STENCIL_VIEW_DESC dsDesc;
	dsDesc.Format = format.DEPTH_FLOAT.dxgiFormat;
	dsDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	dsDesc.Texture2D.MipSlice = 0;

	HRESULT hr = m_d3d10->getDevice()->CreateDepthStencilView( m_tex->getD3dTexture(), &dsDesc, &m_d3dDepthStencil );
	WinDebug::handleResult( hr, "Failed creating DepthStencilView" );
}

D3dDepthStencil::~D3dDepthStencil() {
	delete m_tex;
}

D3dDepthStencil::D3dDepthStencil( int width, int height, D3d10 * d3d10 ) : m_d3d10(d3d10),
	m_tex( new D3dTexture( width, height, D3dFormat::DEPTH_FLOAT, D3dTexture::DepthStencil, d3d10 ) ),
	m_d3dDepthStencil(NULL) {
	p_init( D3dFormat::DEPTH_FLOAT );
}



