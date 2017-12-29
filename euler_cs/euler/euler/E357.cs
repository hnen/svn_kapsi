using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace euler
{
    class E357
    {

        static int N = 100000000;

        public static void Main( string [] args ) {
            
            var primes_list = Primes.GenPrimes( N );

            var primes = new HashSet<int>();
            foreach( int p in primes_list ) {
                primes.Add( p );
            }

            long answ = 0;

            int n = 1;
            while ( 2*(primes_list[n]-2) < N ) {
                int a = 2*(primes_list[n]-2);

                //if ( n%10000 == 0 ) {
                //    Console.WriteLine( "" + a );
                //}

                bool primeGen = true;
                foreach ( var d in Primes.ComputeDivisors( Primes.ComputePrimeFactors( a, primes_list ) ) ) {
                    if ( !primes.Contains( d + a / d ) ) {
                        primeGen = false;
                        break;
                    }
                }
                if ( primeGen ) {
                    Console.Write( " " + a );
                    answ += a;
                }

                n++;
            }

            Console.WriteLine( "!! " + answ );

        }

    }
}
