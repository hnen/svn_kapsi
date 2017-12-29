
var teksti_list = [
	{filename: "text0.png"},
	{filename: "text1.png"}
];


function teksti_init() {
	for (var t in teksti_list) {
		var o = teksti_list[t];
		p_teksti_createTeksture(o);
	}
}

function p_teksti_createTeksture(o) {
	gl_createTextureFromImage(o.filename, "data/" + o.filename, function(tex) {
		o.texture = tex;
	} );
}

function teksti_render(pos, normal, up, N, T, range, scale,size, seed, straight) {

	GL.disable(GL.DEPTH_TEST);
	GL.disable(GL.CULL_FACE);
	GL.disable(GL.BLEND);

	var Z = vec3_normalized(normal);
	var X = vec3_normalized(vec3_cross(Z, up));
	var Y = vec3_cross(X,Z);
	var orient = mat_orient(X,Y,Z);
	var tran = mat_translate(pos);

	for (var n = 0; n < N; n+=1) {
		var _T = noise(seed*32342+n*231) * (1.0 + noise(n*213+seed*21)) - 2.0 + T;
		var mod = 1.0 + noise(seed*32131+n*2321) * 2.0;
		var phase = Math.floor(_T/mod) + n*231 + seed*2313;
		_T = _T % mod;
		var tex_id = Math.floor(noise(phase) * teksti_list.length);
		var a = 1.5 + (Math.sin(t()*4+noise(phase*2)*6.28)*3) * 0.25;
		var ang = 0;
		if (straight) {
			ang = noise(phase*4) * 6.28;
		} else {
			ang = n / N * 6.28;
		}
		var dirvec = [Math.sin(ang), Math.cos(ang)];
		var rad = range[0] + noise(phase*3)*noise(phase*3) * (range[1] - range[0]);
		var radt = mat_translate([dirvec[0]*rad, dirvec[1]*rad,0]);
		var mat_orient2;
		if (straight) {
			var dirr = vec3_normalized([noise(phase*5), noise(phase*6), noise(phase*7)]);
			mat_orient2 = mat_orient([dirr[0], dirr[1], 0], [dirr[1], -dirr[0], 0], [0,0,1]);
			//mat_orient2 = mat_identity();
		} else {
			mat_orient2 = mat_orient([dirvec[0], dirvec[1], 0], [dirvec[1], -dirvec[0], 0], [0,0,1]);
		}
		if (_T > 0)
		{
			if (_T > mod - 1.0) {
				a *= 1.0 - (_T - mod + 1.0);
			}
			var mat = mat_mul(mat_mul(mat_orient2, mat_mul(radt, mat_mul(orient, tran))), mat_scale([scale,scale,scale]));
			mat = mat_mul(mat_scale([size,size,size]), mat);
			teksti_rendertekst(teksti_list[tex_id], mat, _T, a);
		}
	}


}

function teksti_rendertekst(text, local_to_view, t, alpha) {
	var proj = mat_perspective(cam_fov * Math.PI / 180.0, 16.0/9.0, 1.0, 1000.0);
	proj = mat_mul(mat_cam(), proj);
	GL.useProgram(shader_tekst);
	GL.activeTexture(GL.TEXTURE0);
	GL.bindTexture(GL.TEXTURE_2D, text.texture.texture);
	GL.uniform1i(shader_tekst.uniform_tex, 0);
	GL.uniform2f(shader_tekst.uniform_size, text.texture.texture_width, text.texture.texture_height);
	GL.uniform1f(shader_tekst.uniform_t, t);
	GL.uniform1f(shader_tekst.uniform_alpha, alpha);
	GL.uniformMatrix4fv(shader_tekst.uniform_local_to_screen, false, new Float32Array(mat_mul(local_to_view,proj)));
	GL.enableVertexAttribArray(shader_tekst.attrib_pos);
	GL.bindBuffer(GL.ARRAY_BUFFER, vb_quad);
	GL.vertexAttribPointer(shader_tekst.attrib_pos, vb_quad.itemSize, GL.FLOAT, false, 0, 0);
	GL.enable(GL.BLEND);
	GL.blendFunc(GL.SRC_ALPHA, GL.ONE);
	GL.drawArrays(GL.TRIANGLE_STRIP, 0, vb_quad.numItems);
	GL.disableVertexAttribArray(shader_tekst.attrib_pos);
	GL.bindBuffer(GL.ARRAY_BUFFER, null);
}


