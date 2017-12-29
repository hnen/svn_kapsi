using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Debug = System.Diagnostics.Debug;

using SharpDX.Direct3D11;
using SharpDX;

namespace TerrainTest {
    public abstract class Texture {

        protected readonly DeviceContext ctx;

        protected Texture2D texture;
        protected ShaderResourceView shaderResourceView;

        public Texture2D TextureObj { get { return texture; } }
        public ShaderResourceView ShaderResourceView { get { return shaderResourceView; } }

        protected readonly SharpDX.DXGI.Format format;

        protected readonly int width, height;

        // 4 channel RGBA mips, first mip is original data(pointer copied)
        public static byte [][] GenerateMips( byte [] data, int width, int numMips ) {
            byte [][] ret = new byte[numMips][];

            int height = data.Length / (4*width);

            ret[0] = data;            
            for ( int l = 1 ; l < numMips ; l++ ) {
                int ow = width;
                int oh = height;
                width /= 2;
                height /= 2;

                byte [] ndata = new byte[width * height * 4];
                int c = 0;
                for ( int y = 0 ; y < height ; y++ ) {
                    for ( int x = 0 ; x < width ; x++ ) {
                        int oy = y << 1;
                        int ox = x << 1;
                        int r0 = ret[l - 1][(oy * ow + ox << 2) + 0];
                        int g0 = ret[l - 1][(oy * ow + ox << 2) + 1];
                        int b0 = ret[l - 1][(oy * ow + ox << 2) + 2];
                        int a0 = ret[l - 1][(oy * ow + ox << 2) + 3];

                        int r1 = ret[l - 1][(oy * ow + (ox + 1) << 2) + 0];
                        int g1 = ret[l - 1][(oy * ow + (ox + 1) << 2) + 1];
                        int b1 = ret[l - 1][(oy * ow + (ox + 1) << 2) + 2];
                        int a1 = ret[l - 1][(oy * ow + (ox + 1) << 2) + 3];

                        int r2 = ret[l - 1][((oy + 1) * ow + ox << 2) + 0];
                        int g2 = ret[l - 1][((oy + 1) * ow + ox << 2) + 1];
                        int b2 = ret[l - 1][((oy + 1) * ow + ox << 2) + 2];
                        int a2 = ret[l - 1][((oy + 1) * ow + ox << 2) + 3];

                        int r3 = ret[l - 1][((oy+1) * ow + (ox + 1) << 2) + 0];
                        int g3 = ret[l - 1][((oy+1) * ow + (ox + 1) << 2) + 1];
                        int b3 = ret[l - 1][((oy+1) * ow + (ox + 1) << 2) + 2];
                        int a3 = ret[l - 1][((oy+1) * ow + (ox + 1) << 2) + 3];

                        byte r = (byte)((r0 + r1 + r2 + r3 + 2) >> 2);
                        byte g = (byte)((g0 + g1 + g2 + g3 + 2) >> 2);
                        byte b = (byte)((b0 + b1 + b2 + b3 + 2) >> 2);
                        byte a = (byte)((a0 + a1 + a2 + a3 + 2) >> 2);

                        ndata[c++] = r;
                        ndata[c++] = g;
                        ndata[c++] = b;
                        ndata[c++] = a;
                    }
                }
                ret[l] = ndata;
            }

            return ret;
        }


        public static SamplerState CreateSamplerState( Device device, TextureAddressMode addressMode, Filter filter ) {
            return new SamplerState( device, new SamplerStateDescription {
                AddressU = addressMode,
                AddressV = addressMode,
                AddressW = addressMode,
                Filter = filter,
                MaximumLod = 1000.0f
            } );
        }

        public Texture( Device device, int width, int height, SharpDX.DXGI.Format format ) {
            ctx = device.ImmediateContext;

            this.format = format;
            this.width = width;
            this.height = height;


        }
    }



    public class StaticTexture<T> : Texture where T : struct {



        public StaticTexture( Device device, int width, int height, SharpDX.DXGI.Format format, T[] data ) : base( device, width, height, format ) {
            T [][] ndata = new T[1][];
            ndata[0] = data;
            Init( device, width, height, format, ndata );
        }

        public StaticTexture( Device device, int width, int height, SharpDX.DXGI.Format format, T [][] data ) : base(device, width, height, format ) {
            Init( device, width, height, format, data );
        }

        private void Init( Device device, int width, int height, SharpDX.DXGI.Format format, T [][] data ) {
            int pixelSize = Marshal.SizeOf( typeof( T ) ) * 8;
            int formatSize = SharpDX.DXGI.FormatHelper.SizeOfInBits( format );
            long textureSize = formatSize * width * height;
            long providedDataSize = pixelSize * data[0].Length;

            Debug.Assert( textureSize == providedDataSize );

            DataRectangle [] texdata = new DataRectangle[data.Length];
            int dheight = height;
            for ( int i = 0 ; i < data.Length ; i++ ) {
                texdata[i] = new DataRectangle( (int)(textureSize / (8 * dheight)), new DataStream( data[i], true, true ) );
                textureSize /= 4;
                dheight /= 2;
            }

            texture = new Texture2D( device, new Texture2DDescription {
                ArraySize = 1,
                BindFlags = BindFlags.ShaderResource,
                CpuAccessFlags = 0,
                Format = format,
                Height = height,
                MipLevels = data.Length,
                OptionFlags = 0,
                SampleDescription = new SharpDX.DXGI.SampleDescription {
                    Count = 1,
                    Quality = 0
                },
                Usage = ResourceUsage.Default,
                Width = width
            }, texdata );

            shaderResourceView = new ShaderResourceView( device, texture );
        }
    }

    public class MutableTexture : Texture {

        public MutableTexture( Device device, int width, int height, SharpDX.DXGI.Format format ) : base(device, width, height, format ) {
            texture = new Texture2D( device, new Texture2DDescription {
                ArraySize = 1,
                BindFlags = BindFlags.ShaderResource,
                CpuAccessFlags = CpuAccessFlags.Write,
                Format = format,
                Height = height,
                MipLevels = 1,
                OptionFlags = 0,
                SampleDescription = new SharpDX.DXGI.SampleDescription {
                    Count = 1,
                    Quality = 0
                },
                Usage = ResourceUsage.Dynamic,
                Width = width
            } );

            shaderResourceView = new ShaderResourceView( device, texture );
        }

        public void SetData<T>( T[] data ) where T : struct {

            int pixelSize = Marshal.SizeOf( typeof( T ) ) * 8;
            int formatSize = SharpDX.DXGI.FormatHelper.SizeOfInBits( format );
            long textureSize = formatSize * width * height;
            long providedDataSize = pixelSize * data.Length;

            Debug.Assert( textureSize == providedDataSize );

            var dataBox = ctx.MapSubresource( texture, 0, (int)(textureSize / 8), MapMode.WriteDiscard, MapFlags.None );
            dataBox.Data.WriteRange<T>( data );
            ctx.UnmapSubresource( texture, 0 );

        }
    }

}
