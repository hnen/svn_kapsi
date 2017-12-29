
#include <Windows.h>

#include "Demo.h"
#include "poopie.h"
#include "boido.h"

using namespace ::Gfx;

Kirves*	Demo::m_kirves;
Swarmo*	Demo::m_boido;
Swarmo*	Demo::m_ranka;


bool sync_beat1( int timeMs ) {
	return timeMs > 2328 && (timeMs - 2328) % 1910 < 255;
}

float sync_fftglitch() {
	DemoResources res = DemoResources::get();
	float * spc = res.music->getSpectrum( 64 );
	float f = 0;
	for ( int i = 4; i < 32; i++ ) {
		f += spc[i];
	}
	return f;
}

float sync_fftbeat( int t ) {

	if ( t < 30470 ) {
		return 0;
	}

	t -= 30470;

	float b = (float)((126 * t) / 60) / 1000;
	float b0 = fmodf( b, 1.0f );

	if ( b0 < 0.6f ) {
		return qerp( 1.0f, 0.0f, b0/0.6f );
	} else {
		return 0;
	}

}

bool sync_first( int timeMs ) {
	//return timeMs > 15637;
	return timeMs > 30470;
}

void Demo::init() {

	kuva = 0;
	overlay = 0;

	DemoResources & res = DemoResources::get();
 
	res.load( m_renderer );
	res.music->play();

	m_kirves = new Kirves();
	m_boido = new Swarmo();
	m_ranka = new Swarmo(1);

	icopaska.init( res.mdata_icos );
	icoqllo.init( res.mdata_qllo );
	icof.init( res.mdata_f );

}
bool onoff =false;

