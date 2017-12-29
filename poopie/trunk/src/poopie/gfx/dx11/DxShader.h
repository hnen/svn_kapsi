
#ifndef __POOPIE_GFX_DX11_SHADER
#define __POOPIE_GFX_DX11_SHADER

#include <d3d11.h>

#include "d3dx11effect.h"

#include "poopie/gfx/Shader.h"
#include "poopie/gfx/RenderTarget.h"
#include "poopie/math/Matrix.h"

#include "poopie/gfx/dx11/DxDevice.h"
#include "poopie/gfx/dx11/DxTexture.h"
#include "poopie/gfx/dx11/DxRenderTarget.h"
#include "poopie/gfx/dx11/DxBuffer.h"

namespace poopie {
namespace gfx {
namespace dx11 {

	class DxShader : public Shader {
	public:

		void apply();
		void initialize( const void * shaderData, uint32 shaderDataSize );

		void setTechniqueAndPass( const char * techniqueName, int pass );
		void setTechniqueAndPass( int techniqueId, int pass );

		void bindVertexLayout( VertexLayout * vlayout ) {
			m_vlayout = vlayout;
		}

		ID3DX11Effect * getDxHandle() { return m_d3dxeffect; }
		ID3DX11EffectPass * DxShader::getDxPassHandle() { return m_d3dxpass; }

		// slow as hell, could at least cache variables or even use constant buffers..
		void setVariable( const char * name, const Matrix4 & m ) {
			ID3DX11EffectVariable * var = m_d3dxeffect->GetVariableByName( name );
			if ( var ) {
				var->AsMatrix()->SetMatrix( (float*)&m );
			}
		}

		void setVariable( const char * name, const Vector4 & f ) {
			ID3DX11EffectVariable * var = m_d3dxeffect->GetVariableByName( name );
			if ( var ) {
				var->AsVector()->SetFloatVector( (const float*)&f );
			}
		}

		void setConstants( const char * name, Buffer * data ) {
			ID3DX11EffectConstantBuffer * cb = m_d3dxeffect->GetConstantBufferByName( name );
			if ( cb ) {
				cb->SetConstantBuffer( ((DxBuffer*)data)->GetDxHandle() );
			}
		}

		void setVariable( const char * name, float f ) {
			ID3DX11EffectVariable * var = m_d3dxeffect->GetVariableByName( name );
			if ( var ) {
				var->AsScalar()->SetFloat( f );
			}
		}
		void setVariable( const char * name, Texture * tex ) {
			ID3DX11EffectVariable * var = m_d3dxeffect->GetVariableByName( name );
			if ( var ) {
				if ( tex ) {
					var->AsShaderResource()->SetResource( ((DxTexture*)tex)->getDxShaderResourceView() );
				} else {
					var->AsShaderResource()->SetResource( NULL );
				}
			}
		}

		VertexLayout * getVertexLayout() { return m_vlayout; }

		//void setVariable( const char * name, const Vector4 & v );
		//void setVariable( const char * name, const Vector3 & v );
		//void setVariable( const char * name, float f );


	private:
		DxShader( DxDevice * device ) : m_device(device), m_d3dxeffect(0), m_vlayout(0), m_d3dxpass(0) {}

		VertexLayout * m_vlayout;

		ID3DX11Effect * m_d3dxeffect;
		ID3DX11EffectPass * m_d3dxpass;

		DxDevice * m_device;

		friend class DxRenderer;
	};

}
}
}


#endif

