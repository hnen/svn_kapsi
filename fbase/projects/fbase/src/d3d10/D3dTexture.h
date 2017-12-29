#ifndef __D3DTEXTURE
#define __D3DTEXTURE

#include <D3D10.h>
#include "D3d10.h"
#include "../File.h"

class D3dTexture {
public:
	enum TextureInitFlag {
		NoMips = 1<<0,
		RenderTarget = 1<<1,
		Dynamic = 1<<2,
		DepthStencil = 1<<3
	};

	typedef uint TextureInitFlag_i;


private:

	D3d10 * m_d3d10;
	ID3D10Texture2D * m_d3dTexture;
	ID3D10ShaderResourceView * m_d3dShaderResourceView;

	D3D10_TEXTURE2D_DESC m_texDesc;

	void p_release();

	void p_createShaderResourceView( uint textureInitFlags );

	void p_initFromFile( const File & file, D3dFormat format, uint textureInitFlags = 0 );
	void p_init( int width, int height, D3dFormat format, uint textureInitFlags, const void * data = 0 );

public:

	int getWidth() const;
	int getHeight() const;

	D3dTexture( D3d10 * d3d10 ) : m_d3d10(d3d10), m_d3dTexture(0), m_d3dShaderResourceView(0) {}
	D3dTexture( const File & file, D3d10 * d3d10 ) : m_d3d10(d3d10), m_d3dTexture(0), m_d3dShaderResourceView(0) { p_initFromFile( file, D3dFormat::RGBA_BYTE ); }


	D3dTexture( int width, int height, D3dFormat format, uint textureInitFlags, D3d10 * d3d10 ) : m_d3d10(d3d10), m_d3dTexture(0), m_d3dShaderResourceView(0) {
		p_init( width, height, format, textureInitFlags, 0 );
	}

	~D3dTexture() { p_release(); }

	ID3D10ShaderResourceView * getD3dShaderResource() { return m_d3dShaderResourceView; }
	ID3D10Texture2D * getD3dTexture() { return m_d3dTexture; }


};




#endif