void Demo::render() {
	DemoResources & res = DemoResources::get();

	float t = timef();
	float it = time();
	float dt = t-lastt;
	lastt = t;

	//t += 30.0f;
	//it += 30000;

	p = Matrix4::perspective( 3.14f * 35.0f / 180.0f, 1280.0f / 720.0f, 0.01f, 1000.0f );
	//Matrix4 v = Matrix4::LookAt( Vector3( 0,0, 1 ) * 8.5f, Vector3(0,0,0), Vector3(0,1,0) );

	camView = Vector3(0,0,0);
	camPos = Vector3( 0, 0.0f, 1 ) * 20.0f;


	if ( it > 60971 ) {
		int cit = it - 60971;
		float ct = ((float)cit) / 1000.0f;
		float irp0 = MIN(1,ct/25.0f);
		Vector3 camPos2 = Vector3( sin(ct * 0.5f) * cerp(30.0f, 5.0f, irp0), cos(ct * 0.5f) * cerp(30.0f, 5.0f, irp0), cerp(-1.0f, -30.0f, irp0) );
		float irp = MIN(1,ct / 1.5f);
		camPos = Vector3( cerp(camPos.x, camPos2.x, irp), cerp(camPos.y, camPos2.y, irp), cerp(camPos.z, camPos2.z, irp) );
		camView = Vector3( 0, 0, cerp( 0, -10, MIN(1,ct/10.0f) ) );
	}
		
	if ( it > 75317 ) {
		Vector3 dest(0,0,0);
		float tt = MIN(1,((float)(it-75317)) / 1000.0f);
		camView = Vector3( cerp( camView.x, dest.x, tt ), cerp( camView.y, dest.y, tt ), cerp( camView.z, dest.z, tt ) );
	}

	if ( it > 76317 ) {
		camView = Vector3()-m_boido->Avg();
	}

	if ( it > 90552 ) {
		Vector3 dest(0,0,0);
		float tt = MIN(1,((float)(it-90552)) / (float)( 91254-90552 ));
		camView = Vector3( cerp( camView.x, dest.x, tt ), cerp( camView.y, dest.y, tt ), cerp( camView.z, dest.z, tt ) );
		camPos = camPos * (1.0f + cerp( 0.0f, 0.5f, tt ));
	}

	if ( it > 98000 ) {
		Vector3 dest(0,0,0);
		float tt = MIN(1,((float)(it-98000)) / (float)( 100000-98000 ));
		camPos = camPos * (1.0f + cerp( 0.0f, 1.0f, tt ));	
	}

	if ( it > 137311 - 2000 - 2000 ) {
		float tt = MIN(1,((float)(it-(137311 - 2000 - 2000))) / (float)( 2000 ));
		camPos = camPos * (cerp( 1.0f, 0.15f, tt ));	
	}

	if ( it > 137311 - 2000 ) {
		float tt = MIN(1,((float)(it-(137311 - 2000))) / (float)( 2000 ));
		camPos = camPos * (cerp( 1.0f, 2.0f, tt ));
	}

	if ( it > 135311 ) {
		float tt = MIN(1,((float)(it-(135311))) / (float)( 2000 ));
		Vector3 vc = Vector3( sin(t), cos(t), 0.1f ) * 20.0f;
		camPos = Vector3( cerp( camPos.x, vc.x, tt ), cerp( camPos.y, vc.y, tt ), cerp( camPos.z, vc.z, tt ) );
	}

	if ( it > 137311 ) {
		float tt = MIN(1,((float)(it-(137311))) / (float)( 2000 ));
		Vector3 vc = Vector3( sin(t), cos(t), -2.0f ) * 10.0f;
		camPos = Vector3( camPos.x, camPos.y, camPos.z + cerp( 0, 5.0f, tt ) );
		//camView = Vector3( );
		camView = Vector3( 0, 0, cerp( 0, -30, tt ) );
	}

	
	if ( it > 150160 ) {
		float tt = MIN(1,((float)(it-(150160))) / (float)( 2000 ));
		Vector3 vc = Vector3( sin(t), cos(t), -2.0f ) * 10.0f;
		camPos = Vector3( camPos.x, camPos.y, camPos.z + cerp( 0.0f, -175.0f, tt ) );
		//camView = Vector3( );
		camView = Vector3( 0, 0, cerp( -30, -20, tt ) );
	}

	v = Matrix4::LookAt( camPos, camView, Vector3(0,1,0) );// * Matrix4::rotZ( timef() * 0.1f );


	vp = v*p;

	overlay = 0;

	//m_renderer->setActiveRenderTarget( m_renderer->getBackbufferRenderTarget(), res.rt_depth );
	m_renderer->setActiveRenderTarget( DemoResources::get().rt, res.rt_depth );
	m_renderer->clear( DemoResources::get().rt, Color4() );
		

	m_renderer->clear( m_renderer->getBackbufferRenderTarget(), Color4(0,0,0,0) );
	m_renderer->clear( res.rt_depth );

	float b = pow( sync_fftbeat(it) * 2.0f, 2.0f );
	float g = pow( sync_fftglitch() * 4.0f, 2.0f );

	res.shader_basic->setVariable( "v", v );

	

		//m_kirves->renderV(timef());
		m_renderer->clear( res.rt_depth );
	
	//icopaska.render( Matrix4::rotX( t ) * Matrix4::scale( Vector3(1,1,1) * 4.0f ), t );

	if ( t>(2*60)+2 )
	{
		//m_kirves->renderV(timef());
		//m_renderer->clear( res.rt_depth );
	}
	else if (sync_first(it) )
	{
		m_renderer->clear( res.rt_depth );
	}


	if ( t > 10.0f ) {
		float palloscale = lerp( 0.0f, 1.4f, MIN( 1.0f, (t-10.0f)/10.0f ) ) + powf( sync_fftglitch(), 2.0f ) + sync_fftbeat(it);
		drawMesh( res.mesh_pallo, Matrix4::scale( Vector3(1,1,1) * palloscale ) );
	}

	//ifd ( it > 139311 ) {


		//icof.render( Matrix4(), t - 139.311f, 0.0f, 0.0f, 0.0f );

//	} else


	if ( sync_first( it ) && it < 198340 ) {
//		renderHandTree( Matrix4(), 4,
//			3.14f * 0.5f + cosf( t * 0.2f ),
//			0.5f - cosf( t * 0.6f ) * 0.5f,
//			0.5f - cosf( t * 0.5f ) * 1.5f );



		if ( EXPLO < 1.0f ) {

		if ( it < 45790  ) {
			this->overlay = sin( 5.0f * ((float)(it-45790)) / 1000.0f) + sync_fftglitch() * 3.0f;
			if ( sync_fftbeat(it) >= 0.5f && !onoff ) {
				onoff= true;
				kuva++;
			}
			if ( !sync_fftbeat(it) >=1.0f ) {
				onoff = false;
			}
		}

		if ( it > 45790 ) {

			float ft = MIN( 1, (float)(it-45790) / 500 );
			float fadein = cerp( 0,1, ft );


			int N = 13;

			if ( it < 150160 ) {

			for ( int n = 0; n < N; n++ ) {



				Matrix4 sc= Matrix4::scale( Vector3(1,1,1) * fadein );
				Matrix4 m= Matrix4::rotZ(  6.28f * n / N );
				Matrix4 mx= Matrix4::rotX( cerp(-3.14f*0.5f,0, ft ) );

				Vector3 ps[5];
				for( int i = 0; i < 5; i++ ) {
					ps[i] =   (mx * sc * m).mulVec( 
						Vector3(0, 3.0f + i * 3.0f, 0) + 
						( Vector3( sin(i*1.9f + t*1.5), 0, cos(i*2.1f + t*1.5) ) * 2.0f + Vector3( cos(i*1.3f + t*2.5), 0, cos(i*1.7f + t*4.5) ) * 1.5f
						  + Vector3( sin( i * (0.4 + g * 4.0f) ) * b, 0, cos( i * (0.4 + g * 4.0f) ) * b )
						 
							) * lerp( 0.0f, 2.0f, (float)i/4 )
						);
				}
				Spline s( ps, 5 );

				renderBack( s, 12 );
			}
			}

			if ( it > 60971 && it < 198340 ) {
				float startt = 30470.0f;
				float ebin = 1.0f;
				//ebin = ((float)it-(60971+startt))/(float)startt;
				//if ( ebin<0 ) ebin = 1.0f;

				float tt = ((float)(it - 60971)) / 1000.0f;


				Matrix4 oblx = Matrix4::rotX( 3.141f * 0.5f ) * Matrix4::scale( Vector3(1,1,1) * 2.0f );

				if ( it > 137311 ) {
					float sc = lerp( 1.0f, 5.0f, MIN(1, ((float)(it-137311)) / 10000.0f ) ) ;
					oblx = Matrix4::scale( Vector3(1,1,1) * sc ) * oblx;
				}

				float fukkaus666 = 1.0f;

				if ( it > 167624 ) {
					fukkaus666 = 1.0f + powf( sync_fftglitch(), 2.0f ) * 40.0f;
					this->overlay = sin( 5.0f * ((float)(it-45790)) / 1000.0f) + sync_fftglitch() * 3.0f;
					if ( sync_fftbeat(it) >= 0.5f && !onoff ) {
						onoff= true;
						kuva++;
					}
					if ( !sync_fftbeat(it) >=1.0f ) {
						onoff = false;
					}
				}

				if ( it > 196322 ) {
					tt = cerp( 5.0f, 0.0f, ((float)(it-196322)) / (198340-196322) );



				}

				renderHandTree( oblx, 0,
					3.14f * 0.5f + cosf( t * 0.6f ) * 0.3f + sinf( t * 0.4f ) * 0.3f * fukkaus666,
					0.5f - cosf( t * 0.9f ) * 0.5f * fukkaus666,
					0.5f - cosf( t * 0.5f ) + sinf( t * 0.25f ) * 1.5f * fukkaus666, tt - 2.0f,
					ebin );
			}


		}
		}


		drawOverlay2( t );

		if ( it > 76317 && it < 91254 ) {
			m_boido->render( dt );
		}
		if ( it < 137311 - 2000 ) {
			EXPLO = qerp( 0.0f, 1.1f, MIN(1,MAX(0, ((float)(it - 76317) / 1000.0f) ) / 5.0f ) );
			EXPLO = sqrtf(EXPLO);
		}

		if ( it > 137311 - 2000 ) {
			EXPLO = qerp ( 1.0f, 0.0f, MAX(0,MIN(1, ((float)(it - (137311 - 2000))) / 3000.0f ) ) );
		}

		if ( it > 91254 && it < 131775 + 5000 ) {
			float RT = 1.0f;
			//if ( it > 95000 ) {
			//	RT = cerp( 0.0f, 1.0f, (it-95000)/5000.0f );
			//}

			float giggelson = (float)(it-91254) / 1000.0f;

			if ( it > 100000 ) {
				giggelson = lerp( 5.0f, 0.0f, (float)(it-100000) / 1000.0f );
			}

			icopaska.render( Matrix4::scale( Vector3( 16,16,16 ) * (1.0f+sync_fftbeat(it)*0.2f) ), giggelson, RT, 0.0f, 1.0f );

			if ( it > 100000 ) {
				float fukkaus = MIN(1,MAX(0,lerp( 0.0f, 16.0f, ((float)(it-122040)) / 4000.0f )));

				fukkaus *= fukkaus;

				if ( it > 100000 + 2000 && it < 122040 ) {
					this->overlay = sin( 5.0f * ((float)(it-45790)) / 1000.0f) + sync_fftglitch() * 3.0f;
					if ( sync_fftbeat(it) >= 0.5f && !onoff ) {
						onoff= true;
						kuva++;
					}
					if ( !sync_fftbeat(it) >=1.0f ) {
						onoff = false;
					}

				}

				float KYRPA = (float)(it-100000) / 1000.0f;

				if ( it > 131775 ) {
					KYRPA = cerp( 20.0f, 0.0f, MIN(1, ((float)(it - 131775)) / 5000.0f ) );
				}

				icoqllo.render( Matrix4::scale( Vector3( 16,16,16 ) * (1.0f+sync_fftbeat(it)*0.2f) ), KYRPA, RT, fukkaus, 0.1f );
			}

		}


	} else {
//		if ( it > 22852 )
//			m_kirves->render(timef()-22.852f,dt);
/*		if ( sync_beat1( it ) ) {
			Gfx::drawTex( res.tex_f22, 0, 0, ALIGN_IMG_HCVC | ALIGN_SCR_HCVC, 1.0f );
		} else {
//			drawOverlay( t );
		}*/

		if ( it > 198340 ) {
			t = lerp( 30.0f, 0.0f, ((float)(it-198340)) / 30000.0f );
		}

		drawOverlay( t );
	}

		m_renderer->clear( res.rt_depth );
		//m_ranka->renderBOAA( dt );

	drawOverlay667( timef() );

	m_renderer->setActiveRenderTarget( m_renderer->getBackbufferRenderTarget(), res.rt_depth );
	m_renderer->clear( m_renderer->getBackbufferRenderTarget(), Color4(0,0,0,0) );
	m_renderer->clear( res.rt_depth );



	drawOverlay666( timef() );


}


