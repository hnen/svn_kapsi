
#include "SpaceInput.h"

namespace space {

	Vector2 SpaceInput::getDirection() {

		Vector2 dir = Vector2(0,0);

		if ( m_window->isKeyDown( VK_UP ) ) {
			dir = dir + Vector2(0,1);
		}
		if ( m_window->isKeyDown( VK_DOWN ) ) {
			dir = dir + Vector2(0,-1);
		}
		if ( m_window->isKeyDown( VK_LEFT ) ) {
			dir = dir + Vector2(-1,0);
		}
		if ( m_window->isKeyDown( VK_RIGHT ) ) {
			dir = dir + Vector2(1,0);
		}

		return dir;
	}

	bool SpaceInput::fireDown() {
		return m_window->isKeyDown( VK_CONTROL );
	}

	bool SpaceInput::keyboardCharacterDown( char character ) {
		if ( character >= 'a' || character <= 'z' ) character = character - 'a' + 'A';
		return m_window->isKeyDown( character );
	}


}


