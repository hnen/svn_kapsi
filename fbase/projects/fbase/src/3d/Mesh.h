
#ifndef __MESH
#define __MESH

#include "VertexBuffer.h"
#include "IndexBuffer.h"

class Mesh {

	VertexBuffer * m_vertexBuffer;
	IndexBuffer * m_indexBuffer;

public:

	Mesh( VertexBuffer * vertexBuffer, IndexBuffer * indexBuffer );


};

#endif


