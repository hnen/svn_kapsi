
function meshrenderer_renderWithCamera(obj, camera) {

	var view = camera.transform.cached_worldToLocalMatrix.slice();
	var proj = camera.camera.mat_proj.slice();

	// quite an ugly fix to flip the Z coordinate in view space 'cause it's the same in unity :D
	// should find a real mathematical explanation for this.....

	proj[10] = -proj[10];
	proj[11] = -proj[11];
	view[2] = -view[2];
	view[6] = -view[6];
	view[10] = -view[10];
	view[14] = -view[14];

	var local_to_view = mat_mul(obj.transform.cached_localToWorldMatrix, view);
	var local_to_proj = mat_mul(obj.transform.cached_localToWorldMatrix, mat_mul(view, proj));
	var world_to_view = view;
	var mesh = obj.MeshFilter.mesh;

	if (!mesh) {
		mesh = obj.MeshFilter.sharedMesh;
	}

	meshrenderer_renderMesh(mesh, obj.MeshRenderer.sharedMaterials, local_to_view, local_to_proj, world_to_view)

}

function meshrenderer_renderMesh(mesh, mats, local_to_view, local_to_proj, world_to_view) {
	meshrenderer_prepareGlState();
	for (var i = 0; i < mesh.ibs.length; i++) {
		var mat = mats[i];
		var shader_asset = mat.shader;
		for (var j = 0; j < shader_asset.shaders.length; j++) {
			meshrenderer_renderMeshBuffer(mesh, mats, i, j, local_to_view, local_to_proj, world_to_view);
		}
	}
}

function meshrenderer_renderMeshBuffer(mesh, mats, submesh, pass, local_to_view, local_to_proj, world_to_view) {
	var i = submesh;
	var j = pass;

	var mat = mats[i];
	var shader_asset = mat.shader;
	var ib = mesh.ibs[i];

	var shader = shader_asset.shaders[j];

	meshrenderer_prepareShader(shader, local_to_proj, local_to_view, world_to_view, mesh.vb_vertices, mesh.vb_normals,
		mesh.vb_uv);
	meshrenderer_applyProperties(mat, shader);

	wgl.bindBuffer(wgl.ELEMENT_ARRAY_BUFFER, ib);
	wgl.drawElements(wgl.TRIANGLES, ib.numItems, wgl.UNSIGNED_INT, 0);
}


function meshrenderer_prepareGlState() {
	wgl.enable(wgl.CULL_FACE);
	wgl.cullFace(wgl.FRONT);
	wgl.enable(wgl.DEPTH_TEST);

}

function meshrenderer_applyProperties(mat, shader) {
	var props = mat.properties;
	mat._textureCounter = 0;
	for(var propName in props) {
		var propValue = props[propName];
		meshrenderer_applyProperty(mat, shader, propName, propValue);
	}

}

function meshrenderer_applyProperty(mat, shader, propName, propValue) {
	if (!shader.hasOwnProperty('uniform' + propName)) {
		shader['uniform' + propName] = wgl.getUniformLocation(shader, propName);
	}
	var uloc = shader['uniform' + propName];

	if (Array.isArray(propValue)) {
		if (propValue.length == 4) {
			wgl.uniform4f(uloc, propValue[0], propValue[1], propValue[2], propValue[3]);
		} else if (propValue.length == 3) {
			wgl.uniform3f(uloc, propValue[0], propValue[1], propValue[3]);
		} else if (propValue.length == 2) {
			wgl.uniform2f(uloc, propValue[0], propValue[1]);
		} else if (propValue.length == 1) {
			wgl.uniform1f(uloc, propValue[0]);
		} else {
			throw "only vector-length float values supported yet";
		}
	} else if(propValue.hasOwnProperty('type')) {
		if (propValue.type == "RenderTexture") {
			wgl.activeTexture(wgl.TEXTURE0 + mat._textureCounter);
			wgl.bindTexture(wgl.TEXTURE_2D, propValue.rt.texture);
			wgl.uniform1i(uloc, mat._textureCounter);
			mat._textureCounter++;
		} else if (propValue.type == "Texture2D") {
			wgl.activeTexture(wgl.TEXTURE0 + mat._textureCounter);
			wgl.bindTexture(wgl.TEXTURE_2D, propValue.texture);
			wgl.uniform1i(uloc, mat._textureCounter);
			mat._textureCounter++;
		} else {
			throw "unsupported property type \"" + propValue.type + "\"";
		}
	} else {
		throw "non-array properties not supported yet";
	}

}


function meshrenderer_prepareShader(shader, local_to_proj, local_to_view, world_to_view, vb_vertices,
	vb_normals, vb_uvs) {
	wgl.useProgram(shader);
	wgl.uniformMatrix4fv(shader.uniform_localtoscreen, false, new Float32Array(
		local_to_proj));
	wgl.uniformMatrix4fv(shader.uniform_localtoview, false, new Float32Array(
		local_to_view));
	wgl.uniformMatrix4fv(shader.uniform_worldtoview, false, new Float32Array(
		world_to_view));

	wgl.enableVertexAttribArray(shader.attrib_pos);
	wgl.bindBuffer(wgl.ARRAY_BUFFER, vb_vertices);
	wgl.vertexAttribPointer(shader.attrib_pos, vb_vertices.itemSize, wgl.FLOAT,
		false, 0, 0);

	if (shader.attrib_nrm != -1) {
		if (vb_normals) {
			wgl.enableVertexAttribArray(shader.attrib_nrm);
			wgl.bindBuffer(wgl.ARRAY_BUFFER, vb_normals);
			wgl.vertexAttribPointer(shader.attrib_nrm, vb_normals.itemSize, wgl.FLOAT,
				false, 0, 0);
		} else {
			wgl.disableVertexAttribArray(shader.shader.attrib_nrm);
			console.error('Shader wants normals, but none supplied.')
		}
	}

	if (shader.attrib_uv != -1) {
		if (vb_uvs) {
			wgl.enableVertexAttribArray(shader.attrib_uv);
			wgl.bindBuffer(wgl.ARRAY_BUFFER, vb_uvs);
			wgl.vertexAttribPointer(shader.attrib_uv, vb_uvs.itemSize, wgl.FLOAT,
				false, 0, 0);
		} else {
			wgl.disableVertexAttribArray(shader.attrib_uv);
			console.error('Shader wants uvs, but none supplied.')
		}
	}
}
