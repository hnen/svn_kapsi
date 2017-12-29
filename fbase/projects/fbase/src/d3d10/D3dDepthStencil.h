
#include <D3D10.h>
#include "D3d10.h"
#include "D3dTexture.h"


class D3dDepthStencil {
	
	D3d10 * m_d3d10;
	ID3D10DepthStencilView * m_d3dDepthStencil;
	D3dTexture * m_tex;

	void p_init( D3dFormat foramt );

public:

	D3dDepthStencil( int width, int height, D3d10 * d3d10 );
	~D3dDepthStencil();

};


