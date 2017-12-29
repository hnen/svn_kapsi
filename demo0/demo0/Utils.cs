using System;
using System.Collections.Generic;

using OpenTK;

class VecUtl {
    public static float Dot( ref Vector3 a, ref Vector3 b ) {
        return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
    }
}

static class Utils {

    static long startT;

    public static float Min( float a, float b ) {
        return a < b ? a : b;
    }

    public static float Max( float a, float b ) {
        return a > b ? a : b;
    }

    public static float Max( params float [] a ) {
        float x = float.MinValue;
        foreach( float v in a ) {
            if ( v > x ) x = v;
        }
        return x;
    }

    public static T Min<T>( params T [] min ) where T : IComparable<T> {
        T ret = min[0];
        foreach( T v in min ) {
            if ( ret.CompareTo( v ) > 0 ) ret = v;
        }
        return ret;
    }
    public static T Max<T>( params T [] max ) where T : IComparable<T> {
        T ret = max[0];
        foreach( T v in max ) {
            if ( ret.CompareTo( v ) < 0 ) ret = v;
        }
        return ret;
    }

    static Utils() {
        startT = System.DateTime.Now.Ticks / 10000;
    }

    public static float Time {
        get {
            return (float)(System.DateTime.Now.Ticks / 10000 - startT) / 1000.0f;
        }
    }

	public static int Mod( this int i, int m ) {
        return (i % m + (i < 0 ? m : 0)) % m;
    }

    public static float PointLineDist( Vector3 A, Vector3 B, Vector3 P ) {
        Vector3 a = (B-A);
        float l = a.Length;
        a /= l;
        float x = Vector3.Dot( (P-A), a );
        float y = a.LengthSquared;
        if ( x <= 0 ) {
            return (A-P).LengthSquared;
        } else
        if ( x >= l ) {
            return (B-P).LengthSquared;
        } else {
            Vector3 P_ = A + a * x / y;
            return (P-P_).LengthSquared;
        }
    }

    public static T [][][] Alloc3<T>( int size ) {
        T [][][] ret = new T[size][][];
        for ( int z = 0; z < size; z++ ) {
            ret[z] = new T[size][];
            for ( int y = 0; y < size; y++ ) {
                ret[z][y] = new T[size];
            }
        }
        return ret;
    }

}
