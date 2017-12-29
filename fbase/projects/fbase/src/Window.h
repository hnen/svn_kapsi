/*
 * Window.h
 *
 *  Created on: 29.4.2010
 *      Author: hth
 */

#ifndef WINDOW_H_
#define WINDOW_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "containers/string.h"



class Window {

	HWND m_hwnd;
	bool m_fullscreen;
	bool m_isVisible;

	bool m_keyDown[256];

	int m_xsize;
	int m_ysize;

	void p_init( int xsize, int ysize, bool fullscreen, bool resizable = false );
	void p_kill();

	void p_checkValidity() const;

	String p_getWin32Error() const;

	WNDCLASSEX p_createWindowClass( const String & wndClassName );

	void p_keyPressed( int vKeycode );
	void p_keyReleased( int vKeycode );

public:

	Window( int xsize, int ysize, bool fullscreen );
	~Window();

	void update();

	bool isValid() const;

	void show() ;
	void hide() ;
	bool isVisible() const { return m_isVisible; }

	bool isKeyDown( int vKeycode ) const { return m_keyDown[vKeycode]; }

	int getYSize() const { return m_ysize; }
	int getXSize() const { return m_xsize; }
	HWND getWindowHandle() const { return m_hwnd; }
	bool isFullscreen() const { return m_fullscreen; }

	/**
	 * Win32 system windowProc, do not call manually.
	 */
	LRESULT CALLBACK p_windowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

};


#endif /* WINDOW_H_ */
