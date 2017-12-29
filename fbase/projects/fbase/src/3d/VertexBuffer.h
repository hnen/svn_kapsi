
#ifndef __VERTEXBUFFER
#define __VERTEXBUFFER

#include "VertexType.h"

class VertexBuffer {
public:

	enum BufferInitFlags {
		Dynamic = 1<<0
	};

private:

	TArray m_bufferCache;
	uint m_bufferInitFlags;

protected:

	VertexType m_vertexType;
	virtual void p_upload( TArray & data ) = 0;

public:

	VertexBuffer( VertexType & vertexType, TArray & data, uint bufferInitFlags = 0 );

	VertexType & getVertexType() { return m_vertexType; };

	TArray & getCacheRef() { return m_bufferCache; };
	void copyToCache( TArray & data );
	void uploadCache();

	virtual void draw( int primitiveSize = 3 ) = 0;

};

#endif


