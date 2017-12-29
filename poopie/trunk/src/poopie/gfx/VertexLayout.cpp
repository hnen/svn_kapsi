
#include "poopie/gfx/VertexLayout.h"
#include "poopie/gfx/Format.h"

namespace poopie {
namespace gfx {

	bool VertexLayout::hasUVs() {
		const Format * f = getFormatList();
		int cnt = getFormatListElementCount();
		for( int i = 0; i < cnt; i++ ) {
			if ( f[i].name.lowerCase().equals( "texcoord" ) ) {
				return true;
			}
		}
		return false;
	}

	bool VertexLayout::hasTangents() {
		const Format * f = getFormatList();
		int cnt = getFormatListElementCount();
		for( int i = 0; i < cnt; i++ ) {
			if ( f[i].name.lowerCase().equals( "tangent" ) ) {
				return true;
			}
		}
		return false;
	}

}
}

