
#include "TerrainGen.h"


namespace space {


	Array<char> iterateAutomaton( Array<char> & input, int w, int h, int B, int S ) {
		Array<char> ret(w*h);
		for ( int i = 0; i < w*h; i++ ) {
			int l = 0;
			
			int x = i%w;
			int y = i/w;

			bool n0 = (x == 0 || y == 0) || input[i-w-1];
			bool n1 = (y == 0) || input[i-w];
			bool n2 = (x == w-1 || y == 0) || input[i-w+1];
			bool n3 = (x == 0) || input[i-1];
			bool n4 = (x == w-1) || input[i+1];
			bool n5 = (y == h-1 || x == 0) || input[i+w-1];
			bool n6 = (y == h-1) || input[i+w];
			bool n7 = (y == h-1 || x == w-1) || input[i+w+1];

			if ( n0 ) l++;
			if ( n1 ) l++;
			if ( n2 ) l++;
			if ( n3 ) l++;
			if ( n4 ) l++;
			if ( n5 ) l++;
			if ( n6 ) l++;
			if ( n7 ) l++;

			int rule;
			if ( input[i] ) {
				rule = S;
			}  else {
				rule = B;
			}

			bool survive = false;
			while ( rule ) {
				int s = rule%10;
				if ( s == l ) {
					survive = true;
					break;
				}
				rule/=10;
			}
			ret[i] = survive ? 1 : 0;

		}
		return ret;
	}


	Array<char> TerrainGen::generate( int owidth, int oheight, int subdivs, int iters0 ) {

		Array<char> i0(owidth*oheight);


		for( int i = 0; i < owidth*oheight; i++ ) {
			i0[i] = rand()&1;
		}

		for ( int i = 0; i < iters0; i++ ) {
			i0 = iterateAutomaton( i0, owidth, oheight, 678, 345678 );
		}

		Array<char> i1 = i0;
		int nwidth = owidth;
		int nheight = oheight;
		do {
			i0 = iterateAutomaton( i1, nwidth, nheight, 5678, 5678 );
			subdivs--;

			if ( subdivs >= 0 ) {
				nwidth *= 2;
				nheight *= 2;
				i1 = Array<char>( nwidth * nheight );
				for( int i = 0; i < nwidth*nheight; i++ ) {
					int x = (i%nwidth) / 2;
					int y = i/(2*nwidth);
					i1[i] = i0[y*(nwidth/2)+x];
				}
			}

		} while( subdivs >= 0 );


		return i0;
	}

}

