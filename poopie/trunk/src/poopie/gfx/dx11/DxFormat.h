#ifndef __POOPIE_GFX_DX11_FORMAT
#define __POOPIE_GFX_DX11_FORMAT

#include <d3d11.h>
#include "d3dx11effect.h"

#include "../Format.h"

namespace poopie {
namespace gfx {
namespace dx11 {

	class DxFormat {
	public:

		static DXGI_FORMAT toDxgiFormat( const Format & el );
		static D3D11_INPUT_ELEMENT_DESC * createInputElementDesc( Format * elements, int elCount );

	};

}
}
}

#endif
