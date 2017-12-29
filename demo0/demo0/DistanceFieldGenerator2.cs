using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Diagnostics;

using OpenTK;

namespace demo0 {


    class DistanceFieldGenerator2 {
		public int size;
		public  Vector3 [] _positions;
		public  int [] indices;

		public Vector3 bound_min, bound_max;
        public float bound_max_dim;

        public float [][][] Field;


		public DistanceFieldGenerator2( Vector3[] positions, int[] indices, int size ) {
			this.size = size;
			this._positions = positions;
			this.indices = indices;
			Generate();
		}

		public DistanceFieldGenerator2( MeshImporter.Mesh mesh, int size ) {
			this.size = size;
			this._positions = mesh.verts.Select( ( a, _ ) => a.pos ).ToArray();
			this.indices = mesh.inds;
			Generate();
        }

      struct IVec3 {
            public readonly int x, y, z;
			public IVec3( int x, int y, int z ) {
				this.x = x;
				this.y = y;
				this.z = z;
			}

            private static Vector3 [][][] fposlut;
            private static float cr;
            private static float cl;

            public static IVec3 operator + ( IVec3 a, IVec3 b ) {
                return new IVec3( a.x + b.x, a.y + b.y, a.z + b.z );
            }

            public float ClosestDist( Vector3 p, int size ) {
                Vector3 fp = fpos(size);
                float rx = Utils.Max( Utils.Min( p.X, fp.X + cl ), fp.X - cl );
                float ry = Utils.Max( Utils.Min( p.Y, fp.Y + cl ), fp.Y - cl );
                float rz = Utils.Max( Utils.Min( p.Z, fp.Z + cl ), fp.Z - cl );
                return (new Vector3(rx,ry,rz) - p).LengthFast;
                //return (fpos(size) - p).LengthFast - cr;
            }

            public float FarestDist( Vector3 p, int size ) {
                Vector3 fp = fpos(size);
                float s0 = p.X < fp.X ? cl : -cl;
                float s1 = p.Y < fp.Y ? cl : -cl;
                float s2 = p.Z < fp.Z ? cl : -cl;
                Vector3 fp0 = fp + new Vector3( s0, s1, s2 );
                return (fp0 - p).LengthFast;
            }

            public float CenterDist( Vector3 p, int size ) {
                return (fpos(size) - p).LengthFast;
            }


            public float FarestDist( Edge e, int size ) {
                return CenterDist(e,size) + cr;
            }

            public float ClosestDist( Edge e, int size ) {
                return CenterDist(e,size) - cr;
            }

            public float CenterDist( Edge e, int size ) {
                Vector3 P = fpos(size);
                Vector3 A = e.p0;
                Vector3 B = e.p1;
                //Vector3 a = B-A;
                Vector3 a = e.a;
                float l = e.length;
                //float l = a.Length;
                //a.Normalize();
                //a /= l;
                float x = Vector3.Dot( (P-A), a );
                float y = a.LengthSquared;
                if ( x <= -cr ) {
                    //return CenterDist( A, size );
                    return float.PositiveInfinity;
                } else
                if ( x >= l+cr ) {
                    //return CenterDist( B, size );
                    //return float.PositiveInfinity;
                    return float.PositiveInfinity;
                } else {
                    if ( x <= 0.0f ) {
                        return CenterDist( A, size );
                    } else if ( x > l ) {
                        return CenterDist( B, size );
                    } else {
                        Vector3 P_ = A + a * x / y;
                        return (P-P_).Length;
                    }
                }
            }

            public float FarestDist( Tri t, int size ) {
                return CenterDist(t,size) + cr;
            }

            public float ClosestDist( Tri t, int size ) {
                return CenterDist(t,size) - cr;
            }

