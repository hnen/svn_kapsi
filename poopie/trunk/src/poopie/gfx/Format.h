
#ifndef __POOPIE_GFX_FORMAT
#define __POOPIE_GFX_FORMAT

#include "poopie/base/String.h"
#include "poopie/defs.h"

namespace poopie {
namespace gfx {
	enum FORMAT_TYPE {
		FORMAT_TYPE_FLOAT, FORMAT_TYPE_UINT, FORMAT_TYPE_USHORT, FORMAT_TYPE_UBYTE
	};

	struct Format {
		FORMAT_TYPE fmt;
		int num;
		bool normalized;
		String name;

		Format() { 
		}
		//Format( String & name, FORMAT_TYPE fmt, int numChannels ) : name(name), fmt(fmt), num(num) {

		Format( FORMAT_TYPE fmt, int numChannels ) : fmt(fmt), num(numChannels), normalized(true) {}
		Format( const char * name, FORMAT_TYPE fmt, int numChannels ) : name(name), fmt(fmt), num(numChannels), normalized(true) {}
		Format( const char * name, FORMAT_TYPE fmt, int numChannels, bool normalized ) : name(name), fmt(fmt), num(num), normalized(normalized) {}

		Format& operator=(const Format &f)
		{
			fmt = f.fmt;
			num = f.num;
			normalized = f.normalized;
			name = f.name;
			return *this;
		}

		int getSize() const {
			if ( fmt == FORMAT_TYPE_FLOAT || fmt == FORMAT_TYPE_UINT ) {
				return 4 * num;
			} else if ( fmt == FORMAT_TYPE_USHORT ) {
				return 2 * num;
			} else if ( fmt == FORMAT_TYPE_UBYTE ) {
				return 1 * num;
			} else {
				POOP_ERROR( "Unknown format." );
				return -1;
			}
		}

	};
}
}
#endif