
#ifndef __VERTEXTYPE
#define __VERTEXTYPE

#include "../const.h"
#include "../containers/Array.h"

struct VertexElement {

	enum VertexElementType { POSITION, NORMAL, TEXCOORD, COLOR };

	VertexElementType type;
	int id;

	int getSize() const {
		if ( type == POSITION ) return sizeof(float)*3;
		if ( type == NORMAL ) return sizeof(float)*3;
		if ( type == TEXCOORD ) return sizeof(float)*2;
		if ( type == COLOR ) return sizeof(byte)*4;
		return 0;
	}

	VertexElement( VertexElementType type, int id ) : type(type), id(id) {}
	VertexElement( VertexElementType type ) : type(type), id(0) {}
};

class VertexType {

	Array<VertexElement> m_elementList;

public:

	VertexType() {}
	VertexType( Array<VertexElement> elementList ) : m_elementList(elementList) {}
	Array<VertexElement> getElementList() { return m_elementList; }

	VertexType & add( const VertexElement &a ) { m_elementList.add( a ); return *this; }
	VertexType & add( VertexElement::VertexElementType elementType ) { m_elementList.add( VertexElement( elementType ) ); return *this; }
	VertexType & add( VertexElement::VertexElementType elementType, int id ) { m_elementList.add( VertexElement( elementType, id ) ); return *this; }

	// id = -1 => don't care. Returns -1 if not found.
	int getByteOffset( VertexElement::VertexElementType elementType, int id = -1 ) {
		int ret = 0;
		for ( int l = 0; l < m_elementList.getLength(); l++ ) {
			if ( m_elementList[l].type == elementType && (id == -1 || m_elementList[l].id == id) ) {
				return ret;
			} else {
				ret += m_elementList[l].getSize();
			}
		}
		return -1;
	}

	int getSize() {
		int ret = 0;
		for ( int l = 0; l < m_elementList.getLength(); l++ ) {
			ret += m_elementList[l].getSize();
		}
		return ret;
	}

};

#endif


