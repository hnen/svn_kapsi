using System;
using System.Collections.Generic;

using System.Diagnostics;

using SharpDX;
namespace TerrainTest {

    class TerrainBlock {

        private readonly Window window;

        private readonly int x_qual, y_qual;

        private readonly ITerrainHeightmap terrainMap;

        private Vector2 size;
        private Vector2 pos;
        private Matrix objMatrix;

        public Matrix ObjMatrix { get { return objMatrix; } }
        public Vector2 Pos { get { return pos; } }
        public Vector2 Size { get { return size; } }

        //public VertexBuffer<ShaderTest.Vertex> VertexBuffer { get { return vb; } }
        public IndexBuffer IndexBuffer { get { return ib; } }

        IndexBuffer ib;
        VertexBuffer<ShaderTest.Vertex> vb;

        uint [] indices;
        ShaderTest.Vertex [] verts;

        public TerrainBlock( Window window, ITerrainHeightmap terrainMap, Vector2 xz, Vector2 size, int x_quality, int z_quality ) {
            this.window = window;
            this.terrainMap = terrainMap;
            this.pos = xz;
            this.size = size;
            this.x_qual = x_quality;
            this.y_qual = z_quality;
            this.objMatrix = Matrix.Translation( new Vector3( xz.X, 0, xz.Y ) );

            indices = new uint[(x_qual - 1) * (y_qual - 1) * 2 * 3];
            verts = new ShaderTest.Vertex[x_qual * y_qual];

            //vb = new VertexBuffer<ShaderTest.Vertex>( window.Device, x_quality * z_quality );
            ib = new IndexBuffer( window.Device, (x_quality - 1) * (z_quality - 1) * 2 * 3 );

            GenerateIndices();
        }

        public void Translate( Vector2 xz, Vector2 size ) {
            this.pos = xz;
            this.size = size;
            this.objMatrix = Matrix.Translation( new Vector3( xz.X, 0, xz.Y ) );
            //GenerateVertices( false );
        }

        public void GenerateIndices() {

            // index buffer
            int i_n = 0;
            for ( int y = 0 ; y < y_qual - 1 ; y++ ) {
                for ( int x = 0 ; x < x_qual - 1 ; x++ ) {
                    uint i0 = (uint)(y * x_qual + x);
                    uint i1 = (uint)(i0 + 1);
                    uint i2 =(uint)(i0 + x_qual);
                    uint i3 = (uint)(i2 + 1);
                    indices[i_n++] = i1;
                    indices[i_n++] = i0;
                    indices[i_n++] = i2;
                    indices[i_n++] = i3;
                    indices[i_n++] = i1;
                    indices[i_n++] = i2;
                }
            }
            ib.SetData( indices );
        }

        /*
        public void GenerateVertices( bool computeNormals ) {

            // vertex buffer
            float [] prevRow = new float[x_qual];
            float w = 1.0f / (x_qual - 1) * size.X;
            for ( int y = -1 ; y < y_qual ; y++ ) {
                float prevy = 0.0f;
                for ( int x = -1 ; x < x_qual ; x++ ) {
                    float wx = ((float)x / (x_qual - 1)) * size.X;
                    float wz = ((float)y / (y_qual - 1)) * size.Y;
                    float wy = terrainMap.GetHeight( wx + pos.X, wz + pos.Y );

                    if ( y >= 0 && x >= 0 ) {
                        verts[y * x_qual + x].position = new Vector3( wx, wy, wz );
                        //float dx = prevy - wy;
                        //float dz = prevRow[x] - wy;
                        float dx = prevy - terrainMap.GetHeight( wx + pos.X + w, wz + pos.Y );
                        float dz = prevRow[x] - terrainMap.GetHeight( wx + pos.X, wz + pos.Y + w );
                        verts[y * x_qual + x].normal = new Vector3( dx, 2.0f * w, dz ).Normalized();
                    }
                    if ( x >= 0 ) prevRow[x] = wy;
                    prevy = wy;
                }
            }

            if ( computeNormals ) {
                // compute normals
                for ( int n = 0 ; n < verts.Length ; n++ ) {
                    verts[n].normal = new Vector3( 0,0,0 );
                }

                for ( int n = 0 ; n < indices.Length / 3 ; n++ ) {
                    uint i0 = indices[n * 3 + 0];
                    uint i1 = indices[n * 3 + 1];
                    uint i2 = indices[n * 3 + 2];

                    Vector3 p0 = verts[i0].position;
                    Vector3 p1 = verts[i1].position;
                    Vector3 p2 = verts[i2].position;

                    Vector3 d0 = p1 - p0;
                    Vector3 d1 = p2 - p0;

                    Vector3 normal = Vector3.Cross( d0, d1 );
                    normal.Normalize();

                    verts[i0].normal += normal;
                    verts[i1].normal += normal;
                    verts[i2].normal += normal;
                }

                for ( int n = 0 ; n < verts.Length ; n++ ) {
                    verts[n].normal.Normalize();
                }
            }

            vb.SetData( verts );

        }        
        */

    }
}