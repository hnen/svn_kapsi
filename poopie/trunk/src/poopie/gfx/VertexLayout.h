#ifndef __POOPIE_GFX_VERTEXLAYOUT
#define __POOPIE_GFX_VERTEXLAYOUT

#include "poopie/defs.h"

namespace poopie {
namespace gfx {

	struct Format;
	
	class VertexLayout {
	public:

		virtual const Format * getFormatList() = 0;
		virtual int getFormatListElementCount() = 0;
		virtual int getStride() = 0;

		bool hasUVs();
		bool hasTangents();

	};

}
}


#endif

