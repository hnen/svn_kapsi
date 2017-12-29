
#include "poopie/gfx/dx11/DxRenderer.h"
#include "poopie/gfx/dx11/DxDevice.h"
#include "poopie/gfx/dx11/DxTexture.h"
#include "poopie/gfx/dx11/DxShader.h"
#include "poopie/gfx/dx11/DxBuffer.h"
#include "poopie/gfx/dx11/DxFormat.h"
#include "poopie/gfx/dx11/DxRenderTarget.h"
#include "poopie/gfx/dx11/DxVertexLayout.h"
#include "poopie/gfx/dx11/DxDepthStencil.h"

#include "poopie/gfx/Format.h"
#include "poopie/gfx/ImageData.h"

#include "poopie/ui/win32/WinUIElement.h"

namespace poopie {
namespace gfx {
	
	Renderer * Renderer::create() {
		return new dx11::DxRenderer();
	}

namespace dx11{

	void DxRenderer::initialize( poopie::ui::UIElement * el ) {
		m_device = new DxDevice( dynamic_cast<poopie::ui::win::WinUIElement*>(el) );
		m_backbufferRenderTarget = (DxRenderTarget*)createRenderTarget( m_device->getBackbufferTexture() );

		m_primaryRenderTarget = 0;
		setActiveRenderTarget( m_backbufferRenderTarget, NULL );
		m_activeVertexBuffer = 0;
	}

	DxRenderer::~DxRenderer() {
		setActiveVertexBuffer( 0 );
		delete m_device;
	}


	void DxRenderer::setActiveRenderTarget( RenderTarget * _rt, DepthStencil * _dt ) {
		DxRenderTarget * rt = (DxRenderTarget*)_rt;
		DxDepthStencil * dt = (DxDepthStencil*)_dt;
		ID3D11RenderTargetView * d3drt = rt->getRenderTargetView();
		m_device->getDeviceContext()->OMSetRenderTargets( 1, &d3drt, dt != NULL ? dt->m_dxView : NULL );
		m_primaryRenderTarget = rt;

		D3D11_VIEWPORT vp;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.Width = (FLOAT)rt->getWidth();
		vp.Height = (FLOAT)rt->getHeight();
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		m_device->getDeviceContext()->RSSetViewports( 1, &vp );

	}

	void DxRenderer::setActiveRenderTarget( RenderTarget * _rt0, RenderTarget * _rt1, DepthStencil * _dt ) {
		DxRenderTarget * rt0 = (DxRenderTarget*)_rt0;
		DxRenderTarget * rt1 = (DxRenderTarget*)_rt1;
		DxDepthStencil * dt = (DxDepthStencil*)_dt;
		ID3D11RenderTargetView * d3drt[] = {
			rt0->getRenderTargetView(),
			rt1->getRenderTargetView()
		};
		m_device->getDeviceContext()->OMSetRenderTargets( 2, d3drt, dt != NULL ? dt->m_dxView : NULL );
		m_primaryRenderTarget = rt0;

		D3D11_VIEWPORT vp;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.Width = (FLOAT)rt0->getWidth();
		vp.Height = (FLOAT)rt0->getHeight();
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		m_device->getDeviceContext()->RSSetViewports( 1, &vp );
	}

	void DxRenderer::clear( RenderTarget * _rt, const Color4 & color ) {
		DxRenderTarget * rt = (DxRenderTarget*)_rt;
		m_device->getDeviceContext()->ClearRenderTargetView( rt->getRenderTargetView(), (const float*)&color );
	}

	void DxRenderer::clear( DepthStencil * _dt ) {
		DxDepthStencil * dt = (DxDepthStencil*)_dt;
		m_device->getDeviceContext()->ClearDepthStencilView( dt->m_dxView, D3D11_CLEAR_STENCIL | D3D11_CLEAR_DEPTH, 1.0f, 0 );
	}



	void DxRenderer::setActiveVertexBuffer( Buffer * vertexBuffer ) {
		if ( vertexBuffer ) {
			POOP_ASSERT( vertexBuffer->GetType() == VERTEX );
			DxBuffer * vb = (DxBuffer*)vertexBuffer;
			ID3D11Buffer * dxbuf = vb->GetDxHandle();
			if ( m_activeVertexBuffer != dxbuf ) {
				m_activeVertexBuffer = dxbuf;
				unsigned int stride = vb->GetStride();
				unsigned int zero = 0;
				m_device->getDeviceContext()->IASetVertexBuffers( 0, 1, &dxbuf, &stride, &zero );
			}
		} else {
			m_activeVertexBuffer = 0;
			m_device->getDeviceContext()->IASetVertexBuffers( 0, 0, NULL, NULL, NULL );
		}
	}

