function meshrenderer_render(obj, camera) {
	GL.enable(GL.CULL_FACE);
	GL.cullFace(GL.FRONT);
	GL.enable(GL.DEPTH_TEST);

	var mat = obj.MeshRenderer.sharedMaterial;
	var shader = mat.shader;

	var local_to_proj = mat_mul(obj.transform.localToWorldMatrix, mat_mul(camera.transform.worldToLocalMatrix, camera.camera.mat_proj));
	var mesh = obj.MeshFilter.mesh;

	meshrenderer_prepareShader(shader, local_to_proj, mesh.vb_vertices, mesh.vb_normals,
		mesh.vb_uv);

	GL.bindBuffer(GL.ELEMENT_ARRAY_BUFFER, mesh.ib);
	GL.drawElements(GL.TRIANGLES, mesh.ib.numItems, GL.UNSIGNED_INT, 0);
}


function meshrenderer_prepareShader(shader, local_to_proj, vb_vertices,
	vb_normals, vb_uvs) {
	GL.useProgram(shader.shader);
	GL.uniformMatrix4fv(shader.shader.uniform_localtoscreen, false, new Float32Array(
		local_to_proj));

	GL.enableVertexAttribArray(shader.shader.attrib_pos);
	GL.bindBuffer(GL.ARRAY_BUFFER, vb_vertices);
	GL.vertexAttribPointer(shader.shader.attrib_pos, vb_vertices.itemSize, GL.FLOAT,
		false, 0, 0);

	if (shader.shader.attrib_nrm != -1) {
		if (vb_normals) {
			GL.enableVertexAttribArray(shader.shader.attrib_nrm);
			GL.bindBuffer(GL.ARRAY_BUFFER, vb_normals);
			GL.vertexAttribPointer(shader.shader.attrib_nrm, vb_normals.itemSize, GL.FLOAT,
				false, 0, 0);
		} else {
			GL.disableVertexAttribArray(shader.shader.attrib_nrm);
			console.error('Shader ' + shader.name +
				' wants normals, but none supplied.')
		}
	}

	if (shader.shader.attrib_uv != -1) {
		if (vb_uvs) {
			GL.enableVertexAttribArray(shader.shader.attrib_uv);
			GL.bindBuffer(GL.ARRAY_BUFFER, vb_uvs);
			GL.vertexAttribPointer(shader.shader.attrib_uv, vb_uvs.itemSize, GL.FLOAT,
				false, 0, 0);
		} else {
			GL.disableVertexAttribArray(shader.shader.attrib_uv);
			console.error('Shader ' + shader.name +
				' wants normals, but none supplied.')
		}
	}
}
