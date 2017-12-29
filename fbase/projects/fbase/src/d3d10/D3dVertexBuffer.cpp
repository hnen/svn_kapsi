
#include "../containers/ByteStream.h"
#include "../containers/Array.h"
#include "../WinDebug.h"
#include "D3dVertexBuffer.h"

void D3dVertexBuffer::p_release() {
	if ( m_d3dbuffer ) {
		m_d3dbuffer->Release();
		m_d3dbuffer = 0;
	}
	m_numVertices = 0;
}

void D3dVertexBuffer::draw( int primitiveSize ) {
	switch( primitiveSize ) {
		case 1:
			drawD3d( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );
			break;
		case 2:
			drawD3d( D3D10_PRIMITIVE_TOPOLOGY_LINELIST );
			break;
		case 3:
			drawD3d( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
			break;
		default:
			FERROR( "D3dVertexBuffer::draw: primitiveSize = " + primitiveSize + " not supported." );
			break;
	};
}

D3D10_BUFFER_DESC D3dVertexBuffer::p_createBufferDesc( VertexType & vertexType, int numElements, bool dynamic ) {
	D3D10_BUFFER_DESC desc;
	desc.Usage = dynamic ? D3D10_USAGE_DYNAMIC : D3D10_USAGE_IMMUTABLE;
	desc.ByteWidth = vertexType.getSize() * numElements;
	desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = dynamic ? D3D10_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags = 0;
	return desc;
}

void D3dVertexBuffer::setActive() {
	FASSERT( m_d3dbuffer );
	unsigned int stride = m_vertexType.getSize();
	unsigned int offset = 0;
	m_d3d10->getDevice()->IASetVertexBuffers( 0, 1, &m_d3dbuffer, &stride, &offset );
}

// See D3dEffect::p_getInputElementDesc for element ordering.
/*
ByteStream D3dVertexBuffer::p_createVertexData( TArray & vertices ) {
	ByteStream data;
	for( int l = 0; l < vertices.getLength(); l++ ) {
		TVertex * v = vertices[l];
		if ( v->getPosition() ) {
			data.add( (void*)v->getPosition(), sizeof(Vector3) );
		}
		if ( v->getNormal() ) {
			data.add( (void*)v->getNormal(), sizeof(Vector3) );
		}
		if ( v->getColor() ) {
			unsigned int tmpCol = v->getColor()->getARGB8();
			data.add( &tmpCol, sizeof(unsigned int) );
		}
		for( int l = 0; l < VERTEX_MAX_TEXCOORDS; l++ ) {
			if ( v->getTexCoord( l ) ) {
				data.add( (void*)v->getTexCoord(l), sizeof(Vector2) );
			}
		}
	}
	return data;
}
*/

void D3dVertexBuffer::drawD3d( D3D10_PRIMITIVE_TOPOLOGY primitiveTopology ) {
	m_d3d10->getDevice()->IASetPrimitiveTopology( primitiveTopology );
	setActive();
	m_d3d10->getDevice()->Draw( m_numVertices, 0 );
}

void D3dVertexBuffer::p_upload( TArray & data ) {
	FASSERT( m_isDynamic );

	uint * buf = 0;
	HRESULT hr = m_d3dbuffer->Map( D3D10_MAP_WRITE_DISCARD, 0, (void**)&buf );
	WinDebug::handleResult( hr, "Problem uploading vertex buffer" );
	memcpy( buf, data.getRawDataPtr(), data.getLength() * m_vertexType.getSize() );
	m_d3dbuffer->Unmap();
	
}

void D3dVertexBuffer::p_createBuffer( TArray & vertices, VertexType vertexType, bool dynamic ) {
	p_release();

	m_isDynamic = dynamic;
	m_vertexType = vertexType;
	m_numVertices = vertices.getLength();
	D3D10_BUFFER_DESC desc = p_createBufferDesc( m_vertexType, m_numVertices, dynamic );

    D3D10_SUBRESOURCE_DATA initData;
	
	//ByteStream vertexData = p_createVertexData( vertices );
	initData.pSysMem = vertices.getRawDataPtr();
	initData.SysMemPitch = initData.SysMemSlicePitch = vertexType.getSize() * vertices.getLength();

	HRESULT hr = m_d3d10->getDevice()->CreateBuffer( &desc, &initData, &m_d3dbuffer );

	WinDebug::handleResult( hr, "Failed creating D3d10 Vertex Buffer." );
}

