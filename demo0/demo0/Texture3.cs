using System;
using System.Collections.Generic;

using OpenTK;
using OpenTK.Platform.Windows;
using OpenTK.Graphics.OpenGL;

class Texture3 {

    public int gl_TexHandle;

    public Texture3( float [][][] data ) {
        float [,,] dat = new float[data.Length, data[0].Length, data[0][0].Length];
        for( int x = 0; x < data.Length; x++ ) {
            for( int y = 0; y < data[0].Length; y++ ) {
                for ( int z = 0; z < data[0][0].Length; z++ ) {
                    dat[x,y,z] = data[x][y][z];
                }
            }
        }
        Init( dat );
    }

    private void Init( float [,,] data ) {
        gl_TexHandle = GL.GenTexture();
        
        //GL.Enable( EnableCap.Texture3D );
        GL.BindTexture( TextureTarget.Texture3D, gl_TexHandle );
        
        GL.TexParameter( TextureTarget.Texture3D, TextureParameterName.TextureMinFilter, (int)OpenTK.Graphics.OpenGL.All.Linear );
        GL.TexParameter( TextureTarget.Texture3D, TextureParameterName.TextureMagFilter, (int)OpenTK.Graphics.OpenGL.All.Linear );
        GL.TexParameter( TextureTarget.Texture3D, TextureParameterName.TextureWrapS, (int)OpenTK.Graphics.OpenGL.All.Repeat );
        GL.TexParameter( TextureTarget.Texture3D, TextureParameterName.TextureWrapT, (int)OpenTK.Graphics.OpenGL.All.Repeat );
        GL.TexParameter( TextureTarget.Texture3D, TextureParameterName.TextureWrapR, (int)OpenTK.Graphics.OpenGL.All.Repeat );

        GL.TexImage3D( TextureTarget.Texture3D, 0, PixelInternalFormat.R32f, data.GetLength(0), data.GetLength(1), data.GetLength(2), 0, PixelFormat.Red, PixelType.Float, data );
    }



}
