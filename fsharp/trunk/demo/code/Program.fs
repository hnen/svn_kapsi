
open Engine.D3dWrapper
open Engine.Shader
open Engine.Renderer
open Engine.Buffer

open System

//open SharpDX
//open SharpDX.Direct3D
//open SharpDX.Direct3D11
//open SharpDX.DXGI


Device.Init 1280 720 false

let shaderTest : Shader = Shader.Load "data/testShader.xml"

let data =
        [| -0.5f;-0.5f; 0.0f; 0.0f;
            0.5f;-0.5f; 0.0f; 0.0f;
           -0.5f; 0.5f; 0.0f; 0.0f; 
            0.5f; 0.5f; 0.0f; 0.0f |]
let vbTest = VertexBuffer.NewStatic 4 shaderTest.VertexLayout.Stride data

let (indices : uint16[]) = [| 0us; 2us; 1us; 3us |]
let ibTest = IndexBuffer.NewStatic 4 GfxFormats.Uint16 indices

let cbDataVs = ConstantBufferData.CreateFromLayout shaderTest.VsConstantLayout
let cbVs = ConstantBuffer.NewDynamic 4



Renderer.Launch ( fun ctx ->
(
    Renderer.Clear

    cbDataVs.Set( "aspectRatio", (float32 Device.Form.Height) / (float32 Device.Form.Width) )
    cbDataVs.WriteToCB cbVs

    shaderTest.SetActive
    Device.Device.ImmediateContext.VertexShader.SetConstantBuffer( 0, cbVs.HwBuffer )
    Renderer.DrawIndexed vbTest ibTest

    Renderer.Update
) 
)
