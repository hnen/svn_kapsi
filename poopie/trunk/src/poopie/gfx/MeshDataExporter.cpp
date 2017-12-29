
#include <stdio.h>
#include "MeshData.h"

namespace poopie {

namespace gfx {


	void MeshData::exportOBJ( char * fileName ) {
		FILE * f = fopen( fileName, "w" );
		
		for ( int i = 0; i < vertices.getLength(); i++ ) {
			fprintf( f, "v %.6f %.6f %.6f\n", vertices[i].position.x, vertices[i].position.y, vertices[i].position.z );
		}
		fprintf( f, "s 1\n" ); // wtf does this mean

		for ( int i = 0; i < faces.getLength(); i++ ) {
			if ( faces[i].v[3].i == -1 ) {
				fprintf( f, "f %d %d %d\n", faces[i].v[0].i + 1, faces[i].v[1].i + 1, faces[i].v[2].i + 1 );
			} else {
				fprintf( f, "f %d %d %d %d\n", faces[i].v[0].i + 1, faces[i].v[1].i + 1, faces[i].v[2].i + 1, faces[i].v[3].i + 1 );
			}
		}

		fclose( f );
		f = 0;
	}


}


}


