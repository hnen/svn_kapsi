
#include "ScalarField.h"

namespace space {

	ScalarField::ScalarField( int width, int height ) : m_data( width*height ), m_width(width), m_height(height) {
	}

	void ScalarField::setValue( float value, int x, int y ) {
		m_data[y*m_width+x] = value;
	}

	float ScalarField::getValue( int x, int y ) {
		x = MAX(x,0);
		y = MAX(y,0);
		x = MIN(x,m_width-1);
		y = MIN(y,m_height-1);
		return m_data[y*m_width+x];
	}

	float ScalarField::getValue( const Vector2 & pos ) {
		float fx = fmodf(pos.x,1.0f);
		float fy = fmodf(pos.y,1.0f);
		int ix = (int)pos.x;
		int iy = (int)pos.y;
		
		float v0 = getValue(ix,iy) * (1-fx) + getValue(ix+1,iy) * fx;
		float v1 = getValue(ix,iy+1) * (1-fx) + getValue(ix+1,iy+1) * fx;

		return v0 * (1-fy) + v1 * fy;
	}

	Vector2 ScalarField::gradient( const Vector2 & pos ) {

		float fx = fmodf(pos.x,1);
		float fy = fmodf(pos.y,1);
		int ix = (int)pos.x;
		int iy = (int)pos.y;

		float P0 = getValue(ix,iy);
		float P1 = getValue(ix+1,iy);
		float P2 = getValue(ix,iy+1);
		float P3 = getValue(ix+1,iy+1);

		return Vector2( P1-P0 + (P0-P1-P2+P3)*fy, P2-P0 + (P0-P1-P2+P3)*fx );
	}

	/*
	bool ScalarField::isGradientZero( int ix, int iy ) {
		float P0 = getValue(ix,iy);
		float P1 = getValue(ix+1,iy);
		float P2 = getValue(ix,iy+1);
		float P3 = getValue(ix+1,iy+1);

		//y=0: x = (P1-P2)/(P3-P2)
		//x=0: y = (P2-P1)/(P3-P1)
		//y=1: x = (P0-P2)/(P0-P1)
		//x=1: y = (P0-P1)/(P0-P2)

		// (A-B)/(C-B)
		// P3 >= P1 >= P2 || P3 <= P1 <= P2 || P3 >= P2 >= P1 || P3 <= P2 <= P1

		return ( P1 >= P2 && P1 <= P3 ) || ( P1 <= P2 && P1 >= P3 ) || ( P2 >= P1 && P2 <= P3 ) || ( P2 <= P1 && P2 >= P3 ) ||

	}
	*/

	ScalarField * ScalarField::radiusFieldFromDistanceField( ScalarField * distanceField ) {
		ScalarField * nscalar = new ScalarField( distanceField->m_width, distanceField->m_height );

		return nscalar;
	}


}

