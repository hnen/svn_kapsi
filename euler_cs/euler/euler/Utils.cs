using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace euler
{
    static class Utils
    {

        public static Func<A, R> Memoize<A,R>( this Func<A,R> f ) {
            var mem = new Dictionary<A, R>();
            return a => {
                R val;
                if ( mem.TryGetValue( a, out val ) ) {
                    return val;
                } else {
                    val = f(a);
                    mem.Add( a, val );
                    return val;
                }
            };
        }

        public static Func<A,B, R> Memoize<A,B,R>( this Func<A,B,R> f ) {
            var mem = new Dictionary<Tuple<A, B>, R>();
            return (a,b) => {
                R val;
                if ( mem.TryGetValue( new Tuple<A,B>(a,b), out val ) ) {
                    return val;
                } else {
                    val = f(a,b);
                    mem.Add( new Tuple<A,B>(a,b), val );
                    return val;
                }
            };
        }

    }
}
