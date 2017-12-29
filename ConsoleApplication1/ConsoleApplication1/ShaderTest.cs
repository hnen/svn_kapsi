using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using Matrix = SharpDX.Matrix;
using Vector3 = SharpDX.Vector3;
using Vector2 = SharpDX.Vector2;
using InputElement = SharpDX.Direct3D11.InputElement;

namespace TerrainTest {
    class ShaderTest {

        [StructLayout( LayoutKind.Explicit, Size = 2*4 + 2*4 + 16*4 + 16*4 + 3*4 + 4 + 2*4 + 2*4 )]
        public struct VSConst {

            /*
            [FieldOffset( 0 )]
            public Matrix wvp;

            [FieldOffset( 16 * 4 )]
            public Matrix wv;

            [FieldOffset( 32 * 4 )]
            public Vector3 lightDir;

            [FieldOffset( 35 * 4 )]
            public float farz;
            */

            [FieldOffset( 0 )]
            public Vector2 blockPosition;

            [FieldOffset( 2*4 )]
            public Vector2 blockSize;

            [FieldOffset( 2*4 + 2*4 )]
            public Matrix vp;

            [FieldOffset( 2*4 + 2*4 + 16*4 )]
            public Matrix v;

            [FieldOffset( 2*4 + 2*4 + 16*4 + 16*4 )]
            public Vector3 lightDirView;

            [FieldOffset( 2 * 4 + 2 * 4 + 16 * 4 + 16 * 4 + 3 * 4 )]
            public int blockVertCount;

            [FieldOffset( 2 * 4 + 2 * 4 + 16 * 4 + 16 * 4 + 3 * 4 + 1 * 4 )]
            public Vector2 terrainTexSize;

            //float2 blockPosition;
            //float2 blockSize;

            //float4x4 wvp;
            //float4x4 wv;
            //float3 lightDirView;
            //float  farz;

            //int blockVertCount; // Block width in vertices


            public VSConst( Vector2 blockPositionWorld, Vector2 blockSizeWorld, Vector2 terrainTexSize, Matrix v, Matrix p, Vector3 lightDirWorld, int blockVertCount ) {
                this.vp = v * p;
                this.v = v;
                this.blockPosition = blockPositionWorld;
                this.blockSize = blockSizeWorld;
                this.lightDirView = Vector3.TransformNormal( lightDirWorld,  v );
                this.blockVertCount = blockVertCount;
                this.terrainTexSize = terrainTexSize;
            }

        }

        [StructLayout( LayoutKind.Explicit, Size = 7 * 4 )]
        public struct Vertex {
            [FieldOffset( 0 )]
            public Vector3 position;

            [FieldOffset( 3 * 4 )]
            public Vector3 normal;

            public Vertex( Vector3 position, Vector3 normal ) { this.position = position; this.normal = normal; }
        }

        public ConstantBuffer<VSConst> cbuffer;
        public Shader shader;

        public ShaderTest( Window window ) {
            cbuffer = new ConstantBuffer<VSConst>( window.Device );
            shader = new Shader( window.Device, "data/shader.fx", "VS", "PS", new InputElement [] {
                new InputElement( "POSITION", 0, SharpDX.DXGI.Format.R32G32B32_Float, 0 ),
                new InputElement( "NORMAL", 0, SharpDX.DXGI.Format.R32G32B32_Float, 3*4, 0 ),
            } );
        }




    }
}
