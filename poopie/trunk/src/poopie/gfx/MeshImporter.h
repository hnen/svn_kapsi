#ifndef __POOPIE_GFX_MESHIMPORTER
#define __POOPIE_GFX_MESHIMPORTER

namespace poopie {
namespace gfx {

	class MeshData;

class MeshImporter {
public:

	//MeshImporter();

	static MeshData * Import( void * data, int dataSize );


};

}
}

#endif
