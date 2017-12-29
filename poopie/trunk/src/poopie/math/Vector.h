/*
 * Vector.h
 *
 *  Created on: 16.5.2010
 *      Author: hth
 */

#ifndef VECTOR_H_
#define VECTOR_H_

#include <math.h>

namespace poopie {

struct Vector4 {
	float x, y, z, w;

	Vector4() : x(0), y(0), z(0), w(w) {}
	Vector4( float x, float y, float z, float w ) : x(x), y(y), z(z), w(w) {}

	inline Vector4 operator + ( const Vector4 & a ) const { return Vector4( x + a.x, y + a.y, z + a.z, w + a.w ); }
	inline Vector4 operator - ( const Vector4 & a ) const { return Vector4( x - a.x, y - a.y, z - a.z, w - a.w ); }
	inline Vector4 operator * ( float a ) const { return Vector4( x * a, y * a, z * a, w * a ); }
	inline Vector4 operator / ( float a ) const { return Vector4( x / a, y / a, z / a, w / a ); }

	inline float dot( const Vector4 & a ) const { return x * a.x + y * a.y + z * a.z + w * a.w; }

};


struct Vector3 {
	float x, y, z;

	Vector3() : x(0), y(0), z(0) {}
	Vector3( float x, float y, float z ) : x(x), y(y), z(z) {}
	
	inline static Vector3 one() { return Vector3(1,1,1); }
	inline static Vector3 zero() { return Vector3(0,0,0); }
	inline Vector4 xyz0() { return Vector4(x,y,z,0); }
	inline Vector4 xyz1() { return Vector4(x,y,z,1); }

	inline Vector3 operator + ( const Vector3 & a ) const { return Vector3( x + a.x, y + a.y, z + a.z ); }
	inline Vector3 operator - ( const Vector3 & a ) const { return Vector3( x - a.x, y - a.y, z - a.z ); }
	inline Vector3 operator - () const { return Vector3( -x, -y, -z ); }
	inline Vector3 operator * ( float a ) const { return Vector3( x * a, y * a, z * a ); }
	inline Vector3 operator / ( float a ) const { return Vector3( x / a, y / a, z / a ); }

	inline Vector3 & operator += ( const Vector3 & a ) { x += a.x; y += a.y; z += a.z; return *this; }
	inline Vector3 & operator *= ( const float a ) { x *= a; y *= a; z *= a; return *this; }

	inline Vector3 normalized() const { float l = length(); return l == 0 ? Vector3(0,0,0) : Vector3(x,y,z) / length(); }
	inline void normalize() { float l = length(); if(l==0)return; x/=l; y/=l; z/=l; }

	inline float dot( const Vector3 & a ) const { return x * a.x + y * a.y + z * a.z; }
	inline Vector3 cross ( const Vector3 & a ) const { return Vector3 ( y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x ); }

	inline float lengthSq() const { return dot(*this); }
	inline float length() const { return sqrtf( lengthSq() ); }

};


struct Vector2 {
	float x, y;

	Vector2() : x(0), y(0) {}
	Vector2( float x, float y ) : x(x), y(y) {}

	inline bool operator == ( const Vector2 & a ) const { return x == a.x && y == a.y;  }

	inline Vector2 operator + ( const Vector2 & a ) const { return Vector2( x + a.x, y + a.y ); }
	inline Vector2 operator - ( const Vector2 & a ) const { return Vector2( x - a.x, y - a.y ); }
	inline Vector2 operator * ( float a ) const { return Vector2( x * a, y * a ); }
	inline Vector2 operator / ( float a ) const { return Vector2( x / a, y / a ); }

	inline float dot( const Vector2 & a ) const { return x * a.x + y * a.y ; }
	inline Vector3 cross ( const Vector2 & a ) const { return Vector3 ( 0, 0, x * a.y - y * a.x ); }

	inline float lengthSq() const { return dot(*this); }
	inline float length() { return sqrtf( lengthSq() ); }

	inline Vector2 rotate( float angle ) { return Vector2( x * cosf(angle) - y * sinf(angle), x * sin(angle) + y * cos(angle) ); }

	inline Vector2 normalized() { return (*this) / length(); }

	Vector3 x0y() { return Vector3( x,0,y ); }
	Vector3 xy0() { return Vector3( x,y,0 ); }

};


struct IntVec2 {
	int x, y;

	IntVec2() : x(0), y(0) {}
	IntVec2( int x, int y ) : x(x), y(y) {}

	bool operator==( const IntVec2 & a ) const { return x == a.x && y == a.y; }

};


}

#endif /* VECTOR_H_ */