float Demo::timef() {
	return (float)time() / 1000.0f;
}

unsigned int Demo::time() {
#ifndef FMOD_DISABLE
	return DemoResources::get().music->getTime();
#else
	return timeGetTime();
#endif
}

void Demo::setTime( unsigned int time ) {
#ifndef FMOD_DISABLE
	DemoResources::get().music->setTime( time );
#else
#endif
}


void Demo::drawMesh( Mesh * m, Matrix4 & w ) {
	DemoResources & res = DemoResources::get();

	res.shader_basic->setVariable( "wv", w*v );
	res.shader_basic->setVariable( "wvp", w*vp );

	m_renderer->setActiveShader( res.shader_basic );
	m_renderer->setActiveVertexLayout( res.vlayout_basic );

	m_renderer->drawMesh( m );
}

void Demo::drawOverlay( float t ) {
	DemoResources & res = DemoResources::get();
	Renderer * r = Demo::get().getRenderer();


	//float a = 10.0f + sin(t) * 10.0f;
	float a = sync_fftglitch();
	float b = 20.0f;
	float c = t * t * 0.01f;


	res.shader_pattern->setVariable( "aspect", (float)r->getPrimaryRenderTarget()->getWidth() / r->getPrimaryRenderTarget()->getHeight() );
	res.shader_pattern->setVariable( "tex_image", res.tex_qlmio );
	res.shader_pattern->setVariable( "prms", Vector4(a,b,c,12) );
	r->setActiveShader( res.shader_pattern );
	r->draw( 4, RT_TRIANGLESTRIP );
}

