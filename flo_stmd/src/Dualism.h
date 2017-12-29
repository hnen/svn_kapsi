
#ifndef _DUALISM
#define _DUALISM

#include <Windows.h>

#include "Demo.h"
#include "poopie.h"
#include <poopie\ui\win32\Window.h>

using namespace poopie;
using namespace poopie::gfx;

class Dualism
{
public:
	Dualism()
	{
	}

	void Init( )
	{
		Demo demo = Demo::get();
		DemoResources & res = DemoResources::get();
		Renderer* render = demo.getRenderer();

		{
			MeshData * mdata = MeshGen::Cube();
			mdata->subdivide(true);
			mdata->subdivide(true);
			mdata->computeNormals();
			mdata->flipFaces();

			float z = 25.0f;
			mdata->scale( z );
			mesh_ball = mdata->createMesh( render, res.vlayout_basic );
		}
		if ( false )
		{
			MeshData * mdata = MeshGen::Cube();
			mdata->subdivide(false);
			mdata->computeNormals();
			mdata->scale( 0.2f );
			mesh_cube = mdata->createMesh( render, res.vlayout_basic );
		}
		{
			MeshData * mdata = MeshGen::Cube();
			mdata->computeNormals();
			float q = 0.2f;
			mdata->scale( q );
			mesh_point = mdata->createMesh( render, res.vlayout_basic );
		}

		LoadShaders();

		Format fmt[] = {
			Format( "position", FORMAT_TYPE_FLOAT, 4 ),
			Format( "normal", FORMAT_TYPE_FLOAT, 4 )
		};
		vlayout = render->createVertexLayout( shader_map, fmt, sizeof(fmt)/sizeof(Format) );

		
		Format mf( FORMAT_TYPE_UBYTE, 4 );
		for ( int i=0; i<2; i++ )
		{
			int w = render->getBackbufferRenderTarget()->getWidth();
			int h = render->getBackbufferRenderTarget()->getHeight();
			sphereT[i] = render->createRenderTargetTexture( w, h, mf );
			sphere[i] = render->createRenderTarget( sphereT[i] );
		}
	}


	void LoadShaders()
	{
		Demo demo = Demo::get();
		DemoResources & res = DemoResources::get();
		Renderer* render = demo.getRenderer();
		{
			File file( "data/dualism.fxo" );
			shader_scene = render->createShader( "Obj", 0, file.getData(), file.getDataSize() );
		}
		{
			File file( "data/dualism.fxo" );
			shader_map = render->createShader( "Dual", 0, file.getData(), file.getDataSize() );
		}
	}

	void RenderStuff( Matrix4& view, Matrix4& proj, Shader* shader, bool renderCaster=true, bool renderReceiver=true )
	{
		Demo& demo = Demo::get();
		Renderer* render = demo.getRenderer();
		DemoResources& res = DemoResources::get();

		float a = demo.timef();

		Matrix4 w = Matrix4::identity();
		shader->setVariable( "v", view  );
		shader->setVariable( "wv", w * view  );
		shader->setVariable( "wvp", w * view * proj );

		render->setActiveShader( shader );

		if ( renderReceiver )
			render->drawMesh( mesh_ball );
		if ( renderCaster || renderReceiver )
			render->drawMesh( res.mesh_ship );

	}

