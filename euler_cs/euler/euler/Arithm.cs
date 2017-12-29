using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace euler
{
    class Arithm
    {

        public static long Pow(int x, short power) {
            if (power == 0) return 1;
            if (power == 1) return x;
            // ----------------------
            int n = 15;
            while ((power <<= 1) >= 0) n--;

            long tmp = x;
            while (--n > 0)
                tmp = tmp * tmp * 
                        (((power <<= 1) < 0)? x : 1);
            return tmp;
        }

        public static long PowMod(long x, long exp, long mod) {
            if (exp == 0) return 1%mod;
            if (exp == 1) return x%mod;
            // ----------------------
            long res = 1;
            while ( exp > 0 ) {
                if ( exp%2 == 1 ) {
                    res = (res * x) % mod;
                }
                exp = exp >> 1;
                x = (x*x) % mod;
            }
            return res;
        }
    }
}
