using System;
using System.Collections.Generic;

using Vector3 = SharpDX.Vector3;

namespace TerrainTest {

    interface ITerrainHeightmap {
        float GetHeight( float x, float y );
        Texture GetTexture();
        Texture GetNormalmap();
    }

    /*
    class ExtHeightmap : ITerrainHeightmap {

        private DSHeightmap hm0;
        private DSHeightmap hm1;
        private DSHeightmap hm2;
        private DSHeightmap hm3;
        private DSHeightmap hm4;

        public ExtHeightmap() {

            hm0 = new DSHeightmap( 1024, 1024, 2000, 2000, 5.0f, 423 );
            hm1 = new DSHeightmap( 1024, 1024, 2500, 2500, 10.0f, 43242 );
            hm2 = new DSHeightmap( 1024, 1024, 5000, 5000, 25.0f, 654564 );
            hm3 = new DSHeightmap( 1024, 1024, 10000, 10000, 50.0f, 2342 );
            hm4 = new DSHeightmap( 1024, 1024, 20000, 20000, 100.0f, 4324223 );

        }

        public float GetHeight( float x, float y ) {
            float d = (float)Math.Sqrt( x * x + y * y );
            return (hm0.GetHeight( x, y ) + hm1.GetHeight( x, y ) + hm2.GetHeight( x, y ) + hm3.GetHeight( x, y ) + hm4.GetHeight( x, y )) * 0.75f;
        }


    }
    */

    class DSHeightmap : ITerrainHeightmap {

        private readonly int X_QUAL;
        private readonly int Y_QUAL;

        private readonly float xsize;
        private readonly float ysize;
        private readonly float height;

        //private float [,] data;
        //private float [][] data;
        private float [] data;

        private Random random;

        private MutableTexture heightmapTexture;
        private StaticTexture<byte> normalmapTexture;

        public float maxY = 0.0f;

        public Texture GetTexture() {
            return heightmapTexture;
        }
        public Texture GetNormalmap() {
            return normalmapTexture;
        }

        public DSHeightmap( int x_qual, int y_qual, float xsize, float ysize, float height, int seed, SharpDX.Direct3D11.Device device ) {
            this.X_QUAL = x_qual;
            this.Y_QUAL = y_qual;
            this.xsize = xsize;
            this.ysize = ysize;
            this.height = height;

            random = new Random( seed );

            data = new float[y_qual * x_qual];

            Generate();

            // Compute normals
            
            byte [] tmpData = new byte[x_qual * y_qual * 4];
            for ( int y = 0 ; y < Y_QUAL ; y++ ) {
                for ( int x = 0 ; x < X_QUAL ; x++ ) {
                    float hx0 = data[y.Mod(Y_QUAL) * X_QUAL + (x - 1).Mod(X_QUAL)];
                    float hx1 = data[y.Mod( Y_QUAL ) * X_QUAL + (x + 1).Mod( X_QUAL )];
                    float hy0 = data[(y - 1).Mod( Y_QUAL ) * X_QUAL + x];
                    float hy1 = data[(y + 1).Mod( Y_QUAL ) * X_QUAL + x];
                    float ux = xsize / (X_QUAL - 1);
                    float uy = ysize / (Y_QUAL - 1);
                    Vector3 v = new Vector3( -ux, hx0, 0 ) - new Vector3( ux, hx1, 0 );
                    Vector3 h = new Vector3( 0, hy0, -uy ) - new Vector3( 0, hy1, uy );
                    Vector3 n = Vector3.Cross( h, v ).Normalized();

                    tmpData[(y * X_QUAL + x) * 4 + 0] = (byte)((n.X + 1.0f) * 128);
                    tmpData[(y * X_QUAL + x) * 4 + 1] = (byte)((n.Y + 1.0f) * 128);
                    tmpData[(y * X_QUAL + x) * 4 + 2] = (byte)((n.Z + 1.0f) * 128);
                }
            }

            normalmapTexture = new StaticTexture<byte>( device, x_qual, y_qual, SharpDX.DXGI.Format.R8G8B8A8_UNorm, Texture.GenerateMips( tmpData, 2048, 11 ) );
            //normalmapTexture.SetData( tmpData );

            heightmapTexture = new MutableTexture( device, x_qual, y_qual, SharpDX.DXGI.Format.R32_Float );
            heightmapTexture.SetData( data );


        }

