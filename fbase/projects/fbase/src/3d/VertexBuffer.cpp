
#include "VertexBuffer.h"


VertexBuffer::VertexBuffer( VertexType & vertexType, TArray & data, uint bufferInitFlags ) : m_vertexType(vertexType), m_bufferCache(vertexType.getSize()), m_bufferInitFlags( bufferInitFlags ) {
	m_bufferCache = data.cloneRaw();
}

void VertexBuffer::uploadCache() {
	p_upload( m_bufferCache );
}

