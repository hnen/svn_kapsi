/*
 * Matrix.h
 *
 *  Created on: 16.5.2010
 *      Author: hth
 */

#ifndef MATRIX_H_
#define MATRIX_H_

#include "poopie/math/Vector.h"

namespace poopie {

struct Matrix2 {

	union {
		struct {
			float _00, _01;
			float _10, _11;
		};
		struct {
			float a, b, c, d;
		};
		float m[4];
	};

	Matrix2() : a(1.0f), b(0.0f), c(0.0f), d(1.0f) {}
	Matrix2( float a, float b, float c, float d ) : a(a), b(b), c(c), d(d) {}

	float det() {
		return a*d - b*c;
	}

	Matrix2 operator / ( float f ) {
		return Matrix2( a/f, b/f, c/f, d/f );
	}

	Matrix2 operator * ( float f ) {
		return Matrix2( a*f, b*f, c*f, d*f );
	}

	Matrix2 operator * ( const Matrix2 & m ) const {
		return Matrix2(
				a * m.a + b * m.c, a * m.b + b * m.d,
				c * m.a + d * m.c, c * m.b + d * m.d );
	}

	Matrix2 inverse() {
		return Matrix2( d, -b, -c, a ) / det();
	}

	Matrix2 operator + ( const Matrix2 & m ) const {
		return Matrix2( a + m.a, b + m.b, c + m.c, d + m.d );
	}

	Matrix2 operator - ( const Matrix2 & m ) const {
		return Matrix2( a - m.a, b - m.b, c - m.c, d - m.d );
	}

	Matrix2 operator - () const {
		return Matrix2( -a, -b, -c, -d );
	}

};

struct Matrix4 {

	union {
		struct {
			float _00, _01, _02, _03;
			float _10, _11, _12, _13;
			float _20, _21, _22, _23;
			float _30, _31, _32, _33;
		};
		struct {
			Vector3 side; float _03;
			Vector3 up;   float _13;
			Vector3 dir;  float _23;
			Vector3 pos;  float _33;
		};
		float m[16];
		float v[4][4];
	};

	Matrix4() :
		side( Vector3(1, 0, 0) ), _03(0.0f),
		up  ( Vector3(0, 1, 0) ), _13(0.0f),
		dir ( Vector3(0, 0, 1) ), _23(0.0f),
		pos ( Vector3(0, 0, 0) ), _33(1.0f)
			{}

	Matrix4( Matrix2 A, Matrix2 B, Matrix2 C, Matrix2 D ) {
		_00 = A.a; _01 = A.b; _02 = B.a; _03 = B.b;
		_10 = A.c; _11 = A.d; _12 = B.c; _13 = B.d;
		_20 = C.a; _21 = C.b; _22 = D.a; _23 = D.b;
		_30 = C.c; _31 = C.d; _32 = D.c; _33 = D.d;
	}

	Matrix4( Vector3 side, Vector3 up, Vector3 dir ) :
		side( side ), _03(0.0f),
		up  ( up ), _13(0.0f),
		dir ( dir ), _23(0.0f),
		pos ( Vector3(0, 0, 0) ), _33(1.0f)
			{}

	Matrix4( Vector3 side, Vector3 up, Vector3 dir, Vector3 pos ) :
		side( side ), _03(0.0f),
		up  ( up ), _13(0.0f),
		dir ( dir ), _23(0.0f),
		pos ( pos ), _33(1.0f)
			{}


	Matrix4 operator * ( float a ) const {
		Matrix4 ret = *this;
		for ( int l = 0; l < 16; l++ )
			ret.m[l] *= a;
		return ret;
	}

	Matrix4 operator / ( float a ) const {
		Matrix4 ret = *this;
		for ( int l = 0; l < 16; l++ )
			ret.m[l] /= a;
		return ret;
	}

	Vector3 preMulVec ( const Vector3 & a, float w = 1.0f ) const {
		float nx = _00 * a.x + _10 * a.y + _20 * a.z + _30 * w;
		float ny = _01 * a.x + _11 * a.y + _21 * a.z + _31 * w;
		float nz = _02 * a.x + _12 * a.y + _22 * a.z + _32 * w;
		float nw = _03 * a.x + _13 * a.y + _23 * a.z + _33 * w;
		//return Vector3( nx / nw, ny / nw, nz / nw );
		return Vector3( nx, ny, nz );
	}

	Vector3 mulVec ( const Vector3 & a, float w = 1.0f ) const {
		float nx = _00 * a.x + _01 * a.y + _02 * a.z + _03 * w;
		float ny = _10 * a.x + _11 * a.y + _12 * a.z + _13 * w;
		float nz = _20 * a.x + _21 * a.y + _22 * a.z + _23 * w;
		float nw = _30 * a.x + _31 * a.y + _32 * a.z + _33 * w;
		//return Vector3( nx / nw, ny / nw, nz / nw );
		return Vector3( nx, ny, nz );
	}


