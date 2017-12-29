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

__author__ = ["h"]
__version__ = '0.2'
__bpydoc__ = """\
"""


import bpy


def readMesh(filename, objName):
    file = open(filename, "rb")

    def line_to_face(line):
        # Each triplet is an xyz float
        line_split = line.split()
        try:
            line_split_float = map(float, line_split)
        except:
            return None

        if len(line_split) in {9, 12}:
            return zip(*[iter(line_split_float)] * 3)  # group in 3's
        else:
            return None

    faces = []
    for line in file.readlines():
        face = line_to_face(line)
        if face:
            faces.append(face)

    file.close()

    # Generate verts and faces lists, without duplicates
    verts = []
    coords = {}
    index_tot = 0
    faces_indices = []

    for f in faces:
        fi = []
        for i, v in enumerate(f):
            index = coords.get(v)

            if index is None:
                index = coords[v] = index_tot
                index_tot += 1
                verts.append(v)

            fi.append(index)

        faces_indices.append(fi)

    mesh = bpy.data.meshes.new(objName)
    mesh.from_pydata(verts, [], faces_indices)

    return mesh


def addMeshObj(mesh, objName):
    scn = bpy.context.scene

    for o in scn.objects:
        o.select = False

    mesh.update()
    mesh.validate()

    nobj = bpy.data.objects.new(objName, mesh)
    scn.objects.link(nobj)
    nobj.select = True

    if scn.objects.active is None or scn.objects.active.mode == 'OBJECT':
        scn.objects.active = nobj


def read(filepath):
    #convert the filename to an object name
    objName = bpy.path.display_name_from_filepath(filepath)
    mesh = readMesh(filepath, objName)
    addMeshObj(mesh, objName)
