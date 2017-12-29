using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Diagnostics;

using System.IO;

//using System.Collections.Generic;
using System.Runtime.InteropServices;

using OpenTK;

namespace demo0 {

    public class MeshImporter {

        public class Mesh {

            [StructLayout(LayoutKind.Sequential, Pack = 1)]
            public struct Vertex {
                public Vector3 pos;
                public Vector3 normal;
            }

            public Vertex [] verts;
            public int [] inds;
        }

        private struct Face {
            public int i0, i1, i2, i3;
        }

        public static Mesh Cube() {
            Mesh.Vertex [] verts = {
                new Mesh.Vertex { pos = new Vector3( 1, 1, 1) },   //0
                new Mesh.Vertex { pos = new Vector3(-1, 1, 1) },  //1
                new Mesh.Vertex { pos = new Vector3(-1, 1,-1) }, //2
                new Mesh.Vertex { pos = new Vector3( 1, 1,-1) },  //3
                new Mesh.Vertex { pos = new Vector3( 1,-1, 1) },  //4
                new Mesh.Vertex { pos = new Vector3(-1,-1, 1) }, //5
                new Mesh.Vertex { pos = new Vector3(-1,-1,-1) },//6
                new Mesh.Vertex { pos = new Vector3( 1,-1,-1) }  //7
            };

            int [] inds = {
                1, 0, 2,    3, 2, 0,
                4, 5, 6,    6, 7, 4,
                0, 1, 5,    5, 4, 0,
                2, 3, 6,    7, 6, 3,

                3, 0, 7,    4, 7, 0,
                1, 2, 6,    6, 5, 1,

            };

            var mesh = new Mesh();
            mesh.inds = inds;
            mesh.verts = verts;
            return mesh;

        }

        // Note: faces are 4 verts each
        public static Mesh Import( string filename ) {
            Console.WriteLine( string.Format( "Importing mesh {0}", filename ) );

            using ( BinaryReader i = new BinaryReader( File.Open( filename, FileMode.Open ) ) ) {
                
                // Header
                char [] h = i.ReadChars(4);
                Debug.Assert( h[0] == 'M' && h[1] == 'E' && h[2] == 'S' && h[3] == 'H' );

                int vertCount = i.ReadInt32();
                int faceCount = i.ReadInt32();


                Mesh ret = new Mesh();
                ret.verts = new Mesh.Vertex[vertCount];
                Face [] faces = new Face[faceCount];

                int triangleCount = 0;
                bool end = false;
                do {
                    char [] meshBlockb = i.ReadChars(4);
                    String meshBlock = new String( meshBlockb );
                    switch ( meshBlock ) {
                        case "POSI": {
                                int width = i.ReadInt32();
                                for ( int n = 0 ; n < vertCount ; n++ ) {
                                    float x = i.ReadSingle();
                                    float y = i.ReadSingle();
                                    float z = i.ReadSingle();
                                    ret.verts[n].pos = new Vector3( x, y, z );
                                }
                            }
                            break;
                        case "NORM": {
                                int width = i.ReadInt32();
                                for ( int n = 0 ; n < vertCount ; n++ ) {
                                    float x = i.ReadSingle();
                                    float y = i.ReadSingle();
                                    float z = i.ReadSingle();
                                    ret.verts[n].normal = new Vector3( x, y, z );
                                }
                            }
                            break;
                        case "INDS": {
                                int width = i.ReadInt32();
                                for ( int n = 0 ; n < faceCount ; n++ ) {
                                    faces[n].i0 = i.ReadInt32();
                                    faces[n].i1 = i.ReadInt32();
                                    faces[n].i2 = i.ReadInt32();
                                    faces[n].i3 = i.ReadInt32();
                                    triangleCount++;
                                    if ( faces[n].i3 != -1 ) {
                                        triangleCount++;
                                    }
                                }
                            }
                            break;
                        case "END\0":
                            end = true;
                            break;
                        default:
                            throw new InvalidDataException();

                    }
                } while ( !end );

                // Triangulate faces!
                ret.inds = new int[triangleCount * 3];
                int tri = 0;
                for( int l = 0; l < faceCount; l++ ) {
                    ret.inds[tri * 3 + 0] = faces[l].i0;
                    ret.inds[tri * 3 + 1] = faces[l].i1;
                    ret.inds[tri * 3 + 2] = faces[l].i2;
                    tri++;
                    if ( faces[l].i3 != -1 ) {
                        ret.inds[tri * 3 + 0] = faces[l].i0;
                        ret.inds[tri * 3 + 1] = faces[l].i2;
                        ret.inds[tri * 3 + 2] = faces[l].i3;
                        tri++;
                    }
                }

                Console.WriteLine( string.Format( "Imported mesh {0}, verts = {1}, faces = {2}, triangles = {3}", filename, vertCount, faceCount, triangleCount ) );
                return ret;
            }

        }


    }
}
