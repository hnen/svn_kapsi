
#ifndef DEBUGTOOLS_H_
#define DEBUGTOOLS_H_

#include <stdio.h>
#include <stdlib.h>

#include "containers\String.h"

class DebugTools {
public:
	class Exception {
		String m_exceptionString;
	public:

		Exception( const String & exceptionString ) :
			m_exceptionString(exceptionString) {}

		const String & getString() { return m_exceptionString; }

	};

	static void error( const String & errorStr ) {
		printf( " !! %s\n", errorStr.getCStr() );
		throw( Exception( errorStr ) );
	}

	static void warning( const String & warningStr ) {
		printf( " !- %s\n", warningStr.getCStr() );
	}

	static void log( const String & logStr ) {
		printf( " -- %s\n", logStr.getCStr() );
	}

};

#define FERROR(...) DebugTools::error( String() + __VA_ARGS__ )

#ifndef FINAL
#define FASSERT(...) if ( !(__VA_ARGS__) ) { DebugTools::error( String() + "Assertion failure: \"" #__VA_ARGS__ "\" @ " __FILE__ ": " + __LINE__ ); }
#define FWARNING(...) DebugTools::warning( String() + __VA_ARGS__ );
#define FLOG(...) DebugTools::log( String() + __VA_ARGS__ );
#else
#define FASSERT(...)
#define FWARNING(...)
#define FLOG(...)
#endif

#endif