	void DxRenderer::setActiveVertexLayout( VertexLayout * vl ) {
		DxVertexLayout * dvl = (DxVertexLayout*)vl;
		m_device->getDeviceContext()->IASetInputLayout( dvl->GetDxHandle() );
	}

	void DxRenderer::setActiveIndexBuffer( Buffer * indexBuffer ) {
		POOP_ASSERT( indexBuffer->GetType() == INDEX );
		POOP_ASSERT( indexBuffer->GetStride() == 2 || indexBuffer->GetStride() == 4 );
		if ( indexBuffer ) {
			DxBuffer * ib = (DxBuffer*)indexBuffer;
			ID3D11Buffer * dxbuf = ib->GetDxHandle();
			unsigned int stride = ib->GetStride();
			unsigned int zero = 0;
			DXGI_FORMAT format = stride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
			m_device->getDeviceContext()->IASetIndexBuffer( dxbuf, format, 0 );
		} else {
			m_device->getDeviceContext()->IASetVertexBuffers( 0, 0, NULL, NULL, NULL );
		}
	}

	Buffer * DxRenderer::createIndexBufferStatic( unsigned int size, const void * data ) {
		return new DxBuffer( m_device, INDEX, size, 4, false, false, data );
	}

	Buffer * DxRenderer::createVertexBufferStatic( unsigned int stride, unsigned int size, const void * data ) {
		DxBuffer * nbuffer = new DxBuffer( m_device, VERTEX, size, stride, false, false, data );
		return nbuffer;
	}

	Buffer * DxRenderer::createIndexBufferDynamic( unsigned int size ) {
		return new DxBuffer( m_device, INDEX, size, 4, true, false, NULL );
	}

	Buffer * DxRenderer::createVertexBufferDynamic( unsigned int stride, unsigned int size ) {
		DxBuffer * nbuffer = new DxBuffer( m_device, VERTEX, size, stride, true, false, NULL );
		return nbuffer;
	}

	Buffer * DxRenderer::createConstantBufferDynamic( unsigned int size ) {
		DxBuffer * nbuffer = new DxBuffer( m_device, CONSTANT, 1, size, true, false, NULL );
		return nbuffer;
	}
	Buffer * DxRenderer::createConstantBufferStatic( unsigned int size ) {
		DxBuffer * nbuffer = new DxBuffer( m_device, CONSTANT, 1, size, false, false, NULL );
		return nbuffer;
	}

	void DxRenderer::copyBufferData( Buffer * dst, Buffer * src, int dstOffset, int srcOffset, int byteCount ) {
		D3D11_BOX srcb = {0,0,0,0,0,0};
		srcb.left = srcOffset;
		srcb.right = srcOffset + byteCount;
		m_device->getDeviceContext()->CopySubresourceRegion( ((DxBuffer*)dst)->GetDxHandle(), 0, dstOffset, 0, 0, ((DxBuffer*)src)->GetDxHandle(), 0, &srcb );
	}

	int DxRenderer::fetchElementsWrittenInStreamOutput( int str ) {
		D3D11_QUERY_DESC qdesc;
		qdesc.Query = D3D11_QUERY_SO_STATISTICS;
		qdesc.MiscFlags = 0;
		ID3D11Query * q = 0;
		D3D11_QUERY_DATA_SO_STATISTICS result;
		m_device->getDevice()->CreateQuery( &qdesc, &q );
		m_device->getDeviceContext()->GetData( q, &result, sizeof(result), 0 );
		return result.NumPrimitivesWritten;
	}

	Buffer * DxRenderer::createStreamOutputBuffer( unsigned int size ) {
		DxBuffer * nbuffer = new DxBuffer( m_device, RAW, 1, size, true, true, NULL );
		return nbuffer;
	}

	void DxRenderer::begin() {
		m_device->getDeviceContext()->Begin( NULL );
	}
	void DxRenderer::end() {
		m_device->getDeviceContext()->End( NULL );
	}

