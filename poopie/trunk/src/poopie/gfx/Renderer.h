
#ifndef __POOPIE_GFX_RENDERER
#define __POOPIE_GFX_RENDERER

#include "poopie/gfx/Mesh.h"
#include "poopie/gfx/Buffer.h"

#include "poopie/defs.h"
#include "poopie/math/Color4.h"

namespace poopie {

	namespace ui {
		class UIElement;
	}

namespace gfx {

	class RenderTarget;
	class Texture;
	class Shader;	
	class Buffer;
	class VertexLayout;
	class ImageData;
	class DepthStencil;
	struct Format;

	enum RenderTopology {
		RT_TRIANGLES, RT_TRIANGLESTRIP, RT_POINTS
	};

	class Renderer {
	public:


		static Renderer * create();
		virtual void initialize( ui::UIElement * dest ) = 0;
		virtual void clear( RenderTarget * rt, const Color4 & color )  { POOP_TODO; }
		virtual void clear( DepthStencil * dt )  { POOP_TODO; }
		virtual void draw( int vertexCount, RenderTopology rt = RT_TRIANGLES ) { POOP_TODO; }
		virtual void drawIndexed( int vertexCount, RenderTopology rt = RT_TRIANGLES ) { POOP_TODO; }
		virtual void drawIndexedInstanced( int vertexCount, int instanceCount, RenderTopology rt = RT_TRIANGLES ) { POOP_TODO; }
		virtual void update() = 0;

		virtual void begin() { POOP_TODO; }
		virtual void end() { POOP_TODO; }
		
		virtual void setActiveRenderTarget( RenderTarget * rt0, DepthStencil * depth ) { POOP_TODO; }
		virtual void setActiveRenderTarget( RenderTarget * rt0, RenderTarget * rt1, DepthStencil * depth ) { POOP_TODO; }
		virtual RenderTarget * getPrimaryRenderTarget() { POOP_TODO; return 0; }
		virtual void setActiveVertexBuffer( Buffer * vertexBuffer ) { POOP_TODO; }
		virtual void setActiveIndexBuffer( Buffer * indexBuffer ) { POOP_TODO; }
		virtual void setActiveShader( Shader * shader ) { POOP_TODO; }
		virtual void setActiveVertexLayout( VertexLayout * vl ) { POOP_TODO; }
		virtual void setStreamOutputBuffers( Buffer * so0, Buffer * so1 = NULL, Buffer * so2 = NULL, Buffer * so3 = NULL ) { POOP_TODO; }

		virtual RenderTarget * createRenderTarget( Texture * targetTexture ) { POOP_TODO; return 0; };
		virtual Texture * createRenderTargetTexture( int width, int height, Format & format ) = 0;
		virtual DepthStencil * createDepthStencilRenderTarget( int width, int height ) = 0;
		virtual Texture * createTexture( ImageData * imageData ) = 0;
		virtual RenderTarget * getBackbufferRenderTarget() { POOP_TODO; return 0; };
		virtual Shader * createShader( const char * technique, int pass, const void * shaderData, uint32 dataSize ) { POOP_TODO; return 0; };
		virtual Shader * createShader( const void * shaderData, uint32 dataSize ) { POOP_TODO; return 0; };
		virtual Buffer * createVertexBufferStatic( unsigned int stride, unsigned int size, const void * data ) { POOP_TODO; return 0; }
		virtual Buffer * createIndexBufferStatic( unsigned int size, const void * data ) { POOP_TODO; return 0; }
		virtual Buffer * createVertexBufferDynamic( unsigned int stride, unsigned int size ) { POOP_TODO; return 0; }
		virtual Buffer * createIndexBufferDynamic( unsigned int size ) { POOP_TODO; return 0; }
		virtual Buffer * createConstantBufferStatic( unsigned int size ) { POOP_TODO; return 0; }
		virtual Buffer * createConstantBufferDynamic( unsigned int size ) { POOP_TODO; return 0; }
		virtual Buffer * createStreamOutputBuffer( unsigned int size ) { POOP_TODO; return 0; }
		virtual void copyBufferData( Buffer * dst, Buffer * src, int dstOffset, int srcOffset, int byteCount ) { POOP_TODO; }
		virtual VertexLayout * createVertexLayout( Shader * shader, Format * fmtList, int fmtCount ) { POOP_TODO; return 0; }

		virtual int fetchElementsWrittenInStreamOutput( int stream ) { POOP_TODO; return 0; }

		virtual ~Renderer() {};

		void drawMeshInstanced( poopie::gfx::Mesh * mesh, int instanceCount ) {
			setActiveVertexBuffer( mesh->m_vertices );
			setActiveIndexBuffer( mesh->m_indices );
			drawIndexedInstanced( mesh->m_indices->getActiveElements(), instanceCount );
		}

		void drawMesh( poopie::gfx::Mesh * mesh ) {
			setActiveVertexBuffer( mesh->m_vertices );
			setActiveIndexBuffer( mesh->m_indices );
			drawIndexed( mesh->m_indices->getActiveElements() );
		}


	protected:

		// Only derived classes can create an instance.
		Renderer() {};

	};

}
}

#endif