void Demo::drawOverlay666( float t ) {
	DemoResources & res = DemoResources::get();
	Renderer * r = Demo::get().getRenderer();

	res.shader_juusto->setVariable( "tex_image", res.rt_tex );
	res.shader_juusto->setVariable( "tex_paletti", res.tex_baletti );
	res.shader_juusto->setVariable( "tex_noise", res.tex_noise );
	res.shader_juusto->setVariable( "t", t );
	r->setActiveShader( res.shader_juusto );
	r->draw( 4, RT_TRIANGLESTRIP );
}

int perkele = 0;

void Demo::drawOverlay667( float t ) {
	DemoResources & res = DemoResources::get();
	Renderer * r = Demo::get().getRenderer();

	res.shader_overlay2->setVariable( "a", Demo::get().overlay );
	res.shader_overlay2->setVariable( "g", sync_fftglitch() );
	res.shader_overlay2->setVariable( "orig", Vector4(0,0,0,0) );
	res.shader_overlay2->setVariable( "size", Vector4(1,1,0,0) );
	res.shader_overlay2->setVariable( "tex_image", res.tex_overlay[Demo::get().kuva%5] );
	r->setActiveShader( res.shader_overlay2 );
	r->draw( 4, RT_TRIANGLESTRIP );
}

void Demo::drawOverlay2( float t ) {
	DemoResources & res = DemoResources::get();
	Renderer * r = Demo::get().getRenderer();

	//float a = 10.0f + sin(t) * 10.0f;
	float a = sync_fftglitch();
	float b = 90.0f;
	float c = 2.0f;

	if ( t > 90.552 ) {
		float xx = MIN(1,(t-90.552) / (91.114f - 90.552f));
		c = cerp( c, 0.65f, xx ) + (sync_fftglitch() * 3.0f + sync_fftbeat(t)) * lerp( 0,1, xx );
		b = cerp( b, 10.0f, xx );
	}

	res.shader_pattern2->setVariable( "aspect", (float)r->getPrimaryRenderTarget()->getWidth() / r->getPrimaryRenderTarget()->getHeight() );
	res.shader_pattern2->setVariable( "tex_image", res.tex_silma );
	res.shader_pattern2->setVariable( "prms", Vector4(a,b,c,23) );
	res.shader_pattern2->setVariable( "t", t );
	Vector3 dir = Vector3(0,0,0)-Demo::get().camPos.normalized();
	res.shader_pattern2->setVariable( "camDir", Vector4(dir.x, dir.y, dir.z, 0 ) );
	r->setActiveShader( res.shader_pattern2 );
	r->draw( 4, RT_TRIANGLESTRIP );
}

