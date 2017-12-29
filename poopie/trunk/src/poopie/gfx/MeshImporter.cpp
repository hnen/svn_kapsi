
#include "MeshImporter.h"

#include "poopie/gfx/MeshData.h"
#include "poopie/gfx/Material.h"
#include "poopie/base/ByteStream.h"
#include "poopie/base/String.h"

#include "poopie/defs.h"

#define SCMP(header, str) header[0] == str[0] && header[1] == str[1] && header[2] == str[2] && header[3] == str[3]

namespace poopie {
namespace gfx {

	MeshData * MeshImporter::Import( void * data, int dataSize ) {
		POOP_INTERNAL_LOG( String("Importing mesh of size ") + dataSize );

		ByteStream bs( data, dataSize );

		int numVertices = -1;
		int numFaces = -1;
		MeshData * ret = 0;

		while( !bs.endOfStream() ) {
			char * header = bs.readChars( 4 );
			int chunkSize = bs.read<int>();

			unsigned int startPos = bs.getPosition();

			if( SCMP(header, "MESH" ) ) {
				numVertices = bs.read<int>();
				numFaces = bs.read<int>();
				ret = new MeshData();
				//ret->indices.resize( numFaces * 3 );
				ret->vertices.resize( numVertices );
				ret->faces.resize( numFaces );
			} else if( SCMP(header, "POSI" ) ) {
				int kolmonen = bs.read<int>();
				POOP_ASSERT( kolmonen == 3 );
				for( int i = 0; i < numVertices; i++ ) {
					ret->vertices[i].position.x =  bs.read<float>();
					ret->vertices[i].position.z = -bs.read<float>();
					ret->vertices[i].position.y =  bs.read<float>();
					//printf("v: %i = %f,  %f,  %f \n", i, ret->vertices[i].position.x, ret->vertices[i].position.y, ret->vertices[i].position.z );
				}
			} else if( SCMP(header, "NORM" ) ) {
				int kolmonen = bs.read<int>();
				POOP_ASSERT( kolmonen == 3 );
				for( int i = 0; i < numVertices; i++ ) {
					ret->vertices[i].normal.x =  bs.read<float>();
					ret->vertices[i].normal.z = -bs.read<float>();
					ret->vertices[i].normal.y =  bs.read<float>();
				}
			} else if( SCMP(header, "INDS" ) ) {
				int nelonen = bs.read<int>();
				POOP_ASSERT( nelonen == 4 );
				for( int i = 0; i < numFaces; i ++ ) {
					int i0 = bs.read<int>();
					int i1 = bs.read<int>();
					int i2 = bs.read<int>();
					int i3 = bs.read<int>();

					ret->faces[i].v[0].i = i3;
					ret->faces[i].v[1].i = i2;
					ret->faces[i].v[2].i = i1;
					ret->faces[i].v[3].i = i0;

					int minuses = (ret->faces[i].v[0].i == -1 ? 1 : 0) + (ret->faces[i].v[1].i == -1 ? 1 : 0) + (ret->faces[i].v[2].i == -1 ? 1 : 0) + (ret->faces[i].v[3].i == -1 ? 1 : 0);
					POOP_ASSERT( minuses <= 1 );

					if ( ret->faces[i].v[0].i == -1 ) {
						ret->faces[i].v[0] = ret->faces[i].v[1];
						ret->faces[i].v[1] = ret->faces[i].v[2];
						ret->faces[i].v[2] = ret->faces[i].v[3];
						ret->faces[i].v[3].i = -1;
					} else
					if ( ret->faces[i].v[1].i == -1 ) {
						ret->faces[i].v[1] = ret->faces[i].v[2];
						ret->faces[i].v[2] = ret->faces[i].v[3];
						ret->faces[i].v[3].i = -1;
					} else
					if ( ret->faces[i].v[2].i == -1 ) {
						ret->faces[i].v[2] = ret->faces[i].v[3];
						ret->faces[i].v[3].i = -1;
					}

				}

			} else if( SCMP(header, "FUV0" ) ) {
				// Face UV 0
				int kakkonen = bs.read<int>();
				POOP_ASSERT( kakkonen == 2 );
				for( int i = 0; i < numFaces; i ++ ) {
					Vector2 uv0 = bs.read<Vector2>();
					Vector2 uv1 = bs.read<Vector2>();
					Vector2 uv2 = bs.read<Vector2>();
					Vector2 uv3 = bs.read<Vector2>();

					uv0.y = 1.0f - uv0.y;
					uv1.y = 1.0f - uv1.y;
					uv2.y = 1.0f - uv2.y;
					uv3.y = 1.0f - uv3.y;

					ret->faces[i].v[0].uv = uv3;
					ret->faces[i].v[1].uv = uv2;
					ret->faces[i].v[2].uv = uv1;
					ret->faces[i].v[3].uv = uv0;
				}
			} else if( SCMP(header, "MATS" ) ) {
				int numMaterials = bs.read<int>();
				ret->materials.resize( numMaterials );
				for( int i = 0; i < numMaterials; i++ ) {
					ret->materials[i] = MaterialData::fromStream( bs );
				}
				bs.skip( chunkSize - (bs.getPosition() - startPos) );
			} else if( SCMP(header, "END" ) ) {
				break;
			} else {
				POOP_WARNING( String("Unknown chunk: ") + String(header) );
				bs.skip( chunkSize );
			}
		}

		return ret;
	}


}
}