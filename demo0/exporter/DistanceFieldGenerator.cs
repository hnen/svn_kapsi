using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Diagnostics;
using System.Threading;

using OpenTK;

namespace demo0 {


    class DistanceFieldGenerator {

		public int size;
		public  Vector3 [] positions;
		public  int [] indices;

		public Vector3 bound_min, bound_max;
        public float bound_max_dim;

		public DistanceFieldGenerator( Vector3[] positions, int[] indices, int size ) {
			this.size = size;
			this.positions = positions;
			this.indices = indices;
            IVec3.dfg = this;
			Generate();
		}

		public DistanceFieldGenerator( MeshImporter.Mesh mesh, int size ) {
			this.size = size;
			this.positions = mesh.verts.Select( ( a, _ ) => a.pos ).ToArray();
			this.indices = mesh.inds;
            IVec3.dfg = this;
			Generate();
        }

        struct IVec3 {

			public static DistanceFieldGenerator dfg;

            public readonly int x,y,z;

			//public IVec3( ) { }
			public IVec3( int x, int y, int z ) {
				this.x = x;
				this.y = y;
				this.z = z;
			}

            public static IVec3 operator + ( IVec3 a, IVec3 b ) {
                return new IVec3( a.x + b.x, a.y + b.y, a.z + b.z );
            }

            public Vector3 fpos {
                get {
                    return dfg.bound_max_dim * (new Vector3( x, y, z ) / (dfg.size-1)) + dfg.bound_min;
                }
            }
        }

		private IVec3 ToIVec( Vector3 pos ) {
			Vector3 ivec = ((pos - bound_min) / bound_max_dim) * (size-1);
			return new IVec3( (int)ivec.X, (int)ivec.Y, (int)ivec.Z );
		}

        class VertW<T> /* : IComparable<VertW> */ {
            public IVec3 pos;
            public T src;

            //public VertW() {} 
            public VertW( IVec3 pos, T src ) {
                this.pos = pos; this.src = src;
            }

            //public float Value {
            //    get {
            //        return (pos.fpos-src).Length;
            //    }
            //}

            //public int CompareTo( VertW a ) {
            //    return Value.CompareTo( a.Value );
            //}
        }

        T [][][] Alloc3<T>( int size ) {
            T [][][] ret = new T[size][][];
            for ( int z = 0; z < size; z++ ) {
                ret[z] = new T[size][];
                for ( int y = 0; y < size; y++ ) {
                    ret[z][y] = new T[size];
                }
            }
            return ret;
        }

		private void ComputeBounds() {

			Vector3 bmin = new Vector3( float.MaxValue, float.MaxValue, float.MaxValue );
			Vector3 bmax = new Vector3( float.MinValue, float.MinValue, float.MinValue );
			foreach ( Vector3 p in positions ) {
				bmin.X = Math.Min( bmin.X, p.X );
				bmin.Y = Math.Min( bmin.Y, p.Y );
				bmin.Z = Math.Min( bmin.Z, p.Z );
				bmax.X = Math.Max( bmax.X, p.X );
				bmax.Y = Math.Max( bmax.Y, p.Y );
				bmax.Z = Math.Max( bmax.Z, p.Z );
			}

			bound_min = bmin;
			bound_max = bmax;
		}

        public float [][][] Field;
        //public float [,,] Field;

