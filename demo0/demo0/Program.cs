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

uniform mat4 matrix_wv_inv;
uniform mat4 matrix_wv;
uniform mat4 matrix_wvp;


in vec3 in_pos;
in vec3 in_normal;

//out vec3 viewDir;
out vec3 objPos;
out vec3 objPosWorld;


void main(void) {
    //viewDir = normalize( (matrix_wv * vec4( in_pos, 1 )).xyz );
    //viewDir = (matrix_wv_inv * vec4(viewDir,0)).xyz;
	objPosWorld = in_pos;
    objPos = (in_pos + 1.0f) * 0.5f; // todo supply mesh bounds etc
    gl_Position = matrix_wvp * vec4( in_pos, 1 );
}

";
        string ps = @"
#version 130

precision highp float;

uniform float alpha;

uniform sampler3D df0;
uniform sampler3D df1;

uniform mat4 matrix_wv_inv;
uniform mat4 matrix_wv;

uniform float interp;
uniform float T;

in vec3 viewDir;
in vec3 objPos;
in vec3 objPosWorld;
out vec4 out_frag_color;

#define EPSILON 0.01f

float sampledf( vec3 pos ) {
	//return texture( df0, pos ).x - 0.00005f;
	return interp * (texture( df0, pos ).x - 0.025f) + (1.0f - interp) * (texture( df1, pos ).x - 0.025f);// + sin( pos.x * 15.0f + T ) * 0.01f + cos( pos.z * 13.0f + T * 1.5f ) * 0.01f;
	//return ((1.0f - interp) * texture( df1, pos ).x + interp) * (interp * texture( df0, pos ).x + (1.0f - interp));// + (sin( pos.x * 15.0f + T ) + cos( pos.z * 13.0f + T * 1.5f )) * 0.1f;
}

void main(void) {
    vec3 viewDir = normalize( (matrix_wv * vec4( objPosWorld, 1 )).xyz );
    viewDir = (matrix_wv_inv * vec4(viewDir,0)).xyz;

    vec3 vDir = normalize(viewDir);


    float dist = 0.0f;
    vec3 i = objPos;
    float err = 1.0f;
    while( err > 1.0f/(256.0f) && i.x >= -EPSILON && i.x < 1+EPSILON && i.y >= -EPSILON && i.y < 1+EPSILON && i.z >= -EPSILON && i.z < 1+EPSILON ) {
		float sample = sampledf(i);
        err = sample * 0.95f;
        i = i + vDir * err;
        dist = dist + err;
    }

	float d = 1.0f / 128.0f;
    float sx0 = sampledf( i + vec3(1,0,0) * d );
    float sy0 = sampledf( i + vec3(0,1,0) * d );
    float sz0 = sampledf( i + vec3(0,0,1) * d );

    float sx1 = sampledf( i - vec3(1,0,0) * d );
    float sy1 = sampledf( i - vec3(0,1,0) * d );
    float sz1 = sampledf( i - vec3(0,0,1) * d );

	vec3 normal = normalize( (matrix_wv * vec4( vec3(sx0-sx1,sy0-sy1,sz0-sz1), 0 )).xyz );
	
	bool inside = i.x >= 0 && i.x < 1 && i.y >= 0 && i.y < 1 && i.z >= 0 && i.z < 1;	

	float br = normal.z;
    //float br = 1.0f / (1.0f + dist);
    //float br = 1.0f / (1.0f + texture3D( df, objPos ).x * 10.0f);
	//float br = sample;
    out_frag_color = vec4( br, br, br, inside );
	//out_frag_color = vec4( br, br, br, 1.0f );
	//out_frag_color = vec4( 1,1,1, alpha );
}

