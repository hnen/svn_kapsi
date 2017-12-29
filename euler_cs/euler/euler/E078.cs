using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

class E078 {

    const int MAX_COINS = 100;
    static int [,] A = new int[MAX_COINS, MAX_COINS];

    static int p( int x, int y ) {
        if ( x == 1 ) {
            return 1;
        } else {
            int r = 1;
            for ( int x0 = 1; x0 < y; x0++ ) {
                r += p( x0, x-x0 );
            }
            return r;
        }
    }

    public static void Main( string [] args ) {

        
        Console.WriteLine( p(5,5) );
        Console.ReadKey();

    }

}
