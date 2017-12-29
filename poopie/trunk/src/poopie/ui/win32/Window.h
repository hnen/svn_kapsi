
#ifndef POOPIE_WINDOW_H_
#define POOPIE_WINDOW_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "poopie/base/String.h"
#include "poopie/ui/win32/WinUIElement.h"

namespace poopie {
namespace ui {
namespace win {

class Window : public WinUIElement {

	HWND m_hwnd;
	bool m_fullscreen;
	bool m_isVisible;

	static bool m_keyDown[256];
	static bool m_keyDownPrev[256];
	static bool m_keyPressed[256];
	static bool m_keyReleased[256];

	// mouse buttons are mapped: 0 = left, 1 = middle, 2 = right
	// enums could be nice
	static bool m_mouseButtonDown[3];
	static bool m_mouseButtonPrev[3];
	static bool m_mouseButtonPressed[3];
	static bool m_mouseButtonReleased[3];

	int m_mouseX;
	int m_mouseY;

	int m_xsize;
	int m_ysize;

	void p_init( int xsize, int ysize, bool fullscreen, bool resizable = false );
	void p_kill();

	void p_checkValidity() const;

	String p_getWin32Error() const;

	WNDCLASSEX p_createWindowClass( const String & wndClassName );

	void p_keyPressed( int vKeycode );
	void p_keyReleased( int vKeycode );

	void p_mouseButtonPressed( int button );
	void p_mouseButtonReleased( int button );
	void p_mouseMove( int x, int y );

public:

	Window( int xsize, int ysize, bool fullscreen );
	~Window();

	void update();

	bool isValid() const;
	static bool keyPressed( int keyCode ) {
		return m_keyPressed[keyCode];
	}

	bool mouseLeftPressed() const { return m_mouseButtonPressed[0]; }
	bool mouseMidPressed() const { return m_mouseButtonPressed[1]; }
	bool mouseRightPressed() const { return m_mouseButtonPressed[2]; }
	int getMouseX() const { return m_mouseX; }
	int getMouseY() const { return m_mouseY; }

	void show() ;
	void hide() ;
	bool isVisible() const { return m_isVisible; }

	static bool isKeyDown( int vKeycode ) { return m_keyDown[vKeycode]; }

	int getYSize() const { return m_ysize; }
	int getXSize() const { return m_xsize; }
	HWND getWindowHandle() const { return m_hwnd; }
	bool isFullscreen() const { return m_fullscreen; }

	/**
	 * Win32 system windowProc, do not call manually.
	 */
	LRESULT CALLBACK p_windowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

};

}
}
}

#endif


