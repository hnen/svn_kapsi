using System;
using System.Diagnostics;

public class AssertionFailureException : Exception {
}

public class Dbg {

    [Conditional("DEBUG")]
    public static void Assert( bool condition ) {
        if ( !condition ) {
            throw new AssertionFailureException();
        }
    }

}