	VertexLayout * DxRenderer::createVertexLayout( Shader * shader, Format * fmtList, int fmtCount ) {
		DxVertexLayout * vl = new DxVertexLayout( m_device, (DxShader*)shader, fmtList, fmtCount );
		((DxShader*)shader)->bindVertexLayout( vl );
		return vl;
	}

	void DxRenderer::setActiveShader( Shader * shader ) {
		if ( shader->getVertexLayout() ) {
			setActiveVertexLayout( shader->getVertexLayout() );
		}
		shader->apply();
	}

	void DxRenderer::update() {
		m_device->getSwapChain()->Present(0,0);
	}

	void DxRenderer::setTopology( RenderTopology rt ) {
		if ( rt == RT_TRIANGLES ) {
			m_device->getDeviceContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		} else if ( rt == RT_TRIANGLESTRIP ) {
			m_device->getDeviceContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		} else if ( rt == RT_POINTS ) {
			m_device->getDeviceContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
		} else {
			POOP_TODO;
		}
	}


	void DxRenderer::drawIndexed( int vertexCount, RenderTopology rt ) {
		setTopology( rt );
		m_device->getDeviceContext()->DrawIndexed( vertexCount, 0, 0 );
	}

	void DxRenderer::drawIndexedInstanced( int vertexCount, int instanceCount, RenderTopology rt ) {
		setTopology( rt );
		m_device->getDeviceContext()->DrawIndexedInstanced( vertexCount, instanceCount, 0, 0, 0 );
	}

	void DxRenderer::draw( int vertexCount, RenderTopology rt ) {
		setTopology( rt );
		m_device->getDeviceContext()->Draw( vertexCount, 0 );
	}

	Shader * DxRenderer::createShader( const void * shaderData, uint32 shaderDataSize ) {
		POOP_INTERNAL_LOG( String("Creating shader of size ") + shaderDataSize );
		DxShader * ret = new DxShader( m_device );
		ret->initialize( shaderData, shaderDataSize );
		ret->setTechniqueAndPass( 0,0 );
		return ret;
	}

	Shader * DxRenderer::createShader( const char * technique, int pass, const void * shaderData, uint32 shaderDataSize ) {
		POOP_INTERNAL_LOG( String("Creating shader of size ") + shaderDataSize + String( " for technique " ) + technique + String( " and pass #" ) + pass );
		DxShader * ret = new DxShader( m_device );
		ret->initialize( shaderData, shaderDataSize );
		ret->setTechniqueAndPass( technique, pass );
		return ret;
	}

	RenderTarget * DxRenderer::createRenderTarget( Texture * _texture ) {
		DxTexture * texture = (DxTexture*)_texture;
		ID3D11RenderTargetView * rv = NULL;
		m_device->getDevice()->CreateRenderTargetView( texture->getDxTexture(), NULL, &rv );
		DxRenderTarget * ret = new DxRenderTarget( rv, _texture );
		rv->Release();
		return ret;
	}

	RenderTarget * DxRenderer::getPrimaryRenderTarget() {
		return m_primaryRenderTarget;
	}

	DepthStencil * DxRenderer::createDepthStencilRenderTarget( int width, int height ) {
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );
		desc.Width = width;
		desc.Height = height;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.ArraySize = 1;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.MiscFlags = 0;
		ID3D11Texture2D * t = NULL;
		m_device->getDevice()->CreateTexture2D( &desc, 0, &t );

		/*
		D3D11_SHADER_RESOURCE_VIEW_DESC srdesc;
		srdesc.Format = desc.Format;
		srdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srdesc.Texture2D.MostDetailedMip = 0;
		srdesc.Texture2D.MipLevels = desc.MipLevels;
		ID3D11ShaderResourceView * rsv = 0;
		m_device->getDevice()->CreateShaderResourceView( t, &srdesc, &rsv );
		*/

		DxTexture * texture = new DxTexture( t, 0 );

		//D3D11_DEPTH_STENCIL_DESC desc;
		//desc.

		ID3D11DepthStencilView * dsv = NULL;
		m_device->getDevice()->CreateDepthStencilView( texture->getDxTexture(), NULL, &dsv );
		//DxRenderTarget * ret = new DxRenderTarget( rv, _texture );
		//rv->Release();

		DepthStencil * ret = new DxDepthStencil( dsv, texture );
		return ret;
	}


