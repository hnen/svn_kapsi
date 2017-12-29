using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace euler
{
    class E351 {

        public static readonly int N = 100000000;

        static void Main ( string[] args ) {

            Console.WriteLine( "Computing primes..." );

            var primes = Primes.GenPrimes( N+1 );

            Console.WriteLine( "Number of primes " + primes.Count() + "\nLast prime " + primes[primes.Count()-1] + "\nComputing result..." );

            var tots = Primes.ComputeTotients( N, primes );
            long answ = 0;
            for( int i = 2; i <= N; i++ ) {
                answ += i - tots[i];
            }
            answ *= 6;
            Console.WriteLine( "Answer: " + answ );

            Console.ReadKey();
        }
    }
}
