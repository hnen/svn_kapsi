
#include <png.h>
#include <pngstruct.h>

#include "poopie/base/ByteStream.h"
#include "poopie/base/File.h"

#include "poopie/gfx/ImageData.h"
#include "poopie/gfx/ImageImporter.h"

namespace poopie {
namespace gfx {

	void pngread(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
		if ( png_ptr->io_ptr == 0 ) {
			POOP_ERROR( "Something wrong with libpng" );
			return;
		}

		ByteStream * bs = (ByteStream*)png_ptr->io_ptr;
		bs->readBytesTo( byteCountToRead, outBytes );
	}

	ImageData * ImageImporter::importPng( String & fileName ) {
		File * f = new File( fileName );
		return importPng( f->getData(), f->getDataSize() );
	}

	ImageData * ImageImporter::importPng( void * data, int dataSize ) {

		ByteStream bs(data, dataSize);

		unsigned char * header = (unsigned char*)bs.readChars( 8 );
		bool isPng = !png_sig_cmp( header, 0, 8 );
		if ( !isPng ) {
			POOP_ERROR( "Invalid png." );
			return 0;
		}

		png_structp png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
		if ( !png_ptr ) {
			POOP_ERROR( "Error initing libpng" );
			return 0;
		}

		png_infop info_ptr = png_create_info_struct(png_ptr);
		if ( !info_ptr ) {
			POOP_ERROR( "Error initing libpng" );
			return 0;
		}

		if ( setjmp( png_jmpbuf( png_ptr ) ) ) {
			POOP_ERROR( "Error loading the PNG!" );
			return 0;
		}

		png_set_read_fn( png_ptr, &bs, pngread );
		png_set_sig_bytes(png_ptr, 8);
		png_read_info(png_ptr, info_ptr);

		int width = png_get_image_width( png_ptr, info_ptr );
		int height = png_get_image_height( png_ptr, info_ptr );

		png_byte colorType = png_get_color_type( png_ptr, info_ptr );
		int bdepth = png_get_bit_depth( png_ptr, info_ptr );
		int depth = bdepth / 2;

		int numOfPasses = png_set_interlace_handling( png_ptr );
		png_read_update_info( png_ptr, info_ptr );

		png_bytep * row_pointers = (png_bytep*)malloc( sizeof(png_bytep) * height );
		for ( int y = 0 ; y < height ; y++ ) {
			row_pointers[y] = (png_byte*)malloc( png_get_rowbytes(png_ptr, info_ptr) );
		}

		png_read_image( png_ptr, row_pointers );

		if ( (colorType != PNG_COLOR_TYPE_RGBA) || depth != 4 ) {
			POOP_ERROR( "Png must be 32bit RGBA!" );
		}

		unsigned char * fdata = new unsigned char[width * height * depth];

		for ( int y = 0; y < height; y++ ) {
			png_byte * row = row_pointers[y];
			for ( int x = 0; x < width; x++ ) {
				int * px = (int*)(row + (x*depth));
				((int*)fdata)[y*width+x] = *px;
			}
		}

		ImageDataFormat fmt;
		if ( depth == 4 ) {
			fmt = (ImageDataFormat)(IMG_FMT_RGBA | IMG_FMT_UBYTE);
		} else if ( depth == 3 ) {
			fmt = (ImageDataFormat)(IMG_FMT_RGB | IMG_FMT_UBYTE);
		} else {
			POOP_ERROR( "Unknown type png" );
		}

		ImageData * ret = new ImageData( width, height, fdata, fmt );

		for ( int y = 0 ; y < height ; y++ ) {
			free( row_pointers[y] );
		}
		free ( row_pointers );

		delete [] header;
		return ret;
	}



}
}

