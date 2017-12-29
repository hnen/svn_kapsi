
function mesh_render(mesh, shader, local_to_view, proj) {

	GL.enable(GL.CULL_FACE);
	GL.cullFace(GL.BACK);
	GL.enable(GL.DEPTH_TEST);

	GL.useProgram(shader);
	GL.uniformMatrix4fv(shader.uniform_localtoscreen, false, new Float32Array(mat_mul(local_to_view,proj)));
	GL.uniformMatrix4fv(shader.uniform_localtoview, false, new Float32Array(local_to_view));

	GL.enableVertexAttribArray(shader.attrib_pos);
	GL.bindBuffer(GL.ARRAY_BUFFER, mesh.vb_position);
	GL.vertexAttribPointer(shader.attrib_pos, mesh.vb_position.itemSize, GL.FLOAT, false, 0, 0);

	GL.enableVertexAttribArray(shader.attrib_norm);
	GL.bindBuffer(GL.ARRAY_BUFFER, mesh.vb_normal);
	GL.vertexAttribPointer(shader.attrib_norm, mesh.vb_normal.itemSize, GL.FLOAT, false, 0, 0);

	GL.bindBuffer(GL.ELEMENT_ARRAY_BUFFER, mesh.ib);
	GL.drawElements(GL.TRIANGLES, mesh.ib.numItems, GL.UNSIGNED_INT, 0);

	GL.disableVertexAttribArray(shader.attrib_pos);
	GL.disableVertexAttribArray(shader.attrib_norm);

}

