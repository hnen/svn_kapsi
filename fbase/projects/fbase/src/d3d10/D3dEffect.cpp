/*
 * Effect.cpp
 *
 *  Created on: 26.5.2010
 *      Author: hth
 */

#include "d3d10_mingw.h"
#include "D3dEffect.h"
#include "../WinDebug.h"


// See D3dVertexBuffer::p_createVertexData for element ordering.
Array<D3D10_INPUT_ELEMENT_DESC> D3dEffect::p_getInputElementDesc( VertexType & vertexType ) const {
	Array<D3D10_INPUT_ELEMENT_DESC> ret;

	int offset = 0;
	for( int l = 0; l < vertexType.getElementList().getLength(); l++ ) {
		VertexElement el = vertexType.getElementList()[l];
		if ( el.type == VertexElement::POSITION ) {
			D3D10_INPUT_ELEMENT_DESC d = { "POSITION", el.id, DXGI_FORMAT_R32G32B32_FLOAT, 0, offset, D3D10_INPUT_PER_VERTEX_DATA, 0 };
			ret.add( d );
			offset += sizeof(float) * 3;
		} else
		if ( el.type == VertexElement::NORMAL ) {
			D3D10_INPUT_ELEMENT_DESC d = { "NORMAL", el.id, DXGI_FORMAT_R32G32B32_FLOAT, 0,offset, D3D10_INPUT_PER_VERTEX_DATA, 0 };
			ret.add( d );
			offset += sizeof(float) * 3;
		} else
		if ( el.type == VertexElement::COLOR ) {
			D3D10_INPUT_ELEMENT_DESC d = { "COLOR", el.id, DXGI_FORMAT_R8G8B8A8_UINT, 0,offset, D3D10_INPUT_PER_VERTEX_DATA, 0 };
			ret.add( d );
			offset += sizeof(unsigned char) * 4;
		}
		if ( el.type == VertexElement::TEXCOORD ) {
			D3D10_INPUT_ELEMENT_DESC d = { "TEXCOORD", el.id, DXGI_FORMAT_R32G32_FLOAT, 0,offset, D3D10_INPUT_PER_VERTEX_DATA, 0 };
			ret.add( d );
			offset += sizeof(float) * 2;
		}
	}

	return ret;
}

void D3dEffect::p_init( const char * effectData, int dataSize ) {
	int hlslFlags = D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;

#ifndef FINAL
	hlslFlags |= D3D10_SHADER_DEBUG;
#else
	hlslFlags |= D3D10_SHADER_OPTIMIZATION_LEVEL3 | D3D10_SHADER_PARTIAL_PRECISION;
#endif

	FLOG( "Creating shader: " + dataSize );

	ID3D10Blob * errors = NULL;

	HRESULT hr = D3DX10CreateEffectFromMemory( effectData, dataSize, "fshader", NULL, NULL, "fx_4_0", hlslFlags, 0, m_d3d10->getDevice(),
			NULL, NULL, &m_effect, &errors, NULL );

	WinDebug::handleResult( hr, String() + "Failed creating D3D10 effect" + (errors ? String() + ", " + (char*)errors->GetBufferPointer() : "") );
	FASSERT( m_effect->IsValid() );

}

void D3dEffect::p_createVertexLayout( const Array<D3D10_INPUT_ELEMENT_DESC> & inputElementDesc ) {
	p_unsetVertexLayout();

	if ( m_pass != -1 ) {
		HRESULT hr = 0;

		D3D10_PASS_DESC PassDesc;
		hr = m_technique->GetPassByIndex( m_pass )->GetDesc( &PassDesc );

		WinDebug::handleResult( hr, "Invalid pass" );

		hr = m_d3d10->getDevice()->CreateInputLayout( inputElementDesc.getDataPtr(), inputElementDesc.getLength(),
    			PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_vertexLayout );

		WinDebug::handleResult( hr, String() + "Failed creating D3D10 input layout for technique " + m_techniqueName + ", pass " + m_pass );

	}
}

void D3dEffect::p_unsetVertexLayout() {
	if ( m_vertexLayout ) {
		m_vertexLayout->Release();
		m_vertexLayout = 0;
	}
}

void D3dEffect::p_unsetTechnique() {
	setPass( -1 );
	if ( m_technique ) {
		m_technique = 0;
	}
}

void D3dEffect::p_initPass() {
	p_createVertexLayout( p_getInputElementDesc( m_vertexType ) );
}

void D3dEffect::p_release() {
	if ( m_effect ) {
		m_effect->Release();
		m_effect = 0;
	}
}

void D3dEffect::setPass( int pass ) {
	if ( pass != m_pass ) {
		m_pass = pass;
		p_initPass();
	}
}

void D3dEffect::p_initTechnique( const String & str ) {
	m_technique = m_effect->GetTechniqueByName( str.getCStr() );
	FASSERT( m_technique->IsValid() );
}

void D3dEffect::setTechnique( const String & str ) {
	p_unsetTechnique();
	p_initTechnique( str );
	m_techniqueName = str;
	setPass( 0 );
}

void D3dEffect::setActive( const String & techniqueName, int pass ) {
	setTechnique( techniqueName );
	setActive( pass );
}

void D3dEffect::setActive( int pass ) {
	setPass( pass );
	setActive();
}

int D3dEffect::getNumPasses() {
	D3D10_TECHNIQUE_DESC td;
	HRESULT hr = m_technique->GetDesc( &td );
	WinDebug::handleResult( hr, "Failed getting shader technique information." );
	return td.Passes;
}


void D3dEffect::setActive() {
	FASSERT( m_pass >= 0 );
	m_d3d10->getDevice()->IASetInputLayout( m_vertexLayout );
	HRESULT hr = m_technique->GetPassByIndex(m_pass)->Apply(0);
	WinDebug::handleResult( hr, "Failed setting shader active." );
}