void Demo::renderBack( Spline spline, int count ) {
	DemoResources & res = DemoResources::get();

	float x = 0;

	if ( Demo::get().EXPLO >= 1.0f ) return;

	int it = Demo::get().time();

	srand ( 342889 );

	//for ( int i = 0; i < count; i++ ) {
	//	float x = ((float)i) / (count-1);
	while ( x <= 1.0f ) {
		Vector3 explo(RND(-1,1), RND(-1,1), RND(-1,1) );
		Vector3 pos = spline.getPoint( x ) + explo * Demo::get().EXPLO * 10.0f;

		if ( it > 135311 ) {
			pos.y *= 3.0f;
		}

		Vector3 d = spline.getTangent( x ).normalized();
		Vector3 s = d.cross( Vector3(0,0,1) ).normalized();
		Vector3 c = d.cross(s).normalized();
		Matrix4 rot = Matrix4( s, d, c );
		//float scale = (4.0f * 0.5f * x * (1 - x)) * 0.9f + 0.1f;
		float scale = qerp( 1.0f, 0.1f, x );
		x += scale * 0.055f;
		scale *= 1.0f - Demo::get().EXPLO;
		Demo::get().drawMesh( res.mesh_nikama,  Matrix4::scale( Vector3(1,1,1) * scale ) * rot * Matrix4::translate( pos ) );
	}

}