        private void IterateElements<T>( float [][][] field, float [][][] prevfield,
                IEnumerable<T> inputValues, 
                Func<VertW<T>, float > computeValue,
                Func<T, Vector3> computeMidPoint,
                Func<VertW<T>, bool> additionalConstraint
                 ) {
            int tsize = field.Length;
            this.size = tsize;
            
            float maxerr = bound_max_dim * (1.733f / 2.0f) / tsize;
            //float maxerr = 0.0f;

            int vertN = 0;
            bool [][][] visited = Alloc3<bool>(tsize);

            foreach( T p in inputValues ) {
                //Console.WriteLine( string.Format( "{0}% ready..", ((float)vertN / positions.Length) * 100.0f ) );
                vertN++;

                Queue<VertW<T>> q = new Queue<VertW<T>>();

                VertW<T> v = new VertW<T>( this.ToIVec( computeMidPoint( p ) ), p );
	            q.Enqueue( v );

                //bool [,,] visited = new bool[size,size,size];
                //bool [][][] visited = Alloc3<bool>(size);
                for( int z = 0; z < tsize; z++ ) for( int y = 0; y < tsize; y++ ) for( int x = 0; x < tsize; x++ ) visited[z][y][x] = false;

                int cellsVisited = 0;

                while ( q.Count > 0 ) {
                    VertW<T> vw = q.Dequeue();

                    if ( visited[vw.pos.z][vw.pos.y][vw.pos.x] || !additionalConstraint( vw ) ) {
                        continue;
                    }

                    visited[vw.pos.z][vw.pos.y][vw.pos.x] = true;
                    float val = computeValue( vw );
                    if ( ( prevfield == null || val - 2.0f * maxerr <= prevfield[vw.pos.z/2][vw.pos.y/2][vw.pos.x/2]) && val - maxerr <= field[vw.pos.z][vw.pos.y][vw.pos.x] ) {
                        cellsVisited++;
                        field[vw.pos.z][vw.pos.y][vw.pos.x] = val;

                        IVec3 pos = vw.pos;
                        if ( vw.pos.x < tsize-1 &&  !visited[vw.pos.z][vw.pos.y][vw.pos.x+1] ) q.Enqueue( new VertW<T>( pos + new IVec3( 1, 0, 0 ), vw.src ) );
                        if ( vw.pos.x > 0 &&        !visited[vw.pos.z][vw.pos.y][vw.pos.x-1] ) q.Enqueue( new VertW<T>( pos + new IVec3(-1, 0, 0 ), vw.src ) );
                        if ( vw.pos.y < tsize-1 &&  !visited[vw.pos.z][vw.pos.y+1][vw.pos.x] ) q.Enqueue( new VertW<T>( pos + new IVec3( 0, 1, 0 ), vw.src ) );
                        if ( vw.pos.y > 0 &&        !visited[vw.pos.z][vw.pos.y-1][vw.pos.x] ) q.Enqueue( new VertW<T>( pos + new IVec3( 0,-1, 0 ), vw.src ) );
                        if ( vw.pos.z < tsize-1 &&  !visited[vw.pos.z+1][vw.pos.y][vw.pos.x] ) q.Enqueue( new VertW<T>( pos + new IVec3( 0, 0, 1 ), vw.src ) );
                        if ( vw.pos.z > 0 &&        !visited[vw.pos.z-1][vw.pos.y][vw.pos.x] ) q.Enqueue( new VertW<T>( pos + new IVec3( 0, 0,-1 ), vw.src ) );
                    }   

                }
                //Console.WriteLine( string.Format( "Visited {0}% of cells", 100.0f * (float)((double)cellsVisited) / (size*size*size) ) );
            }
        }

        struct EdgeI {
            public EdgeI( int i0, int i1 ) {
                this.i0 = i0;
                this.i1 = i1;
            }
            public int i0, i1;
        }

        class Edge {
            public Vector3 p0, p1;
            public Edge( Vector3 p0, Vector3 p1 ) {
                this.p0 = p0;
                this.p1 = p1;
            }
        }

        class Tri : IComparable<Tri> {
            public Vector3 p0, p1, p2;

            public Vector3 n;

            public Vector3 n0, n1, n2;

            public Tri( Vector3 p0, Vector3 p1, Vector3 p2 ) {
                this.p0 = p0;
                this.p1 = p1;
                this.p2 = p2;
                this.n = Vector3.Cross( p1-p0, p2-p0 );
                n.Normalize();

                n0 = Vector3.Cross( p1 - p0, n ); 
                n1 = Vector3.Cross( p2 - p1, n ); 
                n2 = Vector3.Cross( p0 - p2, n ); 
            }

