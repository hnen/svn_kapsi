
#define DEV
#define TARGET_FRAME_RATE 30

#include <Windows.h>

#include "poopie/base/File.h"
#include "poopie/math/Color4.h"
#include "poopie/gfx/Renderer.h"
#include "poopie/gfx/Format.h"
#include "poopie/gfx/Shader.h"
#include "poopie/gfx/ImageImporter.h"
#include "poopie/gfx/RenderTarget.h"
#include "poopie/gfx/MeshImporter.h"
#include "poopie/ui/win32/Window.h"
#include "poopie/defs.h"
#include "poopie/math/Vector.h"
#include "poopie/base/String.h"
#include "poopie/gfx/Mesh.h"
#include "poopie/gfx/MeshData.h"

#include "poopie/sfx/Music.h" 
#include "poopie/math/Matrix.h"

//#include "MeshGen.h"
#include "Demo.h"

using poopie::ui::win::Window;
using namespace poopie::ui;
using namespace poopie::gfx;
using namespace poopie::sfx;
using namespace poopie;

long firstTime = 0;
long time() {
	if ( !firstTime ) firstTime = timeGetTime();
	return timeGetTime() - firstTime;
}
float timef() {
	return (float)time() / 1000.0f;
}

//int main ( int argc, char ** argv ) {
int CALLBACK WinMain(
  _In_  HINSTANCE hInstance,
  _In_  HINSTANCE hPrevInstance,
  _In_  LPSTR lpCmdLine,
  _In_  int nCmdShow ) {

	{
		Window win(1280,720,false);
		Renderer * renderer = Renderer::create();
		renderer->initialize( &win );

		//Demo * demo = new Demo( renderer );
		Demo & demo = Demo::get( renderer );

		win.show();
		demo.init();
		int t0 = timeGetTime();
		int frame = 0;
		unsigned int lastTime = demo.time();
		while ( win.isVisible() && !win.isKeyDown( VK_ESCAPE ) ) {
			// fps counter
			int ct = timeGetTime();
			int ft = ct - t0;
			frame++;
			if ( ft > 500 ) {
				printf( "FPS %.2f\n", (float)(frame*1000)/ft );
				t0 = timeGetTime();
				frame = 0;
			}
#ifdef DEV
			if ( Window::isKeyDown( VK_RIGHT ) ) {
				demo.setTime( demo.time() + 250 );
			}
			if ( win.keyPressed( VK_UP ) ) {
				demo.setTime( demo.time() + 15000 );
			}
			if ( Window::isKeyDown( VK_LEFT ) ) {
				if ( demo.time() < 250 ) {
					demo.setTime( 0 );
				} else {
					demo.setTime( demo.time() - 250 );
				}
			}
			if ( win.keyPressed( VK_DOWN ) ) {
				if ( demo.time() < 15000 ) {
					demo.setTime( 0 );
				} else {
					demo.setTime( demo.time() - 15000 );
				}
			}
#endif
			if ( demo.time() > 251000 ) {
					demo.setTime( 0 );
//				break;
			}

			if ( !Window::isKeyDown( VK_SPACE ) )
			{
				demo.render();
				renderer->update();
			}
			win.update();
		}
	}

	return 0;
}





