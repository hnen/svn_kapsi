
#ifndef __DEMO
#define __DEMO

#include "DemoResources.h"
#include "kirves.h"
#include "boido.h"

class Paska {
public:

	struct Edge {
		Edge() {}

		Edge( int i0, int i1 ) {
			if( i0 > i1 ) {
				this->i0 = i1;
				this->i1 = i0;
			} else {
				this->i0 = i0;
				this->i1 = i1;
			}
		}

		int other( int i ) {
			if ( i == i0 ) {
				return i1;
			} else {
				return i0;
			}
		}

		bool operator == ( const Paska::Edge & e ) const {
			return i0 == e.i0 && i1 == e.i1;
		}

		int i0, i1;
	};

	void init( MeshData * data );
	void render( Matrix4 w, float t, float R, float fukkaus, float paska );

	MeshData * md;
	Array<Edge> edket;

	Array<DynamicArray<Paska::Edge>> konnekts;


};

class Demo {
public:

	Demo( Renderer * renderer ) : m_renderer(renderer) 
	{
		lastt = 0;
	}

	void init();
	void render();

	void setTime( unsigned int time );
	unsigned int time();
	float timef();

	void drawMesh( Mesh * m, Matrix4 & w );

	static Demo & get( Renderer * renderer = NULL ) {
		static Demo instance(renderer);
		return instance;
	}

	Renderer * getRenderer() { return m_renderer; };

	static void renderHandTree( Matrix4 m, int n, float a, float, float, float, float spread = 1.0f  );

	static void drawOverlay667( float t );
	static void drawOverlay666( float t );
	static void drawOverlay( float t );
	static void drawOverlay2( float t );
	static void renderBack( Spline spline, int count );

	float EXPLO;

	float * computePerlin2( int w, int h, int cellW, int seed );
	Vector2 * computeFluidField( float * scalarInput, int w, int h );

	Vector3 camPos, camView;

	float overlay;
	int kuva;

private:
	static Kirves*	m_kirves;
	static Swarmo* m_boido;
	static Swarmo* m_ranka;

	Paska icopaska;
	Paska icoqllo;
	Paska icof;

	Matrix4 v, p, vp;

	unsigned int m_startTime;
	float lastt;

	Renderer * m_renderer;

};

#include "Gfx.h"

#endif

