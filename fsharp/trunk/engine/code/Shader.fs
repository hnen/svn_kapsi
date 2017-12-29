
namespace Engine

module Shader =

    open D3dWrapper
    open Buffer

    open SharpDX
    open SharpDX.Direct3D11
    open SharpDX.DXGI
    open SharpDX.D3DCompiler

    open System.Xml
    open System.IO

    type Shader( filename, vs:Option<string>, gs:Option<string>, ps:Option<string>, vertexLayout:Option<BufferLayout>, vsConstantLayout:Option<BufferLayout>, gsConstantLayout:Option<BufferLayout>, psConstantLayout:Option<BufferLayout>) =
        let vsb = match vs with
                    | None -> null
                    | Some vs -> D3DCompiler.ShaderBytecode.CompileFromFile( filename, vs, VERTEX_SHADER_PROFILE,
                                                                            D3DCompiler.ShaderFlags.None, D3DCompiler.EffectFlags.None )
        let psb = match ps with
                    | None  -> null
                    | Some ps -> D3DCompiler.ShaderBytecode.CompileFromFile( filename, ps, PIXEL_SHADER_PROFILE,
                                                                            D3DCompiler.ShaderFlags.None, D3DCompiler.EffectFlags.None )
        let gsb = match gs with
                    | None -> null
                    | Some gs -> D3DCompiler.ShaderBytecode.CompileFromFile( filename, gs, GEOMETRY_SHADER_PROFILE,
                                                                            D3DCompiler.ShaderFlags.None, D3DCompiler.EffectFlags.None )
    
        let m_vertexLayout = vertexLayout
        let m_dxVertexLayout = match m_vertexLayout with
                                | Some(layout) -> new InputLayout( Device.Device, ShaderSignature.GetInputSignature( vsb ), layout.CreateDXInputElementList )
                                | None -> null

        let m_vsConstantLayout = vsConstantLayout
        let m_gsConstantLayout = gsConstantLayout
        let m_psConstantLayout = psConstantLayout


        let m_vs = if vsb <> null then new VertexShader( Device.Device, vsb ) else null
        let m_gs = if gsb <> null then new GeometryShader( Device.Device, gsb ) else null
        let m_ps = if psb <> null then new PixelShader( Device.Device, psb ) else null

        member this.SetActive =
            if m_vs <> null then Device.Device.ImmediateContext.VertexShader.Set( m_vs )
            if m_gs <> null then Device.Device.ImmediateContext.GeometryShader.Set( m_gs )
            if m_ps <> null then Device.Device.ImmediateContext.PixelShader.Set( m_ps )
            Device.Device.ImmediateContext.InputAssembler.SetInputLayout( m_dxVertexLayout )

        static member LoadFX filename layout = 
            new Shader( filename, Some( VERTEX_SHADER_METHOD ), None, Some( PIXEL_SHADER_METHOD ), layout, None, None, None )

        member this.VertexLayout with get() = m_vertexLayout.Value
        member this.VsConstantLayout with get() = m_vsConstantLayout.Value
        member this.GsConstantLayout with get() = m_gsConstantLayout.Value
        member this.PsConstantLayout with get() = m_psConstantLayout.Value

        // Load from XML
        static member Load filename =
            let file, doc = (File.OpenText( filename ), new XmlDocument()) in
                doc.LoadXml( file.ReadToEnd() );
                let node = doc.FirstChild
                // Load file name
                let filename = match node.Attributes.GetNamedItem( "code" ) with
                                | null -> raise ( System.FormatException( "Shader tag does not have \"code\" attribute." ) )
                                | x -> x
                let SelectMethod (x:XmlNode) = if x <> null && x.Attributes <> null then match x.Attributes.GetNamedItem( "method" ) with
                                                                                                | null -> raise( System.FormatException( "Shader " + x.Name + " doesn't have a method attribute" ) )
                                                                                                | x -> Some( x.Value )
                                               else None

                // Get method nodes
                let nodes = [ "//vs"; "//gs"; "//ps" ] |> List.map ( fun x -> node.SelectSingleNode( x ) )

                // Get method names
                let vsMethod, gsMethod, psMethod = let list = nodes |> List.map ( fun x -> SelectMethod x ) in ( list.Item 0, list.Item 1, list.Item 2 )

                // Read vertex layout
                let vertexLayoutNode = (nodes.Item 0).SelectSingleNode( "//layout" );
                let vertexLayout = if vertexLayoutNode <> null then Some( BufferLayout.Load vertexLayoutNode ) else None

                // Read constant buffer layouts
                let vsConstantBufferNode = (nodes.Item 0).SelectSingleNode( "//cbuffer" );
                let vsConstantBufferLayout = if vsConstantBufferNode <> null then Some( BufferLayout.Load vsConstantBufferNode ) else None
                let gsConstantBufferNode = (nodes.Item 0).SelectSingleNode( "//cbuffer" );
                let gsConstantBufferLayout = if gsConstantBufferNode <> null then Some( BufferLayout.Load gsConstantBufferNode ) else None
                let psConstantBufferNode = (nodes.Item 0).SelectSingleNode( "//cbuffer" );
                let psConstantBufferLayout = if psConstantBufferNode <> null then Some( BufferLayout.Load psConstantBufferNode ) else None

                new Shader( filename.Value, vsMethod, gsMethod, psMethod, vertexLayout, vsConstantBufferLayout, gsConstantBufferLayout, psConstantBufferLayout )

