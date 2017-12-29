
#include "poopie/defs.h"
#include "poopie/base/Array.h"
#include "DxFormat.h"


namespace poopie {
namespace gfx {
namespace dx11 {

	D3D11_INPUT_ELEMENT_DESC * DxFormat::createInputElementDesc( Format * elements, int elCount ) {
		D3D11_INPUT_ELEMENT_DESC * ret = new D3D11_INPUT_ELEMENT_DESC[elCount];

		for( int i = 0; i < elCount; i++ ) {
			Format & el = elements[i];
			D3D11_INPUT_ELEMENT_DESC dsc;
			dsc.Format = toDxgiFormat( el );
			dsc.SemanticName = el.name.getCStr();
			dsc.SemanticIndex = 0;
			dsc.InputSlot = 0;
			dsc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			dsc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			dsc.InstanceDataStepRate = 0;
			ret[i] = dsc;
		}

		return ret;
	}

	DXGI_FORMAT DxFormat::toDxgiFormat( const Format & el ) {
		if ( el.fmt == FORMAT_TYPE_FLOAT ) {
			if ( el.num == 1 ) {
				return DXGI_FORMAT_R32_FLOAT;
			} else if ( el.num == 2 ) {
				return DXGI_FORMAT_R32G32_FLOAT;
			} else if ( el.num == 3 ) {
				return DXGI_FORMAT_R32G32B32_FLOAT;
			} else if ( el.num == 4 ) {
				return DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
		} else if ( el.fmt == FORMAT_TYPE_UINT ) {
			if ( el.num == 1 ) {
				return DXGI_FORMAT_R32_UINT;
			} else if ( el.num == 2 ) {
				return DXGI_FORMAT_R32G32_UINT;
			} else if ( el.num == 3 ) {
				return DXGI_FORMAT_R32G32B32_UINT;
			} else if ( el.num == 4 ) {
				return DXGI_FORMAT_R32G32B32A32_UINT;
			}
		} else if ( el.fmt == FORMAT_TYPE_USHORT ) {
			if ( el.normalized ) {
				if ( el.num == 1 ) {
					return DXGI_FORMAT_R16_UNORM;
				} else if ( el.num == 2 ) {
					return DXGI_FORMAT_R16G16_UNORM;
				} else if ( el.num == 4 ) {
					return DXGI_FORMAT_R16G16B16A16_UNORM;
				}
			} else {
				if ( el.num == 1 ) {
					return DXGI_FORMAT_R16_UINT;
				} else if ( el.num == 2 ) {
					return DXGI_FORMAT_R16G16_UINT;
				} else if ( el.num == 4 ) {
					return DXGI_FORMAT_R16G16B16A16_UINT;
				}
			}
		} else if ( el.fmt == FORMAT_TYPE_UBYTE ) {
			if ( el.normalized ) {
				if ( el.num == 1 ) {
					return DXGI_FORMAT_R8_UNORM;
				} else if ( el.num == 2 ) {
					return DXGI_FORMAT_R8G8_UNORM;
				} else if ( el.num == 4 ) {
					return DXGI_FORMAT_R8G8B8A8_UNORM;
				}
			} else {
				if ( el.num == 1 ) {
					return DXGI_FORMAT_R8_UINT;
				} else if ( el.num == 2 ) {
					return DXGI_FORMAT_R8G8_UINT;
				} else if ( el.num == 4 ) {
					return DXGI_FORMAT_R8G8B8A8_UINT;
				}
			}
		}
		POOP_ERROR( "Not implemented/not possible." );
	}




}
}
}

