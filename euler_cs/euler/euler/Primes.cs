using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace euler
{
    class Primes
    {

        static long computed = 0;
        private static void ComputeTotients( int N, int a, long val, int tot, int [] dst, int [] histo, List<int> primes ) {
            if ( val > N ) {
                return;
            }

            computed++;
            if ( (computed%100000) == 0 ) {
                Console.WriteLine( string.Format( "{0}%", (double)(computed * 100)/N ) );
            }

            dst[val] = tot;

            for ( int i = a; i < histo.Length; i++ ) {
                if ( val * primes[i] > N ) break;
                histo[i]++;
                if ( histo[i] == 1 ) {
                    ComputeTotients( N, i, val * primes[i], tot *(primes[i]-1), dst, histo, primes );
                } else {
                    ComputeTotients( N, i, val * primes[i], tot * primes[i], dst, histo, primes );
                }
                histo[i]--;
            }
        }

        public static int [] ComputeDivisors( Tuple<int,int> [] primeFactors ) {
            return Divisors( primeFactors ).ToArray();
        }

        private static IEnumerable<IEnumerable<T>> GetPowerSet<T>(List<T> list)
        {
            return from m in Enumerable.Range(0, 1 << list.Count)
                   select
                       from i in Enumerable.Range(0, list.Count)
                       where (m & (1 << i)) != 0
                       select list[i];
        }

        private static IEnumerable<int> Divisors( Tuple<int,int> [] primeFactors ) {
            List<int> all_primes = new List<int>();
            int n = 0;
            foreach ( var tpl in primeFactors ) {
                for ( int i = 0; i < tpl.Item2; i++ ) {
                    all_primes.Add( tpl.Item1 );
                    n++;
                }
            }
            var power_set_primes = GetPowerSet(all_primes);

            foreach (var p in power_set_primes) {
                var factor = p.Aggregate(1, (x, y) => x * y);
                yield return factor;
            }
        }

        public static Tuple<int, int> [] ComputePrimeFactors( int n, List<int> primes ) {
            List<Tuple<int,int>> ret = new List<Tuple<int,int>>();
            int p = 0;
            while ( n > 1 ) {
                if ( n%primes[p] == 0 ) {
                    int q = 0;
                    while ( n%primes[p] == 0 ) {
                        n /= primes[p];
                        q++;
                    }
                    ret.Add( Tuple.Create( primes[p], q ) );
                }
                p++;
            }
            return ret.ToArray();
        }

        public static int [] ComputeTotients( int N, List<int> primes ) {
            int [] ret = new int[N+1];
            int [] histo = new int[primes.Count];
            
            ComputeTotients( N, 0, 1, 1, ret, histo, primes );

            return ret;
        }

        public static int Totient( int n, List<int> primes ) {
            if ( n < 2 ) return 0;
            if ( n < 3 ) return 1;
            if ( n < 4 ) return 2;

            int r = n;
            int d = 0;
            while ( primes[d] <= r ) {
                if ( n % primes[d] == 0 ) {
                    n -= n/primes[d];
                }
                d++;
            }
            return n;
        }

        public static bool [] GenSieve( int maxPrime ) {
            bool [] isComposite = new bool[maxPrime+1];
            isComposite[0] = true;
            isComposite[1] = true;
            isComposite[2] = false;
            int n = 2;
            do {
                for( int i = n*2; i <= maxPrime; i+=n ) {
                    isComposite[i] = true;
                }
                do {
                    n++;
                } while( n <= maxPrime && isComposite[n] );
            } while( n <= maxPrime );

            return isComposite;
        }

        public static List<int> GenPrimes( int maxPrime ) {
            bool [] sieve = GenSieve( maxPrime );
            List<int> ret = new List<int>();
            for( int i = 0; i < sieve.Length; i++ ) {
                if ( !sieve[i] ) {
                    ret.Add( i );
                }
            }
            return ret;
        }

        private static bool MR( int a, long n, long d, long s ) {
            long mod = (int)Arithm.PowMod(a, d, n);
            if ( mod == 1 || mod == n-1 ) {
                return true;
            }
            int r = 0;
            long e = 2;
            while( r < s ) {
                if ( Arithm.PowMod( a, e*d, n ) == n-1 ) {
                    return true;
                }
                e<<=1;
                r++;
            }
            return false;
        }

        public static bool IsPrimeSl(long n) {
            if ( n < 2 ) return false;
	        if(n != 2 && n % 2 == 0) {
	            return false;
            }
            
            for ( int m = 3; m*m <= n; m++ ) {
                if ( n%m == 0 ) {
                    return false;
                }
            }
            return true;
        }

        public static bool IsPrime(long n) {
	        if(n < 2) {
	            return false;
            }
	        if(n != 2 && n % 2 == 0) {
	            return false;
            }
            if ( n == 3 || n == 2 ) {
                return true;
            }

            int numexp = 0;
	        long s = n - 1;
	        while(s % 2 == 0) {
	            s >>= 1;
                numexp++;
            }
            System.Diagnostics.Debug.Assert( (1<<numexp) * s == n-1 );
            //915743663

            if ( n < 1373653 ) {
                return MR( 2, n, s, numexp ) && MR( 3, n, s, numexp );
            } else if ( n < 9080191 ) {                            
                return MR( 31, n, s, numexp ) && MR( 73, n, s, numexp );
            } else if ( n < 4759123141 ) {
                return MR( 2, n, s, numexp ) && MR( 7, n, s, numexp ) && MR( 61, n, s, numexp );
            } else if ( n < 2152302898747 ) {
                return MR( 2, n, s, numexp ) && MR( 3, n, s, numexp ) && MR( 5, n, s, numexp ) && MR( 7, n, s, numexp ) && MR( 11, n, s, numexp );
            } else if ( n < 3474749660383 ) {
                return MR( 2, n, s, numexp ) && MR( 3, n, s, numexp ) && MR( 5, n, s, numexp ) && MR( 7, n, s, numexp ) && MR( 11, n, s, numexp ) && MR( 13, n, s, numexp );
            } else if ( n < 341550071728321 ) {
                return MR( 2, n, s, numexp ) && MR( 3, n, s, numexp ) && MR( 5, n, s, numexp ) && MR( 7, n, s, numexp ) && MR( 11, n, s, numexp ) && MR( 13, n, s, numexp ) && MR( 17, n, s, numexp );
            } else {
                System.Diagnostics.Debug.Assert( false );
            }
            return false;
        }

    }
}
