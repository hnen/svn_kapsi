
#include "ImageData.h"

namespace poopie {
namespace gfx {

	ImageDataPixel::ImageDataPixel( ImageDataFormat fmt ) : m_fmt(fmt), m_own(true), m_ptr(new unsigned char[ImageData::pixelSize(fmt)]) {
	}

	ImageDataPixel::ImageDataPixel( ImageDataPixel & i ) : m_fmt(i.m_fmt), m_ptr(new unsigned char[ImageData::pixelSize(i.m_fmt)]) {
		memcpy( m_ptr, i.m_ptr, ImageData::pixelSize( m_fmt ) );
	}

	ImageDataPixel::ImageDataPixel( float r ) : m_fmt((ImageDataFormat)(IMG_FMT_R | IMG_FMT_FLOAT)), m_ptr(new unsigned char[ImageData::pixelSize(m_fmt)]) {
		((float*)m_ptr)[0] = r;
	}

	ImageDataPixel::ImageDataPixel( float r, float g ) : m_fmt((ImageDataFormat)(IMG_FMT_RG | IMG_FMT_FLOAT)), m_ptr(new unsigned char[ImageData::pixelSize(m_fmt)]) {
		((float*)m_ptr)[0] = r;
		((float*)m_ptr)[1] = g;
	}

	ImageDataPixel::ImageDataPixel( float r, float g, float b ) : m_fmt((ImageDataFormat)(IMG_FMT_RGB | IMG_FMT_FLOAT)), m_ptr(new unsigned char[ImageData::pixelSize(m_fmt)]) {
		((float*)m_ptr)[0] = r;
		((float*)m_ptr)[1] = g;
		((float*)m_ptr)[2] = b;
	}

	ImageDataPixel::ImageDataPixel( float r, float g, float b, float a ) : m_fmt((ImageDataFormat)(IMG_FMT_RGBA | IMG_FMT_FLOAT)), m_ptr(new unsigned char[ImageData::pixelSize(m_fmt)]) {
		((float*)m_ptr)[0] = r;
		((float*)m_ptr)[1] = g;
		((float*)m_ptr)[2] = b;
		((float*)m_ptr)[3] = a;
	}

	void ImageDataPixel::convertChannel( const void * sdat, void *ddat, ImageDataFormat m_fmt, ImageDataFormat fmt ) const {
		if ( (m_fmt&IMG_FMT_DATATYPE_MASK) == (fmt&IMG_FMT_DATATYPE_MASK) ) {
			memcpy( ddat, sdat, ImageData::channelSize(fmt) );
		} else 
		if ( (m_fmt&IMG_FMT_DATATYPE_MASK) == IMG_FMT_FLOAT ) {
			float sval = *((float*)sdat);
			if ( (fmt&IMG_FMT_DATATYPE_MASK) == IMG_FMT_UBYTE ) {
				unsigned char * dval = (unsigned char*)ddat;
				*dval = (unsigned char)(CLAMP(sval, 0, 1) * 255.0f);
			} else
			if ( (fmt&IMG_FMT_DATATYPE_MASK) == IMG_FMT_UINT ) {
				unsigned int * dval = (unsigned int*)ddat;
				*dval = (unsigned int)(CLAMP(sval, 0, 1) * 255.0f);
			} else {
				POOP_ERROR( "Can't convert from IMG_FMT_FLOAT" );
			}
		} else
		if ( (m_fmt&IMG_FMT_DATATYPE_MASK) == IMG_FMT_UINT ) {
			unsigned int sval = *((unsigned int*)sdat);
			if ( (fmt&IMG_FMT_DATATYPE_MASK) == IMG_FMT_FLOAT ) {
				float * dval = (float*)ddat;
				*dval = ((float)sval) / 255.0f;
			} else
			if ( (fmt&IMG_FMT_DATATYPE_MASK) == IMG_FMT_UBYTE ) {
				unsigned char * dval = (unsigned char*)ddat;
				*dval = (unsigned char)CLAMP(0, 255, sval);
			} else {
				POOP_ERROR( "Can't convert from IMG_FMT_UBYTE" );
			}
		} else
		if ( (m_fmt&IMG_FMT_DATATYPE_MASK) == IMG_FMT_UBYTE ) {
			unsigned char sval = *((unsigned char*)sdat);
			if ( (fmt&IMG_FMT_DATATYPE_MASK) == IMG_FMT_FLOAT ) {
				float * dval = (float*)ddat;
				*dval = ((float)sval) / 255.0f;
			} else
			if ( (fmt&IMG_FMT_DATATYPE_MASK) == IMG_FMT_UINT ) {
				unsigned int * dval = (unsigned int*)ddat;
				*dval = (unsigned int)sval;
			} else {
				POOP_ERROR( "Can't convert from IMG_FMT_UBYTE" );
			}
		} else {
			POOP_ERROR( "Unknown source format." );
		}
	}

