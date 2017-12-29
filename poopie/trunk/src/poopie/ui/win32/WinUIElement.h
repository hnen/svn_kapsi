
#ifndef __POOP_UI_WIN_ELEMENT
#define __POOP_UI_WIN_ELEMENT

#include <windows.h>

#include "poopie/ui/UIElement.h"

namespace poopie {
namespace ui {
namespace win {


	class WinUIElement : public UIElement {
	public:

		virtual HWND getWindowHandle() const = 0;

	};



}
}
}

#endif


