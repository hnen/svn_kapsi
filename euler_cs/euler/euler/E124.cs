using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace euler
{
    class E124
    {

        static List<int> primes;

        static int rad(int n) {
            List<int> fctrs = new List<int>();
            int i = 0;
            while( n > 1 ) {
                int p = primes[i];
                if ( n % p == 0 ) { 
                    fctrs.Add( p );
                    while(n % p == 0) {
                        n /= p;
                    }
                }
                i++;
            }
            int m = 1;
            foreach( int f in fctrs ) {
                m *= f;
            }
            return m;
        }

        class K : IComparable<K> {
            public int a, b;
            public K(int a, int b) {
                this.a = a;
                this.b = b;
            }
            public int CompareTo( K k ) {
                return b.CompareTo(k.b);
            }
        }

        static void Main( string [] args ) {
            primes = Primes.GenPrimes( 100000 );
            Console.WriteLine( "primes genned" );

            List<K> radfunc = new List<K>();
            for( int i = 1; i <= 100000; i++ ) {
                int r = rad(i);
                //Console.WriteLine( "" + i + " = " + r );
                radfunc.Add( new K( i, rad(i) ) );
            }

            radfunc.Sort();

            System.Console.WriteLine( "" + radfunc[9999].a );
            Console.ReadKey();
        }

    }
}
