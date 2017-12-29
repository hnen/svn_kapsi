using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using SharpDX;
using SharpDX.Direct3D;
using SharpDX.Direct3D11;
using SharpDX.DXGI;

using Buffer = SharpDX.Direct3D11.Buffer;
using Device = SharpDX.Direct3D11.Device;

namespace TerrainTest {
    class ConstantBuffer<T> {

        DeviceContext ctx;
        Buffer cbuffer;
        DataStream dataStream;

        public Buffer Buffer { get { return cbuffer; } }

        public ConstantBuffer( Device device ) {
            ctx = device.ImmediateContext;

            int size = Marshal.SizeOf( typeof(T) );

            cbuffer = new Buffer( device, new BufferDescription {
                Usage = ResourceUsage.Default,
                BindFlags = BindFlags.ConstantBuffer,
                SizeInBytes = size,
                CpuAccessFlags = CpuAccessFlags.None,
                OptionFlags = ResourceOptionFlags.None,
                StructureByteStride = 0
            } );

            dataStream = new DataStream( size, true, true );
        }

        public void SetData( T data ) {
            Marshal.StructureToPtr( data, dataStream.DataPointer, false );
            var dataBox = new DataBox( 0, 0, dataStream );
            ctx.UpdateSubresource( dataBox, cbuffer, 0 );
        }

        public void Dispose() {
            if ( dataStream != null ) {
                dataStream.Dispose();
                dataStream = null;
            }

            if ( cbuffer != null ) {
                cbuffer.Dispose();
                cbuffer = null;
            }

        }


    }
}
