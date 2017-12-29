namespace Engine

module Buffer =

    open D3dWrapper

    open SharpDX
    open SharpDX.Direct3D11
    open SharpDX.DXGI
    open SharpDX.D3DCompiler

    open System.Xml


    type BufferLayoutElement =
        struct
            val Format : GfxFormat
            val Name : string
            new( format, name ) = { Format = format; Name = name; }
        end

    type BufferLayout(elements) =
        let m_elements : BufferLayoutElement[] = elements

        member this.GetElementList = m_elements

        member this.Stride with get() = m_elements |> Seq.fold (fun acc x -> (match x.Format with GfxFormat(_,size,_) -> size) + acc) 0

        member this.CreateDXInputElementList = [| for el in m_elements do yield new InputElement( el.Name, 0, (match el.Format with GfxFormat(fmt,_,_) -> fmt), 0 ) |]

        static member Load (node:XmlNode) = new BufferLayout( [| for n in node.ChildNodes do yield new BufferLayoutElement( GfxFormats.GetFormatFromString n.Name, n.Attributes.GetNamedItem("name").Value ) |] )




    [<AbstractClass>]
    type Buffer(length, stride, usage : ResourceUsage, initdata, bindFlag) =
        let m_length = length
        let m_stride = stride

        let m_bufDesc = BufferDescription(
                            BindFlags = bindFlag,
                            CpuAccessFlags = (if usage = ResourceUsage.Dynamic then CpuAccessFlags.Write else CpuAccessFlags.None),
                            OptionFlags = ResourceOptionFlags.None,
                            SizeInBytes = m_length * m_stride,
                            Usage = usage,
                            StructureByteStride = m_stride
                        )
        let m_buf = if initdata <> null then new Direct3D11.Buffer( Device.Device, initdata, m_bufDesc ) else new Direct3D11.Buffer( Device.Device, m_bufDesc )

        member private this.Size = m_length*m_stride

        member this.Lock =
            Device.Device.ImmediateContext.MapSubresource( m_buf, 0, this.Size, MapMode.WriteDiscard, Direct3D11.MapFlags.None ).Data
        member this.Unlock =
            Device.Device.ImmediateContext.UnmapSubresource( m_buf, 0 );

        member this.HwBuffer with get() = m_buf

        member this.Stride with get() = m_stride
        member this.Count with get() = m_length


    type VertexBuffer(length, stride, usage : ResourceUsage, initdata) =
        inherit Buffer(length, stride, usage, initdata, BindFlags.VertexBuffer)

        //static member NewStatic length stride (initdata:'a[]) =
        static member NewStatic length stride initdata =
            let vbDataStream = new DataStream( int stride * int length, true, true )
            vbDataStream.WriteRange( initdata )
            vbDataStream.Position <- int64 0
            new VertexBuffer( length, stride, ResourceUsage.Default, vbDataStream )

        static member NewDynamic length stride =
            new VertexBuffer( length, stride, ResourceUsage.Dynamic, null )

    type IndexBuffer(length, format : GfxFormat, usage : ResourceUsage, initdata) =
        inherit Buffer(length, (match format with GfxFormat(_, x, _) -> x), usage, initdata, BindFlags.IndexBuffer)

        let m_dxFormat = match format with GfxFormat(x, _, _) -> x

        member this.HwFormat with get() = m_dxFormat

        //static member NewStatic length stride (initdata:'a[]) =
        static member NewStatic length (format : GfxFormat) initdata =
            let vbDataStream = new DataStream( (match format with GfxFormat(_, x, _) -> x) * int length, true, true )
            vbDataStream.WriteRange( initdata )
            vbDataStream.Position <- int64 0
            new IndexBuffer( length, format, ResourceUsage.Default, vbDataStream )

        static member NewDynamic length format =
            new IndexBuffer( length, format, ResourceUsage.Dynamic, null )

    type ConstantBuffer(size, usage : ResourceUsage, initdata) =
        inherit Buffer(1, size, usage, initdata, BindFlags.ConstantBuffer)

        static member NewStatic size initdata =
            let vbDataStream = new DataStream( int size, true, true )
            vbDataStream.WriteRange( initdata )
            vbDataStream.Position <- int64 0
            new ConstantBuffer( size, ResourceUsage.Default, vbDataStream )

        static member NewDynamic size =
            new ConstantBuffer( size * 4  , ResourceUsage.Dynamic, null )

 
    type ConstantBufferSlot =
        struct
            val Size : int
            val Name : string
            new( size, name ) = { Size = size; Name = name }
        end

    type ConstantBufferData(slots) =
        let m_slots : ConstantBufferSlot[] = slots
    
        let p_computeSize = m_slots |> Array.fold (fun acc x -> acc + x.Size) 0

        let m_data : float32[] = Array.zeroCreate p_computeSize

        let m_size = p_computeSize

        let p_getOffset key =
            let mutable k = 0
            let mutable pos = 0
            while m_slots.[k].Name <> key do
                pos <- pos + m_slots.[k].Size
                k <- k + 1
            pos

        member this.Set (key, (value:float32)) = m_data.[p_getOffset key] <- value
        member this.Set (key, (values:List<float32>)) = let offs = p_getOffset key in values |> List.mapi (fun n v -> ( m_data.[offs+n] <- v ) )
        member this.Set (key, (value:Math.Vector4)) = let offs = p_getOffset key in ( m_data.[offs] <- value.x; m_data.[offs+1] <- value.y; m_data.[offs+2] <- value.z; m_data.[offs+3] <- value.w )
        member this.Set (key, (value:Math.Vector3)) = let offs = p_getOffset key in ( m_data.[offs] <- value.x; m_data.[offs+1] <- value.y; m_data.[offs+2] <- value.z )
        member this.Set (key, (value:Math.Vector2)) = let offs = p_getOffset key in ( m_data.[offs] <- value.x; m_data.[offs+1] <- value.y )

        member this.Length = m_size

        member this.WriteToCB (cb:ConstantBuffer) =
            let ds = cb.Lock 
            ds.WriteRange m_data
            cb.Unlock



        static member CreateFromLayout (layout:BufferLayout) =
            new ConstantBufferData( [| for (el:BufferLayoutElement) in layout.GetElementList do let numFloats = ( match el.Format with GfxFormat(_,size,_) -> (size/4) ) in yield new ConstantBufferSlot( numFloats, el.Name ) |] )
            

