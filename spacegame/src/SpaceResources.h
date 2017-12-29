
#ifndef __SPACE_RESOURCES_H
#define __SPACE_RESOURCES_H

#include "poopie_base.h"
#include "poopie_gfx.h"
#include "poopie_sfx.h"

namespace space {

class SpaceResources {
public:
	
	SpaceResources( Renderer * renderer ) {
		
		// BASIC SHADER
		{
			POOP_LOG( "Loading shader..." );

			File file_basic( "data/shader/test.fxo" );
			shader_basic = renderer->createShader( "Poop", 0, file_basic.getData(), file_basic.getDataSize() );

			Format fmt[] = {
				Format( "position", FORMAT_TYPE_FLOAT, 4 ),
				Format( "normal", FORMAT_TYPE_FLOAT, 4 )
			};

			vlayout_basic = renderer->createVertexLayout( shader_basic, fmt, sizeof(fmt)/sizeof(Format) );

		}

		// ALUS
		{
			POOP_LOG( "Loading alusmodel" );

			File file_alus( "data/alus.f" );
			mdata_alus = MeshImporter::Import( file_alus.getData(), file_alus.getDataSize() );
			mdata_alus->flipFaces();
			mdata_alus->computeNormals();
			mesh_alus = mdata_alus->createMesh( renderer, vlayout_basic );
		}

		// AMMUS
		{
			POOP_LOG( "Loading bullet" );
			File file_bullet( "data/ammus.f" );
			mdata_bullet = MeshImporter::Import( file_bullet.getData(), file_bullet.getDataSize() );
			mdata_bullet->flipFaces();
			//mdata_bullet = MeshGen::Cube();
			//mdata_bullet->subdivide( true );
			//mdata_bullet->subdivide( true );
			mdata_bullet->computeNormals();
			mesh_bullet = mdata_bullet->createMesh( renderer, vlayout_basic );
		}

		// CUBE
		{
			POOP_LOG( "Generating cube" );
			mdata_cube = MeshGen::Cube();
			//mdata_cube->subdivide(true);
			//mdata_cube->subdivide(true);
			mdata_cube->computeNormals();
			mesh_cube = mdata_cube->createMesh( renderer, vlayout_basic );
		}

		// MUSIC
		{
			POOP_LOG( "Loading music" );
			//File file_music( "data/Infader - Misfit(320_final).mp3" );
			//music_main = Music::create( file_music.getData(), file_music.getDataSize() );
		}
	}

	~SpaceResources() {

		delete shader_basic;
		delete vlayout_basic;

		delete mdata_alus;
		delete mdata_cube;
		delete mesh_alus;
		delete mesh_cube;

	}

	VertexLayout * vlayout_basic;
	Shader * shader_basic;

	MeshData * mdata_alus;
	Mesh * mesh_alus;

	MeshData * mdata_cube;
	Mesh * mesh_cube;

	MeshData * mdata_bullet;
	Mesh * mesh_bullet;
	
	Music * music_main;

};


}


#endif