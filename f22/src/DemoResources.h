
#ifndef __DEMO_RESOURCES
#define __DEMO_RESOURCES

#include "poopie.h"

class DemoResources {
public:

	static DemoResources & get() {
		static DemoResources instance;
		return instance;
	}

	void load( Renderer * renderer ) {
		// MUSIC
		{
			File fileMusic( "data/myyr.mp3" );
			music = Music::create( fileMusic.getData(), fileMusic.getDataSize() );
		}

		// DEPTH BUFFER
		rt_depth = renderer->createDepthStencilRenderTarget( 1280, 720 );

		// SHADER: basic
		{

			File file( "data/basic.fxo" );
			shader_basic = renderer->createShader( "Poop", 0, file.getData(), file.getDataSize() );

			Format fmt[] = {
				Format( "position", FORMAT_TYPE_FLOAT, 4 ),
				Format( "normal", FORMAT_TYPE_FLOAT, 4 )
			};

			vlayout_basic = renderer->createVertexLayout( shader_basic, fmt, sizeof(fmt)/sizeof(Format) );
		}

		// SHADER: overlay
		{
			File file( "data/overlay.fxo" );
			shader_overlay = renderer->createShader( "Transparent", 0, file.getData(), file.getDataSize() );
		}
		// SHADER: overlay
		{
			File file( "data/overlay2.fxo" );
			shader_overlay2 = renderer->createShader( "Pattern", 0, file.getData(), file.getDataSize() );
		}

		// SHADER: pattern
		{
			File file( "data/pattern.fxo" );
			shader_pattern = renderer->createShader( "Pattern", 0, file.getData(), file.getDataSize() );
		}

		// SHADER: juusto
		{
			File file( "data/homonaaama.fxo" );
			shader_juusto = renderer->createShader( "Pattern", 0, file.getData(), file.getDataSize() );
		}

		// SHADER: pattern
		{
			File file( "data/pattern2.fxo" );
			shader_pattern2 = renderer->createShader( "Pattern", 0, file.getData(), file.getDataSize() );
		}

		// MESH: hands
		{
			File file0( "data/kaesi0.f" );
			File file1( "data/kaesi1.f" );
			File file2( "data/kaesi2.f" );

			mdata_hand0 = MeshImporter::Import( file0.getData(), file0.getDataSize() );
			mdata_hand1 = MeshImporter::Import( file1.getData(), file1.getDataSize() );
			mdata_hand2 = MeshImporter::Import( file2.getData(), file2.getDataSize() );

			mdata_hand0->subdivide(true);
			//mdata_hand1->subdivide(true);
			mdata_hand2->subdivide(true);
			mdata_hand0->computeNormals();
			mdata_hand1->computeNormals();
			mdata_hand2->computeNormals();

			mesh_hand0 = mdata_hand0->createMesh( renderer, vlayout_basic );
			mesh_hand1 = mdata_hand1->createMesh( renderer, vlayout_basic );
			mesh_hand2 = mdata_hand2->createMesh( renderer, vlayout_basic );
		}

		{
			File file( "data/nikama.f" );
			MeshData * mdata = MeshImporter::Import( file.getData(), file.getDataSize() );
			mdata->flipFaces();
			mdata->subdivide(true);
			mdata->subdivide(true);
			mdata->computeNormals();
			mesh_nikama = mdata->createMesh( renderer, vlayout_basic );
		}
		{
			File file( "data/kylki.f" );
			MeshData * mdata = MeshImporter::Import( file.getData(), file.getDataSize() );
			mdata->flipFaces();
			mdata->subdivide(true);
			mdata->subdivide(true);
			mdata->computeNormals();
			mesh_kylki = mdata->createMesh( renderer, vlayout_basic );
		}
		{
			File file( "data/mach.f" );
			MeshData * mdata = MeshImporter::Import( file.getData(), file.getDataSize() );
			mdata->flipFaces();
//			mdata->subdivide(true);
//			mdata->subdivide(true);
			mdata->computeNormals();
			mesh_machete = mdata->createMesh( renderer, vlayout_basic );
		}

		// MESH: PALLO
		{
			MeshData * mdata = MeshGen::Cube();
			mdata->subdivide(true);
			mdata->subdivide(true);
			mdata->computeNormals();
			mesh_pallo = mdata->createMesh( renderer, vlayout_basic );
		}

		// MESHDATA: ICOSADERY
		{
			File file( "data/icos.f" );
			mdata_icos = MeshImporter::Import( file.getData(), file.getDataSize() );			
		}
		// MESHDATA: QLLO
		{
			File file( "data/qllo.f" );
			mdata_qllo = MeshImporter::Import( file.getData(), file.getDataSize() );			
		}
		// MESHDATA: F
		{
			File file( "data/f.f" );
			mdata_f = MeshImporter::Import( file.getData(), file.getDataSize() );			
		}

		// TEXTURE: paletti
		{
			File file( "data/baletti.png" );
			ImageData * img = ImageImporter::importPng( file.getData(), file.getDataSize() );
			img->generateMips();
			tex_baletti = renderer->createTexture( img );
			delete img;
		}

		// TEXTURE: noise
		{
			ImageData * img = ImageImporter::genNoise(512,512);
			img->generateMips();
			tex_noise = renderer->createTexture( img );
			delete img;
		}

		// TEXTURE: f22
		{
			File file( "data/f22.png" );
			ImageData * img = ImageImporter::importPng( file.getData(), file.getDataSize() );
			img->generateMips();
			tex_f22 = renderer->createTexture( img );
			delete img;
		}
		// TEXTURE: silma
		{
			File file( "data/silma.png" );
			ImageData * img = ImageImporter::importPng( file.getData(), file.getDataSize() );
			img->generateMips();
			tex_silma = renderer->createTexture( img );
			delete img;
		}
		// TEXTURE: silma
		{
			File file( "data/qlmio.png" );
			ImageData * img = ImageImporter::importPng( file.getData(), file.getDataSize() );
			img->generateMips();
			tex_qlmio = renderer->createTexture( img );
			delete img;
		}
		// TEXTURE: taide
		{
			File file( "data/taide0.png" );
			ImageData * img = ImageImporter::importPng( file.getData(), file.getDataSize() );
			img->generateMips();
			tex_overlay[0] = renderer->createTexture( img );

			file = File( "data/taide1.png" );
			img = ImageImporter::importPng( file.getData(), file.getDataSize() );
			img->generateMips();
			tex_overlay[1] = renderer->createTexture( img );

			file = File( "data/taide2.png" );
			img = ImageImporter::importPng( file.getData(), file.getDataSize() );
			img->generateMips();
			tex_overlay[2] = renderer->createTexture( img );

			file = File( "data/taide3.png" );
			img = ImageImporter::importPng( file.getData(), file.getDataSize() );
			img->generateMips();
			tex_overlay[3] = renderer->createTexture( img );

			file = File( "data/taide4.png" );
			img = ImageImporter::importPng( file.getData(), file.getDataSize() );
			img->generateMips();
			tex_overlay[4] = renderer->createTexture( img );
			//delete img;
		}


		int iw = renderer->getBackbufferRenderTarget()->getWidth();
		int ih = renderer->getBackbufferRenderTarget()->getHeight();
		
		rt_tex = renderer->createRenderTargetTexture( iw, ih, Format(  FORMAT_TYPE::FORMAT_TYPE_UBYTE, 4 ) );
		rt = renderer->createRenderTarget( rt_tex );



	}

	Music * music;
	DepthStencil * rt_depth;

	RenderTarget * rt;
	Texture * rt_tex;

	MeshData * mdata_hand0;
	MeshData * mdata_hand1;
	MeshData * mdata_hand2;
	Mesh * mesh_hand0;
	Mesh * mesh_hand1;
	Mesh * mesh_hand2;

	MeshData * mdata_qllo;
	MeshData * mdata_f;

	MeshData * mdata_icos;

	Mesh * mesh_pallo;

	Mesh * mesh_nikama;
	Mesh * mesh_kylki;

	Mesh * mesh_machete;

	VertexLayout * vlayout_basic;
	Shader * shader_basic;
	Shader * shader_overlay;
	Shader * shader_overlay2;
	Shader * shader_pattern;
	Shader * shader_pattern2;
	Shader * shader_juusto;
	Shader * shader_sprite;

	Texture * tex_baletti;
	Texture * tex_noise;
	Texture * tex_f22;
	Texture * tex_silma;
	Texture * tex_qlmio;
	Texture * tex_overlay[5];
	//Texture * tex_0;
	//Texture * tex_1;
	//Texture * tex_2;
	//Texture * tex_3;
	//Texture * tex_4;

private:

};


#endif



