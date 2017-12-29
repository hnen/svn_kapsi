
#ifndef __SPACE_FIELD_H
#define __SPACE_FIELD_H

#include "poopie_base.h"

namespace space {

	class ScalarField {
	public:

		ScalarField( int width, int height );

		void setValue( float value, int x, int y );
		float getValue( int x, int y );
		float getValue( const Vector2 & pos );
		Vector2 gradient( const Vector2 & pos );

		static ScalarField * radiusFieldFromDistanceField( ScalarField * distanceField );
		
	private:

		bool isGradientZero( int x, int y ); // true if gradient is zero inside the cell [(x,y);(x+1,y+1)]
		
		int m_width;
		int m_height;
		Array<float> m_data;

	};

}


#endif