	void Render( )
	{
		Demo& demo = Demo::get();
		Renderer* render = demo.getRenderer();
		DemoResources& res = DemoResources::get();

		
		if ( poopie::ui::win::Window::keyPressed( VK_RETURN ) )
			LoadShaders();

		float a = demo.timef();
		Vector3 tgt = Vector3( 0,0,-0.5f );
		Vector3 eye = Vector3( sinf(a),0,cosf(a) );
		Matrix4 v = Matrix4::LookAt( tgt, tgt + Vector3(0,0,1), Vector3(0,1,0) );
		Matrix4 p = Matrix4::perspective( 3.14f * 35.0f / 180.0f, 1280.0f / 720.0f, 0.01f, 100.0f );

	Vector3 camView = Vector3(0,0,0);
	Vector3 camPos = eye * 4.0f;
//	Vector3 camPos = Vector3( 0.0f, 0.0f, 15.0f );

		Matrix4 proj = Matrix4::identity();

		// map

		Vector3 wild = Vector3( sinf(a*1.1f), 1.3f*cosf(a*0.7f), sinf(a+cosf(a/1.3f)) ) * 4.5f;
//		Vector3 wild = Vector3( -cosf(a*0.4f), -sinf(a*0.4f), 1 ) * 5.5f;
//		Matrix4 light = Matrix4::LookAt( wild, wild + Vector3(0,0,1), Vector3(0,1,0) );
		Matrix4 light =  Matrix4::translate( wild );
		
//	if ( false )
	{

//		render->setActiveVertexLayout( res.vlayout_basic );
		render->setActiveVertexLayout( vlayout );
		shader_map->setVariable( "light", light  );
		render->setActiveShader( shader_map );


		for ( int i=0; i<2; i++ )
		{
			render->setActiveRenderTarget( sphere[i], res.rt_depth );
			render->clear( sphere[i], Color4(0,0,0,0) );
			render->clear( res.rt_depth );
			shader_map->setVariable( "dir", i*2-1 );
			RenderStuff( light * v, Matrix4::identity(), shader_map, true, true );
//			RenderStuff( light * v, Matrix4::identity(), shader_map, i==1, i==0 );
//		v = Matrix4::LookAt( tgt, tgt - Vector3(0,0,1), Vector3(0,1,0) );
//			v = Matrix4::LookAt( eye, -tgt, Vector3(0,1,0) );
		}
	}

		/**/

//		if ( false )
		if ( poopie::ui::win::Window::isKeyDown( 'C' ) )
		{
			// render maps
			render->setActiveRenderTarget( render->getBackbufferRenderTarget(), res.rt_depth );
			render->clear( render->getBackbufferRenderTarget(), Color4(0.2f,0,0,1) );
			render->clear( res.rt_depth );

			int i = rand()%2;
			Gfx::drawTexFS( sphereT[i] );
			return;
		}


	proj = Matrix4::perspective( 3.14f * 35.0f / 180.0f, render->getBackbufferRenderTarget()->getWidth() / (float)render->getBackbufferRenderTarget()->getHeight(), 0.01f, 100.0f );
	v = Matrix4::LookAt( camPos, camView, Vector3(0,1,0) );// * Matrix4::rotZ( timef() * 0.1f );


	// paske test
/**/	
	if ( false )
	{
			render->setActiveRenderTarget( render->getBackbufferRenderTarget(), res.rt_depth );
			render->clear( render->getBackbufferRenderTarget(), Color4(0.2f,0,0,1) );
			render->clear( res.rt_depth );
		render->setActiveShader( res.shader_alus );
			res.shader_alus->setVariable( "v", v );
			res.shader_alus->setVariable( "wv", v );
			res.shader_alus->setVariable( "wvp", v*proj );
			res.shader_alus->setVariable( "p", proj );
			demo.getRenderer()->drawMesh( res.mesh_ship );
			demo.getRenderer()->drawMesh( this->mesh_ball );
		return;
	}/**/


	render->setActiveRenderTarget( render->getBackbufferRenderTarget(), res.rt_depth );
	render->clear( render->getBackbufferRenderTarget(), Color4(0.2f,0,0,1) );
	render->clear( res.rt_depth );

	render->setActiveVertexLayout( res.vlayout_basic );

/*	render->setActiveShader( res.shader_alus );
	res.shader_alus->setVariable( "v", v );
	res.shader_alus->setVariable( "wv", v );
	res.shader_alus->setVariable( "wvp", v*proj );
		Gfx::drawTex( sphereT[rand()%2], 0, 0, Gfx::Align::ALIGN_IMG_HCVC | Gfx::Align::ALIGN_SCR_HCVC, 1.0f );
		*/

	Shader* s = shader_scene;
//	s = res.shader_alus;
	render->setActiveShader( s);

	s->setVariable( "p", proj );
	s->setVariable( "light", light  );
	s->setVariable( "lightPos", wild.xyz1()  );

	s->setVariable( "tex_front", sphereT[0] );
	s->setVariable( "tex_back", sphereT[1] );

	s->setVariable( "tex_test", res.tex_envmap );
//			RenderStuff( v, proj, s );
	RenderStuff( v, proj, s, false,true);



	}

	VertexLayout* vlayout;

	poopie::gfx::Mesh* mesh_ball;
	poopie::gfx::Mesh* mesh_cube;
	poopie::gfx::Mesh* mesh_point;
	poopie::gfx::Shader* shader_map;
	poopie::gfx::Shader* shader_scene;


	Texture* sphereT[2];	
	RenderTarget* sphere[2];	

};


#endif
