

#include "poopie/gfx/dx11/DxTexture.h"
#include "poopie/gfx/dx11/DxFormat.h"

namespace poopie {
namespace gfx {
namespace dx11 {

	DxTexture::DxTexture( ID3D11Texture2D * dxTexture, ID3D11ShaderResourceView * dxResView ) {
		m_dxTexture = dxTexture;
		m_dxTexture->AddRef();
		if ( dxResView ) {
			m_dxResourceView = dxResView;
			m_dxResourceView->AddRef();
		}

		D3D11_TEXTURE2D_DESC d;
		dxTexture->GetDesc( &d );
		m_width = d.Width;
		m_height = d.Height;

	}

	DxTexture::~DxTexture() {
		m_dxTexture->Release();
		if ( m_dxResourceView ) {
			m_dxResourceView->Release();
		}
	}

	void DxTexture::setDebugName( const char * name ) {
#ifdef _DEBUG
		char * paska = new char[strlen(name)+1];
		strcpy(paska, name);
		m_dxTexture->SetPrivateData( WKPDID_D3DDebugObjectName, strlen(paska), paska );
#endif
	}


}
}
}

