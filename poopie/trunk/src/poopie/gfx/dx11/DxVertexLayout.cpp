
#include "poopie/gfx/dx11/DxShader.h"
#include "poopie/gfx/dx11/DxDevice.h"
#include "poopie/gfx/dx11/DxVertexLayout.h"
#include "poopie/gfx/dx11/DxFormat.h"

#include "poopie/gfx/Format.h"

namespace poopie {
namespace gfx {
namespace dx11 {

	DxVertexLayout::DxVertexLayout( DxDevice * device, DxShader * shader, Format * elements, int elCount ) {
		m_stride = -1;
		D3DX11_PASS_DESC desc;
		shader->getDxPassHandle()->GetDesc( &desc );
		device->getDevice()->CreateInputLayout( DxFormat::createInputElementDesc( elements, elCount ), elCount, desc.pIAInputSignature, desc.IAInputSignatureSize, &m_layout );
		
		m_formatList = new Format[elCount];
		for ( int i=0; i<elCount; i++ )
			m_formatList[i] = elements[i];
		m_formatListCount = elCount;
	}


}
}
}
