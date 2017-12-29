using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Diagnostics;

using System.Runtime.InteropServices;

using OpenTK;
using OpenTK.Graphics.OpenGL;

namespace demo0 {


    class Mesh<Vertex> : IDisposable where Vertex : struct {

        protected int vaoHandle;
        protected int vboHandle;
        protected int ibHandle;

        protected int vertexStride;

        public readonly int VertexCount;
        public int TriangleCount { get { return IndexCount / 3; } }
        public readonly int IndexCount;

        int [] indices;

        public Mesh( Vertex [] vertices, int [] indices ) {

            Console.WriteLine( "Init Mesh." );

            this.indices = indices;
            GL.GenBuffers( 1, out vboHandle );
            GL.GenBuffers( 1, out ibHandle );

            vertexStride = Marshal.SizeOf( typeof( Vertex ) );

            Console.WriteLine( string.Format( "Generated VBOs: stride {0}, verts {1}, inds {2}", vertexStride, vertices.Length, indices.Length ) );

            GL.BindBuffer( BufferTarget.ArrayBuffer, vboHandle );
            GL.BufferData<Vertex>( BufferTarget.ArrayBuffer, 
                new IntPtr( vertexStride * vertices.Length ), 
                vertices, BufferUsageHint.StaticDraw );

            GL.BindBuffer( BufferTarget.ElementArrayBuffer, ibHandle );
            GL.BufferData( BufferTarget.ElementArrayBuffer,
                new IntPtr( 4 * indices.Length ),
                indices, BufferUsageHint.StaticDraw );

            VertexCount = vertices.Length;
            IndexCount = indices.Length;

            GL.BindBuffer(BufferTarget.ArrayBuffer, 0);
            GL.BindBuffer(BufferTarget.ElementArrayBuffer, 0);

            GL.GenVertexArrays(1, out vaoHandle);

            Debug.Assert( GL.GetError() == ErrorCode.NoError );

        }

        public void BindWithShader( int shaderHandle, params string [] vertexChannels ) {
            for ( int i = 0; i < vertexChannels.Length; i++ ) {
                GL.BindAttribLocation(shaderHandle, i, vertexChannels[i]);
            }
        }

        public void Dispose() {
            GL.DeleteBuffers( 1, ref vboHandle );
            GL.DeleteBuffers( 1, ref ibHandle );
        }

        public void Draw() {
            //GL.BindBuffer( BufferTarget.ElementArrayBuffer, ibHandle );
            GL.BindVertexArray(vaoHandle);

            var err = GL.GetError();
            Debug.Assert( err == ErrorCode.NoError );

            GL.DrawElements(BeginMode.Triangles, IndexCount, DrawElementsType.UnsignedInt, IntPtr.Zero);
            err = GL.GetError();
            Debug.Assert( err == ErrorCode.NoError );
            //GL.DrawElements( BeginMode.Triangles, IndexCount, DrawElementsType.UnsignedInt, indices );

        }

    }

    // Wrapping to avoid writing the MeshImporter.Mesh.Vertex every time
    class FMesh : Mesh<MeshImporter.Mesh.Vertex> {

        private FMesh( MeshImporter.Mesh.Vertex[] vertices, int[] indices, int shaderProgramHandle ) : base( vertices, indices ) {

            Console.WriteLine( "Init FMesh." );
        
            // create VAO
            GL.BindVertexArray( vaoHandle );

            GL.EnableVertexAttribArray( 0 );
            GL.BindBuffer( BufferTarget.ArrayBuffer, vboHandle );
            GL.VertexAttribPointer( 0, 3, VertexAttribPointerType.Float, true, vertexStride, 0 );
            GL.BindAttribLocation(shaderProgramHandle, 0, "in_pos");

            GL.EnableVertexAttribArray( 1 );
            GL.BindBuffer( BufferTarget.ArrayBuffer, vboHandle );
            GL.VertexAttribPointer( 1, 3, VertexAttribPointerType.Float, false, vertexStride, 12 );
            GL.BindAttribLocation(shaderProgramHandle, 1, "in_normal");

            GL.BindBuffer(BufferTarget.ElementArrayBuffer, ibHandle);

            GL.BindVertexArray( 0 );

            Debug.Assert( GL.GetError() == ErrorCode.NoError );

        }

        public static FMesh Import( string filename, int shader ) {
            var mesh = MeshImporter.Import( filename );
            return new FMesh( mesh.verts, mesh.inds, shader );
        }
        
        public static MeshImporter.Mesh ImportData( string filename ) {
            var mesh = MeshImporter.Import( filename );
            return mesh;
        }

        public static FMesh Create( MeshImporter.Mesh importedMesh, int shaderProgramHandle ) {
            return new FMesh( importedMesh.verts, importedMesh.inds , shaderProgramHandle);
        }

    }

}
