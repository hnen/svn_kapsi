
#ifndef __POOPIE_GFX_DX_RENDERER
#define __POOPIE_GFX_DX_RENDERER

#include <d3d11.h>
#include "poopie/gfx/Renderer.h"
#include "poopie/gfx/Format.h"
#include "poopie/gfx/ImageData.h"
#include "poopie/base/Array.h"

namespace poopie {
namespace gfx {

	struct Format;
	class DepthStencil;

namespace dx11 {

	class DxDevice;
	class DxRenderTarget;
	class DxTexture;
	class DxDepthTarget;
	class DxBuffer;
	
	class DxRenderer : public Renderer {
	public:

		void initialize( ui::UIElement * dst );

		void clear( RenderTarget * rt, const Color4 & color );
		void clear( DepthStencil * dt );
		void draw( int vertexCount, RenderTopology rt = RT_TRIANGLES );
		void drawIndexed( int vertexCount, RenderTopology rt = RT_TRIANGLES );
		void drawIndexedInstanced( int vertexCount, int instanceCount, RenderTopology rt = RT_TRIANGLES );
		void update();

		void begin();
		void end();

		RenderTarget * createRenderTarget( Texture * target );
		RenderTarget * getBackbufferRenderTarget() { return (RenderTarget*)m_backbufferRenderTarget; };
		Texture * createRenderTargetTexture( int width, int height, Format & format );
		DepthStencil * createDepthStencilRenderTarget( int width, int height );

		Texture * createTexture( ImageData * imageData );
		Shader * createShader( const char * technique, int pass, const void * shaderData, uint32 shaderDataSize );
		Shader * createShader( const void * shaderData, uint32 shaderDataSize );
		Buffer * createVertexBufferStatic( unsigned int stride, unsigned int size, const void * data );
		Buffer * createIndexBufferStatic( unsigned int size, const void * data );
		Buffer * createVertexBufferDynamic( unsigned int stride, unsigned int size );
		Buffer * createIndexBufferDynamic( unsigned int size );
		Buffer * createConstantBufferStatic( unsigned int size );
		Buffer * createConstantBufferDynamic( unsigned int size );
		Buffer * createStreamOutputBuffer( unsigned int size );
		void copyBufferData( Buffer * dst, Buffer * src, int dstOffset, int srcOffset, int byteCount );
		VertexLayout * createVertexLayout( Shader * shader, Format * fmtList, int fmtCount );

		void setActiveRenderTarget( RenderTarget * rt0, DepthStencil * depth );
		void setActiveRenderTarget( RenderTarget * rt0, RenderTarget * rt1, DepthStencil * depth );
		RenderTarget * getPrimaryRenderTarget();
		void setActiveVertexBuffer( Buffer * vertexBuffer );
		void setActiveIndexBuffer( Buffer * indexBuffer );
		void setActiveShader( Shader * shader );
		void setActiveVertexLayout( VertexLayout * vl );
		void setStreamOutputBuffers( Buffer * so0, Buffer * so1 = NULL, Buffer * so2 = NULL, Buffer * so3 = NULL );

		DXGI_FORMAT getDxgiFormat( ImageDataFormat fmt );
		
		int fetchElementsWrittenInStreamOutput( int stream );

		~DxRenderer();

	private:

		DxDevice * m_device;

		DxRenderTarget * m_backbufferRenderTarget;
		DxRenderTarget * m_primaryRenderTarget;

		struct {
			Array<DxRenderTarget*> m_activeRT;
			DxDepthTarget * m_activeDepthRT;
		} m_activeRT;

		void setTopology( RenderTopology rt );

		ID3D11Buffer * m_activeVertexBuffer;

	};
	
}
}
}


#endif

