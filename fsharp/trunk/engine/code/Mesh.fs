
namespace Engine

module Mesh =

    open D3dWrapper
    open Shader
    open Buffer

    type Mesh(vb, ib, shader) =
        let m_indexBuffer : IndexBuffer = ib
        let m_vertexBuffer : VertexBuffer = vb
        let m_shader : Shader = shader