            public bool PointInside( Vector3 p ) {
                float d0 = Vector3.Dot( p - p0, n0 );
                float d1 = Vector3.Dot( p - p1, n1 );
                float d2 = Vector3.Dot( p - p2, n2 );

                return d0 < 0 && d1 < 0 && d2 < 0;
            }
            
            public int CompareTo( Tri b ) {
                float apc = p0.Z + p1.Z + p2.Z;
                float bpc = b.p0.Z + b.p1.Z + b.p2.Z;
                return apc.CompareTo( bpc );
            }

        }

        private HashSet<EdgeI> CollectEdges() {
            HashSet<EdgeI> ret = new HashSet<EdgeI>();
            for ( int i = 0; i < indices.Length; i += 3 ) {
                for( int j = 0; j < 3; j++ ) {
                    int i0 = indices[i+j];
                    int i1 = indices[i+(j+1)%3];
                    int a = i0 < i1 ? i0 : i1;
                    int b = i0 < i1 ? i1 : i0;
                    EdgeI e = new EdgeI( a, b );
                    if ( !ret.Contains( e ) ) {
                        ret.Add( e );
                    }
                }
            }
            return ret;
        }
        
        private float [][][] ComputeUSignedField( IEnumerable<Vector3> positions, IEnumerable<Edge> edges, IEnumerable<Tri> triangles ) {
            Stopwatch sw = Stopwatch.StartNew();

            int s = 1;

            float [][][] pfield = null;


            int sz = this.size;
            while ( s < sz ) {
                s *= 2;
                Console.WriteLine( "Doing " + s );
                float [][][] field = Alloc3<float>(s);
                for( int z = 0; z < s; z++ ) for( int y = 0; y < s; y++ ) for( int x = 0; x < s; x++ ) field[z][y][x] = float.MaxValue;

                IterateElements<Tri>( field, pfield, triangles,  
                    delegate (VertW<Tri> a) {
                        return (float)Math.Abs( Vector3.Dot( a.src.n, (a.pos.fpos - a.src.p0) ) ) ;
                    },
                    e => ((e.p0 + e.p1 + e.p2) * 0.3333f),
                    (a) => a.src.PointInside( a.pos.fpos ) );

                IterateElements<Edge>( field, pfield, edges,  
                    delegate (VertW<Edge> a) {
                        Vector3 BA = a.src.p1 - a.src.p0;
                        float x = Vector3.Dot( BA, a.pos.fpos - a.src.p0 ) / BA.LengthSquared;
                        return (a.pos.fpos - (a.src.p0 + BA * x) ).Length;
                    },
                    e => ((e.p0 + e.p1) * 0.5f),
                    delegate( VertW<Edge> a ) {
                        Vector3 BA = a.src.p1 - a.src.p0;
                        float x = Vector3.Dot( BA, a.pos.fpos - a.src.p0 );
                        float y = BA.LengthSquared;
                        return x >= 0.0f && x <= y;
                    }//,
                    //(_) => true
                );

                IterateElements<Vector3>( field, pfield, positions,  (a) => (a.pos.fpos - a.src).Length, v => v, (_) => true );

                pfield = field;
            }

            long dur = sw.ElapsedMilliseconds;
            sw.Stop();

            Console.WriteLine( "Took " + ((float)dur / 1000.0f) + " seconds" );

            return pfield;
        }