            public float CenterDist( Tri t, int size ) {
                Vector3 P = fpos(size);
                float d0 = Vector3.Dot( P - t.p0, t.n0 );
                float d1 = Vector3.Dot( P - t.p1, t.n1 );
                float d2 = Vector3.Dot( P - t.p2, t.n2 );
                if ( d0 >= cr || d1 >= cr || d2 >= cr ) {
                    //return CenterDist( new Edge( t.p0, t.p1 ), size );
                    return float.PositiveInfinity;
                } else {
                    if ( d0 >= 0 ) {
                        //return CenterDist( new Edge( t.p0, t.p1 ), size );
                        return CenterDist( t.e0, size );
                    } else if ( d1 >= 0 ) {
                        //return CenterDist( new Edge( t.p1, t.p2 ), size );
                        return CenterDist( t.e1, size );
                    } else if ( d2 >= 0 ) {
                        //return CenterDist( new Edge( t.p2, t.p0 ), size );
                        return CenterDist( t.e2, size );
                    } else {
                        return (float)Math.Abs( Vector3.Dot( t.n, (P - t.p0) ) ) ;
                    }
                }
            }

            public Vector3 fpos(int size) {
                if ( fposlut == null || fposlut.Length != size ) {
                    fposlut = Utils.Alloc3<Vector3>( size );
                    for( int iz = 0; iz < size; iz++ ) {
                        for( int iy = 0; iy < size; iy++ ) {
                            for( int ix = 0; ix < size; ix++ ) {
                                fposlut[iz][iy][ix] = new Vector3((float)ix + 0.5f, (float)iy + 0.5f, (float)iz + 0.5f) / size;
                            }
                        }
                    }
                    cl =  0.5f / size;
                    cr = (0.5f / size) * 1.7321f;
                }
                return fposlut[z][y][x];
                //return (new Vector3((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f)) / size;
            }
        }


		private IVec3 ToIVec( Vector3 pos ) {
			return new IVec3( (int)pos.X, (int)pos.Y, (int)pos.Z );
		}


