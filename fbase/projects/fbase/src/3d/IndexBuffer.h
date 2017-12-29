
#ifndef __INDEXBUFFER
#define __INDEXBUFFER

#include "../const.h"
#include "../math/Vector.h"
#include "../containers/Array.h"

class VertexBuffer;

class IndexBuffer {
public:

	enum BufferInitFlags {
		Dynamic = 1<<0
	};

private:
	
	uint m_bufferInitFlags;
	Array<uint> m_bufferCache;
	int m_bufferSize;

protected:
	virtual void p_upload( Array<uint> & data ) = 0;
	IndexBuffer( Array<uint> & data, uint bufferInitFlags );

public:

	virtual void draw( VertexBuffer * vertexBuffer, int primitiveSize ) = 0;

	Array<uint> & getCacheRef() { return m_bufferCache; };
	void copyToCache( Array<uint> & data );

	void uploadCache();
	void sortCache( const Vector3 & dir, VertexBuffer * vertexBuffer, int primitiveSize = 3 );

};


#endif


