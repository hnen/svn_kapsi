
#ifndef __POOPIE_GFX_IMAGEIMPORTER
#define __POOPIE_GFX_IMAGEIMPORTER

namespace poopie {
namespace gfx {

	class ImageData;

	class ImageImporter {
	public:

		static ImageData * importPng( void * data, int dataSize );
		static ImageData * importPng( String & fileName );

	};

}
}


#endif