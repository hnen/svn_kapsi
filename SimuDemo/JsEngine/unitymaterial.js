
var p_unitymaterial_intermediate_rendertarget = null;
var p_unitymaterial_orig_framebuffer = null;


var unitymaterial_prototype = {

    SetPass: function(i) {
        GL._mat = this;
        GL._matpass = i;
        if (this.p_GetPassCount() == 2) {
            if (i == 0) {
                if (!p_unitymaterial_intermediate_rendertarget) {
                    p_unitymaterial_intermediate_rendertarget = {type: "RenderTexture"};
                    p_unitymaterial_intermediate_rendertarget.rt = gl_createRenderTexture("tmp", 1024, 1024);
                }
                p_unitymaterial_orig_framebuffer = wgl.getParameter(wgl.FRAMEBUFFER_BINDING);
                wgl.bindFramebuffer(wgl.FRAMEBUFFER, p_unitymaterial_intermediate_rendertarget.rt.framebuffer);
            } else {
                GL._mat.properties["_GrabTexture"] = p_unitymaterial_intermediate_rendertarget;
                wgl.bindFramebuffer(wgl.FRAMEBUFFER, p_unitymaterial_orig_framebuffer);
            }
        } else if (this.p_GetPassCount() == 1) {
        } else {
            throw "Don't know what to do with " + this.p_GetPassCount() + " pass material.";
        }
    },
    
    p_GetPassCount: function() {
        return this.shader.passes.length;
    }

}

var unitymaterial_properties = {
    passCount: { get: function() { return this.p_GetPassCount(); }},
};
