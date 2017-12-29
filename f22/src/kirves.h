
#include "poopie.h"

#ifndef _KIRVES
#define _KIRVES


class Kirves
{
public:
	Kirves();

	~Kirves()
	{
		for ( int a=0; a<items->getLength(); a++ )
			delete items->getData()[a];
		delete items;
	}


	void render( float t, float dt );
	void renderWing( Matrix4&mat,  float t );
	void renderV( float t );

private:
	class Item
	{
	public:
		float posz;
		float size;
		Vector3 pos;
		const static int num = 10;
		Item( Vector3 pos_, float size_ ) : maxSize(size_), pos(pos_), t(0), size(0)
		{
			started = false;
			Vector3* vec = new Vector3[num];
			vec[0] = Vector3(0,0,0);
			for ( int a=1; a<num-2; a++ ) 
				vec[a] = rndv();
			vec[num-1] = Vector3(0,0,0);
			sp = new poopie::Spline( vec, num );
		};
		static const int lifetime = 500;
		~Item()
		{
			delete sp;
		}
		void start()
		{
			t = 0;
			size = 0;
			posz = 200;
			started = true;
		}
		bool running()
		{
			return started && t <= (lifetime/1000.0f);
		}
		bool end()
		{
			return started && t>(lifetime/1000.0f);
		}
		Matrix4 get(float delta)
		{
			if ( !running() ) return Matrix4::identity();
			
			float f = sinf(t*PI/(float)(lifetime/1000.0f));
			size = f;// * maxSize;
			posz = 50.0f - 100.0f * (t/(lifetime/1000.0f));

			Vector3 e = sp->getPoint(f);
			Vector3 offs = Vector3(0,0,0);
			Matrix4 m = Matrix4::LookAt( e+offs, offs, Vector3(0,1,0) );
//			m = m * Matrix4::scale(Vector3(size,size,size));s
			t+=delta;
			return m;
		}
	private:
		bool started;
		float maxSize;
		float t;
		poopie::Spline *sp;
		float rndf()
		{
			return RND(0,1.0f);
		}
		Vector3& rndv()
		{
			return Vector3(rndf(),rndf(),rndf());
		}
	};

	poopie::DynamicArray<Item*> *items;

	float sync_fftglitch(int a);

};


#endif

