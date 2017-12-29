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

    class IndexBuffer {

        private readonly DeviceContext ctx;
        private readonly int count;

        Buffer buffer;

        public Buffer Buffer { get { return buffer; } }
        public int IndexCount { get { return count; } } 

        public IndexBuffer( Device device, int count ) {
            this.count = count;
            this.ctx = device.ImmediateContext;

            buffer = new Buffer( device, new BufferDescription {
                BindFlags = BindFlags.IndexBuffer,
                CpuAccessFlags = CpuAccessFlags.Write,
                OptionFlags = ResourceOptionFlags.None,
                SizeInBytes = count * 4, // 16-bit buffer
                Usage = ResourceUsage.Dynamic,
                StructureByteStride = 0
            } );

        }

        public void SetData( uint [] indices ) {
            Debug.Assert( indices.Length <= count );

            DataStream dataStream = new DataStream( 4 * count, true, true );

            var dataBox = ctx.MapSubresource( buffer, 0, indices.Length * 4, MapMode.WriteDiscard, MapFlags.None );
            dataBox.Data.WriteRange<uint>( indices );
            ctx.UnmapSubresource( buffer, 0 );

            dataStream.Dispose();
        }


        ~IndexBuffer() {
            buffer.Dispose();
        }

    }
}
