
#include <Windows.h>
#include "poopie/base/File.h"
#include "poopie/math/Vector.h"
#include "poopie/math/Matrix.h"
#include "poopie/math/Frustum.h"
#include "poopie/base/DynamicArray.h"
#include "poopie/gfx/MeshData.h"
#include "poopie/gfx/MeshImporter.h"
#include "poopie/gfx/Mesh.h"
#include "poopie/math/Spline.h"
#include "poopie/gfx/Format.h"
#include "poopie/gfx/VertexLayout.h"
#include "poopie/gfx/Shader.h"
#include "poopie/gfx/Material.h"
#include "poopie/gfx/ImageImporter.h"
#include "poopie/gfx/ImageData.h"
#include "poopie/gfx/LodMesh.h"

#include "poopie/math/Matrix.h"
#include "poopie/base/HashMap.h"

#include "Demo.h"
#include "MeshGen.h"

LodMesh * m_lmesh_meteo;
Buffer * g_cb;

Spline * m_camspline;

struct CBuffer {
	Matrix4 w[1024];
};

DynamicArray<Matrix4> meteos;


void Demo::init() {
	{
		File fileMusic( "data/biisi.mp3" );
		m_song = Music::create( fileMusic.getData(), fileMusic.getDataSize() );
	}

	{
		File fileShader( "data/shader/test.fxo" );
		m_shader_vertexLit = m_renderer->createShader( "Poop", 0, fileShader.getData(), fileShader.getDataSize() );
	}

	Format fmts [] = {
		Format( "position", FORMAT_TYPE_FLOAT, 4 ),
		Format( "normal", FORMAT_TYPE_FLOAT, 4 ),
		Format( "tangent", FORMAT_TYPE_FLOAT, 4 ),
		Format( "texcoord", FORMAT_TYPE_FLOAT, 2 ),
		Format( "lodpos", FORMAT_TYPE_FLOAT, 3 ),
		Format( "lodnormal", FORMAT_TYPE_FLOAT, 3 ),
	};

	m_renderer->createVertexLayout( m_shader_vertexLit, fmts, 6 );

	{
		File fileModel( "data/meteo2.f" );
		MeshData * mdata[8];
		
		mdata[0] = MeshImporter::Import( fileModel.getData(), fileModel.getDataSize() );
		
		for ( int i = 1; i < 8; i++ ) {
			mdata[i] = new MeshData( *mdata[i-1] );

			printf( "Subdividing LOD #%d...\n", i+1 );
			mdata[i]->subdivide(true);
		}

		{
			ImageData * data = ImageImporter::importPng( String("refe/cloudnoise.png") );
			for ( int i = 0; i < 8; i++ ) {
				printf( "Processing LOD #%d...\n", i+1 );
				mdata[i]->computeNormals();
				mdata[i]->displace( data, 0.5f, 0.2f );
				mdata[i]->computeFaceTangents();
			}
			delete data;
		}


		for ( int i = 1; i < 8; i++ ) {
			printf( "Processing LOD #%d...\n", i+1 );
			mdata[i]->computeLodPositions(mdata[i-1]);
		}


		for ( int i = 1; i < 8; i++ ) {
			printf( "Processing LOD #%d...\n", i+1 );
			mdata[i]->processForRender( m_shader_vertexLit->getVertexLayout() );
		}
		//m_mesh_meteo = mdata[->createMesh( m_renderer, m_shader_vertexLit->getVertexLayout() );

		Mesh * meshes[] = { 
			mdata[7]->createMesh( m_renderer, m_shader_vertexLit->getVertexLayout() ),
			mdata[6]->createMesh( m_renderer, m_shader_vertexLit->getVertexLayout() ),
			mdata[5]->createMesh( m_renderer, m_shader_vertexLit->getVertexLayout() ),
			mdata[4]->createMesh( m_renderer, m_shader_vertexLit->getVertexLayout() ),
			mdata[3]->createMesh( m_renderer, m_shader_vertexLit->getVertexLayout() ),
			mdata[2]->createMesh( m_renderer, m_shader_vertexLit->getVertexLayout() ),
			mdata[1]->createMesh( m_renderer, m_shader_vertexLit->getVertexLayout() ),
			mdata[0]->createMesh( m_renderer, m_shader_vertexLit->getVertexLayout() ),
		};

		m_lmesh_meteo = new LodMesh( Array<Mesh*>(meshes,8), 4.0f );

		m_mat_meteo = mdata[0]->materials[0].createMaterial( m_renderer, m_shader_vertexLit );

		g_cb = m_renderer->createConstantBufferDynamic( sizeof( CBuffer ) );

	}

	m_rt_depth = m_renderer->createDepthStencilRenderTarget( 1280, 720 );


	for ( int i = 0; i < 150000; i ++ ) {
		Vector3 pos = Vector3( Hash::rnd( i*6+0 ), Hash::rnd( i*6+1 ), Hash::rnd( i*6+2 ) ) * 125.0f;
		Matrix4 rot = Matrix4::rotateAxis( Vector3( Hash::rnd( i*6+3 ), Hash::rnd( i*6+4 ), Hash::rnd( i*6+5 ) ).normalized(), Hash::rnd( i*231321, 0, 6.28f ) );
		Matrix4 w = rot * Matrix4::translate( pos );
		meteos.append(w);
	}

	Vector3 * ps = new Vector3[20];
	for ( int j = 0; j < 20; j++ ) {
		int seed = j + 3422;
		ps[j] = meteos[Hash::superFastHash(&seed,4)%meteos.getLength()].pos + Vector3( Hash::rnd(j*10), Hash::rnd(j*100), Hash::rnd(j*1000) ) * 2.0f;
	}
	m_camspline = new Spline( ps, 20 );


	m_song->play();

}



