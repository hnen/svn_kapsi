
#ifndef __POOPIE_GFX_IMAGEDATA
#define __POOPIE_GFX_IMAGEDATA

#include <string.h>

#include "poopie/base/Array.h"

namespace poopie {
namespace gfx {

	enum ImageDataFormat {
		IMG_FMT_R = 3, IMG_FMT_RG = 2, IMG_FMT_RGB = 1, IMG_FMT_RGBA = 0,
		IMG_FMT_CHANNELS_MASK = 3,

		IMG_FMT_UINT = (2<<2),IMG_FMT_FLOAT = (1<<2), IMG_FMT_UBYTE = 0,
		IMG_FMT_DATATYPE_MASK = 12,

		IMG_FMT_R_UINT = IMG_FMT_R | IMG_FMT_UINT, IMG_FMT_RG_UINT = IMG_FMT_RG | IMG_FMT_UINT, IMG_FMT_RGB_UINT = IMG_FMT_RGB | IMG_FMT_UINT, IMG_FMT_RGBA_UINT = IMG_FMT_RGBA | IMG_FMT_UINT, 
		IMG_FMT_R_UBYTE = IMG_FMT_R | IMG_FMT_UBYTE, IMG_FMT_RG_UBYTE = IMG_FMT_RG | IMG_FMT_UBYTE, IMG_FMT_RGB_UBYTE = IMG_FMT_RGB | IMG_FMT_UBYTE, IMG_FMT_RGBA_UBYTE = IMG_FMT_RGBA | IMG_FMT_UBYTE, 
		IMG_FMT_R_FLOAT = IMG_FMT_R | IMG_FMT_FLOAT, IMG_FMT_RG_FLOAT = IMG_FMT_RG | IMG_FMT_FLOAT, IMG_FMT_RGB_FLOAT = IMG_FMT_RGB | IMG_FMT_FLOAT, IMG_FMT_RGBA_FLOAT = IMG_FMT_RGBA | IMG_FMT_FLOAT, 
	};

	// private
	class ImageDataPixel {
	public:

		ImageDataPixel( void * ptr, ImageDataFormat fmt ) : m_ptr(ptr), m_fmt(fmt), m_own(false) {}
		ImageDataPixel( ImageDataFormat fmt );
		ImageDataPixel( ImageDataPixel & i );
		ImageDataPixel( float r );
		ImageDataPixel( float r, float g );
		ImageDataPixel( float r, float g, float b );
		ImageDataPixel( float r, float g, float b, float a );

		~ImageDataPixel() {
			if ( m_own ) {
				delete [] m_ptr;
			}
		}

		void copyTo( ImageDataFormat fmt, void * out_ptr );
		ImageDataPixel & operator = ( ImageDataPixel & px ) { px.copyTo( m_fmt, m_ptr ); return *this; }
		ImageDataPixel operator + ( ImageDataPixel & px );
		ImageDataPixel operator - ( ImageDataPixel & px );
		ImageDataPixel operator * ( float f );
		ImageDataPixel operator / ( int i );

		ImageDataPixel convertTo( ImageDataFormat fmt );

		float get( int c ) const;
		float luminance() const;

	private:
		
		void convertChannel( const void * input, void * output, ImageDataFormat inputFmt, ImageDataFormat outputFmt ) const;

		void * const m_ptr;
		ImageDataFormat m_fmt;
		bool m_own;

	};

	class ImageData {
	public:

		ImageData( int width, int height, void * data, ImageDataFormat fmt = (ImageDataFormat)0 ) : m_width(width), m_height(height), m_fmt(fmt) {
			m_data.resize(1);
			m_data[0] = new unsigned char[width*height*numOfChannels(fmt)*channelSize(fmt)];
			memcpy( m_data[0], data, width*height*numOfChannels(fmt)*channelSize(fmt) );
		}
		ImageData( int width, int height, ImageDataFormat fmt = (ImageDataFormat)0 ) : m_width(width), m_height(height), m_fmt(fmt) {
			m_data.resize(1);
			m_data[0] = new unsigned char[width*height*numOfChannels(fmt)*channelSize(fmt)];
		}

		~ImageData() {
			for ( uint i = 0; i < m_data.getLength(); i++ ) {
				delete [] m_data[i];
			}
		}

		int getWidth() const { return m_width; }
		int getHeight() const { return m_height; }
		int getDepth() const { return pixelSize(m_fmt); }
		ImageDataFormat getFormat() const { return m_fmt; }

		int getMips() const { return m_data.getLength(); }

		unsigned char * getData( int mip ) { return (unsigned char*)m_data[mip]; }

		void generateMips();

		static int pixelSize( ImageDataFormat f );
		static int numOfChannels( ImageDataFormat f );
		static int channelSize( ImageDataFormat f );

		static ImageData * genNoise( int width, int height );
		static ImageData * genNoise32( int width, int height );
		static ImageData * genPerlin( int width, int height, int cellW, int seed );

	private:

		int m_width;
		int m_height;
		ImageDataFormat m_fmt;
		Array<void*> m_data;

	};

	class ImageData3 {
	public:

		ImageData3( int width, int height, int depth, void * data, ImageDataFormat fmt = (ImageDataFormat)0 ) : m_width(width), m_height(height), m_depth(depth), m_fmt(fmt) {
			m_data.resize(1);
			m_data[0] = new unsigned char[width*height*depth*ImageData::pixelSize(fmt)];
			memcpy( m_data[0], data, width*height*ImageData::pixelSize(fmt) );
		}
		ImageData3( int width, int height, int depth, ImageDataFormat fmt = (ImageDataFormat)0 ) : m_width(width), m_height(height), m_depth(depth), m_fmt(fmt) {
			m_data.resize(1);
			m_data[0] = new unsigned char[width*height*depth*ImageData::pixelSize(fmt)];
		}

		~ImageData3() {
			for ( uint i = 0; i < m_data.getLength(); i++ ) {
				delete [] m_data[i];
			}
		}

		int getWidth() const { return m_width; }
		int getHeight() const { return m_height; }
		int getDepth() const { return m_depth; }
		ImageDataFormat getFormat() const { return m_fmt; }

		int getMips() const { return m_data.getLength(); }

		unsigned char * getData( int mip ) { return (unsigned char*)m_data[mip]; }


	private:

		int m_width;
		int m_height;
		int m_depth;
		ImageDataFormat m_fmt;
		Array<void*> m_data;

	};



}
}


#endif