void Demo::renderHandTree( Matrix4 m, int n, float a, float b, float c, float t, float spread ) {

	if ( n == 0 ) srand( 1295893 );
	
	if ( Demo::get().EXPLO >= 1.0f ) {
		return;
	}

	if ( n >= 3 ) return;

	t = MAX(t,0);

	float rat = 1.5f;

	DemoResources & res = DemoResources::get();

	float q0 = cerp(0.0f, 1.0f, MIN(1,t/rat) ) + sync_fftglitch() * 0.2f;

	m = Matrix4::scale( Vector3(1, q0 ,1) ) * m;


	Vector3 explo = Vector3(RND(-1,1),RND(-1,1),RND(-1,1)) * 10.0f;


	Demo::get().drawMesh( res.mesh_hand2, m * Matrix4::scale( Vector3(1,1,1) * (1.0f - Demo::get().EXPLO ) ) * Matrix4::translate( explo * Demo::get().EXPLO ) );
		if ( spread!=1.0f) 
		{
			// TODO:
//			m_kirves->renderWing( m, spread*sinf(a*b+c) );
		}
		m = Matrix4::rotateAxis( Vector3(1,0,0), b * 0.5f ) * Matrix4::translate( Vector3( 0.0f, 4.2f, 0.0f) ) * m;

	explo = Vector3(RND(-1,1),RND(-1,1),RND(-1,1));


	for ( int i = 0; i < 5; i++ ) {

		renderHandTree(
			Matrix4::rotateAxis( Vector3(0,0,1), a ) * 
			Matrix4::rotateAxis( Vector3(0,1,0), i * 3.14f*2.0f / 5 * spread ) *
			Matrix4::scale( Vector3(1,1,1) * 0.6f ) * m, n+1, a, b, c, t - rat * 0.25f );

	}


	float q1 = cerp(0.0f, 1.0f, MAX(0,MIN(1,(t-rat*0.25f)/rat)) );
	m = Matrix4::rotateAxis( Vector3(1,0,0), 3.14f * (1-q1) ) * Matrix4::rotateAxis( Vector3(1,0,0), b * 0.5f * q1 ) * m;

	m = Matrix4::scale( Vector3(1, q1 ,1) ) * m;

	Demo::get().drawMesh( res.mesh_hand1, m * Matrix4::scale( Vector3(1,1,1) * (1.0f - Demo::get().EXPLO ) ) * Matrix4::translate( explo * Demo::get().EXPLO ) );
	m = Matrix4::rotateAxis( Vector3(0,0,1), -c * 0.5f ) * Matrix4::translate( Vector3( 0.0f, 3.4f, 0.0f) ) * m;

	explo = Vector3(RND(-1,1),RND(-1,1),RND(-1,1));

	for ( int i = 0; i < 3; i++ ) {
		renderHandTree( 
			Matrix4::rotateAxis( Vector3(0,0,1), a ) * 
			Matrix4::rotateAxis( Vector3(0,1,0), i * 3.14f*2.0f / 3 ) *
			Matrix4::scale( Vector3(1,1,1) * 0.3f ) * m, n+2, a, b, c, t - rat*0.5f );
	}
	m = Matrix4::rotateAxis( Vector3(0,0,1), -c * 0.5f ) * m;

	float q2 = cerp(0.0f, 1.0f, MAX(0,MIN(1,(t-rat*0.5f)/2.5f)) );
	m = Matrix4::scale( Vector3(1, 1 ,1) * q2 ) * m;

	Demo::get().drawMesh( res.mesh_hand0, m * Matrix4::scale( Vector3(1,1,1) * (1.0f - Demo::get().EXPLO ) ) * Matrix4::translate( explo * Demo::get().EXPLO ) );


}


