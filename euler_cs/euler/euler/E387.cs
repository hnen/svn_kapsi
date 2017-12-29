using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace euler
{
    class E387
    {

        static long S( long N ) {
            long ret = 0;
            while ( N > 0 ) {
                ret += N%10;
                N/=10;
            }
            return ret;
        }

        //static bool [] IsPrime = Primes.GenSieve( 4759123141*2 );

        static IEnumerable<long> Primeable( long N ) {
            long [] ns = { 1, 3, 7, 9 };
            foreach ( long n in ns ) {
                long P = N*10 + n;
                if ( Primes.IsPrime(P) ) {
                //if ( !IsPrime[P] ) {
                    Console.WriteLine( P );
                    yield return P;
                }
            }
            //return 0;
        }

        static long Search( long N ) {
            long ret = 0;
            for ( long i = 0; i < 10; i++ ) {
                long N0 = N*10 + i;
                if ( N0 == 0 || N0 > 10000000000000 ) continue;
                long s = S(N0);
                if ( N0%s == 0 ) {
                    if ( Primes.IsPrime( N0/s ) )
                    {
                    //if ( !IsPrime[ N0/s ] ) {
                        //Console.WriteLine( N0/s );
                        foreach ( long P in Primeable( N0 ) )
                        {
                            ret += N0;
                        }
                    }
                    ret += Search( N0 );
                }
            }
            return ret;
        }

        public static void Main( string [] args ) {

            /*
            for( long i = 4759123141; i < 4759123141*2; i++ ) {
                //if ( !IsPrime[i] != Primes.IsPrime(i) ) {
                if ( Primes.IsPrimeSl(i) != Primes.IsPrime(i) ) {
                    Console.WriteLine( "MISCMATCH! " + i );
                }
            }
            */

            Console.WriteLine( Search( 0 ) );
            
        }

    }
}
