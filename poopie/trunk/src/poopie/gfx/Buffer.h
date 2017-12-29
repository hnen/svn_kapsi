
#ifndef __POOPIE_GFX_BUFFER
#define __POOPIE_GFX_BUFFER

#include "poopie/defs.h"

namespace poopie {
namespace gfx {

	enum BufferType {
		VERTEX, INDEX, CONSTANT, RAW
	};

	class Buffer {
	public:

		virtual unsigned int GetStride() = 0;
		virtual unsigned int GetSize() = 0;
		virtual BufferType GetType() = 0;

		virtual ~Buffer() {}

		virtual void setActiveElements( int num ) = 0;
		virtual unsigned int getActiveElements() = 0;
		virtual void * lock() = 0;
		virtual void unlock() = 0;

	};


}
}




#endif