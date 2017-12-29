
#light

module Gfx

open System

open SharpDX
open SharpDX.Direct3D
open SharpDX.Direct3D11
open SharpDX.DXGI
open SharpDX.D3DCompiler
open SharpDX.Windows

//open VertexBuffer

let VERTEX_SHADER_METHOD = "VS"
let GEOMETRY_SHADER_METHOD = "GS"
let PIXEL_SHADER_METHOD = "PS"
let VERTEX_SHADER_PROFILE = "vs_4_0"
let GEOMETRY_SHADER_PROFILE = "gs_4_0"
let PIXEL_SHADER_PROFILE = "ps_4_0"


type Renderer() =

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

    static member Launch f =
        p_mainLoop f

    static member Clear =
        m_device.ImmediateContext.ClearRenderTargetView( m_viewBackbuffer, Color4(1.0f, 0.0f, 0.0f, 0.0f) )

    static member Update =
        m_swapChain.Present( 0, PresentFlags.None ) |> p_handleResult

    static member Draw numVertices =
        m_device.ImmediateContext.Draw( numVertices, 0 )

    static member Draw (vb : VertexBuffer) =
        m_device.ImmediateContext.InputAssembler.SetVertexBuffers( 0, new VertexBufferBinding( vb.HwBuffer, vb.Stride, 0 ) );
        m_device.ImmediateContext.Draw( vb.VertexCount, 0 )

    static member Device with get() = m_device

    static member HandleError = p_handleResult




and VertexBuffer(length, stride, usage : ResourceUsage, initdata) =
    let m_length = length
    let m_stride = stride

    let m_buf = new Direct3D11.Buffer(  Renderer.Device, initdata,
                                        BufferDescription(
                                            BindFlags = BindFlags.VertexBuffer,
                                            CpuAccessFlags = CpuAccessFlags.None,
                                            OptionFlags = ResourceOptionFlags.None,
                                            SizeInBytes = m_length * m_stride,
                                            Usage = ResourceUsage.Default,
                                            StructureByteStride = m_stride
                                        ) )

    member private this.Size = m_length*m_stride

    member this.Lock =
        Renderer.Device.ImmediateContext.MapSubresource( m_buf, 0, this.Size, MapMode.WriteDiscard, Direct3D11.MapFlags.None ).Data


    member this.HwBuffer with get() = m_buf

    member this.Stride with get() = m_stride
    member this.VertexCount with get() = m_length

    //static member NewStatic length stride (initdata:'a[]) =
    static member NewStatic length stride initdata =
        let vbDataStream = new DataStream( int stride * int length, true, true )
        vbDataStream.WriteRange( initdata )
        vbDataStream.Position <- int64 0
        new VertexBuffer( length, stride, ResourceUsage.Default, vbDataStream )

    static member NewDynamic length stride =
        new VertexBuffer( length, stride, ResourceUsage.Dynamic, null )


and Shader( filename, hasVS, hasGS, hasPS ) =
    let vsb = match hasVS with
                | false -> null
                | true -> D3DCompiler.ShaderBytecode.CompileFromFile( filename, VERTEX_SHADER_METHOD, VERTEX_SHADER_PROFILE,
                                                                        D3DCompiler.ShaderFlags.None, D3DCompiler.EffectFlags.None )
    let psb = match hasPS with
                | false -> null
                | true -> D3DCompiler.ShaderBytecode.CompileFromFile( filename, PIXEL_SHADER_METHOD, PIXEL_SHADER_PROFILE,
                                                                        D3DCompiler.ShaderFlags.None, D3DCompiler.EffectFlags.None )
    let gsb = match hasGS with
                | false -> null
                | true -> D3DCompiler.ShaderBytecode.CompileFromFile( filename, GEOMETRY_SHADER_METHOD, GEOMETRY_SHADER_PROFILE,
                                                                        D3DCompiler.ShaderFlags.None, D3DCompiler.EffectFlags.None )


    let m_vs = if vsb <> null then new VertexShader( Renderer.Device, vsb ) else null
    let m_gs = if gsb <> null then new GeometryShader( Renderer.Device, gsb ) else null
    let m_ps = if psb <> null then new PixelShader( Renderer.Device, psb ) else null
    //let m_vs = new VertexShader( Renderer.Device, vsb )
    //let m_ps = new PixelShader( Renderer.Device, psb )
    
    member this.CreateInputLayout elements =
        new InputLayout( Renderer.Device, ShaderSignature.GetInputSignature( vsb ), elements )

    member this.SetActive layout =
        if m_vs <> null then Renderer.Device.ImmediateContext.VertexShader.Set( m_vs )
        if m_gs <> null then Renderer.Device.ImmediateContext.GeometryShader.Set( m_gs )
        if m_ps <> null then Renderer.Device.ImmediateContext.PixelShader.Set( m_ps )
        Renderer.Device.ImmediateContext.InputAssembler.SetInputLayout( layout )





