using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Diagnostics;
using Debug = System.Diagnostics.Debug;

using SharpDX.Direct3D11;
using SharpDX.Direct3D;
using SharpDX;
using Buffer = SharpDX.Direct3D11.Buffer;

namespace TerrainTest {

    class VertexBuffer<T> where T:struct {

        private readonly DeviceContext ctx;
        private readonly int stride;
        private readonly int count;

        Buffer buffer;

        public Buffer Buffer { get { return buffer; } }

        public VertexBufferBinding CreateBinding( int offset ) { return new VertexBufferBinding( Buffer, stride, offset ); }

        public VertexBuffer( Device device, int count ) {
            this.stride = Marshal.SizeOf( typeof( T ) );
            this.count = count;
            this.ctx = device.ImmediateContext;

            buffer = new Buffer( device, new BufferDescription {
                BindFlags = BindFlags.VertexBuffer,
                CpuAccessFlags = CpuAccessFlags.Write,
                OptionFlags = ResourceOptionFlags.None,
                SizeInBytes = stride * count,
                Usage = ResourceUsage.Dynamic,
                StructureByteStride = 0
            } );

            
        }

        public void SetData( T [] verts ) {
            Debug.Assert( verts.Length <= count );

            /*
            for ( int l = 0 ; l < verts.Length ; l++ ) {
                Marshal.StructureToPtr( verts[l], dataStream.DataPointer + stride * l, false );
            }
            var dataBox = new DataBox( 0, 0, dataStream );
            ctx.UpdateSubresource( dataBox, buffer, 0 );
            */

            /*
            var stream = new DataStream( stride * count, true, true );
            stream.WriteRange( verts );
            stream.Position = 0;

            var dataBox = new DataBox( 0, 0, stream );
            ctx.UpdateSubresource( dataBox, buffer, 0 );
            */

            var dataBox = ctx.MapSubresource( buffer, 0, verts.Length * stride, MapMode.WriteDiscard, MapFlags.None );
            dataBox.Data.WriteRange<T>( verts );
            ctx.UnmapSubresource( buffer, 0 );
        }

        public void SetActive( int stream = 0 ) {
            ctx.InputAssembler.SetVertexBuffers( 0, CreateBinding( 0 ) );
        }

        ~VertexBuffer() {
            buffer.Dispose();
        }


    }
}
