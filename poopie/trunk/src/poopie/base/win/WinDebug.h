

#ifndef __POOP_UI_WIN_DEBUG
#define __POOP_UI_WIN_DEBUG

#include <Windows.h>
#include "poopie/defs.h"
#include "poopie/base/String.h"

namespace poopie {
namespace win {

class WinDebug {
public:
	static void handleResult( HRESULT hr, const String & errorMsg ) {
		if ( FAILED(hr) ) {

			LPTSTR winErrorStr = NULL;
			FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, hr, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (LPTSTR)&winErrorStr, 0, NULL );

			POOP_ERROR( errorMsg + ( winErrorStr ? String(": ") + winErrorStr : "" ) );
		}
	}
		
	static void handleResult( const String & errorMsg ) {
		DWORD lastError = GetLastError();
		HRESULT hr = HRESULT_FROM_WIN32(lastError);
		handleResult( hr, errorMsg );
	}
};

}
}

#endif
