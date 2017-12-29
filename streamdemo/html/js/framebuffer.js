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

