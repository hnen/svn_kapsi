# ##### BEGIN GPL LICENSE BLOCK #####
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# ##### END GPL LICENSE BLOCK #####

# <pep8 compliant>


bl_info = {
	"name": "F mesh (.f)",
	"author": "h",
	"version": (0, 2),
	"blender": (2, 5, 7),
	"api": 36103,
	"location": "File > Import-Export > F mesh (.f) ",
	"description": "Import-Export F Mesh",
	"warning": "",
	"category": "Import-Export"}

if "bpy" in locals():
	import imp
	if "import_f" in locals():
		imp.reload(import_f)
	if "export_f" in locals():
		imp.reload(export_f)
else:
	import bpy

from bpy.props import StringProperty, BoolProperty


class FImporter(bpy.types.Operator):
	'''Load F mesh'''
	bl_idname = "import_mesh.f"
	bl_label = "Import F mesh"
	bl_options = {'UNDO'}

	filepath = StringProperty(
			subtype='FILE_PATH',
			)
	filter_glob = StringProperty(default="*.f", options={'HIDDEN'})

	def execute(self, context):
		from . import import_f
		import_f.read(self.filepath)
		return {'FINISHED'}

	def invoke(self, context, event):
		wm = context.window_manager
		wm.fileselect_add(self)
		return {'RUNNING_MODAL'}


class FExporter(bpy.types.Operator):
	'''Save F mesh'''
	bl_idname = "export_mesh.f"
	bl_label = "Export F mesh"

	filepath = StringProperty(
			subtype='FILE_PATH',
			)
	check_existing = BoolProperty(
			name="Check Existing",
			description="Check and warn on overwriting existing files",
			default=True,
			options={'HIDDEN'},
			)
	apply_modifiers = BoolProperty(
			name="Apply Modifiers",
			description="Use transformed mesh data from each object",
			default=True,
			)
	triangulate = BoolProperty(
			name="Triangulate",
			description="Triangulate quads",
			default=True,
			)

	def execute(self, context):
		from . import export_f
		export_f.write(self.filepath,
						 self.apply_modifiers,
						 self.triangulate,
						 )

		return {'FINISHED'}

	def invoke(self, context, event):
		if not self.filepath:
			self.filepath = bpy.path.ensure_ext(bpy.data.filepath, ".f")
		wm = context.window_manager
		wm.fileselect_add(self)
		return {'RUNNING_MODAL'}


def menu_import(self, context):
	self.layout.operator(FImporter.bl_idname, text="F Mesh (.f)")


def menu_export(self, context):
	self.layout.operator(FExporter.bl_idname, text="F Mesh (.f)")


def register():
	bpy.utils.register_module(__name__)

	bpy.types.INFO_MT_file_import.append(menu_import)
	bpy.types.INFO_MT_file_export.append(menu_export)


def unregister():
	bpy.utils.unregister_module(__name__)

	bpy.types.INFO_MT_file_import.remove(menu_import)
	bpy.types.INFO_MT_file_export.remove(menu_export)

if __name__ == "__main__":
	register()
