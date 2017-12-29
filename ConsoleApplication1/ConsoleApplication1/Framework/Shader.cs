using System;
using System.Collections.Generic;
using System.Text;

using SharpDX.D3DCompiler;
using SharpDX.Direct3D;
using SharpDX.Direct3D11;
using SharpDX.DXGI;
using SharpDX.Windows;

using Device = SharpDX.Direct3D11.Device;

namespace TerrainTest {
    class Shader {

        ShaderBytecode vertexShaderByteCode;
        ShaderBytecode pixelShaderByteCode;

        VertexShader vertexShader;
        PixelShader pixelShader;

        DeviceContext ctx;

        InputElement [] vertexInput;
        InputLayout inputLayout;

        public InputLayout InputLayout { get { return inputLayout; } }

        public Shader( Device device, string fileName, string vsMethod, string psMethod, InputElement[] vertexInput ) {
            ctx = device.ImmediateContext;
            vertexShaderByteCode = ShaderBytecode.CompileFromFile( fileName, vsMethod, "vs_4_0", ShaderFlags.Debug | ShaderFlags.SkipOptimization, EffectFlags.None );
            vertexShader = new VertexShader( device, vertexShaderByteCode );
            pixelShaderByteCode = ShaderBytecode.CompileFromFile( fileName, psMethod, "ps_4_1", ShaderFlags.Debug | ShaderFlags.SkipOptimization, EffectFlags.None );
            pixelShader = new PixelShader( device, pixelShaderByteCode );

            if ( vertexInput != null ) {
                this.vertexInput = new InputElement[vertexInput.Length];
                vertexInput.CopyTo( this.vertexInput, 0 );
                this.inputLayout = new InputLayout( device, ShaderSignature.GetInputSignature( vertexShaderByteCode ), this.vertexInput );
            }
        }

        public void SetActive() {
            ctx.VertexShader.Set( vertexShader );
            ctx.PixelShader.Set( pixelShader );
        }

        ~Shader() {
            vertexShader.Dispose();
            pixelShader.Dispose();
        }

    }
}
