
#include "boido.h"

void Swarmo::render( float timeStep )
{ 
	DemoResources & res = DemoResources::get();

	Move( timeStep );

	for ( std::vector<Boido*>::iterator p = m_boid.begin(); p<m_boid.end(); p++ )
	{
		Boido *boid = *p;

		float s = 0.3f;
//		Vector3 di = (boid->m_pos - boid->m_lastPos).normalized();
		Vector3& di = boid->m_dir;
		Vector3 up = Vector3(1,0,0);
		Vector3 side = Vector3(0,1,0);
		side = Matrix4::rotateAxis(di,PI/2.0f).mulVec( side );
		up = side.cross(di);
		Matrix4& m =  Matrix4::scale( Vector3(s,s,s) * 2.0f ) * Matrix4( up, side, di, boid->m_pos );

//		m = m * Matrix4::rotateAxis( di, 1 );
		Demo::get().drawMesh( res.mesh_machete, m );

	}

}

static int missaemeet = 0;
static int montaxon = 70;
static float viimexi = 0.0f;

void Swarmo::renderBOAA( float timeStep )
{ 
	DemoResources & res = DemoResources::get();

	if ( oldies == NULL )
		oldies = new Boido[montaxon];

	Move( timeStep * 5 );
//	Move2( timeStep, montaxon );

	std::vector<Boido*> addendum;

	for ( std::vector<Boido*>::iterator p = m_boid.begin(); p<m_boid.end(); p++ )
	{

		for ( int a=0; a<montaxon; a++ )
		{
			Boido*boid = &(oldies[a]);
			float s = 1.0f;
			Vector3& di = boid->m_dir;
			Vector3 up = Vector3(0,1,0);
			Vector3 side = Vector3(1,0,0);
			side = Matrix4::rotateAxis(di,PI/2.0f).mulVec( side );
			up = side.cross(di);
			Matrix4& m =  Matrix4::scale( Vector3(s,s,s) ) * Matrix4( up, side, di, boid->m_pos );

			Demo::get().drawMesh( res.mesh_nikama, m );
			Demo::get().drawMesh( res.mesh_kylki, m );
		}
		viimexi += timeStep;
		if ( viimexi > 0.02f )
		{
//			if ( missaemeet<montaxon ) 
			{
				int i = (missaemeet++)%montaxon;
				Boido& boo = *(Boido*)*p ;
				oldies[i] = Boido( boo );
				viimexi -= 0.02f;
			}
		}

	}



}
