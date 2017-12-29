
var texrt_flow_scene;
var texrt_flow_gradient;
var texrt_flow_blur;

function flow_init() {
}

function flow_renderScene(y, background) {
	gl_clear([0,0,0,1]);
	if (background) {
		fb_renderFullscreen(shader_blend_gs, tex_vfield_perlin, true);
	}
	var lt = p_musa_audio_el.currentTime;
	var rot = mat_mul(mat_rotX(Math.PI + Math.sin(lt*0.4)*0.3),mat_rotY(Math.PI+Math.cos(lt*0.45)*0.25));
	rot = mat_mul(rot, mat_rotZ(-1.0+lt*0.05));
	var tra = mat_translate([Math.sin(lt*0.3),y,3.8]);
	var obj = mat_mul(rot,tra);
	var per = mat_perspective(cam_fov * Math.PI / 180.0, 16.0/9.0, 1.0, 1000.0);
	mesh_render(mesh_f, shader_mesh, obj, per);
}

function renderMesh(shader, mesh, obj,tex,params) {
	var lt = p_musa_audio_el.currentTime;
//	var rot = mat_mul(mat_rotX(Math.PI + Math.sin(lt*0.4)*0.3),mat_rotY(Math.PI+Math.cos(lt*0.45)*0.25));
//	rot = mat_mul(rot, mat_rotZ(-1.0+lt*0.05));
//	var tra = mat_translate(pos);
//	var obj = tra;
	var per = mat_perspective(cam_fov * Math.PI / 180.0, 16.0/9.0, 1.0, 1000.0);
	mesh_render(mesh, shader, obj, per,tex,params);
}

function flow_computeGradient(dst, src) {
	gl_setRenderTarget(dst);
	gl_clear([0,0,0,0]);
	fb_renderFullscreen(shader_gradient, src, true);
}

function flow_renderParticles(src_grad, src_color) {
	gl_resetRenderTarget();
//	gl_clear([0,0,0,1]);
	vfield_render(src_grad, src_color, true, false);
}

