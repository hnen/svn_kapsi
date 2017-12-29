/*
 * Vector.h
 *
 *  Created on: 16.5.2010
 *      Author: hth
 */

#ifndef VECTOR_H_
#define VECTOR_H_

#include <math.h>
#include "../const.h"

struct Vector3 {
	float x, y, z;

	Vector3() : x(0), y(0), z(0) {}
	Vector3( float x, float y, float z ) : x(x), y(y), z(z) {}

	inline Vector3 operator + ( const Vector3 & a ) const { return Vector3( x + a.x, y + a.y, z + a.z ); }
	inline Vector3 operator - ( const Vector3 & a ) const { return Vector3( x - a.x, y - a.y, z - a.z ); }
	inline Vector3 operator * ( float a ) const { return Vector3( x * a, y * a, z * a ); }
	inline Vector3 operator / ( float a ) const { return Vector3( x / a, y / a, z / a ); }

	inline float dot( const Vector3 & a ) const { return x * a.x + y * a.y + z * a.z; }
	inline Vector3 cross ( const Vector3 & a ) const { return Vector3 ( y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x ); }

	inline float lengthSq() const { return dot(*this); }
	inline float length() { return sqrtf( lengthSq() ); }

};


struct Vector2 {
	float x, y;

	Vector2() : x(0), y(0) {}
	Vector2( float x, float y ) : x(x), y(y) {}

	inline Vector2 operator + ( const Vector2 & a ) const { return Vector2( x + a.x, y + a.y ); }
	inline Vector2 operator - ( const Vector2 & a ) const { return Vector2( x - a.x, y - a.y ); }
	inline Vector2 operator * ( float a ) const { return Vector2( x * a, y * a ); }
	inline Vector2 operator / ( float a ) const { return Vector2( x / a, y / a ); }

	inline float dot( const Vector2 & a ) const { return x * a.x + y * a.y ; }
	inline Vector3 cross ( const Vector2 & a ) const { return Vector3 ( 0, 0, x * a.y - y * a.x ); }

	inline float lengthSq() const { return dot(*this); }
	inline float length() { return sqrtf( lengthSq() ); }

};


#endif /* VECTOR_H_ */
