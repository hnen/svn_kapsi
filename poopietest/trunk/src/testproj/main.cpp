
#include "poopie/base/File.h"
#include "poopie/base/Color4.h"
#include "poopie/gfx/Renderer.h"
#include "poopie/ui/win32/Window.h"
#include "poopie/defs.h"

using poopie::gfx::Shader;
using poopie::gfx::Renderer;
using poopie::File;
using poopie::ui::win::Window;
using poopie::Color4;

int main ( int argc, char ** argv ) {

	{
		Window win(1280,720,false);
		Renderer * renderer = Renderer::create();
		renderer->initialize( &win );

		Shader * testShader = NULL;
		{
			File fileShader( "data/shaders/o/test.fxo" );
			testShader = renderer->createShader( fileShader.getData(), fileShader.getDataSize() );
		}
		
		win.show();
		while ( win.isVisible() ) {

			renderer->clear( renderer->getBackbufferRenderTarget(), Color4(0,0,0,0) );
			renderer->update();

			win.update();
		}
	}

	return 0;
}






