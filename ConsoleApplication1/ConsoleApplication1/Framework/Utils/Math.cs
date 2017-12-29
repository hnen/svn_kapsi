using System;
using System.Collections.Generic;

using SharpDX;

public static class MathExt {

    public static Vector3 Normalized( this Vector3 v ) {
        Vector3 r = v;
        r.Normalize();
        return r;
    }

    public static float NextFloat( this Random r ) {
        return ((float)r.Next( 100000 ) / 100000.0f);
    }
    public static float NextFloat( this Random r, float a, float b ) {
        return a + r.NextFloat() * (b-a);
    }

    public static float Mod( this float f, float m ) {
        int n = (int)(f / m);
        float r = f - n*m;
        return r < 0 ? r + m : r;
    }

    public static float Min( params float[] p ) {
        float best = float.MaxValue;
        foreach ( float n in p ) {
            if ( n < best ) {
                best = n;
            }
        }
        return best;
    }
    public static float Max( params float[] p ) {
        float best = float.MinValue;
        foreach ( float n in p ) {
            if ( n > best ) {
                best = n;
            }
        }
        return best;
    }

    public static float Lerp( float a, float b, float t, float t0 ) {
        return a + (b - a) * t / t0;
    }

    public static float Clamp( this float p, float a, float b ) {
        return p < a ? a : (p > b ? b : p);
    }

    public static int Mod( this int i, int m ) {
        return (i % m + (i < 0 ? m : 0)) % m;
    }

}