DynamicArray<Matrix4> * ls = new DynamicArray<Matrix4>[7];
void Demo::render() {

	float t = timef();

	Matrix4 p = Matrix4::perspective( 3.14f * 35.0f / 180.0f, 1280.0f / 720.0f, 0.01f, 1000.0f );
	//Matrix4 v = Matrix4::LookAt( Vector3( 0,0, 1 ) * 8.5f, Vector3(0,0,0), Vector3(0,1,0) );
	m_mat_meteo->applyShaderConstants();

	Vector3 campos = m_camspline->getPoint( timef() * 0.00125f );
	Vector3 camview = m_camspline->getPoint( timef() * 0.00125f + 0.01f ) * 0.9f;

	//Vector3 campos = (meteos[100].mulVec(Vector3(0,0,0))) + Vector3( sinf( 0.25f ), 0, cosf( 0.25f ) ) * 5.0f;
	//Vector3 camview = (meteos[100].mulVec(Vector3(0,0,0)));

	//Matrix4 v = Matrix4::LookAt( Vector3( sinf(timef() * 0.025), -0.7f, cosf(timef() * 0.025) ) * (160.0f-t*4.0f), Vector3(0,0,0), Vector3(0,1,0) );
	Matrix4 v = Matrix4::LookAt( campos, camview, Vector3(0,1,0) );// * Matrix4::rotZ( timef() * 0.1f );

	m_renderer->setActiveRenderTarget( m_renderer->getBackbufferRenderTarget(), m_rt_depth );
	m_renderer->clear( m_renderer->getBackbufferRenderTarget(), Color4(0,0,0,0) );
	m_renderer->clear( m_rt_depth );

	static int fr = -1;
	fr++;
	fr %= 2;

	int lcount[8];
	if ( !fr ) {

		Frustum f = Frustum::fromMatrix( v*p );

		for ( int i = 0; i < 8; i ++ ) {
			ls[i].clear();
		}
		for ( int i = 0; i < meteos.getLength(); i ++ ) {
			Matrix4 & w = meteos[i];
			Vector3 pos = w.pos;
			Sphere & s = m_lmesh_meteo->meshes[0]->boundingSphere;
			if ( f.intersects( Sphere( w.preMulVec( s.center ), s.radius ) ) ) {
				Matrix4 wv = w*v;
				float z = -wv.pos.z;
				ls[m_lmesh_meteo->selectMeshI( z )].append( w.transpose() );
			}
		}
	}

	m_shader_vertexLit->setVariable( "vp", v*p );
	m_shader_vertexLit->setVariable( "v", v );
	m_shader_vertexLit->setConstants( "InstanceData", g_cb );
	for ( int i = 0; i < 8; i ++ ) {
	//for ( int i = 0; i < 4; i ++ ) {
		if ( i == 7 ) {
			m_shader_vertexLit->setVariable( "lodZ", m_lmesh_meteo->distances[i-1] * 2.0f );
		} else {
			m_shader_vertexLit->setVariable( "lodZ", m_lmesh_meteo->distances[i] );
		}
		m_shader_vertexLit->setVariable( "boundingSphere", m_lmesh_meteo->meshes[i]->boundingSphere.radius );
		
		int numMeshes = ls[i].getLength();
		for ( int j = 0; j < numMeshes; j += 1024 ) {
			//int numElements = MIN(1024, lcount[i]-j );
			int numElements = MIN(1024, ls[i].getLength()-j );
			CBuffer * cb = (CBuffer*)g_cb->lock();
			memcpy( cb->w, &ls[i][j], sizeof(Matrix4) * MIN(1024, ls[i].getLength()-j ) );
			g_cb->unlock();
			//m_renderer->copyBufferData( g_cb, g_lod_cb[i], 0, 64*j, 64*numElements );
			m_renderer->setActiveShader( m_shader_vertexLit );
			m_renderer->drawMeshInstanced( m_lmesh_meteo->meshes[i], numElements );
		}
	}


}


float Demo::timef() {
	return (float)time() / 1000.0f;
}

unsigned int Demo::time() {
#ifndef FMOD_DISABLE
	return m_song->getTime();
#else
	return timeGetTime();
#endif
}

void Demo::setTime( unsigned int time ) {
#ifndef FMOD_DISABLE
	m_song->setTime( time );
#else
#endif
}
