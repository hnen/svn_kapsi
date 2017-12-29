/*
 * Effect.h
 *
 *  Created on: 26.5.2010
 *      Author: hth
 */

#ifndef EFFECT_H_
#define EFFECT_H_

#include "../containers/Array.h"
#include "../const.h"
#include "D3d10.h"
#include "D3dTexture.h"
#include "../File.h"
#include "../3d/VertexType.h"
#include "../math/Matrix.h"

/**
 * F interface for D3d10 effect. Requires D3DX.
 **/

class D3dEffect {

	D3d10 * m_d3d10;

	ID3D10Effect * m_effect;
	ID3D10InputLayout * m_vertexLayout;
	ID3D10EffectTechnique * m_technique;

	String m_techniqueName;

	int m_pass;

	VertexType m_vertexType;

	void p_init( const char * data, int dataSize );
	void p_release();

	Array<D3D10_INPUT_ELEMENT_DESC> p_getInputElementDesc( VertexType & vertexType ) const;
	void p_createVertexLayout( const Array<D3D10_INPUT_ELEMENT_DESC> & inputElementDesc );
	void p_unsetVertexLayout();

	void p_unsetTechnique();
	void p_initPass();

	void p_initTechnique( const String & t );

public:

	D3dEffect( const File & fileSrc, const VertexType & vertexType, D3d10 * d3d10 ) :
	  m_d3d10(d3d10), m_effect(NULL), m_vertexLayout(NULL),
	  m_technique(NULL), m_pass(-1), m_vertexType( vertexType ) {
		p_init( fileSrc.getData(), fileSrc.getDataSize() ); };

	~D3dEffect() { p_release(); }

	void setTechnique( const String & techniqueName );
	void setPass( int pass );

	void setActive( const String & techniqueName, int pass );
	void setActive( int pass );
	void setActive();

	void setTexture( const String & varName, D3dTexture * tex ) { m_effect->GetVariableByName( varName.getCStr() )->AsShaderResource()->SetResource( tex->getD3dShaderResource() );  } 
	void setMatrix( const String & varName, Matrix4 & mx ) { m_effect->GetVariableByName( varName.getCStr() )->AsMatrix()->SetMatrix( (float*)&mx );  } 
	
	int getNumPasses();

};


#endif /* EFFECT_H_ */
