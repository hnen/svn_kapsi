using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;



using System.Threading;

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

        public Vector3 [] fposlut;

        struct IVec3 {

			public static DistanceFieldGenerator dfg;

            //public readonly int x,y,z;
            public readonly int x, y, z;

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
                    return dfg.fposlut[z*dfg.size*dfg.size + y * dfg.size + x];
                    //return dfg.bound_max_dim * (new Vector3( x, y, z ) / (dfg.size-1)) + dfg.bound_min;
                    /*
                    return new Vector3(
                        (float)(dfg.bound_max_dim * x) / (dfg.size - 1) + dfg.bound_min.X,
                        (float)(dfg.bound_max_dim * y) / (dfg.size - 1) + dfg.bound_min.Y,
                        (float)(dfg.bound_max_dim * z) / (dfg.size - 1) + dfg.bound_min.Z
                        );
                    */
                }
            }
        }

		private IVec3 ToIVec( Vector3 pos ) {
			Vector3 ivec = ((pos - bound_min) / bound_max_dim) * (size-1);
			return new IVec3( (int)ivec.X, (int)ivec.Y, (int)ivec.Z );
		}

        struct VertW<T> /* : IComparable<VertW> */ {
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

            float grid_d = 1.0f / this.size;

			bound_min = bmin - (bound_max-bound_min) * (1.0f + grid_d);
			bound_max = bmax + (bound_max-bound_min) * (1.0f + grid_d);
		}

        public float [][][] Field;
        //public float [,,] Field;

        private int IterateElements<T>( float [][][] field, float [][][] prevfield,
                IEnumerable<T> inputValues, 
                Func<VertW<T>, float > computeValue,
                Func<T, Vector3> computeMidPoint,
                Func<VertW<T>, bool> additionalConstraint, bool dopt = false
                 ) {
            int tsize = field.Length;
            this.size = tsize;
            
            //Thread.CurrentThread.Priority = System.Threading.ThreadPriority.Highest;

            float maxerr0 = bound_max_dim * (1.733f / 2.0f) / tsize;
            //float maxerr = 10000.0f;
            //float maxerr = 0.0f;

            fposlut = new Vector3[size*size*size];
            for( int z = 0; z < size; z++ ) { 
                for( int y = 0; y < size; y++ ) { 
                    for( int x = 0; x < size; x++ ) { 
                        fposlut[z*size*size + y*size + x] = bound_max_dim * (new Vector3( x, y, z ) / (size-1)) + bound_min;
                    }
                }
            }

            //bool [][][] visited = Alloc3<bool>(tsize);

			int cellsVisited = 0;

            Mutex paska = new Mutex(true);

            Action<T> doPrimitive = (T p) => { 
                {
                //Console.WriteLine( string.Format( "{0}% ready..", ((float)vertN / positions.Length) * 100.0f ) );

                Queue<VertW<T>> q = new Queue<VertW<T>>();

    		    Vector3 mpf = computeMidPoint( p );
			    IVec3 mp = ToIVec(mpf);
	            q.Enqueue( new VertW<T>( this.ToIVec( computeMidPoint( p ) ), p ) );

                bool [] visited = new bool[size*size*size];
                //bool [][][] visited = Alloc3<bool>(size);
                //for( int z = 0; z < tsize; z++ ) for( int y = 0; y < tsize; y++ ) for( int x = 0; x < tsize; x++ ) visited[z][y][x] = false;

                while ( q.Count > 0 ) {
                    VertW<T> vw = q.Dequeue();

                    if ( visited[vw.pos.z*size*size + vw.pos.y*size + vw.pos.x] ) {
                        continue;
                    }

                    visited[vw.pos.z*size*size + vw.pos.y*size + vw.pos.x] = true;
                    
                    if ( !additionalConstraint( vw ) ) {
                        continue;
                    }

                    float val = computeValue( vw );

                    float maxerr = maxerr0;
                    //if( val > maxerr*2.0f || field[vw.pos.z][vw.pos.y][vw.pos.x] > maxerr*2.0f) maxerr = 0;

                    bool p1;
                    lock( field[vw.pos.z][vw.pos.y] )
                    {
                        p1 = val - maxerr <= field[vw.pos.z][vw.pos.y][vw.pos.x];
                    }

                    if ( p1 ) {
                        bool p0 = prevfield == null || val - 1.7f * maxerr <= prevfield[vw.pos.z/2][vw.pos.y/2][vw.pos.x/2];
                        if ( p0 ) {
                            lock( field[vw.pos.z][vw.pos.y] )
                            {
		                        cellsVisited++;
						        if ( field[vw.pos.z][vw.pos.y][vw.pos.x] > val ) {
    							    field[vw.pos.z][vw.pos.y][vw.pos.x] = val;
						        }
                            }
                        
						    bool y_p = vw.pos.y > mp.y;
						    bool y_m = vw.pos.y < mp.y;
						    bool x_p = vw.pos.x > mp.x;
						    bool x_m = vw.pos.x < mp.x;
						    bool z_p = vw.pos.z > mp.z;
						    bool z_m = vw.pos.z < mp.z;
						    //bool y_p = false;
						    //bool y_m = false;
						    //bool x_p = false;
						    //bool x_m = false;
						    //bool z_p = false;
						    //bool z_m = false;

                            //q.Enqueue( new VertW<T>( new IVec3( pos.x + vw.DX, pos.y, pos.z ), vw.src ) );
                            //q.Enqueue( new VertW<T>( new IVec3( pos.x, pos.y + DY, pos.z ), vw.src ) );
                            //q.Enqueue( new VertW<T>( new IVec3( pos.x + vw.DX, pos.y, pos.z ), vw.src ) );

                            IVec3 pos = vw.pos;
                            if ( !x_m && vw.pos.x < tsize-1 &&      !visited[vw.pos.z*size*size + vw.pos.y * size + vw.pos.x + 1] )
                                q.Enqueue( new VertW<T>( new IVec3( pos.x + 1, pos.y, pos.z ), vw.src ) );
                            if ( !x_p && vw.pos.x > 0 &&            !visited[vw.pos.z*size*size + vw.pos.y * size + vw.pos.x - 1] ) 
                                q.Enqueue(new VertW<T>( new IVec3( pos.x-1, pos.y, pos.z ), vw.src));
                            if ( !y_m && vw.pos.y < tsize - 1 &&    !visited[vw.pos.z*size*size + (vw.pos.y + 1) * size + vw.pos.x] )
                                q.Enqueue(new VertW<T>( new IVec3( pos.x, pos.y+1, pos.z ), vw.src));
                            if ( !y_p && vw.pos.y > 0 &&            !visited[vw.pos.z*size*size + (vw.pos.y - 1) * size + vw.pos.x] ) 
                                q.Enqueue(new VertW<T>(new IVec3( pos.x, pos.y-1, pos.z ), vw.src));
                            if ( !z_m && vw.pos.z < tsize - 1 &&    !visited[(vw.pos.z + 1)*size*size + vw.pos.y * size + vw.pos.x] ) 
                                q.Enqueue(new VertW<T>(new IVec3( pos.x, pos.y, pos.z+1 ), vw.src));
                            if ( !z_p && vw.pos.z > 0 &&            !visited[(vw.pos.z - 1)*size*size + vw.pos.y * size + vw.pos.x] ) 
                                q.Enqueue(new VertW<T>(new IVec3( pos.x, pos.y, pos.z - 1 ), vw.src));
                        }
                    }   

                }
                //Console.WriteLine( string.Format( "Visited {0}% of cells", 100.0f * (float)((double)cellsVisited) / (size*size*size) ) );
                }
            };

            //ManualResetEvent [] doneEvents = new ManualResetEvent[inputValues.Count];
            Thread t = new Thread( () => {
                HashSet<ManualResetEvent> doneEvents = new HashSet<ManualResetEvent>();
                //HashSet<Thread> threads = new HashSet<System.Threading.Thread>();
                //Thread.CurrentThread.Priority = System.Threading.ThreadPriority.Lowest;

                foreach( T p in inputValues ) {
                    T np = p;
                    ManualResetEvent ev = new ManualResetEvent(false);
                    doneEvents.Add( ev );
                    ThreadPool.QueueUserWorkItem( 
                        (_) => {
                            doPrimitive(np);
                            ev.Set();
                        } );

                    if ( doneEvents.Count >= 8 ) {
                        WaitHandle.WaitAll( doneEvents.ToArray() );
                        doneEvents.Clear();
                    }
                    
               }   
               if ( doneEvents.Count > 0 )
                    WaitHandle.WaitAll( doneEvents.ToArray() );


            });            
            t.Start();
            t.Join();

			return cellsVisited;
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

				int c = 0;

				c += IterateElements<Vector3>( field, pfield, positions, 
					delegate( VertW<Vector3> a) {
						Vector3 f = a.pos.fpos;
						return (float)Math.Sqrt( (f.X - a.src.X) * (f.X - a.src.X) + (f.Y - a.src.Y) * (f.Y - a.src.Y) + (f.Z - a.src.Z) * (f.Z - a.src.Z) );
						//(a.pos.fpos.X - a.src.X),
					},
					v => v, (_) => true, false );

				c += IterateElements<Edge>( field, pfield, edges,  
                    delegate (VertW<Edge> a) {
                        Vector3 BA = new Vector3( a.src.p1.X - a.src.p0.X, a.src.p1.Y - a.src.p0.Y, a.src.p1.Z - a.src.p0.Z ); // a.src.p1 - a.src.p0
						Vector3 f = a.pos.fpos;
						Vector3 C = f - a.src.p0;
                        float x = (BA.X * C.X + BA.Y * C.Y + BA.Z * C.Z) / BA.LengthSquared; // Vector3.Dot( BA, C ) / BA.LengthSquared
                        //return (f - (a.src.p0 + BA * x) ).Length;
                        Vector3 d = new Vector3(f.X - (a.src.p0.X + BA.X * x), f.Y - (a.src.p0.Y + BA.Y * x), f.Z - (a.src.p0.Z + BA.Z * x));
                        return d.Length;
                    },
                    e => ((e.p0 + e.p1) * 0.5f),
                    delegate( VertW<Edge> a ) {
                        Vector3 BA = a.src.p1 - a.src.p0;
                        Vector3 fp = a.pos.fpos;
                        float x = Vector3.Dot( BA, fp - a.src.p0 );
                        float y = BA.LengthSquared;
                        return x >= 0.0f && x <= y;
                    }//,
                    //(_) => true
                );

                c += IterateElements<Tri>( field, pfield, triangles,  
                    delegate (VertW<Tri> a) {
                        return (float)Math.Abs( Vector3.Dot( a.src.n, (a.pos.fpos - a.src.p0) ) ) ;
                    },
                    e => ((e.p0 + e.p1 + e.p2) * 0.3333f),
                    (a) => a.src.PointInside( a.pos.fpos ) );

				System.Console.WriteLine( string.Format( "Unoptimality factor {0}", (float)c / (size*size*size) ) );
               
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

            // dfg is static variable, thus it isn't thread safe
            //lock( IVec3.dfg )
            {
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

			        /*
			        for( int z = 0; z < size; z++ ) {
				        for( int y = 0; y < size; y++ ) {
					        for( int x = 0; x < size; x++ ) {
						        Field[z][y][x] -= 0.15f;
					        }
				        }
			        }

			        return;
                     */
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

                    return;
            
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
                                        if ( ifz > z0 && ifz < fz )
                                        {
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

                    return;

			        float [][][] nfield = Alloc3<float>(size);
			        Console.WriteLine( "Smoothing.." );
			        const int brange = 1;
			        const float deviation = 0.84089642f * 0.84089642f;

			        float [][][] weights = Alloc3<float>(brange*2+1);
			        for( int oz = -brange; oz <= brange; oz++ ) {
				        for( int oy = -brange; oy <= brange; oy++ ) {
					        for( int ox = -brange; ox <= brange; ox++ ) {
						        weights[oz+brange][oy+brange][ox+brange] = 
							        (float)(Math.Exp( -(ox*ox+oy*oy+oz*oz) / (2.0f * deviation) ) / Math.Pow(2.0f * Math.PI * deviation, 1.5f));
					        }
				        }
			        }

			        for( int z = 0; z < size; z++ ) {
				        for( int y = 0; y < size; y++ ) {
					        for( int x = 0; x < size; x++ ) {
						        float s = 0.0f;				
						        float wsum = 0.0f;
						        for( int oz = -brange; oz <= brange; oz++ ) {
							        for( int oy = -brange; oy <= brange; oy++ ) {
								        for( int ox = -brange; ox <= brange; ox++ ) {
									        float w = weights[oz+brange][oy+brange][ox+brange];
									        wsum += w;
									        s += w * Field[(z+oz).Mod(size)][(y+oy).Mod(size)][(x+ox).Mod(size)];
								        }
							        }
						        }
						        nfield[z][y][x] = s;
					        }
				        }
			        }
			        Field = nfield;

            }
        }


    }
}