	int ImageData::pixelSize( ImageDataFormat f ) {
		return numOfChannels(f) * channelSize(f);
	}
	int ImageData::numOfChannels( ImageDataFormat f ) {
		return 4 - (((int)f) & 3);
	}
	int ImageData::channelSize( ImageDataFormat f ) {
		if ( (f & IMG_FMT_DATATYPE_MASK) == IMG_FMT_UBYTE ) {
			return 1;
		} else if ( (f&IMG_FMT_DATATYPE_MASK) == IMG_FMT_FLOAT || (f&IMG_FMT_DATATYPE_MASK) == IMG_FMT_UINT ) {
			return 4;
		} else {
			POOP_ERROR( "Unknown format" );
			return -1;
		}
	}


	float ImageDataPixel::get( int c ) const {
		POOP_ASSERT( c < ImageData::numOfChannels(m_fmt) );
		void * dat = ((unsigned char*)m_ptr) + (c*ImageData::channelSize(m_fmt));
		float ret;
		convertChannel( dat, &ret, m_fmt, IMG_FMT_FLOAT );
		return ret;
	}

	float ImageDataPixel::luminance() const {
		int c = ImageData::numOfChannels(m_fmt);
		if ( c == 1 ) {
			return get(0);
		} else if ( c == 3 || c == 4 ) {
			return 0.2126f * get(0) + 0.7152f * get(1) + 0.0722 * get(2);
		} else {
			POOP_ERROR( "Invalid operation" );
			return 0;
		}
	}

	ImageDataPixel ImageDataPixel::convertTo( ImageDataFormat fmt ) {
		if ( fmt == m_fmt ) {
			return ImageDataPixel( m_ptr, m_fmt );
		}
		unsigned char * nptr = new unsigned char[ImageData::pixelSize(fmt)];
		memset( nptr, 0, ImageData::pixelSize(fmt) );
		for ( int n = 0; n < MIN(ImageData::numOfChannels(fmt),ImageData::numOfChannels(m_fmt)); n++ ) {
			void * ddat = &nptr[n*ImageData::channelSize(fmt)];
			void * sdat = &((unsigned char*)m_ptr)[n*ImageData::channelSize(m_fmt)];
			convertChannel( sdat, ddat, m_fmt, fmt );
		}
		return ImageDataPixel( nptr, fmt );
	}

	ImageDataPixel ImageDataPixel::operator + ( ImageDataPixel & b ) {
		ImageDataPixel ret( m_fmt );
		ImageDataPixel t = b.convertTo( m_fmt );
		for ( int n = 0; n < ImageData::numOfChannels( m_fmt ); n++ ) {
			void * adat = &((unsigned char*)m_ptr)[n*ImageData::channelSize(m_fmt)];
			void * bdat = &((unsigned char*)t.m_ptr)[n*ImageData::channelSize(m_fmt)];
			void * cdat = &((unsigned char*)ret.m_ptr)[n*ImageData::channelSize(m_fmt)];
			if ( (m_fmt&IMG_FMT_DATATYPE_MASK) == IMG_FMT_FLOAT ) {
				*((float*)cdat) = *((float*)adat) + *((float*)bdat);
			} else if ( (m_fmt&IMG_FMT_DATATYPE_MASK) == IMG_FMT_UBYTE ) {
				*((unsigned char*)cdat) = (unsigned char)MIN(255, (int)*((unsigned char*)adat) + (int)*((unsigned char*)bdat) );
			} else if ( (m_fmt&IMG_FMT_DATATYPE_MASK) == IMG_FMT_UINT ) {
				*((unsigned int*)cdat) = *((unsigned int*)adat) + *((unsigned int*)bdat);
			} else {
				POOP_ERROR( "Unknown format" );
			}
		}
		return ret;
	}

	ImageDataPixel ImageDataPixel::operator - ( ImageDataPixel & b ) {
		ImageDataPixel ret( m_fmt );
		ImageDataPixel t = b.convertTo( m_fmt );
		for ( int n = 0; n < ImageData::numOfChannels( m_fmt ); n++ ) {
			void * adat = &((unsigned char*)m_ptr)[n*ImageData::channelSize(m_fmt)];
			void * bdat = &((unsigned char*)t.m_ptr)[n*ImageData::channelSize(m_fmt)];
			void * cdat = &((unsigned char*)ret.m_ptr)[n*ImageData::channelSize(m_fmt)];
			if ( (m_fmt&IMG_FMT_DATATYPE_MASK) == IMG_FMT_FLOAT ) {
				*((float*)cdat) = *((float*)adat) - *((float*)bdat);
			} else if ( (m_fmt&IMG_FMT_DATATYPE_MASK) == IMG_FMT_UBYTE ) {
				*((unsigned char*)cdat) = (unsigned char)MAX(0, (int)*((unsigned char*)adat) - (int)*((unsigned char*)bdat) );
			} else if ( (m_fmt&IMG_FMT_DATATYPE_MASK) == IMG_FMT_UINT ) {
				*((unsigned int*)cdat) = (unsigned int)MAX(0,(int)*((unsigned int*)adat) - (int)*((unsigned int*)bdat));
			} else {
				POOP_ERROR( "Unknown format" );
			}
		}
		return ret;
	}

