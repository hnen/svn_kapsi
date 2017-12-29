
#ifndef __SPACE_INPUT
#define __SPACE_INPUT

#include "poopie_base.h"

#include "poopie/ui/win32/Window.h"

namespace space {

	class SpaceInput {
	public:

		SpaceInput( poopie::ui::win::Window * window ) : m_window(window) {}
		Vector2 getDirection();
		bool fireDown();
		bool keyboardCharacterDown( char character );

	private:

		poopie::ui::win::Window * m_window;

	};

}

#endif
