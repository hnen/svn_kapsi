
#ifndef __DEMO_RESOURCES
#define __DEMO_RESOURCES

#include "poopie.h"

class DemoResources {
public:

	static DemoResources & get() {
		static DemoResources instance;
		return instance;
	}


	static int musicSyncTime[37];


	void load( Renderer * renderer ) {
		// MUSIC
		{
			File fileMusic( "data/saxa.mp3" );
			music = Music::create( fileMusic.getData(), fileMusic.getDataSize() );
		}

		// DEPTH BUFFER
		rt_depth = renderer->createDepthStencilRenderTarget( 1280, 720 );

		// SHADER: basic
		{
			File file( "data/basic.fxo" );
			shader_alus = renderer->createShader( "Alus", 0, file.getData(), file.getDataSize() );
			shader_sun = renderer->createShader( "Sun", 0, file.getData(), file.getDataSize() );
			shader_blockglow = renderer->createShader( "Subtract", 0, file.getData(), file.getDataSize() );

			Format fmt[] = {
				Format( "position", FORMAT_TYPE_FLOAT, 4 ),
				Format( "normal", FORMAT_TYPE_FLOAT, 4 )
			};

			vlayout_basic = renderer->createVertexLayout( shader_alus, fmt, sizeof(fmt)/sizeof(Format) );
			//vlayout_basic = renderer->createVertexLayout( shader_sun, fmt, sizeof(fmt)/sizeof(Format) );
		}

		// SHADERS
		{
			File file( "data/overlay.fxo" );
			shader_overlay = renderer->createShader( "Transparent", 0, file.getData(), file.getDataSize() );
			shader_overlay_add = renderer->createShader( "Additive", 0, file.getData(), file.getDataSize() );

			file = File( "data/pp_blur.fxo" );
			shader_blur[0] = renderer->createShader( "Normal_Horiz", 0, file.getData(), file.getDataSize() );
			shader_blur[1] = renderer->createShader( "Normal_Vert", 0, file.getData(), file.getDataSize() );
			shader_blur_expand[0] = renderer->createShader( "Expand_Horiz", 0, file.getData(), file.getDataSize() );
			shader_blur_expand[1] = renderer->createShader( "Expand_Vert", 0, file.getData(), file.getDataSize() );
			shader_blur_expandMore[0] = renderer->createShader( "ExpandMore_Horiz", 0, file.getData(), file.getDataSize() );
			shader_blur_expandMore[1] = renderer->createShader( "ExpandMore_Vert", 0, file.getData(), file.getDataSize() );
			shader_blur_shrink[0] = renderer->createShader( "Shrink_Horiz", 0, file.getData(), file.getDataSize() );
			shader_blur_shrink[1] = renderer->createShader( "Shrink_Vert", 0, file.getData(), file.getDataSize() );

			file = File( "data/floaters_prepass.fxo" );
			shader_floaters_field = renderer->createShader( "Field", 0, file.getData(), file.getDataSize() );
			shader_floaters_vectors = renderer->createShader( "Vectors", 0, file.getData(), file.getDataSize() );
			shader_floaters_intensity = renderer->createShader( "Intensity", 0, file.getData(), file.getDataSize() );

			file = File( "data/floaters.fxo" );
			shader_floaters = renderer->createShader( "Floaters", 0, file.getData(), file.getDataSize() );
		}

		{
			File file( "data/eilien.f" );
			MeshData * mdata = MeshImporter::Import( file.getData(), file.getDataSize() );
			mdata->flipFaces();
			mdata->subdivide(true);
			mdata->subdivide(true);
			mdata->computeNormals();
			mesh_ship = mdata->createMesh( renderer, vlayout_basic );
		}

		// MESH: PALLO
		{
			MeshData * mdata = MeshGen::Cube();
			mdata->subdivide(true);
			mdata->subdivide(true);
			mdata->subdivide(true);
			mdata->subdivide(true);
			mdata->computeNormals();
			mesh_pallo = mdata->createMesh( renderer, vlayout_basic );
		}

		{
			File file( "data/envmap.png");
			//File file( "data/kampela3.png");
			
			ImageData * img = ImageImporter::importPng( file.getData(), file.getDataSize() );
			img->generateMips();
			tex_envmap = renderer->createTexture( img );
			delete img;
		}
		// TEXTURE: perlin
		{
			ImageData * img = ImageData::genPerlin( 1280, 800, 160, 2313 );
			img->generateMips();
			tex_perlin = renderer->createTexture( img );
			delete img;
		}
		// TEXTURE: noise
		{
			ImageData * img = ImageData::genNoise32( 1024, 1024 );
			tex_noise = renderer->createTexture( img );
			delete img;
		}

		// TEXTURE: blob
		{
			File file( "data/blob.png" );
			ImageData * img = ImageImporter::importPng( file.getData(), file.getDataSize() );
			img->generateMips();
			tex_blob = renderer->createTexture( img );
			delete img;
		}

		// RENDERTARGETS & RT TEXTURES
		{
			int iw = renderer->getBackbufferRenderTarget()->getWidth();
			int ih = renderer->getBackbufferRenderTarget()->getHeight();
		

			tex_rt_scene = renderer->createRenderTargetTexture( iw, ih, Format(  FORMAT_TYPE::FORMAT_TYPE_UBYTE, 4 ) );
			tex_rt_scene->setDebugName( "tex_rt_scene" );
			rt_scene = renderer->createRenderTarget( tex_rt_scene );
			tex_rt_mask = renderer->createRenderTargetTexture( iw, ih, Format(  FORMAT_TYPE::FORMAT_TYPE_FLOAT, 1 ) );
			tex_rt_mask->setDebugName( "tex_rt_mask" );
			rt_mask = renderer->createRenderTarget( tex_rt_mask );

			tex_rt_scene_glowGeom = renderer->createRenderTargetTexture( iw, ih, Format(  FORMAT_TYPE::FORMAT_TYPE_UBYTE, 4 ) );
			tex_rt_scene_glowGeom->setDebugName( "tex_rt_scene_glowGeom" );
			rt_scene_glowGeom = renderer->createRenderTarget( tex_rt_scene_glowGeom );

			tex_rt_scene_glowGeom_tmp = renderer->createRenderTargetTexture( iw/4, ih/4, Format(  FORMAT_TYPE::FORMAT_TYPE_UBYTE, 4 ) );
			tex_rt_scene_glowGeom_tmp->setDebugName( "tex_rt_scene_glowGeom_tmp" );
			rt_scene_glowGeom_tmp = renderer->createRenderTarget( tex_rt_scene_glowGeom_tmp );

			tex_rt_scene_glowGeom_blurred = renderer->createRenderTargetTexture( iw/4, ih/4, Format(  FORMAT_TYPE::FORMAT_TYPE_UBYTE, 4 ) );
			tex_rt_scene_glowGeom_blurred->setDebugName( "tex_rt_scene_glowGeom" );
			rt_scene_glowGeom_blurred = renderer->createRenderTarget( tex_rt_scene_glowGeom_blurred );

			tex_rt_mask_tmp = renderer->createRenderTargetTexture( iw/4, ih/4, Format(  FORMAT_TYPE::FORMAT_TYPE_FLOAT, 1 ) );
			tex_rt_mask_tmp->setDebugName( "tex_rt_mask_tmp" );
			rt_mask_tmp = renderer->createRenderTarget( tex_rt_mask_tmp );

			tex_rt_mask_fieldMul = renderer->createRenderTargetTexture( iw/4, ih/4, Format(  FORMAT_TYPE::FORMAT_TYPE_FLOAT, 1 ) );
			tex_rt_mask_fieldMul->setDebugName( "tex_rt_mask_fieldMul" );
			rt_mask_fieldMul = renderer->createRenderTarget( tex_rt_mask_fieldMul );
			tex_rt_mask_intensity = renderer->createRenderTargetTexture( iw/4, ih/4, Format(  FORMAT_TYPE::FORMAT_TYPE_FLOAT, 1 ) );
			tex_rt_mask_intensity->setDebugName( "tex_rt_mask_intensity" );
			rt_mask_intensity = renderer->createRenderTarget( tex_rt_mask_intensity );
			tex_rt_mask_intensityMul = renderer->createRenderTargetTexture( iw/4, ih/4, Format(  FORMAT_TYPE::FORMAT_TYPE_FLOAT, 1 ) );
			tex_rt_mask_intensityMul->setDebugName( "tex_rt_mask_intensityMul" );
			rt_mask_intensityMul = renderer->createRenderTarget( tex_rt_mask_intensityMul );

			tex_rt_floaters_field = renderer->createRenderTargetTexture( iw/2, ih/2, Format(  FORMAT_TYPE::FORMAT_TYPE_FLOAT, 1 ) );
			tex_rt_floaters_field->setDebugName( "tex_rt_floaters_field" );
			rt_floaters_field = renderer->createRenderTarget( tex_rt_floaters_field );
			tex_rt_floaters_intensity = renderer->createRenderTargetTexture( iw/4, ih/4, Format(  FORMAT_TYPE::FORMAT_TYPE_FLOAT, 1 ) );
			tex_rt_floaters_intensity->setDebugName( "tex_rt_floaters_intensity" );
			rt_floaters_intensity = renderer->createRenderTarget( tex_rt_floaters_intensity );

			tex_rt_floaters_vectors = renderer->createRenderTargetTexture( iw/2, ih/2, Format(  FORMAT_TYPE::FORMAT_TYPE_FLOAT, 2 ) );
			tex_rt_floaters_vectors->setDebugName( "tex_rt_floaters_field" );
			rt_floaters_vectors = renderer->createRenderTarget( tex_rt_floaters_vectors );
		}

	}

