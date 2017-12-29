using System;
using System.Collections.Generic;

using SharpDX;
using SharpDX.Direct3D11;
using SharpDX.DXGI;

namespace TerrainTest {
    class Terrain {

        public const float SPHERE_RADIUS = 6368500.0f; // ~earth radius

        public const float SIZE_X = 4000.0f;
        public const float SIZE_Y = 4000.0f;

        public const int    BLOCK_VERTS = 255;
        public const float  BLOCK_VERT_QUAL = 1.0f;

        public const int LAYERS = 4;

        private readonly Window window;

        ShaderTest shader;

        public ShaderTest Shader { get { return shader; } }

        private DSHeightmap heightMap;
        public ITerrainHeightmap HeightMap { get { return heightMap; } }

        private List<TerrainBlock> blocks = new List<TerrainBlock>();

        private Vector3 terrainGenCamPos;
        private float terrainGenGroundDist;

        private Camera cullCamera;

        public float ViewHeight { get { return Math.Max( cullCamera.Position.Y - heightMap.GetHeight(cullCamera.Position.X, cullCamera.Position.Z ), 1.0f ); } }
        //public float ViewHeight { get { return Math.Max( cullCamera.Position.Y, 1.7f ); } }
        
        // Camera eye distance to horizon (how far far plane should be)
        public float ViewDistance { get { return 160000.0f; } }

        // Ground distance to horizon (how much terrain should be generated)
        public float GroundDistance { get { return (float)Math.Acos( SPHERE_RADIUS / ( SPHERE_RADIUS + ViewHeight ) ) * SPHERE_RADIUS; } }

        public Terrain( Window window, Camera cullCamera ) {
            this.window = window;
            this.cullCamera = cullCamera;
            shader = new ShaderTest( window );

            Console.WriteLine( "generating terrain" );
            heightMap = new DSHeightmap( 4096, 4096, SIZE_X, SIZE_Y, 1.0f, (int)System.DateTime.Now.Ticks, window.Device );
            //heightMap = new ExtHeightmap();

            GC.Collect();

            blocks.Clear();
            Generate();
            //Console.Write( "generating meshes" );
            //GC.Collect();
            //terrainBlock = new TerrainBlock( window, heightMap, new Vector2( -SIZE_X * 0.5f, -SIZE_Y * 0.5f ), new Vector2( SIZE_X, SIZE_Y ), QUALITY_X, QUALITY_Y );

            /*
            int s = 2048;
            testtexture = new Texture( window.Device, s, s, Format.R32_Float );
            float [] testdata = new float[s * s];
            for ( int y = 0 ; y < s ; y++ ) {
                for ( int x = 0 ; x < s ; x++ ) {
                    testdata[y * s + x] = (float)(Math.Sin( (double)y / s * 6.28 ) + Math.Sin( (double)x / s * 6.28 )) * 50.0f;
                }
            }
            testtexture.SetData( testdata );
            */

        }

        public void Generate() {

            terrainGenGroundDist = GroundDistance;

            //float terrWidth = GroundDistance * 2.0f;
            //float terrWidth = 20000.0f;
            //float tQual = terrWidth / ((float)Math.Pow( 3.0f, LAYERS ) * (BLOCK_VERTS - 1));

            float camqual = 1.0f + (float)(cullCamera.Position.Y - heightMap.maxY - 50.0f ) * 0.01f;
            float tQual = BLOCK_VERT_QUAL * (float)Math.Max( camqual, 1.0 );
            System.Console.WriteLine( string.Format( "Camera Y = {0} => {1}", cullCamera.Position.Y, camqual ) );

            int N = 0;
            float lmult = 1.0f;
            for ( int n = 0 ; n < LAYERS ; n++ ) {
                float vertDist = tQual * lmult;
                float blockWidth = (BLOCK_VERTS-1) * vertDist;
                lmult *= 3.0f;
                for ( int y = 0 ; y < 3 ; y++ ) {
                    for ( int x = 0 ; x < 3 ; x++ ) {
                        if ( n > 0 && x == 1 && y == 1 ) continue;
                        float fx = cullCamera.Position.X + x * blockWidth - 1.5f * blockWidth;
                        float fy = cullCamera.Position.Z + y * blockWidth - 1.5f * blockWidth;
                        if ( blocks.Count <= N ) {
                            blocks.Add( new TerrainBlock( window, heightMap, new Vector2( fx, fy ), new Vector2( blockWidth, blockWidth ), BLOCK_VERTS, BLOCK_VERTS ) );
                        } else {
                            blocks[N].Translate( new Vector2( fx, fy ), new Vector2( blockWidth, blockWidth ) );
                        }
                        N++;
                    }
                }
            }

            terrainGenCamPos = cullCamera.Position;
        }

