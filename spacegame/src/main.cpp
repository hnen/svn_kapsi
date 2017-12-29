

#define DEV
#define TARGET_FPS 60

#include <conio.h>
#include <stdlib.h>

#include <Windows.h>

//#include "poopie/base/File.h"
//#include "poopie/math/Color4.h"
#include "poopie/gfx/Renderer.h"
//#include "poopie/gfx/Format.h"
//#include "poopie/gfx/Shader.h"
//#include "poopie/gfx/ImageImporter.h"
//#include "poopie/gfx/RenderTarget.h"
//#include "poopie/gfx/MeshImporter.h"
#include "poopie/ui/win32/Window.h"
#include "poopie/defs.h"
//#include "poopie/math/Vector.h"
//#include "poopie/base/String.h"
//#include "poopie/gfx/Mesh.h"
//#include "poopie/gfx/MeshData.h"

#include "poopie/sfx/Music.h"
//#include "poopie/math/Matrix.h"

#include "SpaceEngine.h"

using poopie::ui::win::Window;
using namespace poopie::ui;
using namespace poopie::gfx;
using namespace poopie::sfx;
using namespace poopie;
using namespace space;


int main ( int argc, char ** argv ) {
	{
		Window win(1280,720,false);
		Renderer * renderer = Renderer::create();
		SpaceEngine * engine = new SpaceEngine( renderer );
		renderer->initialize( &win );


		timeBeginPeriod(1);
		srand( timeGetTime() );

		win.show();

		// Create zbuffer
		DepthStencil * zbuffer = renderer->createDepthStencilRenderTarget( 1280, 720 );
		renderer->setActiveRenderTarget( renderer->getBackbufferRenderTarget(), zbuffer );
		
		engine->init( &win );

		POOP_LOG( "All done! Starting main loop." );

		//engine->getResources()->music_main->play();
		//engine->getResources()->music_main->setTime( 45000 );

		int t0 = timeGetTime();
		int frame = 0;
		int lt = timeGetTime();
		while ( win.isVisible() ) {
			int ct = timeGetTime();
			unsigned int dt = ct - lt;
			lt = ct;

			// fps counter
			int ft = ct - t0;
			frame++;
			if ( ft > 1000 ) {
				printf( "FPS %.2f\n", (float)(frame*1000)/ft );
				t0 = timeGetTime();
				frame = 0;
			}

			engine->update( dt );

			renderer->clear( renderer->getBackbufferRenderTarget(), Color4(0,0,0,0) );
			renderer->clear( zbuffer );
			engine->render();


			renderer->update();
			win.update();

			long ot = timeGetTime() - ct;
			if ( TARGET_FPS*ot < 1000 ) {
				Sleep( (1000 - ot * TARGET_FPS)/TARGET_FPS );
			}
		}

		timeEndPeriod(1);

		delete zbuffer;
		engine->cleanup();
		delete engine;

		delete renderer;
		

	}

#ifdef POOP_DEBUG
	POOP_LOG( "All done. Press any key to quit." );
	getch();
#endif

	return 0;
}








