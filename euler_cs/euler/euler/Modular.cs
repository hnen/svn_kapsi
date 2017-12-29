using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace euler
{
    static class Modular
    {

        public static int gcd( int a, int b ) {
            int t;
            while (b != 0) {
                t = b;
                b = a%b;
                a = t;
            }
            return a;
        }

        public static int lcm( int a, int b ) {
            return (a*b) / gcd(a,b);
        }

    }
}