        public void PrintStats() {
            Console.WriteLine( "Camera height: " + ViewHeight + ", viewDist: " + ViewDistance + ", terraingendist: " + GroundDistance );
        }

        public void Update() {
            /*
            if ( (new Vector2( cullCamera.Position.X, cullCamera.Position.Z ) - new Vector2( terrainGenCamPos.X, terrainGenCamPos.Z )).Length() > BLOCK_VERTS * BLOCK_VERT_QUAL * 0.5f ||
                GroundDistance < terrainGenGroundDist * 0.5f || GroundDistance > terrainGenGroundDist * 2.0f ) {
            }
            */
            Generate();
        }

        public void Draw( Matrix objMat, Matrix viewMat, Matrix projMat, Vector3 lightDir, float farz ) {
            var ctx = window.Context;

            // Set renderstates /////////////////////////////////////
            RasterizerStateDescription rsDesc = new RasterizerStateDescription();
            rsDesc.FillMode = FillMode.Solid;
            rsDesc.CullMode = CullMode.Back;
            ctx.Rasterizer.State = new RasterizerState( window.Device, rsDesc );

            ctx.OutputMerger.DepthStencilState = new DepthStencilState( window.Device, new DepthStencilStateDescription {
                DepthWriteMask = SharpDX.Direct3D11.DepthWriteMask.All,
                DepthComparison = Comparison.Less,
                IsDepthEnabled = true,
                IsStencilEnabled = false
            } );

            Matrix worldViewProj = objMat * viewMat * projMat;
            Matrix worldView = objMat * viewMat;
            Vector3 lightDirView = Vector3.TransformNormal( lightDir, viewMat );
            foreach ( TerrainBlock block in blocks ) {
                //DrawBlock( block, worldViewProj, worldView, lightDirView, farz );
                DrawBlock( block, viewMat, projMat, lightDir );
            }
            

        }

        public void DrawBlock( TerrainBlock tblock, Matrix view, Matrix proj, Vector3 lightDir ) {
            var ctx = window.Context;

            // Set Buffers //////////////////////////////////////////
            //Shader.cbuffer.SetData( new ShaderTest.VSConst( (tblock.ObjMatrix * worldViewProj), worldView, lightDirView, farz ) );
            Shader.cbuffer.SetData( new ShaderTest.VSConst( tblock.Pos, tblock.Size, new Vector2( SIZE_X, SIZE_Y ), view, proj, lightDir, BLOCK_VERTS ) );
            ctx.VertexShader.SetShaderResource( 0, heightMap.GetTexture().ShaderResourceView );
            ctx.VertexShader.SetShaderResource( 1, heightMap.GetNormalmap().ShaderResourceView );
            //ctx.VertexShader.SetShaderResource( 0, testtexture.ShaderResourceView );
            ctx.VertexShader.SetSampler( 0, Texture.CreateSamplerState( window.Device, TextureAddressMode.Wrap, Filter.MinMagMipLinear ) );

            ctx.PixelShader.SetShaderResource( 0, heightMap.GetTexture().ShaderResourceView );
            ctx.PixelShader.SetShaderResource( 1, heightMap.GetNormalmap().ShaderResourceView );
            //ctx.VertexShader.SetShaderResource( 0, testtexture.ShaderResourceView );
            ctx.PixelShader.SetSampler( 0, Texture.CreateSamplerState( window.Device, TextureAddressMode.Wrap, Filter.MinMagMipLinear ) );

            ctx.VertexShader.SetConstantBuffer( 0, Shader.cbuffer.Buffer );
            ctx.PixelShader.SetConstantBuffer( 0, Shader.cbuffer.Buffer );
            //ctx.InputAssembler.SetVertexBuffers( 0, tblock.VertexBuffer.CreateBinding( 0 ) );
            ctx.InputAssembler.SetInputLayout( Shader.shader.InputLayout );
            ctx.InputAssembler.SetIndexBuffer( tblock.IndexBuffer.Buffer, Format.R32_UInt, 0 );

            // Draw! ////////////////////////////////////////////////
            Shader.shader.SetActive();
            ctx.DrawIndexed( tblock.IndexBuffer.IndexCount, 0, 0 );
        }

    }
}