        // not fastest but accurate for sure
        private int FillTri( List<float> [,] fld, Tri t ) {
            float minx = Utils.Min( t.p0.X, t.p1.X, t.p2.X );
            float maxx = Utils.Max( t.p0.X, t.p1.X, t.p2.X );

            float miny = Utils.Min( t.p0.Y, t.p1.Y, t.p2.Y );
            float maxy = Utils.Max( t.p0.Y, t.p1.Y, t.p2.Y );

            float minz = Utils.Min( t.p0.Z, t.p1.Z, t.p2.Z );
            float maxz = Utils.Max( t.p0.Z, t.p1.Z, t.p2.Z );

            IVec3 mini = ToIVec( new Vector3( minx, miny, minz ) );
            IVec3 maxi = ToIVec( new Vector3( maxx, maxy, maxz ) );

            int rstr = 0;

            for( int y = mini.y; y <= maxi.y; y++ ) {
                for( int x = mini.x; x <= maxi.x; x++ ) {
                    IVec3 p = new IVec3(x,y,0);
                    Vector3 fp = p.fpos;
                    fp.Z = 0;
                    // is fp inside tri projected to xy plane?
                    Vector3 a = new Vector3( t.p1.X, t.p1.Y, 0 ) - new Vector3( t.p0.X, t.p0.Y, 0 );
                    Vector3 b = new Vector3( t.p2.X, t.p2.Y, 0 ) - new Vector3( t.p1.X, t.p1.Y, 0 );
                    Vector3 c = new Vector3( t.p0.X, t.p0.Y, 0 ) - new Vector3( t.p2.X, t.p2.Y, 0 );
                    bool s0 = Vector3.Cross( fp - new Vector3( t.p0.X, t.p0.Y, 0 ), a ).Z > 0;
                    bool s1 = Vector3.Cross( fp - new Vector3( t.p1.X, t.p1.Y, 0 ), b ).Z > 0;
                    bool s2 = Vector3.Cross( fp - new Vector3( t.p2.X, t.p2.Y, 0 ), c ).Z > 0;
                    if ( (s0 && s1 && s2) || (!s0 && !s1 && !s2) ) {
                        float z = (Vector3.Dot( t.n, t.p0 ) - Vector2.Dot( t.n.Xy, fp.Xy )) / t.n.Z;
                        fld[y,x].Add( z );
                        rstr++;
                    }
                }
            }
            return rstr;
        }

        private void Generate() {
			ComputeBounds();

            Vector3 boundsize = bound_max - bound_min;
            bound_max_dim = Math.Max( boundsize.X, Math.Max( boundsize.Y, boundsize.Z ) );

            var edgi = CollectEdges();
            Edge [] edg = new Edge[edgi.Count];
            int n = 0;
            foreach( var e in edgi ) {
                edg[n] = new Edge( positions[e.i0], positions[e.i1] );
                n++;
            }

            //Tri [] tri = new Tri[indices.Length / 3];
            List<Tri> triangles = new List<Tri>();
            for(int i = 0; i < indices.Length; i += 3 ) {
                //tri[i/3] = new Tri( positions[indices[i+0]], positions[indices[i+1]], positions[indices[i+2]] );
                triangles.Add( new Tri( positions[indices[i+0]], positions[indices[i+1]], positions[indices[i+2]] ) );
            }
            triangles.Sort();

            Field = ComputeUSignedField( positions, edg, triangles );

            // obs triangles must be sorted. also assuming they don't cut each other!
            Console.WriteLine( "Converting to signed field.." );
            List<float> [,] fld = new List<float>[size,size];
            for( int y = 0; y < size; y++ ) {
                for( int x = 0; x < size; x++ ) {
                    fld[y,x] = new List<float>();
                }
            }
            int r = 0;
            foreach( Tri t in triangles ) {
                r += FillTri( fld, t );
            }

            for( int y = 0; y < size; y++ ) {
                for( int x = 0; x < size; x++ ) {
                    float z0 = bound_min.Z;
                    bool inside = false;
                    for( int i = 0; i < fld[y,x].Count; i++ ) {
                        float fz = fld[y,x][i];
                        if ( inside ) {
                            int minz = ToIVec( new Vector3(0,0,z0) ).z;
                            int maxz = ToIVec( new Vector3(0,0,fz) ).z;
                            for( int z = minz; z <= maxz; z++ ) {
                                float ifz = new IVec3(0,0,z).fpos.Z;
                                if ( ifz >= z0 && ifz < fz ) {
                                    Field[z][y][x] = -Field[z][y][x];
                                }
                            }
                        } else {
                            z0 = fz;
                        }
                        inside = !inside;
                    }
                }
            }
            

        }


    }
}
