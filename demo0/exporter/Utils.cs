using System;
using System.Collections.Generic;

using OpenTK;

class VecUtl {
    public static float Dot( ref Vector3 a, ref Vector3 b ) {
        return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
    }
}

class Utils {

    static long startT;

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

}
