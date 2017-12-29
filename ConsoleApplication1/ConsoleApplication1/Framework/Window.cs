using System;
using System.Collections.Generic;

using SharpDX;
using SharpDX.D3DCompiler;
using SharpDX.Direct3D;
using SharpDX.Direct3D11;
using SharpDX.DXGI;
using SharpDX.Windows;

using Device = SharpDX.Direct3D11.Device;

namespace TerrainTest {
    class Window : IDisposable {

        Device device;
        SwapChain swapChain;
        RenderTargetView renderTargetView;
        DepthStencilView depthStencilView;
        RenderForm form;
        Texture2D backBufferTexture;
        Texture2D depthStencilTexture;

        public Device Device { get { return device; } }
        public DeviceContext Context { get { return device.ImmediateContext; } }
        public SwapChain SwapChain { get { return swapChain; } }
        public RenderForm Form { get { return form; } }
        public RenderTargetView RenderTargetView { get { return renderTargetView; } }
        public DepthStencilView DepthStencilView { get { return depthStencilView; } }

        public void Init() {

            form = new RenderForm( "SharpDX - MiniTri Direct3D 11 Sample" );

            form.Width = 1280;
            form.Height = 720;

            var desc = new SwapChainDescription() {
                BufferCount = 1,
                ModeDescription =
                    new ModeDescription( form.ClientSize.Width, form.ClientSize.Height,
                                        new Rational( 60, 1 ), Format.R8G8B8A8_UNorm ),
                IsWindowed = true,
                OutputHandle = form.Handle,
                SampleDescription = new SampleDescription( 1, 0 ),
                SwapEffect = SwapEffect.Discard,
                Usage = Usage.RenderTargetOutput
            };

            Device.CreateWithSwapChain( DriverType.Hardware, DeviceCreationFlags.Debug, desc, out device, out swapChain );
            var context = device.ImmediateContext;

            Factory factory = swapChain.GetParent<Factory>();
            factory.MakeWindowAssociation( form.Handle, WindowAssociationFlags.IgnoreAll );

            // render target buffer
            backBufferTexture = Texture2D.FromSwapChain<Texture2D>( swapChain, 0 );
            renderTargetView = new RenderTargetView( device, backBufferTexture );


            // depth buffer
            depthStencilTexture = new Texture2D( device, new Texture2DDescription {
                ArraySize = 1,
                BindFlags = BindFlags.DepthStencil,
                CpuAccessFlags = CpuAccessFlags.None,
                Format = Format.D24_UNorm_S8_UInt,
                Height = form.ClientSize.Height,
                MipLevels = 1,
                OptionFlags = ResourceOptionFlags.None,
                SampleDescription = new SampleDescription( 1, 0 ),
                Usage = ResourceUsage.Default,
                Width = form.ClientSize.Width
            } );
            depthStencilView = new DepthStencilView( device, depthStencilTexture );

            // render states
            context.InputAssembler.SetPrimitiveTopology( PrimitiveTopology.TriangleList );
            context.Rasterizer.SetViewports( new Viewport( 0, 0, form.ClientSize.Width, form.ClientSize.Height, 0.0f, 1.0f ) );
            context.OutputMerger.SetTargets( depthStencilView, renderTargetView );
            //context.OutputMerger.SetTargets( renderTargetView );

        }


        public void Dispose() {
            if ( renderTargetView != null ) {
                renderTargetView.Dispose();
                renderTargetView = null;
            }
            if ( swapChain != null ) {
                swapChain.Dispose();
                swapChain = null;
            }
            if ( device != null ) {
                device.Dispose();
                device = null;
            }
        }


        ~Window() {
            Dispose();
        }

    }
}
