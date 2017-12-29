using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace euler
{

    class E120 {

        static void Main(string[] args) {

            Func<int,int,int> CycleLen = ( n, m ) => {
                int r = 0;
                int i = n;
                do {
                    i *= n;
                    i %= m;
                    r++;
                } while( i != n );
                return r;
            };
            CycleLen = CycleLen.Memoize();

            int answ = 0;
            for( int a = 3; a <= 1000; a++ ) {
                int c0 = CycleLen(a-1,a*a);
                int c1 = CycleLen(a+1,a*a);
                
                int c = Modular.lcm( c0, c1 );

                int b0 = a-1;
                int b1 = a+1;
                int max = 0;
                for( int x = 0; x < c; x++ ) {
                    int d = (b0+b1)%(a*a);

                    if( d > max ) {
                        max = d;
                    }

                    b0 *= a-1;
                    b1 *= a+1;
                    b0 %= a*a;
                    b1 %= a*a;
                }
                answ += max;
            }
            System.Console.WriteLine( "" + answ );
            System.Console.ReadLine();
        }


    }

}