void Paska::init( MeshData * md ) {

	this->md = md;

	HashSet<Paska::Edge> s;
	for ( int i = 0; i < md->faces.getLength(); i++ ) {
		Face f = md->faces[i];
		int c = f.v[3].i == -1 ? 3 : 4;
		for ( int j = 0; j < c; j++ ) {
			Paska::Edge e( f.v[j].i, f.v[(j+1)%c].i );
			if ( !s.contains( e ) ) {
				s.add( e );
			}
		}		
	}

	s.resetIteration();
	Paska::Edge ei;
	int n = 0;
	edket = Array<Paska::Edge>( s.getLength() );
	while ( s.iterateNext( &ei ) ) {
		edket[n++] = ei;
	}

	konnekts = Array<DynamicArray<Paska::Edge>>( md->vertices.getLength() );
	for ( int i = 0; i < md->faces.getLength(); i++ ) {
		Face f = md->faces[i];
		int c = f.v[3].i == -1 ? 3 : 4;
		for ( int j = 0; j < c; j++ ) {
			Paska::Edge e( f.v[j].i, f.v[(j+1)%c].i );
			konnekts[e.i0].append( e );
			konnekts[e.i1].append( e );
		}
	}

}

struct paskis {

	paskis() {}
	paskis( int v, float t, Vector3 prevp) : v(v), t(t), prevp(prevp) {}

	int v;
	float t;
	Vector3 prevp;

};

void Paska::render( Matrix4 W, float t, float R, float fukkaus, float paska ) {

	Demo demo = Demo::get();
	DemoResources res = DemoResources::get();

	HashSet<Paska::Edge> rendered;
	HashSet<int> visited;
	Queue<paskis> bfs;

	visited.add( edket[0].i0 );
	bfs.push( paskis( edket[0].i0, t, md->vertices[edket[0].i0].position ) );

	R *= (1.0f + fukkaus * sync_fftglitch());

	//for ( int i = 0; i < edket.getLength(); i++ )
	while ( bfs.getLength() > 0 ) {

		paskis p = bfs.pop();

		DynamicArray<Paska::Edge> elist = konnekts[p.v];

		srand( 321 );

		for ( int i_e = 0; i_e < elist.getLength(); i_e++ ) {
			Paska::Edge e = elist[i_e];
			if ( rendered.contains( e ) ) {
				continue;
			}

			rendered.add( e );


			float s = cerp( 0, 1, MAX(0,MIN(1,p.t)) );
			float r = cerp( 0, 1, MAX(0,MIN(1,p.t-0.5f)) );

			Vector3 p0 = md->vertices[e.i0].position;
			Vector3 p1 = md->vertices[e.i1].position;

			Matrix4 w;
			Vector3 prevp;

			Vector3 rnd = Vector3(RND(-1,1),RND(-1,1),RND(-1,1)).normalized();

			if ( p.v == e.i0 ) {
				Vector3 u = (p1-p0) * 0.3f;
				Vector3 l = u.cross( Vector3(1,1,1) ).normalized() * u.length();
				Vector3 d = l.cross(u).normalized() * u.length();

				rnd = rnd.cross( Vector3(0,1,0) ).normalized();

				u=u*s;
				l=l*s;
				d=d*s;


				w = Matrix4::rotateAxis( rnd, 3.14f * (1.0f-r*R) * 0.5f ) * Matrix4( l,u,d ) * Matrix4::translate( p.prevp );
				
				prevp = w.pos + w.up / 0.3f;

			} else {
				Vector3 u = (p0-p1) * 0.3f;
				Vector3 l = u.cross( Vector3(1,1,1) ).normalized() * u.length();
				Vector3 d = l.cross(u).normalized() * u.length();

				rnd = rnd.cross( Vector3(0,1,0) ).normalized();

				u=u*s;
				l=l*s;
				d=d*s;

				//prevp = p1+(p0-p1)*s;

				w = Matrix4::rotateAxis( rnd, 3.14f * (1.0f-r*R) * 0.5f ) * Matrix4( l,u,d ) * Matrix4::translate( p.prevp );
				prevp = w.pos + w.up / 0.3f;

			}

			demo.drawMesh( res.mesh_hand1, w*W );

			//if ( p.t > 0.2f )
			{
				int a = e.other(p.v);
				if ( !visited.contains( a ) ) {
					visited.add( a );
					bfs.push( paskis( a, p.t-RND(0.2f,1.5f) * paska, prevp ) );
				}
			}

		}

	}

}

