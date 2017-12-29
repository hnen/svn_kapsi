
#light

namespace Engine

module D3dWrapper =

    open System

    open SharpDX
    open SharpDX.Direct3D
    open SharpDX.Direct3D11
    open SharpDX.DXGI
    open SharpDX.D3DCompiler
    open SharpDX.Windows

    //open VertexBuffer

    [<Literal>]
    let VERTEX_SHADER_METHOD = "VS"

    [<Literal>]
    let GEOMETRY_SHADER_METHOD = "GS"

    [<Literal>]
    let PIXEL_SHADER_METHOD = "PS"

    [<Literal>]
    let VERTEX_SHADER_PROFILE = "vs_4_0"

    [<Literal>]
    let GEOMETRY_SHADER_PROFILE = "gs_4_0"

    [<Literal>]
    let PIXEL_SHADER_PROFILE = "ps_4_0"

    type GfxFormat = GfxFormat of DXGI.Format * int * String

    module GfxFormats =
        let Uint32 = GfxFormat(DXGI.Format.R32_UInt, 4, "uint32")
        let Uint16 = GfxFormat(DXGI.Format.R16_UInt, 2, "uint16")
        let Float32RGBA = GfxFormat(DXGI.Format.R32G32B32A32_Float, 4*4, "float4")
        let Uint8RGBA = GfxFormat(DXGI.Format.R8G8B8A8_UInt, 4, "ubyte4")
        let Float32 = GfxFormat(DXGI.Format.R32_Float, 4, "float")

        let g_formats = [ Uint32; Uint16; Float32RGBA; Uint8RGBA; Float32 ]

        let GetFormatFromString str = g_formats |> List.find ( fun x -> match x with 
                                                                        | GfxFormat(_,_,q) when q = str -> true
                                                                        | _ -> false )

    type Device() =

        // let's keep things static for now
        static let mutable m_device : Direct3D11.Device = null
        static let mutable m_swapChain : DXGI.SwapChain = null
        static let mutable m_form : SharpDX.Windows.RenderForm = null

        static let mutable m_textureBackbuffer : Texture2D = null;
        static let mutable m_viewBackbuffer : RenderTargetView = null;

        static let p_handleResult (result : Result) =
            if result.Failure then failwithf "D3d error! %s" ( result.ToString() )

        static let p_createD3DWindow w h fullscreen =
            m_form <- new SharpDX.Windows.RenderForm("F#");
            m_form.Width <- w
            m_form.Height <- h

            let swapChainDesc = SharpDX.DXGI.SwapChainDescription( 
                                    BufferCount = 1,
                                    Flags = DXGI.SwapChainFlags.None,
                                    IsWindowed = not fullscreen,
                                    OutputHandle = m_form.Handle,
                                    SampleDescription = new SampleDescription(1,0),
                                    SwapEffect = SwapEffect.Discard,
                                    Usage = Usage.RenderTargetOutput,
                                    ModeDescription = new ModeDescription( w, h, Rational(60,1), Format.R8G8B8A8_UNorm )
                                    )

            let mutable device, swapChain = null, null;
            Device.CreateWithSwapChain( DriverType.Hardware, DeviceCreationFlags.Debug, swapChainDesc, &device, &swapChain ) |> p_handleResult
            m_device <- device
            m_swapChain <- swapChain
            m_textureBackbuffer <- Texture2D.FromSwapChain( m_swapChain, 0 )
            m_viewBackbuffer <- new RenderTargetView( m_device, m_textureBackbuffer )

            let ctx = m_device.ImmediateContext

            ctx.OutputMerger.SetTargets( m_viewBackbuffer ) |> ignore
            ctx.Rasterizer.SetViewports( Viewport( 0.0f, 0.0f, float32 m_form.Width, float32 m_form.Height ) )
            ctx.InputAssembler.SetPrimitiveTopology( PrimitiveTopology.TriangleStrip )

        static let p_mainLoop f = Windows.RenderLoop.Run( m_form, (fun () -> (f m_device.ImmediateContext) ) )
    
        // members

        static member Init x y fullscreen =
            p_createD3DWindow x y fullscreen

        static member Device with get() = m_device
        static member internal ViewBackbuffer with get() = m_viewBackbuffer
        static member internal SwapChain with get() = m_swapChain
        static member Form with get() = m_form

        static member HandleError = p_handleResult



