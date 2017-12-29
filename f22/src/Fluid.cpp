
#include <Windows.h>

#include "Demo.h"
#include "poopie.h"

using namespace ::Gfx;

/*
Vector2 Demo::computeFluidVector( float * scalarInput, int inputw, int inputh ) {



}
*/


float * Demo::computePerlin2( int w, int h, int cellW, int seed ) {

	int gw = (w+cellW) / cellW; 
	int gh = (h+cellW) / cellW; 

	srand (seed);
	Vector2 * grad = new Vector2[gw*gh];
	for ( int y = 0; y < gh; y++ ) {
		for ( int x = 0; x < gw; x++ ) {
			float a = RND(0,6.28f);
			float i = RND(0,1);
			grad[y*gw+x] = Vector2( sinf(a), cosf(a) ) * i;
		}
	}

	float * ret = new float[w*h];

	for ( int y = 0; y < h; y++ ) {
		for ( int x = 0; x < w; x ++ ) {
			int gx = x/cellW, gy = y/cellW;
			Vector2 g0 = grad[gy*gw+gx];
			Vector2 g1 = grad[gy*gw+gx+gw];
			Vector2 g2 = grad[gy*gw+gx+1];
			Vector2 g3 = grad[gy*gw+gx+gw+1];

			Vector2 v0 = Vector2(x,y) - Vector2(gx*cellW,gy*cellW);
			Vector2 v1 = Vector2(x,y) - Vector2(gx*cellW,(gy+1)*cellW);
			Vector2 v2 = Vector2(x,y) - Vector2((gx+1)*cellW,gy*cellW);
			Vector2 v3 = Vector2(x,y) - Vector2((gx+1)*cellW,(gy+1)*cellW);

			float d0 = g0.dot(v0);
			float d1 = g1.dot(v1);
			float d2 = g2.dot(v2);
			float d3 = g3.dot(v3);

			float fx = (float)(x - gx*cellW) / cellW;
			float fy = (float)(y - gy*cellW) / cellW;

			float f0 = cerp( d0, d1, fy );
			float f1 = cerp( d2, d3, fy );

			ret[y*w+x] = cerp( f0, f1, fx );
		}
	}

	return ret;

}

