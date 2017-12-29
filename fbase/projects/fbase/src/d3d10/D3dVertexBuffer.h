
#ifndef __D3DVERTEXBUFFER
#define __D3DVERTEXBUFFER

#include <D3D10.h>

#include "D3d10.h"
#include "../containers/ByteStream.h"
#include "../3d/VertexType.h"
#include "../3d/VertexBuffer.h"

class D3dIndexBuffer;

class D3dVertexBuffer : public VertexBuffer {

	D3d10 * m_d3d10;

	ID3D10Buffer * m_d3dbuffer;
	int m_numVertices;
	bool m_isDynamic;


	void p_release();
	void p_createBuffer( TArray & vertices, VertexType vertexType, bool dynamic);
	D3D10_BUFFER_DESC p_createBufferDesc( VertexType & vertexType, int numElements, bool dynamic );
	ByteStream p_createVertexData( TArray & vertices );

	void p_upload( TArray & data );

public:

	D3dVertexBuffer( TArray & vertices, VertexType vertexType, bool dynamic, D3d10 * d3d10 ) :
		VertexBuffer( vertexType, vertices, dynamic ? VertexBuffer::Dynamic : 0 ),
		m_d3d10(d3d10), m_d3dbuffer(NULL), m_numVertices(0) {

			p_createBuffer( vertices, vertexType, dynamic );
	};
	~D3dVertexBuffer() { p_release(); };

	void setActive();

	void draw( int primitiveSize );

	void drawD3d( D3D10_PRIMITIVE_TOPOLOGY primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	friend class D3dIndexBuffer;
};


#endif

