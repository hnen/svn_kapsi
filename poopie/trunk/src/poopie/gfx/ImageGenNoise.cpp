
#include "ImageData.h"
#include "poopie/math/Vector.h"
#include "poopie/math/Spline.h"

namespace poopie {
namespace gfx {


	ImageData * ImageData::genNoise( int width, int height ) {

		unsigned char * fdata = new unsigned char[width*height*4];
		for ( int n = 0; n < width*height*4; n++ ) {
			fdata[n] = rand()&0xff;
		}

		return new ImageData( width, height, fdata, IMG_FMT_RGBA_UBYTE );
	}

	ImageData * ImageData::genNoise32( int width, int height ) {

		float * fdata = new float[width*height];
		for ( int n = 0; n < width*height; n++ ) {
			fdata[n] = (float)rand()/(float)RAND_MAX;
		}

		return new ImageData( width, height, fdata, IMG_FMT_R_FLOAT );
	}

	ImageData * ImageData::genPerlin( int w, int h, int cellW, int seed ) {
		int gw = (w) / cellW; 
		int gh = (h) / cellW; 

		srand (seed);
		Vector2 * grad = new Vector2[gw*gh];
		for ( int y = 0; y < gh; y++ ) {
			for ( int x = 0; x < gw; x++ ) {
				Vector2 r;
				do {
					r.x = RND(-1,1);
					r.y = RND(-1,1);
				} while( r.lengthSq() > 1.0f );
				grad[y*gw+x] = r;
				//float a = RND(0,6.28f);
				//float i = RND(0,1);
				//grad[y*gw+x] = Vector2( sinf(a), cosf(a) ) * i;
			}
		}

		float * ret = new float[w*h];

		for ( int y = 0; y < h; y++ ) {
			for ( int x = 0; x < w; x ++ ) {

				int X = x;
				int Y = y;

				int gx = (X/cellW), gy = (Y/cellW);
				int igx0 = gx%(gw), igy0 = gy%(gh);
				int igx1 = (gx+1)%(gw), igy1 = (gy+1)%(gh);

				Vector2 g0 = grad[igy0*gw+igx0];
				Vector2 g1 = grad[igy1*gw+igx0];
				Vector2 g2 = grad[igy0*gw+igx1];
				Vector2 g3 = grad[igy1*gw+igx1];

				Vector2 v0 = Vector2(X,Y) - Vector2(gx*cellW,gy*cellW);
				Vector2 v1 = Vector2(X,Y) - Vector2(gx*cellW,(gy+1)*cellW);
				Vector2 v2 = Vector2(X,Y) - Vector2((gx+1)*cellW,gy*cellW);
				Vector2 v3 = Vector2(X,Y) - Vector2((gx+1)*cellW,(gy+1)*cellW);

				v0 = v0 * (2.0f / (float)cellW);
				v1 = v1 * (2.0f / (float)cellW);
				v2 = v2 * (2.0f / (float)cellW);
				v3 = v3 * (2.0f / (float)cellW);

				float d0 = g0.dot(v0) * 0.71f + 0.5f;
				float d1 = g1.dot(v1) * 0.71f + 0.5f;
				float d2 = g2.dot(v2) * 0.71f + 0.5f;
				float d3 = g3.dot(v3) * 0.71f + 0.5f;

				float fx = (float)(X - gx*cellW) / (cellW-1);
				float fy = (float)(Y - gy*cellW) / (cellW-1);

				float f0 = cerp( d0, d1, fy );
				float f1 = cerp( d2, d3, fy );

				ret[y*w+x] = cerp( f0, f1, fx );
			}
		}
		return new ImageData( w, h, ret, IMG_FMT_R_FLOAT );
	}



}
}