function fb_renderFullscreen(shader, fb, disableblend) {
	GL.useProgram(shader);
	GL.activeTexture(GL.TEXTURE0);
	GL.bindTexture(GL.TEXTURE_2D, fb.texture);
	GL.uniform1i(shader.uniform_tex, 0);
	GL.uniform2f(shader.uniform_size, fb.size_x, fb.size_y);
	GL.enableVertexAttribArray(shader.attrib_pos);
	GL.bindBuffer(GL.ARRAY_BUFFER, vb_quad);
	GL.vertexAttribPointer(shader.attrib_pos, vb_quad.itemSize, GL.FLOAT, false, 0, 0);
	if (!disableblend) {
		GL.enable(GL.BLEND);
		GL.blendFunc(GL.SRC_ALPHA, GL.ONE_MINUS_SRC_ALPHA);
	} else {
		GL.disable(GL.BLEND);
	}
	GL.drawArrays(GL.TRIANGLE_STRIP, 0, vb_quad.numItems);
	GL.disableVertexAttribArray(shader.attrib_pos);
	GL.bindBuffer(GL.ARRAY_BUFFER, null);
}

function fb_blur(shader, dst_fb, src_fb, intermediate_fb, radius) {

	GL.disable(GL.CULL_FACE);
	GL.disable(GL.DEPTH_TEST);
	GL.disable(GL.BLEND);

	gl_setRenderTarget(intermediate_fb);
	gl_clear([0,0,0,0]);
	GL.useProgram(shader);
	GL.activeTexture(GL.TEXTURE0);
	GL.uniform1i(shader.uniform_tex, 0);
	GL.uniform1f(shader.uniform_radius, radius);
	GL.bindTexture(GL.TEXTURE_2D, src_fb.texture);
	GL.uniform2f(shader.uniform_size, src_fb.size_x, src_fb.size_y);
	GL.uniform2f(shader.uniform_dir, 1, 0);
	GL.enableVertexAttribArray(shader.attrib_pos);
	GL.bindBuffer(GL.ARRAY_BUFFER, vb_quad);
	GL.vertexAttribPointer(shader.attrib_pos, vb_quad.itemSize, GL.FLOAT, false, 0, 0);
	GL.bindBuffer(GL.ARRAY_BUFFER, vb_quad);
	GL.drawArrays(GL.TRIANGLE_STRIP, 0, vb_quad.numItems);

	gl_setRenderTarget(dst_fb);
	gl_clear([0,0,0,0]);
	GL.useProgram(shader);
	GL.activeTexture(GL.TEXTURE0);
	GL.uniform1i(shader.uniform_tex, 0);
	GL.uniform1f(shader.uniform_radius, radius);
	GL.bindTexture(GL.TEXTURE_2D, intermediate_fb.texture);
	GL.uniform2f(shader.uniform_size, intermediate_fb.size_x, intermediate_fb.size_y);
	GL.uniform2f(shader.uniform_dir, 0, 1);
	GL.enableVertexAttribArray(shader.attrib_pos);
	GL.bindBuffer(GL.ARRAY_BUFFER, vb_quad);
	GL.vertexAttribPointer(shader.attrib_pos, vb_quad.itemSize, GL.FLOAT, false, 0, 0);
	GL.bindBuffer(GL.ARRAY_BUFFER, vb_quad);
	GL.drawArrays(GL.TRIANGLE_STRIP, 0, vb_quad.numItems);


	GL.disableVertexAttribArray(shader.attrib_pos);
	GL.bindBuffer(GL.ARRAY_BUFFER, null);
}

