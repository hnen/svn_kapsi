using System;
using System.Collections.Generic;
using System.Linq;

using System.Diagnostics;


using OpenTK;
using OpenTK.Graphics.OpenGL;

namespace demo0 {
    class Shader {

        int vsHandle;
        int psHandle;


        int progHandle;

        public int gl_ProgramHandle { get { return progHandle; } }

        public Shader( string vsProgram, string psProgram ) {

            vsHandle = GL.CreateShader( ShaderType.VertexShader );
            GL.ShaderSource(vsHandle, vsProgram);
            GL.CompileShader(vsHandle);
            Console.WriteLine(GL.GetShaderInfoLog(vsHandle));

            psHandle = GL.CreateShader( ShaderType.FragmentShader );
            GL.ShaderSource(psHandle, psProgram);
            GL.CompileShader(psHandle);
            Console.WriteLine(GL.GetShaderInfoLog(psHandle));

            progHandle = GL.CreateProgram();
            GL.AttachShader( progHandle, vsHandle );
            GL.AttachShader( progHandle, psHandle );
            GL.LinkProgram(progHandle);

            Console.WriteLine(GL.GetProgramInfoLog(progHandle));

            GL.UseProgram( progHandle );

            Debug.Assert( GL.GetError() == ErrorCode.NoError );
            
        }

        private Dictionary <string, int> uniformLocationCache = new Dictionary<string,int>();

        static int activeProg = -1;

        public int GetUniformLocation( string name ) {
            if ( activeProg != progHandle )
            {
                GL.UseProgram( progHandle );
                activeProg = progHandle;
            }
            int ret;
            if ( !uniformLocationCache.TryGetValue( name, out ret ) ) {
                ret = GL.GetUniformLocation( progHandle, name );
                uniformLocationCache.Add( name, ret );
            }
            return ret;
        }


        public void SetUniform(string name, Vector4 value) { GL.Uniform4( GetUniformLocation( name ), value ); }
        public void SetUniform(string name, Vector3 value) { GL.Uniform3( GetUniformLocation( name ), value ); }
        public void SetUniform(string name, Vector2 value) { GL.Uniform2( GetUniformLocation( name ), value ); }
        public void SetUniform(string name, float value) { GL.Uniform1( GetUniformLocation( name ), value ); }
        public void SetUniform(string name, ref Matrix4 value) { GL.UniformMatrix4( GetUniformLocation( name ), false, ref value ); }

        //public Vector4 this[string name] { set { SetUniform( name, value ); } }
        //public Vector3 this[string name] { set { SetUniform( name, value ); } }
        //public Vector2 this[string name] { set { SetUniform( name, value ); } }
        //public float this[string name] { set { SetUniform( name, value ); } }
        //public Matrix4 this[string name] { set { SetUniform( name, value ); } }

    }
}