	Music * music;
	DepthStencil * rt_depth;

	RenderTarget * rt_scene;
	Texture * tex_rt_scene;

	RenderTarget * rt_scene_glowGeom;
	Texture * tex_rt_scene_glowGeom;
	RenderTarget * rt_scene_glowGeom_tmp;
	Texture * tex_rt_scene_glowGeom_tmp;
	RenderTarget * rt_scene_glowGeom_blurred;
	Texture * tex_rt_scene_glowGeom_blurred;

	RenderTarget * rt_mask;
	Texture * tex_rt_mask;
	RenderTarget * rt_mask_tmp;
	Texture * tex_rt_mask_tmp;
	RenderTarget * rt_mask_fieldMul;
	Texture * tex_rt_mask_fieldMul;
	RenderTarget * rt_mask_intensity;
	Texture * tex_rt_mask_intensity;
	RenderTarget * rt_mask_intensityMul;
	Texture * tex_rt_mask_intensityMul;

	RenderTarget * rt_floaters_field;
	Texture * tex_rt_floaters_field;
	RenderTarget * rt_floaters_vectors;
	Texture * tex_rt_floaters_vectors;
	RenderTarget * rt_floaters_intensity;
	Texture * tex_rt_floaters_intensity;

	Texture * tex_envmap;
	Texture * tex_blob;
	Texture * tex_perlin;
	Texture * tex_noise;

	Mesh * mesh_pallo;
	Mesh * mesh_ship;

	VertexLayout * vlayout_basic;

	Shader * shader_alus;
	Shader * shader_sun;
	Shader * shader_blockglow;

	Shader * shader_overlay;
	Shader * shader_overlay_add;
	Shader * shader_floaters;

	Shader * shader_blur[2];
	Shader * shader_blur_expand[2];
	Shader * shader_blur_expandMore[2];
	Shader * shader_blur_shrink[2];

	Shader * shader_floaters_field;
	Shader * shader_floaters_vectors;
	Shader * shader_floaters_intensity;

private:

};


#endif