	ImageDataPixel ImageDataPixel::operator * ( float f ) {
		ImageDataPixel ret( m_fmt );
		for ( int n = 0; n < ImageData::numOfChannels( m_fmt ); n++ ) {
			void * adat = &((unsigned char*)m_ptr)[n*ImageData::channelSize(m_fmt)];
			void * cdat = &((unsigned char*)ret.m_ptr)[n*ImageData::channelSize(m_fmt)];
			if ( (m_fmt&IMG_FMT_DATATYPE_MASK) == IMG_FMT_FLOAT ) {
				*((float*)cdat) = *((float*)adat) * f;
			} else if ( (m_fmt&IMG_FMT_DATATYPE_MASK) == IMG_FMT_UBYTE ) {
				*((unsigned char*)cdat) = (unsigned char)MIN(255,*((unsigned char*)adat) * f);
			} else if ( (m_fmt&IMG_FMT_DATATYPE_MASK) == IMG_FMT_UINT ) {
				*((unsigned int*)cdat) = (unsigned int)MIN(255,*((unsigned int*)adat) * f);
			} else {
				POOP_ERROR( "Unknown format" );
			}
		}
		return ret;
	}

	ImageDataPixel ImageDataPixel::operator / ( int i ) {
		ImageDataPixel ret( m_fmt );
		for ( int n = 0; n < ImageData::numOfChannels( m_fmt ); n++ ) {
			void * adat = &((unsigned char*)m_ptr)[n*ImageData::channelSize(m_fmt)];
			void * cdat = &((unsigned char*)ret.m_ptr)[n*ImageData::channelSize(m_fmt)];
			if ( (m_fmt&IMG_FMT_DATATYPE_MASK) == IMG_FMT_FLOAT ) {
				*((float*)cdat) = *((float*)adat) / i;
			} else if ( (m_fmt&IMG_FMT_DATATYPE_MASK) == IMG_FMT_UBYTE ) {
				*((unsigned char*)cdat) = *((unsigned char*)adat) / i;
			} else if ( (m_fmt&IMG_FMT_DATATYPE_MASK) == IMG_FMT_UINT ) {
				*((unsigned int*)cdat) = *((unsigned int*)adat) / i;
			} else {
				POOP_ERROR( "Unknown format" );
			}
		}
		return ret;
	}

	void ImageDataPixel::copyTo( ImageDataFormat fmt, void * out_ptr ) {
		ImageDataPixel tmp( this->convertTo( fmt ) );
		memcpy( out_ptr, tmp.m_ptr, ImageData::pixelSize( fmt ) );
	}

	void ImageData::generateMips() {

		int numMips = 0;
		{
			int w = m_width, h = m_height;
			while( w > 1 && h > 1 ) {
				numMips++;
				w >>= 1;
				h >>= 1;
			}
		}

		m_data.resize( numMips );

		int d = pixelSize( m_fmt );

		int w = m_width;
		int h = m_height;
		for( int i = 1; i < numMips; i++ ) {
			w>>=1; h>>=1;
			unsigned char * ndata = new unsigned char[w*h*d];
			unsigned char * odata = (unsigned char*)m_data[i-1];
			for( int y = 0; y < h; y++ ) {
				for ( int x = 0; x < w; x++ ) {
					ImageDataPixel n( &ndata[d*(y*w+x)], m_fmt );
					ImageDataPixel o0( &odata[((2*y)*(2*w)+(2*x))*d], m_fmt );
					ImageDataPixel o1( &odata[((2*y)*(2*w)+(2*x+1))*d], m_fmt );
					ImageDataPixel o2( &odata[((2*y+1)*(2*w)+(2*x))*d], m_fmt );
					ImageDataPixel o3( &odata[((2*y+1)*(2*w)+(2*x+1))*d], m_fmt );

					// prevent overflow
					if ( (m_fmt & IMG_FMT_DATATYPE_MASK) == IMG_FMT_UBYTE ) {
						o0 = o0.convertTo( (ImageDataFormat)(IMG_FMT_UINT | (m_fmt & IMG_FMT_CHANNELS_MASK)) );
						o1 = o1.convertTo( (ImageDataFormat)(IMG_FMT_UINT | (m_fmt & IMG_FMT_CHANNELS_MASK)) );
						o2 = o2.convertTo( (ImageDataFormat)(IMG_FMT_UINT | (m_fmt & IMG_FMT_CHANNELS_MASK)) );
						o3 = o3.convertTo( (ImageDataFormat)(IMG_FMT_UINT | (m_fmt & IMG_FMT_CHANNELS_MASK)) );
					}

					n = (o0 + o1 + o2 + o3) / 4;
				}
			}
			m_data[i] = ndata;
		}
	}


}
}


