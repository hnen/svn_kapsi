
#include "../WinDebug.h"
#include "../DebugTools.h"

#include "D3dTexture.h"

#include <D3DX10.h>

void D3dTexture::p_release() {
	if ( m_d3dTexture ) {
		m_d3dTexture->Release();
	}
	if ( m_d3dShaderResourceView ) {
		m_d3dShaderResourceView->Release();
	}
	m_d3dTexture = 0;
	m_d3dShaderResourceView = 0;
}

int D3dTexture::getWidth() const {
	return m_texDesc.Width;
}

int D3dTexture::getHeight() const {
	return m_texDesc.Height;
}


void D3dTexture::p_createShaderResourceView( uint textureInitFlags ) {
	FASSERT( m_d3dTexture );

	D3D10_TEXTURE2D_DESC texDesc;
	m_d3dTexture->GetDesc( &texDesc );

	D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;

	// WTF: Why this must be -1?
	srvDesc.Texture2D.MipLevels =  (textureInitFlags & NoMips) ? 1 : -1;
	//srvDesc.Texture2D.MipLevels = texDesc.MipLevels;

	//srvDesc.Texture2D.MostDetailedMip = texDesc.MipLevels - 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	m_d3dShaderResourceView = 0;
	HRESULT hr = m_d3d10->getDevice()->CreateShaderResourceView( m_d3dTexture, &srvDesc, &m_d3dShaderResourceView );

	WinDebug::handleResult( hr, "Error creating ShaderResourceView" );

}

void D3dTexture::p_init( int width, int height, D3dFormat format, uint textureInitFlags, const void * data ) {
	p_release();

	FASSERT( !( (textureInitFlags & RenderTarget) != 0 && (textureInitFlags & NoMips) == 0 ) );

	D3D10_TEXTURE2D_DESC texDesc;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = (textureInitFlags & NoMips) ? 1 : 0;
	texDesc.ArraySize = 1;
	texDesc.Format = format.dxgiFormat;

	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	texDesc.Usage = (textureInitFlags & Dynamic) ? D3D10_USAGE_DYNAMIC : D3D10_USAGE_IMMUTABLE;
	
	texDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	if ( textureInitFlags & RenderTarget ) {
		texDesc.BindFlags |= D3D10_BIND_RENDER_TARGET;
	}

	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = (textureInitFlags & NoMips ? 0 : D3D10_RESOURCE_MISC_GENERATE_MIPS );

	D3D10_SUBRESOURCE_DATA resData;
	resData.SysMemPitch = width * 4;
	resData.SysMemSlicePitch = 0;
	resData.pSysMem = data;

	m_d3d10->getDevice()->CreateTexture2D( &texDesc, !data ? NULL : &resData, &m_d3dTexture );
	p_createShaderResourceView( textureInitFlags );

	m_d3dTexture->GetDesc( &m_texDesc );

}

void D3dTexture::p_initFromFile( const File & f, D3dFormat format, uint textureInitFlags ) {

	p_release();

	D3DX10_IMAGE_LOAD_INFO loadInfo;
	ZeroMemory( &loadInfo, sizeof(D3DX10_IMAGE_LOAD_INFO) );
	loadInfo.CpuAccessFlags = D3DX10_DEFAULT;
	loadInfo.Depth = D3DX10_DEFAULT;
	loadInfo.Filter = D3DX10_DEFAULT;
	loadInfo.FirstMipLevel = D3DX10_DEFAULT;
	loadInfo.MipLevels = textureInitFlags & NoMips ? 1 : 0;
	loadInfo.Format = format.dxgiFormat;
	loadInfo.Height = D3DX10_DEFAULT;
	loadInfo.MipFilter = D3DX10_DEFAULT;
	loadInfo.MiscFlags = (textureInitFlags & NoMips ? 0 : D3D10_RESOURCE_MISC_GENERATE_MIPS);
	loadInfo.pSrcInfo = NULL;
	loadInfo.Width = D3DX10_DEFAULT;
	loadInfo.BindFlags = D3D10_BIND_SHADER_RESOURCE;

	ID3D10Resource * resource = NULL;
	HRESULT hr = D3DX10CreateTextureFromMemory( m_d3d10->getDevice(), f.getData(), f.getDataSize(), NULL, NULL, &resource, NULL );
	WinDebug::handleResult( hr );

	D3D10_RESOURCE_DIMENSION type;
	resource->GetType( &type );

	if ( type != D3D10_RESOURCE_DIMENSION_TEXTURE2D ) {
		FERROR( "File loaded isn't a 2d Texture" );
	} else {
		m_d3dTexture = static_cast<ID3D10Texture2D*>(resource);
	}
	p_createShaderResourceView( textureInitFlags );

	m_d3dTexture->GetDesc( &m_texDesc );
}



