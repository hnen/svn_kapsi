/*
 * Window.cpp
 *
 *  Created on: 29.4.2010
 *      Author: hth
 */

#include <stdio.h>
#include <windows.h>

#include <string>

#include "window.h"
#include "DebugTools.h"

int classExCounter = 0;

using std::string;

LRESULT CALLBACK WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	Window * win = (Window*)GetWindowLong( hwnd, GWL_USERDATA );
	if ( win ) {
		return win->p_windowProc( hwnd, uMsg, wParam, lParam );
	} else {
		return DefWindowProc( hwnd, uMsg, wParam, lParam );
	}
}

Window::Window( int xsize, int ysize, bool fullscreen ) :
		m_hwnd(0) {
	p_init( xsize, ysize, fullscreen );
}

WNDCLASSEX Window::p_createWindowClass( const String & wndClassName ) {
	WNDCLASSEX ret;

	memset( &ret, 0, sizeof(WNDCLASSEX ) );

	ret.cbSize = sizeof(WNDCLASSEX);
	ret.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	ret.cbClsExtra = ret.cbWndExtra = 0;
	ret.hInstance = GetModuleHandle(NULL);
	ret.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	ret.hCursor = LoadCursor(NULL, IDC_ARROW);
	ret.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	ret.lpszMenuName = NULL;
	ret.lpszClassName = wndClassName.getCStr();
	ret.lpfnWndProc = WindowProc;

	return ret;
}

void Window::p_init( int xsize, int ysize, bool fullscreen, bool resizable ) {

	for( int l = 0; l < 256; l++ ) {
		m_keyDown[l] = false;
	}

	m_fullscreen = fullscreen;
	m_xsize = xsize;
	m_ysize = ysize;

	String wndClassName = (String( "FClass" ) + (classExCounter++));

	WNDCLASSEX windowClass = p_createWindowClass( wndClassName );
	ATOM classAtom = 0;
	if ( !(classAtom = RegisterClassEx( &windowClass )) ) {
		FERROR( (String( "RegisterClassEx failure: " ) + p_getWin32Error()).getCStr() );
	}

	DWORD dwExStyle = WS_EX_APPWINDOW;
	DWORD dwStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	int x, y;
	if ( fullscreen ) {
		dwStyle |= WS_POPUP;
		x = y = 0;
	} else {
		dwStyle |= WS_OVERLAPPEDWINDOW;
		x = y = CW_USEDEFAULT;
	}

	if ( !resizable ) {
		dwStyle &= ~WS_THICKFRAME;
	}


	m_hwnd = CreateWindowEx( dwExStyle, wndClassName, "F", dwStyle, x, y, xsize, ysize,
	 		NULL, NULL, GetModuleHandle(NULL), NULL );

	if ( !m_hwnd ) {
		FERROR( (String( "Couldn't create a Win32 window: " ) + p_getWin32Error()).getCStr() );
	}

	SetWindowLong( m_hwnd, GWL_USERDATA, (LONG)this );
	LONG userData = GetWindowLong( m_hwnd, GWL_USERDATA );

	FASSERT( this );
	FASSERT( userData == (long)this );

}


String Window::p_getWin32Error() const {
	DWORD dw = GetLastError();
	LPSTR msgBuf;

	FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &msgBuf, 0, NULL );

	return String( msgBuf );

}

Window::~Window() {
	p_kill();
}

bool Window::isValid() const {
	return m_hwnd != 0;
}

void Window::p_kill() {
	DestroyWindow( m_hwnd );
	m_hwnd = NULL;
}

void Window::p_checkValidity() const {
	if ( !isValid() ) {
		FERROR( "Invalid window handle." );
	}
}

void Window::show() {
	p_checkValidity();
	if ( m_fullscreen ) {
		// TODO: ... set display mode
	}
	m_isVisible = true;
	ShowWindow( m_hwnd, SW_SHOW );
	SetFocus( m_hwnd );
}

void Window::update() {
	p_checkValidity();
	MSG msg;
	while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}

void Window::hide() {
	p_checkValidity();
	m_isVisible = false;
	ShowWindow( m_hwnd, SW_HIDE );
}

LRESULT CALLBACK Window::p_windowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {

	if ( this->isValid() ) {
		FASSERT( m_hwnd == hwnd );

		if ( uMsg == WM_CLOSE ) {
			hide();
			return 0;
		} else if ( uMsg == WM_KEYDOWN || uMsg == WM_KEYUP ) {
			int prevState = (lParam>>30)&1;
			if ( uMsg == WM_KEYDOWN && !prevState ) {
				p_keyPressed( wParam );
			} else if ( uMsg == WM_KEYUP ) {
				p_keyReleased( wParam );
			}
		} else if ( uMsg == WM_DESTROY ) {
			m_hwnd = 0;
		}
	} else {
		FWARNING( "Ignored WindowEvent because window is invalid." );
	}

	return DefWindowProc( hwnd, uMsg, wParam, lParam );
}

void Window::p_keyPressed( int vkeycode ) {
	FASSERT( vkeycode >= 0 && vkeycode < 256 );
	m_keyDown[vkeycode] = true;
}

void Window::p_keyReleased( int vkeycode ) {
	FASSERT( vkeycode >= 0 && vkeycode < 256 );
	m_keyDown[vkeycode] = false;
}



