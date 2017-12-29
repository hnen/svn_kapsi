#ifndef __POOPIE_LOGGER_H
#define  __POOPIE_LOGGER_H

#include <stdio.h>
#include "String.h"

namespace poopie {


	class Logger {
	public:

		static void log( const char * c ) {
			printf( "%s\n", c );
		}

		static void log( const String & str ) {
			log( str.getCStr() );
		}

		static void log( int i ) {
			log( String(i) );
		}

		static void log( float f ) {
			log( String(f) );
		}

	};


}

#endif

