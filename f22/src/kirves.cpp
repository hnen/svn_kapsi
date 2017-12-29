
#include "kirves.h"
#include "Demo.h"
#include "DemoResources.h"

float Kirves::sync_fftglitch(int a) {
	DemoResources res = DemoResources::get();
	float * spc = res.music->getSpectrum( 64 );
	float f = 0;
	for ( int i = 0; i < 16; i++ ) {
		f += spc[i+a];
	}
	return f;
}


Kirves::Kirves()
{
	File file( "data/f22small.png" );
	ImageData * img = ImageImporter::importPng( file.getData(), file.getDataSize() );

	items = new poopie::DynamicArray<Item*>();
	unsigned char* data = img->getData(0);
	int l = img->getDepth();
	int pit = img->getWidth()*l;

	Vector3 mid = Vector3( 0.5f, 0.5f, 0 );
	float h = img->getHeight();
	float w = img->getWidth();

	for ( int y=0; y<img->getHeight()-1; y++ )
	{
		for ( int x=0; x<img->getWidth()-1; x++ )
		{
			char c = data[ x*l + y*pit + 1 ];
			if ( c>0 ) 
			{
				Item* i = new Item( Vector3(1.0f-x/w,1.0f-y/h,0)-mid, c/255.0f );
				items->append(i);
			}
		}
	}
	printf(" ::::::::: img %i x %i & %i   count: %i \n", img->getWidth(), img->getHeight(), l, items->getLength() );

	delete img;
}


void Kirves::renderV( float t )
{
	DemoResources & res = DemoResources::get();
	Matrix4 &m = Matrix4::identity();

	int c = 20;
	for ( int a=0; a<c;a++ )
	{
		Vector3 up = Vector3(0,1,0);
		float d = PI * 2.0f / (float)c * a;
		Vector3 at = Vector3( sinf(t+d)*sync_fftglitch(10), 0, cosf(t+d)*sync_fftglitch(4) ) * 30.2f ;
		Vector3 to = Vector3(0,0,10.0f - 9.0f * sync_fftglitch(18) );
		Matrix4& m = Matrix4::LookAt( at, to, up );
		float z = 10.0f;
		Vector3 k(z,z,z);
		m = Matrix4::scale( k ) * m;
		Demo::get().drawMesh( res.mesh_machete, m );
	}
}


void Kirves::renderWing( Matrix4& mat, float t )
{
	DemoResources & res = DemoResources::get();
	Matrix4 &m = Matrix4::identity();

	float kk = 0.65f + 0.3f * abs(sinf(t));
	int c = 20;
	t = 0.1f + t*0.8f;
	for ( int a=0; a<c;a++ )
	{
		Vector3 p = Vector3( 0,0,1 );
/*		float d = PI * 1.0f / (float)c * (float)a + PI;
		d *= kk;*/
		float cm = 5.0f + (a/(float)c) * 1.5f;
		float g = t*(a+t)/(c) * PI - PI/2.0f;

		Matrix4&m = Matrix4::translate( Vector3(0,0,cm) );
		m = m * Matrix4::rotX(g);
//		m = m * Matrix4::rotZ( t - g*0.2f);
		m = m * Matrix4::rotZ( PI / 2.0f );
		m = m * mat;
		Demo::get().drawMesh( res.mesh_machete, m );
	}
	for ( int a=0; a<c;a++ )
	{
		Vector3 p = Vector3( 0,0,1 );
		//float d = PI * 1.0f / (float)c * (float)a + PI;
		//d *= kk;
		float cm = 5.0f + (a/(float)c) * 1.5f;
		float g = t*(a+t)/(c) * PI - PI/2.0f;

		Matrix4&m = Matrix4::rotX(PI) * Matrix4::translate( Vector3(0,0,-cm) );
		m = m * Matrix4::rotX(-g);
//		m = m * Matrix4::rotZ(-t - g*0.2f);
		m = m * mat;
		Demo::get().drawMesh( res.mesh_machete, m );
	}
}

void Kirves::render( float t, float dt )
{
	DemoResources & res = DemoResources::get();

	float xx = this->sync_fftglitch(0);
	float yy = this->sync_fftglitch(5);
	xx *= xx;
	yy *= yy;

	float diu = -cosf(t * 2.0f);

	float s = 140.0f * diu;

	Item**g = items->getData();
	for ( int a=0; a<items->getLength(); a++ )
	{
		Item* i = g[a];
		if ( i->running() )
		{
			Vector3& pos = i->pos * 80.0f;
			Vector3 offs = Vector3(0,0,70-t*14 + i->posz );
			//Matrix4&m = g[a]->get(dt/10.0f);

			Matrix4&m = Matrix4::translate(pos+offs);
			m = Matrix4::rotY( atan2(s,pos.x) * PI ) * m;
			m = i->get(dt) * m;
			float z = i->size;
			z *= z;
			m = Matrix4::scale( Vector3(z,z,z) ) * m;
		//Matrix4&m = Matrix4::translate(pos+offs);
			//Matrix4&m = Matrix4::identity();
			//if ( t > 0.5f && t<0.7f )
			//	printf("------\n%f, %f, %f, %f\n%f, %f, %f, %f\n", m.m[0], m.m[1], m.m[2], m.m[3], m.m[4], m.m[5], m.m[6], m.m[7] );
			//m = Matrix4::rotX(xx) * Matrix4::rotY(yy);
	//		m = m * Matrix4::rotX(xx) * Matrix4::rotY(yy);
			//m = m * Matrix4::rotX(xx) * Matrix4::rotY(yy);
	//		m = m * Matrix4::translate(pos+offs);
	/*		m = m *  Matrix4::LookAt( 
				pos,
				Vector3(0,0,0),
				Vector3(0,1,0) );*/
			//Matrix4& m = Matrix4::LookAt( pos+offs + Vector3(0,0,-5), pos, Vector3(0,1,0) );
//			Demo::get().drawMesh( res.mesh_machete, m );
			Demo::get().drawMesh( res.mesh_nikama, m );
			
		}
		else
		{
//			if ( !i->end() )
			{
				float x_ = i->pos.x - diu;
				if ( sqrtf( x_*x_ ) < 0.1f )
					i->start();
			}
		}

	}
}
