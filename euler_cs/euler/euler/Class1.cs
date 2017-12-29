using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace euler
{
	class Class1
	{

		static Random r = new Random();

		static bool toss1() {
			int t0 = r.Next() & 1;
			int t1 = r.Next() & 1;
			return t0 == 1 || t1 == 1;
		}
		static bool toss2() {
			int t0 = r.Next() & 1;
			if ( t0 == 1 ) return true;
			int t1 = r.Next() & 1;
			return t1 == 1;
		}

        static void Main(string[] args) {

			int T = 0;
			int t0 = 0;
			int t1 = 0;
			while(true) {
				if ( toss1() ) t0++;
				if ( toss2() ) t1++;
				T++;
				System.Console.WriteLine( "p0 = " + (float)t0/T + ", p1 = " + (float)t1/T );
			}

		}


	}
}
