
namespace Engine

module Renderer =


    open SharpDX
    open SharpDX.Direct3D11
    open SharpDX.DXGI

    open D3dWrapper
    open Buffer
    open Shader

    type Renderer() =
        static let p_mainLoop f = Windows.RenderLoop.Run( Device.Form, (fun () -> (f Device.Device.ImmediateContext) ) )

        static member Launch f =
            p_mainLoop f

        static member Clear =
            Device.Device.ImmediateContext.ClearRenderTargetView( Device.ViewBackbuffer, Color4(1.0f, 0.0f, 0.0f, 0.0f) )

        static member Update =
            Device.SwapChain.Present( 0, PresentFlags.None ) |> Device.HandleError

        static member Draw numVertices =
            Device.Device.ImmediateContext.Draw( numVertices, 0 )

        static member Draw (vb : VertexBuffer) =
            Device.Device.ImmediateContext.InputAssembler.SetVertexBuffers( 0, new VertexBufferBinding( vb.HwBuffer, vb.Stride, 0 ) );
            Device.Device.ImmediateContext.Draw( vb.Count, 0 )

        static member DrawIndexed (vb : VertexBuffer) (ib : IndexBuffer) =
            Device.Device.ImmediateContext.InputAssembler.SetVertexBuffers( 0, new VertexBufferBinding( vb.HwBuffer, vb.Stride, 0 ) );
            Device.Device.ImmediateContext.InputAssembler.SetIndexBuffer( ib.HwBuffer, ib.HwFormat, 0 )
            Device.Device.ImmediateContext.DrawIndexed( ib.Count, 0, 0 )
