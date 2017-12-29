
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexType.h"
#include "../math/algorithm/RadixSort.h"

IndexBuffer::IndexBuffer( Array<uint> & data, uint bufferInitFlags ) :
		m_bufferInitFlags(bufferInitFlags), m_bufferSize(data.getLength()) {
	m_bufferCache = data.clone();
}

void IndexBuffer::copyToCache( Array<uint> & data ) {
	if ( data.getDataPtr() != m_bufferCache.getDataPtr() ) {
		FASSERT( data.getLength() == m_bufferSize );
		m_bufferCache.copy( data );		
	}
}

void IndexBuffer::uploadCache() {
	p_upload( m_bufferCache );
}


////////////////////////////////////////////////////////////////////////
// Index sort

template<int N>
struct SortableIndexPrimitive {
	static byte * g_vertBuf;
	static int g_vertSize;
	static int g_vertPosOffset;
	static Vector3 g_dir;
	int m_value[N];

	operator float() {
		Vector3 P(0, 0, 0);
		for ( int l = 0; l < N; l++ ) {
			P = P + *((Vector3*)&g_vertBuf[ m_value[l] * g_vertSize + g_vertPosOffset ]);
		}
		return P.dot( g_dir );
	}
};

byte * SortableIndexPrimitive<1>::g_vertBuf = 0;
int SortableIndexPrimitive<1>::g_vertSize = 0;
int SortableIndexPrimitive<1>::g_vertPosOffset = 0;
Vector3 SortableIndexPrimitive<1>::g_dir;
byte * SortableIndexPrimitive<2>::g_vertBuf = 0;
int SortableIndexPrimitive<2>::g_vertSize = 0;
int SortableIndexPrimitive<2>::g_vertPosOffset = 0;
Vector3 SortableIndexPrimitive<2>::g_dir;
byte * SortableIndexPrimitive<3>::g_vertBuf = 0;
int SortableIndexPrimitive<3>::g_vertSize = 0;
int SortableIndexPrimitive<3>::g_vertPosOffset = 0;
Vector3 SortableIndexPrimitive<3>::g_dir;
byte * SortableIndexPrimitive<4>::g_vertBuf = 0;
int SortableIndexPrimitive<4>::g_vertSize = 0;
int SortableIndexPrimitive<4>::g_vertPosOffset = 0;
Vector3 SortableIndexPrimitive<4>::g_dir;

template<int N>
//void sortIndexBuffer( const Vector3 & dir, ID3D10Buffer * indexBuffer, int numPrimitives, ID3D10Buffer * vertexBuffer ) {
void sortIndexBuffer( const Vector3 & dir, Array<uint> & indexBuffer, int numPrimitives, TArray & vertexBuffer, int vertexSize, int vertexPosOffset ) {
	SortableIndexPrimitive<N> * indexPtr = 0;
	SortableIndexPrimitive<N>::g_dir = dir;
	
	SortableIndexPrimitive<N>::g_vertBuf = (byte*)vertexBuffer.getRawDataPtr();
	SortableIndexPrimitive<N>::g_vertSize = vertexSize;
	SortableIndexPrimitive<N>::g_vertPosOffset = vertexPosOffset;

	indexPtr = (SortableIndexPrimitive<N>*)indexBuffer.getRawDataPtr();

	RadixSortFloat< SortableIndexPrimitive<N> >().sort( indexPtr, numPrimitives );

	SortableIndexPrimitive<N>::g_vertBuf = 0;
}

void IndexBuffer::sortCache( const Vector3 & dir, VertexBuffer * vertexBuffer, int primitiveSize ) {
	int vertexSize = vertexBuffer->getVertexType().getSize();
	int vertexPosOffset = vertexBuffer->getVertexType().getByteOffset( VertexElement::POSITION );
	int numPrimitives = m_bufferCache.getLength() / primitiveSize;
	switch( primitiveSize ) {
		case 1:
			sortIndexBuffer<1>( dir, m_bufferCache, numPrimitives, vertexBuffer->getCacheRef(), vertexSize, vertexPosOffset );
			break;
		case 2:
			sortIndexBuffer<2>( dir, m_bufferCache, numPrimitives, vertexBuffer->getCacheRef(), vertexSize, vertexPosOffset );
			break;
		case 3:
			sortIndexBuffer<3>( dir, m_bufferCache, numPrimitives, vertexBuffer->getCacheRef(), vertexSize, vertexPosOffset );
			break;
		case 4:
			sortIndexBuffer<4>( dir, m_bufferCache, numPrimitives, vertexBuffer->getCacheRef(), vertexSize, vertexPosOffset );
			break;
		default:
			FERROR( "D3dIndexBuffer::sort: Not implemented for primitiveSize = " + primitiveSize );
			break;
	};
}




