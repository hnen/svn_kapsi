
#ifndef __POOPIE_GFX_DX11_BUFFER
#define __POOPIE_GFX_DX11_BUFFER

#include "poopie/gfx/Buffer.h"
#include "poopie/defs.h"
#include <d3d11.h>

namespace poopie {
namespace gfx {
namespace dx11 {

	class DxDevice;

	class DxBuffer : public Buffer {
	public:

		ID3D11Buffer * GetDxHandle() { return m_bufferhandle; }

		unsigned int GetStride() { return m_stride; }
		unsigned int GetSize() { return m_size; }

		void setActiveElements( int num ) {
			POOP_ASSERT( num <= m_size );
			m_activeElements = num;
		}

		unsigned int getActiveElements() { 
			return m_activeElements;
		}

		void * lock();
		void unlock();

	private:

		DxBuffer( DxDevice * device, BufferType type, int size, int stride, bool isDynamic, bool streamOutput, const void * data );
		~DxBuffer();

		BufferType GetType() { return m_type; }

		ID3D11Buffer * m_bufferhandle;

		bool m_dynamic;

		int m_size;
		int m_stride;
		int m_activeElements;

		BufferType m_type;
		DxDevice * m_device;

		bool m_locked;

		friend class DxRenderer;

	};

}
}
}

#endif