        private void Set( int x, int y, float value ) {
            //data[x, y] = value;
            if ( value > maxY ) maxY = value;
            data[x.Mod( X_QUAL)  + y.Mod(Y_QUAL) * X_QUAL] = value;
        }
        private float Get( int x, int y ) {
            //return data[x.Mod( X_QUAL + 1 ), y.Mod( Y_QUAL + 1 )];
            return data[x.Mod( X_QUAL ) + X_QUAL * y.Mod( Y_QUAL )];
        }

        /*
        private void Step( int x, int y, int w, int h, float threshold ) {
            if ( w < 2 || h < 2 ) {
                return;
            }
            float h0 = data[x, y];
            float h1 = data[x + w, y];
            float h2 = data[x + w, y + h];
            float h3 = data[x, y + h];

            float min = MathExt.Min( h0, h1 );
            float max = MathExt.Max( h0, h1 );

            //max = min + (max - min) * (0.0f + threshold);

            float h4 = (h0 + h1 + h2 + h3) / 4.0f;
            float h5 = (h0 + h1) / 2.0f;
            float h6 = (h0 + h3) / 2.0f;
            float h7 = (h1 + h2) / 2.0f;
            float h8 = (h3 + h2) / 2.0f;

            h4 += random.NextFloat( -threshold, threshold );

            int wh = w >> 1;
            int hh = h >> 1;

            Set(x + wh, y + hh, h4);
            Set(x + wh, y,      h5);
            Set(x,      y + hh, h6);
            Set(x + w,  y + hh, h7);
            Set(x + wh, y + h,  h8);

            float nthreshold = threshold * 0.49f;
            Step( x, y, wh, hh, nthreshold );
            Step( x + wh, y, wh, hh, nthreshold );
            Step( x + wh, y + hh, wh, hh, nthreshold );
            Step( x, y + hh, wh, hh, nthreshold );
        }
        */

        private void Generate() {
            float h0 = random.NextFloat( -1.0f, 1.0f ) * height;
            //float h1 = random.NextFloat( -1.0f, 1.0f ) * height;
            //float h2 = random.NextFloat( -1.0f, 1.0f ) * height;
            //float h3 = random.NextFloat( -1.0f, 1.0f ) * height;
            float h1 = h0;
            float h2 = h0;
            float h3 = h0;
            Set( 0, 0, h0 );
            Set( 0, Y_QUAL, h1 );
            Set( X_QUAL, Y_QUAL, h2 );
            Set( X_QUAL, 0, h3 );
            DS( 0.35f, 1.0f );
            //Step( 0, 0, X_QUAL, Y_QUAL, height * 5.0f );

            int R = 1;
            for ( int y = 0 ; y < Y_QUAL ; y++ ) {
                for ( int x = 0 ; x < X_QUAL ; x++ ) {
                    float sum = 0.0f;
                    float weight = 0.0f;
                    for ( int ux = -R ; ux <= R ; ux++ ) {
                        for ( int uy = -R ; uy <= R ; uy++ ) {
                            float w = 1.0f / (1.0f + ux*ux + uy*uy);
                            weight += w;
                            sum += data[(y + uy).Mod( Y_QUAL ) * X_QUAL + (x + ux).Mod( X_QUAL )] * w;
                        }
                    }
                    data[y * X_QUAL + x] = sum / weight;
                }
            }

        }

