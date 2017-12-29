
var okkult_imgs = [
	{ filename: "okkult00", konnekt: [[0.5, 0], [0.5, 1]]},
	{ filename: "okkult01", konnekt: [[0.5, 0], [0.5, 1]]},
	{ filename: "death0", konnekt: [[0.5, 0], [0.5, 1]]},
	{ filename: "death1", konnekt: [[0.5, 0], [0.5, 1]]}
];


function okkult_init() {
	for (var i in okkult_imgs) {
		var o = okkult_imgs[i];
		p_okkult_createTeksture(o);
	}
}

function p_okkult_createTeksture(o) {
	gl_createTextureFromImage(o.filename, "data/" + o.filename + ".png", function(tex) {
		o.texture = tex;
	} );
}

function okkult_render(y, rot, death) {

	var seed = 2312;
	var pos = [0,0,10];

	var mat = mat_mul(mat_rotZ(rot), mat_translate([0,y,5]));

	if (death) {
		okkult_piir(okkult_imgs[2+Math.floor((t()) % 2)], mat, pos, seed, 0);
	} else {
		okkult_piir(okkult_imgs[Math.floor((t()) % 2)], mat, pos, seed, 0);
	}

}
/*
function okkult_kuva(img, local_to_view, rect, seed, N) {
	slot = [Math.floor(noise(seed+231)*2), Math.floor(noise(seed+2312)*2)];
	if (N >= 2) return;

	for (int y = 0; y <= 1; y+=1) {
		for (int x = 0; x <= 1; x+=1) {
			if (x != slot[0] && y != slot[1]) {
				okkult_render(img, local_to_view, [slot[0], slot[1]], seed, N);
			} else {
				okkult_kuva(img, local_to_view, [], seed+3232, N);
			}
		}
	}
}
*/

function okkult_piir(img, local_to_view, pos, seed, N) {
	var alpha = 1.0;
	var proj = mat_perspective(cam_fov * Math.PI / 180.0, 16.0/9.0, 1.0, 1000.0);
	proj = mat_mul(mat_cam(), proj);
	GL.useProgram(shader_okkult);
	GL.activeTexture(GL.TEXTURE0);
	GL.bindTexture(GL.TEXTURE_2D, img.texture.texture);
	GL.uniform1i(shader_okkult.uniform_tex, 0);
	GL.uniform1f(shader_okkult.uniform_aspect, img.texture.texture_width/ img.texture.texture_height);
	GL.uniform1f(shader_okkult.uniform_alpha, alpha);
	GL.uniformMatrix4fv(shader_okkult.uniform_local_to_screen, false, new Float32Array(mat_mul(local_to_view,proj)));
	GL.enableVertexAttribArray(shader_okkult.attrib_pos);
	GL.bindBuffer(GL.ARRAY_BUFFER, vb_quad);
	GL.vertexAttribPointer(shader_okkult.attrib_pos, vb_quad.itemSize, GL.FLOAT, false, 0, 0);
	GL.enable(GL.BLEND);
	GL.blendFunc(GL.SRC_ALPHA, GL.ONE);
	GL.drawArrays(GL.TRIANGLE_STRIP, 0, vb_quad.numItems);
	GL.disableVertexAttribArray(shader_okkult.attrib_pos);
	GL.bindBuffer(GL.ARRAY_BUFFER, null);

}


