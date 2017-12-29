using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace euler
{
    class E077
    {
        const int MAX_VALUE = 71;
        
        static int numPrimes;
        static List<int> primes;

        class Prisum : IComparable<Prisum> {
            int [] f;
            public readonly int p;
            public readonly int Value;
            public Prisum() {
                f = new int[E077.numPrimes];
                Value = 0;
            }
            public Prisum( Prisum t, int p ) {
                this.f = new int[E077.numPrimes];
                t.f.CopyTo( this.f, 0 );
                this.p = p;
                f[p]++;
                Value = Eval();
            }
            int Eval() {
                int ret = 0;
                for( int i = 0; i < f.Length; i++ ) {
                    ret += E077.primes[i] * f[i];
                }
                return ret;            
            }

            public int CompareTo( Prisum a ) {
                return Value.CompareTo( a.Value );
            }
        }

        static void Main(string[] args) {
            
            int MAX_ANSWER = 100000;

            Console.WriteLine( "Generating primes.." );
            int [] find = new int[MAX_ANSWER+1];
            primes = Primes.GenPrimes( MAX_VALUE );
            numPrimes = primes.Count;

            Console.WriteLine( "Running algorithm..(primes=" + primes.Count + ")" );

            Heap<Prisum> h = new Heap<Prisum>( Heap<Prisum>.HeapType.MIN );
            h.Add( new Prisum() );

            int best = 0;

            while(true) {
                Prisum p = h.Pop();
                find[p.Value]++;
                if ( find[p.Value] > best ) {
                    best = find[p.Value];
                    Console.WriteLine( "Best so far " + p.Value + " with " + best + " sums." );
                }
                if ( find[p.Value] > 5000 ) {
                    Console.WriteLine( p.Value );
                    break;
                }
                for( int i = p.p; i < numPrimes; i++ ) {
                    Prisum np = new Prisum( p, i );
                    if ( np.Value <= MAX_ANSWER ) {
                        h.Add( np );
                    }
                }
            }
            Console.ReadKey(true);
        }
    }
}