	Matrix4 operator * ( const Matrix4 & a ) {
		Matrix4 ret;
		for( int y = 0; y < 4; y++ ) {
			for ( int x = 0; x < 4; x++ ) {
				ret.m[(y<<2)+x] = 
					a.m[(0<<2)+x] * m[(y<<2)+0] +
					a.m[(1<<2)+x] * m[(y<<2)+1] +
					a.m[(2<<2)+x] * m[(y<<2)+2] +
					a.m[(3<<2)+x] * m[(y<<2)+3];
			}
		}
		return ret;
	}

	Matrix4 transpose() {
		Matrix4 ret;
		for( int i = 0; i < 4; i++ ) {
			for( int j = 0; j < 4; j++ ) {
				ret.m[(i<<2)+j] = m[(j<<2)+i];
			}
		}
		return ret;
	}

	Matrix4 inverse() {
		Matrix2 A( _00, _01, _10, _11 );
		Matrix2 B( _02, _03, _12, _13 );
		Matrix2 C( _20, _21, _30, _31 );
		Matrix2 D( _22, _23, _32, _33 );

		Matrix2 iA = A.inverse();
		Matrix2 Q = (D - C*iA*B).inverse();
		Matrix2 iABQ = iA * B * Q;

		return Matrix4( iA + iABQ*C*iA, -iABQ, -Q*C*iA, Q );
	}
	
	static Matrix4 translate( Vector3 t ) {
		Matrix4 T;
		T._30 = t.x;
		T._31 = t.y;
		T._32 = t.z;
		return T;
	}

	static Matrix4 scale( Vector3 s ) {
		Matrix4 T;
		T._00 = s.x;
		T._11 = s.y;
		T._22 = s.z;
		return T;
	}

	static Matrix4 rotX( float rot ) {
		float s = sin( rot );
		float c = cos( rot );
		Matrix4 R;
		R._11 = c;
		R._12 = s;
		R._21 = -s;
		R._22 = c;
		return R;
	}

	static Matrix4 rotY( float rot ) {
		float s = sin( rot );
		float c = cos( rot );
		Matrix4 R;
		R._00 = c;
		R._02 = -s;
		R._20 = s;
		R._22 = c;
		return R;
	}

	static Matrix4 rotZ( float rot ) {
		float s = sin( rot );
		float c = cos( rot );
		Matrix4 R;
		R._00 = c;
		R._01 = s;
		R._10 =-s;
		R._11 = c;
		return R;
	}

	static Matrix4 rotateAxis( const Vector3 & a, float alpha ) {
		float s = sinf(alpha);
		float c = cosf(alpha);
		float t = 1.0f - c;
		float X = a.x;
		float Y = a.y;
		float Z = a.z;
		Matrix4 A;
		A.side = Vector3( t*X*X + c,   t*X*Y - s*Z, t*X*Z + s*Y );
		A.up   = Vector3( t*X*Y + s*Z, t*Y*Y + c,   t*Y*Z - s*X );
		A.dir  = Vector3( t*X*Z - s*Y, t*Y*Z + s*X, t*Z*Z + c   );
		return A;
	}


	static Matrix4 ortho( float left, float right, float top, float bottom, float fnear, float ffar ) {
		Matrix4 O;
		O._00 = 2.0f / (right - left);
		O._11 = 2.0f / (top - bottom);
		O._22 = 2.0f / (fnear - ffar);
		O._30 = (right + left) / (right - left);
		O._31 = (top + bottom) / (top - bottom);
		O._32 = (ffar + fnear) / (ffar - fnear);
		return O;
	}

	static Matrix4 perspective( float fovy, float aspect, float fnear, float ffar ) {
		float C = cos(fovy);
		float S = sin(fovy);
		
		Matrix4 P;
		P._00 = C / (S * aspect);
		P._11 = C / S;
		P._22 = (fnear + ffar) / (fnear - ffar);
		P._23 = -1.0f;
		P._32 = 2.0f * ffar * fnear / (fnear - ffar);
		P._33 = 0.0f;
		return P;
	}

	static Matrix4 LookAt( const Vector3 & eye, const Vector3 & at, const Vector3 & up ) {
		Vector3 zaxis = (eye-at).normalized();
		Vector3 xaxis = up.cross(zaxis).normalized();
		Vector3 yaxis = zaxis.cross(xaxis);
		Matrix4 R;
		R.side = Vector3(xaxis.x, yaxis.x, zaxis.x); 
		R.up   = Vector3(xaxis.y, yaxis.y, zaxis.y); 
		R.dir  = Vector3(xaxis.z, yaxis.z, zaxis.z); 
		R.pos  = Vector3( -xaxis.dot(eye), -yaxis.dot(eye), -zaxis.dot(eye) );
		return R;
	}

	static Matrix4 identity() {
		Matrix4 m;
		return m;
	}

};

}
#endif /* MATRIX_H_ */
