
# <pep8 compliant>

__author__ = ["h"]
__version__ = '0.2'
__bpydoc__ = """\
"""

import bpy
import sys
import struct
import io

F_MAT_END = 0
F_MAT_FLOAT4 = 1
F_MAT_FLOAT = 2
F_MAT_IMG = 3

class Material:
	def __init__(self):
		self.diffuseTex = ''
		self.normalmapTex = ''
		self.diffuse = (1.0, 1.0, 1.0)
		self.specular = (1.0, 1.0, 1.0)
		self.specularHardness = 1.0
	
def writeChunk( dst_stream, headerName, src_stream ):
	def writeHeader( dst_stream, headerName ):
		dst_stream.write( struct.pack( '4s', bytes(headerName, sys.getdefaultencoding()) ) )
	src_stream.seek(0)
	data = src_stream.read(-1)
	writeHeader( dst_stream, headerName )
	dst_stream.write( struct.pack( 'i', len(data) ) )
	dst_stream.write( data )
	
def formatFilename( fileName ):
	ret = fileName
	if ret.find( '//' ) != -1:
		ret = ret[2:]
	return ret.replace( '\\', '/' )
	
	
def writeMesh( dst_stream, verts, faces, materials ):

	def writePositions( dst_stream, verts ):
		tmp_stream = io.BytesIO()
		tmp_stream.write( struct.pack( 'i', 3 ) )
		for v in verts:
			for x in v[0]:
				tmp_stream.write( struct.pack( 'f', x ) )
		writeChunk( dst_stream, 'POSI', tmp_stream )

	def writeNormals( dst_stream, verts ):
		tmp_stream = io.BytesIO()
		tmp_stream.write( struct.pack( 'i', 3 ) )
		for v in verts:
			for x in v[1]:
				tmp_stream.write( struct.pack( 'f', x ) )
		writeChunk( dst_stream, 'NORM', tmp_stream )

	def writeFaces( dst_stream, faces ):
		tmp_stream = io.BytesIO()
		tmp_stream.write( struct.pack( 'i', 4 ) )
		for f in faces:
			for i in f[0]:
				tmp_stream.write( struct.pack( 'i', i ) )
			for n in range(4-len(f[0])):
				tmp_stream.write( struct.pack( 'i', -1 ) )
		writeChunk( dst_stream, 'INDS', tmp_stream )

	def writeUVs( dst_stream, faces ):
		tmp_stream = io.BytesIO()
		tmp_stream.write( struct.pack( 'i', 2 ) )
		for f in faces:
			#for i in f[1]:
			numVerts = len(f[0])
			for n in range(numVerts):
				tmp_stream.write( struct.pack( 'f', f[1][n][0] ) )
				tmp_stream.write( struct.pack( 'f', f[1][n][1] ) )
			for n in range(4-numVerts):
				tmp_stream.write( struct.pack( 'f', 0.0 ) )
				tmp_stream.write( struct.pack( 'f', 0.0 ) )
		writeChunk( dst_stream, 'FUV0', tmp_stream )
	
	def writeMaterials( dst_stream, mats ):
		def writeImg( key, value ):
			tmp_stream.write( struct.pack( 'i', F_MAT_IMG ) )
			tmp_stream.write( key.encode() + b'\0' )
			fn = formatFilename( value )
			tmp_stream.write( struct.pack( 'i', len(fn) + 1 ) )
			tmp_stream.write( fn.encode() + b'\0' )
		def writeColor( key, value ):
			tmp_stream.write( struct.pack( 'i', F_MAT_FLOAT4 ) )
			tmp_stream.write( key.encode() + b'\0' )
			tmp_stream.write( struct.pack( 'i', 16 ) )
			tmp_stream.write( struct.pack( 'ffff', value[0], value[1], value[2], 1.0 ) )
		def writeValue( key, value ):
			tmp_stream.write( struct.pack( 'i', F_MAT_FLOAT ) )
			tmp_stream.write( key.encode() + b'\0' )
			tmp_stream.write( struct.pack( 'i', 4 ) )
			tmp_stream.write( struct.pack( 'f', value ) )
		
		tmp_stream = io.BytesIO()
		tmp_stream.write( struct.pack( 'i', len(mats) ) )
		
		for mat in mats:
			if len(mat.diffuseTex) > 0:
				print( "Has diffuseTex: " + mat.diffuseTex )
				writeImg( 'tex_diffuse', mat.diffuseTex )
			
			if len(mat.normalmapTex) > 0:
				print( "Has normalmapTex: " + mat.normalmapTex )
				writeImg( 'tex_normalmap', mat.normalmapTex )

			writeColor( 'col_diffuse', mat.diffuse )
			writeColor( 'col_specular', mat.specular )
			writeValue( 'f_specPow', mat.specularHardness )
			
			tmp_stream.write( struct.pack( 'i', F_MAT_END ) )
		writeChunk( dst_stream, 'MATS', tmp_stream )
	
		
	def writeEnd( dst_stream ):
		writeChunk( dst_stream, 'END', io.BytesIO() )
	
	tmp_stream = io.BytesIO()
	
	tmp_stream.write( struct.pack( 'i', len(verts) ) )
	tmp_stream.write( struct.pack( 'i', len(faces) ) )
	
	writePositions( tmp_stream, verts )
	writeNormals( tmp_stream, verts )
	writeFaces( tmp_stream, faces )
	
	if materials is not None and len(materials) > 0:
		writeMaterials( tmp_stream, materials )
	
	if faces[0][1] is not None:
		writeUVs( tmp_stream, faces )


	writeEnd( tmp_stream )
	
	writeChunk( dst_stream, 'MESH', tmp_stream )