	Texture * DxRenderer::createRenderTargetTexture( int width, int height, Format & format ) {
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );
		desc.Width = width;
		desc.Height = height;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.Format = DxFormat::toDxgiFormat( format );
		desc.ArraySize = 1;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.MiscFlags = 0;
		ID3D11Texture2D * t = NULL;
		m_device->getDevice()->CreateTexture2D( &desc, 0, &t );

		D3D11_SHADER_RESOURCE_VIEW_DESC srdesc;
		srdesc.Format = desc.Format;
		srdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srdesc.Texture2D.MostDetailedMip = 0;
		srdesc.Texture2D.MipLevels = desc.MipLevels;
		ID3D11ShaderResourceView * rsv = 0;
		m_device->getDevice()->CreateShaderResourceView( t, &srdesc, &rsv );

		DxTexture * tx = new DxTexture( t, rsv );
		return tx;
	}

	void DxRenderer::setStreamOutputBuffers( Buffer * so0, Buffer * so1, Buffer * so2, Buffer * so3 ) {
		ID3D11Buffer * tgts [] = { ((DxBuffer*)so0)->GetDxHandle(), ((DxBuffer*)so1)->GetDxHandle(), ((DxBuffer*)so2)->GetDxHandle(), ((DxBuffer*)so3)->GetDxHandle() };
		int n = 0;
		if ( so0 != 0 ) n++;
		if ( so1 != 0 ) n++;
		if ( so2 != 0 ) n++;
		if ( so3 != 0 ) n++;
		m_device->getDeviceContext()->SOSetTargets( n, tgts, NULL );

	}

	DXGI_FORMAT DxRenderer::getDxgiFormat( ImageDataFormat fmt ) {
		switch( fmt ) {
		case IMG_FMT_RGBA_UINT:
			return DXGI_FORMAT_R32G32B32A32_UINT;
		case IMG_FMT_RGB_UINT:
			return DXGI_FORMAT_R32G32B32_UINT;
		case IMG_FMT_RG_UINT:
			return DXGI_FORMAT_R32G32_UINT;
		case IMG_FMT_R_UINT:
			return DXGI_FORMAT_R32_UINT;
		case IMG_FMT_RGBA_UBYTE:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case IMG_FMT_RG_UBYTE:
			return DXGI_FORMAT_R8G8_UNORM;
		case IMG_FMT_R_UBYTE:
			return DXGI_FORMAT_R8_UNORM;
		case IMG_FMT_RGBA_FLOAT:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case IMG_FMT_RGB_FLOAT:
			return DXGI_FORMAT_R32G32B32_FLOAT;
		case IMG_FMT_RG_FLOAT:
			return DXGI_FORMAT_R32G32_FLOAT;
		case IMG_FMT_R_FLOAT:
			return DXGI_FORMAT_R32_FLOAT;
		default:
			POOP_ERROR( "Undefined format" );
			return (DXGI_FORMAT)-1;
		}
	}

	Texture * DxRenderer::createTexture( ImageData * imageData ) {
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );
		desc.Width = imageData->getWidth();
		desc.Height = imageData->getHeight();
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.Format = getDxgiFormat( imageData->getFormat() ); //DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.ArraySize = 1;
		desc.MipLevels = imageData->getMips();
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.MiscFlags = 0;
		ID3D11Texture2D * t = NULL;

		D3D11_SUBRESOURCE_DATA * rdata = new D3D11_SUBRESOURCE_DATA[imageData->getMips()];
		for( int m = 0; m < imageData->getMips(); m++ ) {
			rdata[m].pSysMem = imageData->getData(m);
			rdata[m].SysMemPitch = imageData->getWidth() * imageData->getDepth() >> m;
			rdata[m].SysMemSlicePitch = imageData->getHeight() * imageData->getWidth() * imageData->getDepth() >> m*2;
		}

		m_device->getDevice()->CreateTexture2D( &desc, rdata, &t );

		delete [] rdata;

		D3D11_SHADER_RESOURCE_VIEW_DESC srdesc;
		srdesc.Format = desc.Format;
		srdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srdesc.Texture2D.MostDetailedMip = 0;
		srdesc.Texture2D.MipLevels = desc.MipLevels;
		ID3D11ShaderResourceView * rsv = 0;
		m_device->getDevice()->CreateShaderResourceView( t, &srdesc, &rsv );

		DxTexture * tx = new DxTexture( t, rsv );
		return tx;
	}


	


}
}
}



