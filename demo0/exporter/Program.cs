using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

using OpenTK;
using OpenTK.Graphics;
using OpenTK.Graphics.OpenGL;
using OpenTK.Platform.Windows;

namespace demo0 {
    class Program : GameWindow {

        string vs = @"
#version 130

precision highp float;

uniform mat4 matrix_wv;
uniform mat4 matrix_wvp;


in vec3 in_pos;
in vec3 in_normal;

out vec3 normal;

void main(void) {
    normal = (matrix_wv * vec4( in_normal, 0 )).xyz;
    gl_Position = matrix_wvp * vec4( in_pos, 1 );
}

";
        string ps = @"
#version 130

precision highp float;

uniform float alpha;

in vec3 normal;
out vec4 out_frag_color;

void main(void) {
    out_frag_color = vec4( 1,1,1,alpha );
}

";


        FMesh mesh;
        Shader shader;

        protected override void OnUpdateFrame( FrameEventArgs e ) {

        }

        Matrix4 view;
        Matrix4 viewProj;

        Texture3 dftex;

        public const int SIZE = 64;
            
        protected override void OnRenderFrame( FrameEventArgs e ) {
            GL.ClearColor( new Color4( 0, 0, 0.75f, 0 ) );
            GL.Viewport( 0, 0, Width, Height );
            GL.Clear( ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit );

            GL.Disable( EnableCap.DepthTest );
            GL.CullFace( CullFaceMode.Back );
            GL.Enable( EnableCap.CullFace );

            GL.Enable( EnableCap.Blend );
            GL.BlendFunc( BlendingFactorSrc.SrcAlpha, BlendingFactorDest.One );
            GL.BlendEquation( BlendEquationMode.FuncAdd );


            Matrix4 proj;
            float aspect = ClientSize.Width / (float)(ClientSize.Height);
            Matrix4.CreatePerspectiveFieldOfView( 45.0f / 180.0f * 3.1416f, aspect, 1.0f, 500.0f, out proj );
            Vector3 pos = new Vector3( (float)Math.Sin( Utils.Time ), 0.4f, (float)Math.Cos( Utils.Time ) ) * 3.0f ;
            view = Matrix4.LookAt( pos, new Vector3(0,0,0), new Vector3(0,1,0) );
            viewProj = view * proj;

            for ( int z = 0; z < SIZE; z++ ) {
                for ( int y = 0; y < SIZE; y++ ) {
                    for ( int x = 0; x < SIZE; x++ ) {

                        float dval = df.Field[z][y][x];
                        if ( dval < 0.0f )
                        {
                            float cbsize = 1.0f / SIZE;
                            Vector3 cpos = new Vector3(-1,-1,-1) * 0.5f + new Vector3(x,y,z) / SIZE;
                            Matrix4 obj = Matrix4.Scale( cbsize * 0.5f ) * Matrix4.CreateTranslation( cpos );
                            Matrix4 viewobj = obj * view;
                            Matrix4 worldviewproj = obj * viewProj;
                            shader.SetUniform( "matrix_wv", ref viewobj );
                            shader.SetUniform( "matrix_wvp", ref worldviewproj );
                            //shader.SetUniform( "alpha", 0.12f *(df.Field[z,y,x]));
                            //shader.SetUniform( "alpha", 0.004f / (0.01f + dval) );
                            shader.SetUniform( "alpha", 0.1f );
                            mesh.Draw();
                        }
                    }
                }
            }

            Debug.Assert( GL.GetError() == ErrorCode.NoError );

            SwapBuffers();
        }

        DistanceFieldGenerator df;
        public Program() : base(1280, 720,
            new GraphicsMode(), "F:", 0,
            DisplayDevice.Default, 3, 2,
            GraphicsContextFlags.ForwardCompatible | GraphicsContextFlags.Debug) {

            VSync = VSyncMode.On;

            shader = new Shader( vs, ps );
            //mesh = FMesh.Import( "dat/testcube.f", shader.gl_ProgramHandle );
            mesh = FMesh.Create( MeshImporter.Cube(), shader.gl_ProgramHandle );
            //mesh.BindWithShader( shader.gl_ProgramHandle, "in_pos", "in_normal" ); 

            df = new DistanceFieldGenerator( FMesh.ImportData( "dat/kakki.f" ), SIZE );
            //dftex = new Texture3( df.Field );

            //this.Close();

            Debug.Assert( GL.GetError() == ErrorCode.NoError );
        }



        [STAThread]
        public static void Main() {

            using ( Program prog = new Program() ) {

                Console.WriteLine( "GL ver: " + GL.GetString(StringName.Version) );


                prog.Run( 60.0 );
            }
            

        }

    }
}
