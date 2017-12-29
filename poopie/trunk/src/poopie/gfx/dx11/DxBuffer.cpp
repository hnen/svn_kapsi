
#include <Windows.h>

#include "poopie/gfx/dx11/DxBuffer.h"
#include "poopie/gfx/dx11/DxDevice.h"
#include "poopie/base/String.h"
#include "poopie/defs.h"

namespace poopie {
namespace gfx {
namespace dx11 {


	DxBuffer::DxBuffer( DxDevice * device, BufferType type, int size, int stride, bool isDynamic, bool streamOutput, const void * data ) {
		m_device = device;
		m_type = type;
		m_size = size;
		m_stride = stride;
		m_dynamic = isDynamic;
		m_activeElements = size;
		m_locked = false;

		D3D11_BUFFER_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );
		desc.Usage = isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		desc.ByteWidth = m_stride * m_size;
		desc.CPUAccessFlags = isDynamic ? D3D11_CPU_ACCESS_WRITE : 0;

		if ( type == INDEX ) {
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		} else if ( type == VERTEX ) {
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		} else if ( type == CONSTANT ) {
			// CONSTANT
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		} else {
			if ( streamOutput ) {
				desc.CPUAccessFlags = 0;
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_STREAM_OUTPUT;
			}
		}

		

		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA d3ddata;
		d3ddata.SysMemPitch = 0;
		d3ddata.SysMemSlicePitch = 0;
		d3ddata.pSysMem = data;

		if ( FAILED( m_device->getDevice()->CreateBuffer( &desc, data == NULL ? NULL : &d3ddata, &m_bufferhandle ) ) ) {
			POOP_ERROR( "Failed creating a buffer!" );
		}

#ifdef POOP_DEBUG
		String dbgname = "DxBuffer";
		m_bufferhandle->SetPrivateData( WKPDID_D3DDebugObjectName, dbgname.getLength(), dbgname.getCStr() );
#endif


	}

	DxBuffer::~DxBuffer() {
		String dbgname = "DxBuffer (released)";
#ifdef POOP_DEBUG
		m_bufferhandle->SetPrivateData( WKPDID_D3DDebugObjectName, dbgname.getLength(), dbgname.getCStr() );
		m_bufferhandle->Release();
#endif
	}
	

	void * DxBuffer::lock() {
		POOP_ASSERT( m_dynamic );
		m_locked = true;
		D3D11_MAPPED_SUBRESOURCE res;
		ZeroMemory( &res, sizeof(res) );
		m_device->getDeviceContext()->Map( m_bufferhandle, 0, D3D11_MAP_WRITE_DISCARD, 0, &res );
		return res.pData;
	}

	void DxBuffer::unlock() {
		POOP_ASSERT( m_dynamic );
		m_locked = false;
		m_device->getDeviceContext()->Unmap( m_bufferhandle, 0 );
	}




}
}
}