        // diamond square
        private void DS( float bumpiness, float apower ) {
            Queue<DSStep> bfs = new Queue<DSStep>();

            bfs.Enqueue( new DSStep( 0, 0, 0, X_QUAL, Y_QUAL ) );
            while ( bfs.Count > 0 ) {
                DSStep step = bfs.Dequeue();
                if ( step.w > 1 && step.h > 1 ) {
                    if ( step.phase == 0 ) {
                        float A;
                        float h0, h1, h2, h3;
                        //h0 = data[step.x, step.y];
                        //h1 = data[step.x, step.y + step.h];
                        //h2 = data[step.x + step.w, step.y + step.h];
                        //h3 = data[step.x + step.w, step.y];
                        h0 = data[step.x.Mod( X_QUAL ) + X_QUAL * step.y.Mod( Y_QUAL )];
                        h1 = data[step.x.Mod( X_QUAL ) + X_QUAL * (step.y + step.h).Mod( Y_QUAL )];
                        h2 = data[(step.x + step.w).Mod( X_QUAL ) + X_QUAL * (step.y + step.h).Mod( Y_QUAL )];
                        h3 = data[(step.x + step.w).Mod( X_QUAL ) + X_QUAL * step.y.Mod( Y_QUAL )];

                        //A = (float)(step.w * step.h) / (X_QUAL * Y_QUAL);
                        A = (float)Math.Pow( xsize * step.w / X_QUAL, apower );

                        float nh = (h0 + h1 + h2 + h3) / 4.0f;

                        float er = random.NextFloat( -1.0f, 1.0f ) * A * bumpiness;

                        nh += er;

                        Set( step.x + step.w / 2, step.y + step.h / 2, nh );
                        bfs.Enqueue( new DSStep( 1, step.x, step.y, step.w, step.h ) );
                        // take care of border diamond step
                        if ( step.x + step.w == X_QUAL ) {
                            bfs.Enqueue( new DSStep( 2, step.x + step.w, step.y, step.w, step.h ) );
                        }
                        if ( step.y + step.h == Y_QUAL ) {
                            bfs.Enqueue( new DSStep( 3, step.x, step.y + step.h, step.w, step.h ) );
                        }
                    } else if ( step.phase >= 1 ) {
                        float pA = Get( step.x + step.w / 2, step.y - step.h / 2 );
                        float pB = Get( step.x, step.y );
                        float pC = Get( step.x + step.w, step.y );
                        float pD = Get( step.x - step.w / 2, step.y + step.h / 2 );
                        float pE = Get( step.x + step.w / 2, step.y + step.h / 2 );
                        float pF = Get( step.x, step.y + step.h );

                        float nh, er;
                        //float A = (float)(step.w * step.h) / (2 * X_QUAL * Y_QUAL);
                        //float A = (float)(step.w) / (1.4f * X_QUAL);
                        float A = (float)Math.Pow( xsize * step.w / (1.41f * X_QUAL), apower );

                        float h0, h1, h2, h3;

                        if ( step.phase != 2 ) {
                            h0 = pA; h1 = pB; h2 = pC; h3 = pE;
                            nh = (h0 + h1 + h2 + h3) / 4.0f;
                            er = random.NextFloat( -1.0f, 1.0f ) * A * bumpiness;
                            nh += er;
                            Set( step.x + step.w / 2, step.y, nh );
                        }

                        if ( step.phase != 3 ) {
                            h0 = pB; h1 = pD; h2 = pE; h3 = pF;
                            nh = (h0 + h1 + h2 + h3) / 4.0f;
                            er = random.NextFloat( -1.0f, 1.0f ) * A * bumpiness;
                            nh += er;
                            Set( step.x, step.y + step.h / 2, nh );
                        }

                        if ( step.phase == 1 ) {
                            bfs.Enqueue( new DSStep( 0, step.x, step.y, step.w / 2, step.h / 2 ) );
                            bfs.Enqueue( new DSStep( 0, step.x + step.w / 2, step.y, step.w / 2, step.h / 2 ) );
                            bfs.Enqueue( new DSStep( 0, step.x, step.y + step.h / 2, step.w / 2, step.h / 2 ) );
                            bfs.Enqueue( new DSStep( 0, step.x + step.w / 2, step.y + step.h / 2, step.w / 2, step.h / 2 ) );
                        }
                    }
                }
            }
        }

        class DSStep {
            public int phase; // diamond / square
            public int x, y, w, h;
            public DSStep( int phase, int x, int y, int w, int h ) {
                this.phase = phase;
                this.x = x;
                this.y = y;
                this.w = w;
                this.h = h;
            }

        };


        public float GetHeight( float x, float y ) {
            //x += xsize / 2;
            //y += ysize / 2;
            int ix0 = (int)(X_QUAL * x / xsize);
            int iy0 = (int)(Y_QUAL * y / ysize);
            int ix1 = (int)(X_QUAL * x / xsize) + 1;
            int iy1 = (int)(Y_QUAL * y / ysize) + 1;
            float hy00 = data[ix0.Mod( X_QUAL ) + X_QUAL * iy0.Mod( Y_QUAL )];

            //return hy00;

            float hy01 = data[ix0.Mod( X_QUAL ) + X_QUAL * iy1.Mod( Y_QUAL )];
            float hy11 = data[ix1.Mod( X_QUAL ) + X_QUAL * iy1.Mod( Y_QUAL )];
            float hy10 = data[ix1.Mod( X_QUAL ) + X_QUAL * iy0.Mod( Y_QUAL )];

            float t = (float)xsize / X_QUAL;

            float hx0 = MathExt.Lerp( hy00, hy10, x.Mod( t ), t );
            float hx1 = MathExt.Lerp( hy01, hy11, x.Mod( t ), t );
            return MathExt.Lerp( hx0, hx1, y.Mod( t ), t );
        }


    }
}