
#include "../const.h"
#include "../WinDebug.h"
#include "../3d/VertexType.h"
#include "D3dIndexBuffer.h"
#include "D3dVertexBuffer.h"
#include "../math/algorithm/RadixSort.h"

D3D10_BUFFER_DESC D3dIndexBuffer::p_createBufferDesc( int numElements, bool dynamic ) {
	D3D10_BUFFER_DESC desc;
	desc.Usage = dynamic ? D3D10_USAGE_DYNAMIC : D3D10_USAGE_IMMUTABLE;
	desc.ByteWidth = sizeof(uint) * numElements;
	desc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = dynamic ? D3D10_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags = 0;
	return desc;
}

void D3dIndexBuffer::p_upload( Array<uint> & data ) {
	FASSERT( m_isDynamic );

	uint * buf = 0;
	HRESULT hr = m_d3dbuffer->Map( D3D10_MAP_WRITE_DISCARD, 0, (void**)&buf );
	WinDebug::handleResult( hr, "Problem uploading index buffer" );
	memcpy( buf, data.getRawDataPtr(), data.getLength() * sizeof(uint) );
	m_d3dbuffer->Unmap();
}


void D3dIndexBuffer::p_createBuffer( Array<uint> & indices, bool dynamic ) {
	p_releaseBuffer();
	m_isDynamic = dynamic;
	m_numIndices = indices.getLength();
	D3D10_BUFFER_DESC desc = p_createBufferDesc( m_numIndices, dynamic );

    D3D10_SUBRESOURCE_DATA initData;
	initData.pSysMem = indices.getRawDataPtr();

	HRESULT hr = m_d3d10->getDevice()->CreateBuffer( &desc, &initData, &m_d3dbuffer );

	WinDebug::handleResult( hr, "Failed creating D3d10 Index Buffer." );
}

void D3dIndexBuffer::p_releaseBuffer() {
	if ( m_d3dbuffer ) {
		m_d3dbuffer->Release();
		m_d3dbuffer = 0;
	}
}

D3dIndexBuffer::~D3dIndexBuffer() {
	p_releaseBuffer();
}

D3dIndexBuffer::D3dIndexBuffer( Array<uint> & indices, bool dynamic, D3d10 * d3d10 ) : IndexBuffer(indices, dynamic ? IndexBuffer::Dynamic : 0 ), m_d3dbuffer(0), m_d3d10(d3d10) {
	p_createBuffer( indices, dynamic );	
}

void D3dIndexBuffer::draw( VertexBuffer * vertexBuffer, int primitiveSize ) {
	D3dVertexBuffer * bf = dynamic_cast<D3dVertexBuffer*>(vertexBuffer);
	FASSERT( bf );

	switch( primitiveSize ) {
		case 1:
			drawD3d( bf, D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );
			break;
		case 2:
			drawD3d( bf, D3D10_PRIMITIVE_TOPOLOGY_LINELIST );
			break;
		case 3:
			drawD3d( bf, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
			break;
		default:
			FERROR( "D3dVIndexBuffer::draw: primitiveSize = " + primitiveSize + " not supported." );
			break;
	};
	

}

void D3dIndexBuffer::drawD3d( D3dVertexBuffer * vertexBuffer, D3D10_PRIMITIVE_TOPOLOGY primitiveTopology ) {
	m_d3d10->getDevice()->IASetIndexBuffer( m_d3dbuffer, DXGI_FORMAT_R32_UINT, 0 );
	m_d3d10->getDevice()->IASetPrimitiveTopology( primitiveTopology );
	vertexBuffer->setActive();
	m_d3d10->getDevice()->DrawIndexed( m_numIndices, 0, 0 );
}