";

        FMesh mesh;
        Shader shader;

        protected override void OnUpdateFrame( FrameEventArgs e ) {

        }

        Matrix4 view;
        Matrix4 viewProj;

        Texture3 dftex0;
        Texture3 dftex1;

        public const int SIZE = 128;
            
        protected override void OnRenderFrame( FrameEventArgs e ) {
            GL.ClearColor( new Color4( 0, 0, 0.75f, 0 ) );
            GL.Viewport( 0, 0, Width, Height );
            GL.Clear( ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit );

            GL.Disable( EnableCap.DepthTest );
            GL.CullFace( CullFaceMode.Back );
            GL.Enable( EnableCap.CullFace );

            GL.Enable( EnableCap.Blend );
            GL.BlendFunc( BlendingFactorSrc.SrcAlpha, BlendingFactorDest.OneMinusSrcAlpha );
            GL.BlendEquation( BlendEquationMode.FuncAdd );

            Matrix4 proj;
            float aspect = ClientSize.Width / (float)(ClientSize.Height);
            Matrix4.CreatePerspectiveFieldOfView( 45.0f / 180.0f * 3.1416f, aspect, 0.1f, 500.0f, out proj );
            Vector3 pos = new Vector3( (float)Math.Sin( Utils.Time * 0.1f ), (float)Math.Sin( Utils.Time * 0.4f * 0.1f ) * 0.5f, (float)Math.Cos( Utils.Time * 0.1f ) ) * 1.5f ;
            view = Matrix4.LookAt( pos, new Vector3(0,0,0), new Vector3(0,1,0) );
            viewProj = view * proj;

			/*
            for ( int z = 0; z < SIZE; z++ ) {
                for ( int y = 0; y < SIZE; y++ ) {
                    for ( int x = 0; x < SIZE; x++ ) {

                        float dval = df0.Field[z][y][x] - 0.05f;
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
                            //shader.SetUniform( "alpha", dval * 0.5f );
							shader.SetUniform( "alpha", 0.5f );
                            mesh.Draw();
                        }
                    }
                }
            }
			*/

            Vector3 cpos = Vector3.Zero;
            Matrix4 obj = Matrix4.Scale( 0.5f ) * Matrix4.CreateTranslation( cpos );
            Matrix4 viewobj = obj * view;
            Matrix4 worldviewproj = obj * viewProj;
            Matrix4 viewobjinv = viewobj;
            viewobjinv.Invert();

            GL.ActiveTexture( TextureUnit.Texture0 );
            GL.BindTexture( TextureTarget.Texture3D, dftex0.gl_TexHandle );
            GL.ActiveTexture( TextureUnit.Texture1 );
            GL.BindTexture( TextureTarget.Texture3D, dftex1.gl_TexHandle );

            shader.SetUniform( "df0", 0 );
            shader.SetUniform( "df1", 1 );
            shader.SetUniform( "matrix_wv", ref viewobj );
            shader.SetUniform( "matrix_wv_inv", ref viewobjinv );
            shader.SetUniform( "matrix_wvp", ref worldviewproj );
            shader.SetUniform( "alpha", 1.0f );
            shader.SetUniform( "interp", (float)((Math.Sin( Utils.Time * 1.0f ) + 1.0f) * 0.5f) );
			//shader.SetUniform( "interp", 1.0f );
			shader.SetUniform( "T", Utils.Time );
            mesh.Draw();

            Debug.Assert( GL.GetError() == ErrorCode.NoError );

            SwapBuffers();
        }

        DistanceFieldGenerator3 df0;
        DistanceFieldGenerator3 df1;

        public Program() : base(1280, 720,
            new GraphicsMode(), "F:", 0,
            DisplayDevice.Default, 3, 2,
            GraphicsContextFlags.ForwardCompatible | GraphicsContextFlags.Debug) {

            VSync = VSyncMode.On;

            shader = new Shader( vs, ps );
            //mesh = FMesh.Import( "dat/testcube.f", shader.gl_ProgramHandle );
            mesh = FMesh.Create( MeshImporter.Cube(), shader.gl_ProgramHandle );
            //mesh.BindWithShader( shader.gl_ProgramHandle, "in_pos", "in_normal" ); 

            df0 = new DistanceFieldGenerator3( FMesh.ImportData( "dat/kakki.f" ), SIZE );
            //df0 = new DistanceFieldGenerator2( FMesh.ImportData( "dat/testcube.f" ), SIZE );
            dftex0 = new Texture3( df0.Field );

			df1 = new DistanceFieldGenerator3( FMesh.ImportData( "dat/sphere.f" ), SIZE );
            dftex1 = new Texture3( df1.Field );

            //this.Close();1

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
