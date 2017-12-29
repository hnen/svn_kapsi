
#include "poopie/base/win/WinDebug.h"
#include "poopie/gfx/dx11/DxShader.h"


namespace poopie {
namespace gfx {
namespace dx11 {


	void DxShader::initialize( const void * shaderData, uint32 shaderDataSize ) {
		POOP_ASSERT( m_d3dxeffect == NULL );
		HRESULT hr = D3DX11CreateEffectFromMemory( shaderData, shaderDataSize, 0, m_device->getDevice(), &m_d3dxeffect );

		poopie::win::WinDebug::handleResult( hr, "Error creating shader" );
	}

	void DxShader::setTechniqueAndPass( const char * techniqueName, int pass ) {
		m_d3dxpass = m_d3dxeffect->GetTechniqueByName(techniqueName)->GetPassByIndex(pass);
	}

	void DxShader::setTechniqueAndPass( int techniqueId, int pass ) {
		m_d3dxpass = m_d3dxeffect->GetTechniqueByIndex(techniqueId)->GetPassByIndex(pass);
	}

	void DxShader::apply() {
		if ( !m_d3dxpass ) {
			setTechniqueAndPass(0,0);
		}
		m_d3dxpass->Apply( 0, m_device->getDeviceContext() );
	}


}
}
}