def getMesh(scene, obj, applyMods):
	me = None
	if applyMods or obj.type != 'MESH':
		try:
			me = obj.to_mesh(scene, True, "PREVIEW")
		except:
			print ( "Fucking failed to convert to mesh :/" )
			me = None
		is_tmp_mesh = True
	else:
		me = obj.data
		is_tmp_mesh = False
	return me
	
def getVertices(obj, mesh):
	verts = []
	matrix = obj.matrix_world.copy()
	for n, vert in enumerate(mesh.vertices):
		verts.append( ( matrix * vert.co, matrix.to_3x3() * vert.normal ) )
	print ( "NumVerts: " + str(len(verts)) )
	return verts

def getFaces(mesh):
	faces = []
	
	has_uv = mesh.uv_textures
	
	for i, face in enumerate(mesh.tessfaces):
		uf = None
		uvs = None
		matid = -1
		
		if has_uv:
			uf = mesh.uv_textures.active.data[i]
			uvs = uf.uv
		matid = face.material_index
			
		faces.append( (face.vertices, uvs, matid) )
	return faces;

def getMaterials(mesh):
	mats = []
	for mat in mesh.materials:
		fmat = Material()
		fmat.diffuseStrength = (mat.diffuse_color[0] * mat.diffuse_intensity, mat.diffuse_color[1] * mat.diffuse_intensity, mat.diffuse_color[2] * mat.diffuse_intensity)
		fmat.specularStrength = (mat.specular_color[0] * mat.specular_intensity, mat.specular_color[1] * mat.specular_intensity, mat.specular_color[2] * mat.specular_intensity)
		fmat.specularHardness = mat.specular_hardness
		for texSlot in mat.texture_slots:
			if texSlot is not None and isinstance( texSlot.texture, bpy.types.ImageTexture ):
				tex = texSlot.texture
				img = tex.image	# bpy.types.image
				filename = img.filepath
				if texSlot.use_map_normal:
					# it's a normal map
					normalmapfactor = texSlot.normal_factor	# todo do something with this
					fmat.normalmapTex = filename
				if texSlot.use_map_color_diffuse:
					fmat.diffuseTex = filename
				#if texSlot.use_map_ambient:
				#if texSlot.use_map_specular:
		mats.append( fmat )
	return mats

def write(filepath, applyMods=True, triangulate=True, ):

	scene = bpy.context.scene

	idoffset = 0
	
	verts = []
	faces = []
	materials = []

	for obj in bpy.context.selected_objects:
		me = getMesh( scene, obj, applyMods )

		if me is not None:
			verts = getVertices(obj, me)
			faces = getFaces(me)
			materials = getMaterials(me)
			#if is_tmp_mesh:
			#	bpy.data.meshes.remove(me)
				
			idoffset = idoffset + len(me.vertices)

	
	
	# write the faces to a file
	file_io = io.BytesIO()

	writeMesh( file_io, verts, faces, materials )
	
	file_io.seek(0)
		
	file = open(filepath, "wb")
	data = file_io.read(-1)
	file.write( data )
	file.close()

