#ifndef __POOPIE_GFX_DX11_VERTEXLAYOUT
#define __POOPIE_GFX_DX11_VERTEXLAYOUT

#include <D3D11.h>
#include "poopie/defs.h"
#include "../VertexLayout.h"
#include "../Format.h"

namespace poopie {
namespace gfx {

namespace dx11 {

	class DxDevice;
	class DxShader;
	
	class DxVertexLayout : public VertexLayout {
	public:

		ID3D11InputLayout * GetDxHandle() { return m_layout; }

		const Format * getFormatList() { return m_formatList; }
		int getFormatListElementCount() { return m_formatListCount; }

		int getStride() {
			if ( m_stride > 0 ) 
				return m_stride;
			int ret = 0;
			for ( int i = 0; i < m_formatListCount; i++ ) {
				ret += m_formatList[i].getSize();
			}
			m_stride = ret;
			return ret;
		}

	private:
		int m_stride;

		//DxVertexLayout( DxDevice * device, DxShader * shader, D3D11_INPUT_ELEMENT_DESC * elements, int elCount );
		DxVertexLayout( DxDevice * device, DxShader * shader, Format * elements, int elCount );

		ID3D11InputLayout * m_layout;
		
		Format * m_formatList;
		int m_formatListCount;

		friend class DxRenderer;

	};

}
}
}


#endif

