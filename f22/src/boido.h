

#include <vector>
#include "Demo.h"
#include "DemoResources.h"

#ifndef BOIDO_
#define BOIDO_

class Boido
{
public:
	Boido() { }
	Boido( Boido& b ) : m_pos(b.m_pos), m_lastPos(b.m_lastPos), m_dir(b.m_dir), state(b.state), m_targetDir(b.m_targetDir)
	{
	}
	Boido( Vector3 pos, Vector3 dir ) : m_pos(pos), m_dir(dir) { }
	Vector3	m_pos;
	Vector3	m_lastPos;
	Vector3	m_dir;
	Vector3	m_targetDir;
	int		state;
};



class Swarmo
{
public:
	std::vector<Boido*> m_boid;


	//Vector3	m_dir;
	Vector3	m_dest;
	float	m_localDistance;	// distance of "local" space
	float	m_minDistance;		// to nearby boidos

	int count;

	Boido* oldies;

	Swarmo( int count = 100 ) : count(count)
	{
		oldies = NULL;
		m_dest = Vector3(0,0,0);
		for ( int a=0; a<count; a++ ) 
		{
			Vector3 *mm = new Vector3(RND(-1,1),RND(-1,1),RND(-1,1)) ;
			mm->x *= 10.0f;
			mm->y *= 10.0f;
			mm->z *= 10.0f;
			Vector3 p = *mm;
			p = p * 0.3f;
			Vector3 *d = new Vector3(RND(-1,1),RND(-1,1),RND(-1,1));
			Boido *n = new Boido( p, *d );
			delete mm;
			delete d;
			m_boid.push_back( n );
		}
		m_localDistance = 0.2f;
		m_minDistance = 0.05f;
	}

	~Swarmo()
	{
		delete[] oldies;
/*		while ( m_boid.size() > 0 )
		{
			Boido *p = *m_boid.end();
			m_boid.pop_back();
			delete p;
		}*/
	}

	Vector3 Avg() {
		int n = 0;
		Vector3 ret;
		for ( std::vector<Boido*>::iterator p = m_boid.begin(); p<m_boid.end(); p++ )
		{
			Boido *boid = *p;
			ret =ret + boid->m_pos;
			n++;
		}
		ret = ret / (float)n;
		return ret;
	}

	void	Move( float timeStep )
	{
		if ( (rand()%1000) < 2 )
		{
			m_dest.x = sin( count / 87.0f + cos( count/59.0f) ) * 10.8f;
			m_dest.y = cos( count / 100.0f ) * 10.8f;
			m_dest.z = sin( count / 93.0f + cos(count/111.0f) ) * 10.8f;
		}
		count++;
		for ( std::vector<Boido*>::iterator p = m_boid.begin(); p<m_boid.end(); p++ )
		{
			Boido *b = *p;
			b->m_lastPos = b->m_pos;
			Vector3 localCenter = Vector3(0,0,0); //b->m_pos;
			Vector3 localDir = b->m_dir;

			Vector3 d = b->m_pos - m_dest;
			d.normalize();
			b->m_dir = b->m_dir - d * 0.1f;
			b->m_targetDir = d;
			b->state = 0;

			int numLocal = 0;

			for ( std::vector<Boido*>::iterator bo = m_boid.begin(); bo<m_boid.end(); bo++ )
			{
				Boido *nb = *bo;
				if ( nb != b )
				{
					float dis = (nb->m_pos-b->m_pos).length();
					if ( dis < m_minDistance )
					{
//						b->m_dir = b->m_dir - (b->m_pos-nb->m_pos) * 0.4f;
						localCenter = localCenter + nb->m_pos;
						b->state = 1;
						numLocal++;
					}
				}
			}
			if ( numLocal > 0 )
			{
				localCenter = localCenter / (float) numLocal;
				b->m_dir = b->m_dir + (b->m_pos-localCenter);
			}
			b->m_dir.normalize();
			b->m_pos = b->m_pos + b->m_dir * timeStep * 10.0f;
		}
	}

	void	Move2( float timeStep, int montaxon )
	{
		if ( oldies == NULL )
			return;

		for ( int a=0; a<montaxon; a++ )
		{
			int i = (a+montaxon-1)%montaxon;
			
			Boido *b = &(oldies[a]);
			b->m_dir = b->m_dir - b->m_targetDir * 0.1f;
			b->m_pos = b->m_pos + b->m_dir.normalized() * timeStep * 7.0f;
		}
	}

	void renderBOAA( float timeStep );

	void render( float timeStep );



};


#endif
