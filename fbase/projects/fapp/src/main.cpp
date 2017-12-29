
#include <Windows.h>
#include <FBase.h>

struct Particle {
	
	Vector3 pos;
	
	Particle() {}
	Particle( Vector3 pos ) : pos(pos) {}
	 
};
int main () {

	VertexType g_vertexType = VertexType().add( VertexElement::POSITION );

	try {
		Window window(1024, 768, false);
		D3d10 d3d( &window, Fraction(16, 9) );

		window.show();

		D3dEffect effect( File( "Particle.fxo" ), g_vertexType, &d3d );
		effect.setTechnique( "Particle" );

		Array<Particle> particles;
		Array<uint> indices;
		for( uint l = 0; l < 50000; l++ ) {
//			float y = URND( -1.5f, 1.5f );
//			float r = sqrt( 1.5f*1.5f - y*y );
//			float a = URND( 0.0f, 2*PI );
//			float x = sin(a)*r;
//			float z = cos(a)*r;

			Vector3 pos;
			do {
				pos = Vector3( URND( -2.0f, 2.0f ), URND(-2.0f, 2.0f), URND( -2.0f, 2.0f ) );
			} while( pos.lengthSq() > 2.0f );
			float a = acos( pos.z / pos.length() );
			float b = atan2( pos.y, pos.x );
			pos = pos * pow( (1.0f + (cos(b*4) + sin(a * 8))*0.25f), 2 );
			pos = pos * 0.7f;
			//Particle p = Particle( Vector3( x * (1.5f + sin(y*0.75)), y * (1.f + sin(a)), z * (1.5f + sin(y*0.5)) ) * 0.5f ); 
			Particle p = Particle(pos);
			//Particle p = Particle( Vector3( sin((float)l/20*6.28f), 0.0f, cos((float)l/20*6.28f) ) );
			particles.add(p);
			indices.add( l );
		} 

		D3dVertexBuffer vb( particles, g_vertexType, false, &d3d );
		D3dIndexBuffer ib( indices, true, &d3d );
		
		D3dTexture tex( File( "data/Particle.png" ), &d3d );
		
		DWORD paski = timeGetTime();
		
		DWORD fpsTimer = timeGetTime();
		uint frames = 0;

		float grr = 0.0f;
		while ( window.isVisible() ) {
			window.update();

			frames++;
			if ( timeGetTime() - fpsTimer > 5000 ) {
				float fps = (float)(frames * 1000) / (timeGetTime() - fpsTimer);
				FLOG( "fps: " + fps );
				fpsTimer = timeGetTime();
				frames = 0;
			}

			d3d.clear( Color4( 0.0f, 0.0f, 0.0f, 1.0f ) );

			float timer = (float)(timeGetTime() - paski) / 1000.f + 2.0f;
			
			effect.setActive( 0 );
			effect.setTexture( "diffuse", &tex );
			Matrix4 world = Matrix4::rotY(timer) * Matrix4::rotX(timer*0.75f) * Matrix4::translate( Vector3( 0, 0, -3 ) );
			effect.setMatrix( "world", world );
			effect.setMatrix( "view", Matrix4() );
			Matrix4 proj = Matrix4::perspective( ANG2RAD(45.f), 16.f/9.f, 0.1f, 10.f );
			effect.setMatrix( "proj", proj );
			effect.setMatrix( "worldView", world );
			effect.setMatrix( "worldViewProj", world * proj );

			ib.sortCache( world.inverse().preMulVec( Vector3( 0, 0, 1 ), 0.0f ), &vb, 1 );
			ib.uploadCache();

			ib.draw( &vb, 1 );

			d3d.present();

			if ( window.isKeyDown( VK_ESCAPE ) ) {
				window.hide();
			}
		}

		return 0;
	} catch ( DebugTools::Exception e ) {
		MessageBox( NULL, e.getString().getCStr(), "ERROR", MB_OK );
		return 0;
	}

}