		private void ComputeBounds() {

			Vector3 bmin = new Vector3( float.MaxValue, float.MaxValue, float.MaxValue );
			Vector3 bmax = new Vector3( float.MinValue, float.MinValue, float.MinValue );
			foreach ( Vector3 p in _positions ) {
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

        struct EdgeI {
            public EdgeI( int i0, int i1 ) {
                this.i0 = i0;
                this.i1 = i1;
            }
            public int i0, i1;
        }

        class Edge {
            public readonly Vector3 p0, p1;
            public readonly float length;
            public readonly Vector3 a;
            public Edge( Vector3 p0, Vector3 p1 ) {
                this.p0 = p0;
                this.p1 = p1;
                this.length = (p0-p1).Length;
                this.a = p1-p0;
                a.Normalize();
            }
        }

       class Tri : IComparable<Tri> {
            public readonly Vector3 p0, p1, p2;
            public readonly Vector3 n;
            public readonly Vector3 n0, n1, n2;

            public readonly Edge e0, e1, e2;

            public Tri( Vector3 p0, Vector3 p1, Vector3 p2 ) {
                this.p0 = p0;
                this.p1 = p1;
                this.p2 = p2;
                this.n = Vector3.Cross( p1-p0, p2-p0 );
                this.e0 = new Edge( p0, p1 );
                this.e1 = new Edge( p1, p2 );
                this.e2 = new Edge( p2, p0 );
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

        private int FillTri( List<float> [,] fld, Tri t ) {

            Vector3 p0 = t.p0 * size;
            Vector3 p1 = t.p1 * size;
            Vector3 p2 = t.p2 * size;

            float minx = Utils.Min( p0.X, p1.X, p2.X );
            float maxx = Utils.Max( p0.X, p1.X, p2.X );

            float miny = Utils.Min( p0.Y, p1.Y, p2.Y );
            float maxy = Utils.Max( p0.Y, p1.Y, p2.Y );

            float minz = Utils.Min( p0.Z, p1.Z, p2.Z );
            float maxz = Utils.Max( p0.Z, p1.Z, p2.Z );

            IVec3 mini = ToIVec( new Vector3( minx, miny, minz ) );
            IVec3 maxi = ToIVec( new Vector3( maxx, maxy, maxz ) );

            int rstr = 0;

            for( int y = mini.y; y <= maxi.y; y++ ) {
                for( int x = mini.x; x <= maxi.x; x++ ) {
                    IVec3 p = new IVec3(x,y,0);
                    Vector3 fp = p.fpos(this.size);
                    fp.Z = 0;
                    // is fp inside tri projected to xy plane?
                    Vector3 a = new Vector3( p1.X, p1.Y, 0 ) - new Vector3( p0.X, p0.Y, 0 );
                    Vector3 b = new Vector3( p2.X, p2.Y, 0 ) - new Vector3( p1.X, p1.Y, 0 );
                    Vector3 c = new Vector3( p0.X, p0.Y, 0 ) - new Vector3( p2.X, p2.Y, 0 );
                    bool s0 = Vector3.Cross( fp - new Vector3( p0.X, p0.Y, 0 ), a ).Z > 0;
                    bool s1 = Vector3.Cross( fp - new Vector3( p1.X, p1.Y, 0 ), b ).Z > 0;
                    bool s2 = Vector3.Cross( fp - new Vector3( p2.X, p2.Y, 0 ), c ).Z > 0;
                    if ( (s0 && s1 && s2) || (!s0 && !s1 && !s2) ) {
                        float z = (Vector3.Dot( t.n, p0 ) - Vector2.Dot( t.n.Xy, fp.Xy )) / t.n.Z;
                        fld[y,x].Add( z / size );
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

			Console.WriteLine( "bound_max_dim: " + bound_max_dim + ", bound_min = " + bound_min );

			Vector3 [] npositions = new Vector3[_positions.Length];
			for( int i = 0; i < npositions.Length; i++ ) {
				npositions[i] = ((_positions[i] - bound_min) / bound_max_dim - new Vector3(0.5f, 0.5f, 0.5f)) * (0.9f) + new Vector3(0.5f, 0.5f, 0.5f);
			}

            var edgi = CollectEdges();
            Edge [] edges = new Edge[edgi.Count];
            int n = 0;
            foreach( var e in edgi ) {
                edges[n] = new Edge( npositions[e.i0], npositions[e.i1] );
                n++;
            }

            List<Tri> triangles = new List<Tri>();
            for(int i = 0; i < indices.Length; i += 3 ) {
                //tri[i/3] = new Tri( positions[indices[i+0]], positions[indices[i+1]], positions[indices[i+2]] );
                triangles.Add( new Tri( npositions[indices[i+0]], npositions[indices[i+1]], npositions[indices[i+2]] ) );
            }
            triangles.Sort();

			Field = ComputeUSignedField( npositions, edges.ToArray(), triangles.ToArray() );

			return;

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
                                float ifz = new IVec3(0,0,z).fpos(this.size).Z;
                                if ( ifz > z0 && ifz < fz ) {
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


        void ComputeFldPrim( int [] ret, int [][][] ptr, int x, int y, int z, int [] prevFld, int [][][] prevPtr, Vector3 [] positions, Edge [] edges, Tri [] tris ) {
			int nsize = prevPtr.Length * 2;
            IVec3 ip = new IVec3(x,y,z);
		    //ret[z][y][x] = new List<int>();
            //ret[z][y][x] = new int[ prevFld[z/2][y/2][x/2][0] + 1 ];
		    // compute smallest largest distance
		    float best = float.MaxValue;
		    int besti = -1;
		    //foreach( int i in prevFld[z/2][y/2][x/2] ) {
            for( int n = 0; n < prevFld[prevPtr[z/2][y/2][x/2]]; n++ ) {
                int i = prevFld[prevPtr[z/2][y/2][x/2]+n+1];
                float d = float.MaxValue;
                if ( i >= positions.Length + edges.Length ) {
				    var p = tris[i - positions.Length - edges.Length];
                    d = ip.FarestDist( p, nsize );
                } else
                if ( i >= positions.Length ) {
				    var p = edges[i - positions.Length];
                    d = ip.FarestDist( p, nsize );
                } else {
				    var p = positions[i];
                    d = ip.FarestDist( p, nsize );
                }
			    if ( d < best ) {
				    best = d;
				    besti = i;
			    }
		    }
		    // Add verts which has smaller smallest distance than smallest largest distance
		    //foreach( int i in prevFld[z/2][y/2][x/2] ) {
            for( int n = 0; n < prevFld[prevPtr[z/2][y/2][x/2]]; n++ ) {
                int i = prevFld[prevPtr[z/2][y/2][x/2]+n+1];
                float d = float.MaxValue;
                if ( i >= positions.Length + edges.Length ) {
				    var p = tris[i - positions.Length - edges.Length];
                    d = ip.ClosestDist( p, nsize );
                } else
                if ( i >= positions.Length ) {
				    var p = edges[i - positions.Length];
                    d = ip.ClosestDist( p, nsize );
                } else {
				    var p = positions[i];
                    d = ip.ClosestDist( p, nsize );
                }
			    if ( d <= best ) {
				    //ret[z][y][x].Add( i );
                    ret[ptr[z][y][x] + (1 + ret[ptr[z][y][x]]++)] = i;
			    }
		    }
        }

		//List<int> [][][] ComputeFldPrims( List<int> [][][] prevFld, Vector3 [] positions, Edge [] edges, Tri [] tris ) {
        //int [][][][] ComputeFldPrims( int [][][][] prevFld, Vector3 [] positions, Edge [] edges, Tri [] tris ) {
		Tuple<int[], int[][][]> ComputeFldPrims( int [] prevFld, int [][][] prevPtr, Vector3 [] positions, Edge [] edges, Tri [] tris ) {

			int nsize = prevPtr.Length * 2;
            int nfldsize = 0;
			//int [][][][] ret = Utils.Alloc3<int[]>( nsize );
            int [][][] nptr = Utils.Alloc3<int>( nsize );
			for( int z = 0; z < nsize; z++ ) {
				for( int y = 0; y < nsize; y++ ) {
					for( int x = 0; x < nsize; x++ ) {
                        nptr[z][y][x] = nfldsize;
                        int prevSize = prevFld[prevPtr[z/2][y/2][x/2]];
                        nfldsize += prevSize+1;
                    }
                }
            }
            int [] ret = new int[nfldsize];
			Console.WriteLine( "Allocated for n^" + nsize + " " + (4*nfldsize) + "bytes. (" + (4*nfldsize/(1024*1024)) + "MB)" );

			Console.WriteLine( "" + nsize + "^3..." );

			//long allocated = 0;
			//for( int z = 0; z < nsize; z++ ) {
			//	for( int y = 0; y < nsize; y++ ) {
			//		for( int x = 0; x < nsize; x++ ) {
			//			int size = prevFld[prevPtr[z/2][y/2][x/2]] + 1;
			//			allocated += size;
			//			ret[z][y][x] = new int[size];
			//		}
			//	}
			//}

            Thread t = new Thread( () => {
                HashSet<ManualResetEvent> doneEvents = new HashSet<ManualResetEvent>();
			    for( int z = 0; z < nsize; z++ ) {
				    for( int y = 0; y < nsize; y++ ) {
					    for( int x = 0; x < nsize; x++ ) {
                           //ComputeFldPrim( ret, x, y, z, prevFld, positions, edges, tris );

                            int _x = x, _y = y, _z = z;

                            ManualResetEvent ev = new ManualResetEvent(false);
                            doneEvents.Add( ev );
                            ThreadPool.QueueUserWorkItem( 
                                (_) => {
                                    ComputeFldPrim( ret, nptr, _x, _y, _z, prevFld, prevPtr, positions, edges, tris );
                                    ev.Set();
                                } );

                            if ( doneEvents.Count >= 8 ) {
                                WaitHandle.WaitAll( doneEvents.ToArray() );
                                doneEvents.Clear();
                            }
					    }
				    }
			    }

               if ( doneEvents.Count > 0 )
                    WaitHandle.WaitAll( doneEvents.ToArray() );

            } );
            t.Start();
            t.Join();

			return new Tuple<int[],int[][][]>( ret, nptr );
		}

		float [][][] ComputeFldPrims( Vector3 [] positions, Edge [] edges, Tri [] tris ) {
			//int [][][][] init = Utils.Alloc3<int[]>(1);
			//init[0][0][0] = new int[1 + positions.Length + edges.Length + tris.Length];
			int [] init = new int[1 + positions.Length + edges.Length + tris.Length];
			for( int i = 0; i < positions.Length + edges.Length + tris.Length; i++ ) {
				//init[0][0][0][i+1] = i;
				init[i+1] = i;
			}
            //init[0][0][0][0] = positions.Length + edges.Length + tris.Length;
			init[0] = positions.Length + edges.Length + tris.Length;

			int [][][] ptr = Utils.Alloc3<int>(1);
			ptr[0][0][0] = 0;

            Console.WriteLine( "Maximum primitives " + (positions.Length + edges.Length + tris.Length) );
			int s = 1;
			while( s < size/2 ) {
				s*=2;
				//init = ComputeFldPrims( init, positions, edges, tris );
				var result = ComputeFldPrims( init, ptr, positions, edges, tris );
				init = result.Item1;
				ptr = result.Item2;
			}

			float [][][] ret = Utils.Alloc3<float>( size );
			Console.WriteLine( "Collapsing into a scalar field.." );
			// collapse primitive lists into a field
			for( int z = 0; z < size; z++ ) {
				for( int y = 0; y < size; y++ ) {
					for( int x = 0; x < size; x++ ) {
                        IVec3 ip = new IVec3(x,y,z);
						float best = float.MaxValue;
						//foreach( int i in init[z/2][y/2][x/2] ) {
                        //for( int n = 0; n < init[z/2][y/2][x/2][0]; n++ ) {
                        for( int n = 0; n < init[ptr[z/2][y/2][x/2]]; n++ ) {
                            //int i = init[z/2][y/2][x/2][n+1];
                            int i = init[ptr[z/2][y/2][x/2]+n+1];
                            float d = float.MaxValue;
                            if ( i >= positions.Length + edges.Length ) {
                                int _i = i - positions.Length - edges.Length;
							    var p = tris[_i];
							    d = ip.CenterDist( p, size );
                            } else
                            if ( i >= positions.Length ) {
                                int _i = i - positions.Length;
							    var p = edges[_i];
							    d = ip.CenterDist( p, size );
                            } else {
							    var p = positions[i];
							    d = ip.CenterDist( p, size );
                            }
							if ( d < best ) {
    							best = d;
							}
						}
						ret[z][y][x] = best;
					}
				}
			}


			return ret;
		}


		float [][][] ComputeUSignedField( Vector3 [] positions, Edge [] edges, Tri [] tris ) {
            Stopwatch sw = Stopwatch.StartNew();

			var prims = ComputeFldPrims( positions, edges, tris );
            /*
			float [][][] ret = Utils.Alloc3<float>( size );

			Console.WriteLine( "Collapsing into a scalar field.." );

			// collapse primitive lists into a field
			for( int z = 0; z < size; z++ ) {
				for( int y = 0; y < size; y++ ) {
					for( int x = 0; x < size; x++ ) {
                        IVec3 ip = new IVec3(x,y,z);
						float best = float.MaxValue;
						foreach( int i in prims[z][y][x] )
						{
							Vector3 v = positions[i];
							float d = ip.CenterDist( v, size );
							if ( d < best ) {
								best = d;
							}
						}
						ret[z][y][x] = best;
					}
				}
			}
            */
            long dur = sw.ElapsedMilliseconds;
            sw.Stop();
            Console.WriteLine( "Took " + ((float)dur / 1000.0f) + " seconds" );

			return prims;
		}


	}



}


