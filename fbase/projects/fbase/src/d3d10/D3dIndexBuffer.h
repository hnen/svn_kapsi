
#ifndef __D3DINDEXBUFFER
#define __D3DINDEXBUFFER

#include "../const.h"

#include "../3d/IndexBuffer.h"

#include "D3d10.h"
#include "../containers/Array.h"
#include "../math/Vector.h"

class D3dVertexBuffer;
class VertexBuffer;

class D3dIndexBuffer : public IndexBuffer {

	int m_numIndices;
	ID3D10Buffer * m_d3dbuffer;
	D3d10 * m_d3d10;
	bool m_isDynamic;

	D3D10_BUFFER_DESC p_createBufferDesc( int numElements, bool dynamic );
	void p_createBuffer( Array<uint> & indices, bool dynamic );
	void p_releaseBuffer();

protected:
	void p_upload( Array<uint> & data );
	void p_init( Array<uint> & data, uint bufferInitFlags );
	void p_init( uint size, uint bufferInitFlags );

public:

	D3dIndexBuffer( Array<uint> & indices, bool dynamic, D3d10 * d3d10 );
	~D3dIndexBuffer();

	void draw( VertexBuffer * vertexBuffer, int primitiveSize );
	void drawD3d( D3dVertexBuffer * vertexBuffer, D3D10_PRIMITIVE_TOPOLOGY primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	
};


#endif

