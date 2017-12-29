

#if 0

#include "SpaceEngine.h"
#include "TerrainMesh.h"
#include "SpaceTerrain.h"

namespace space {

	char patterns[][9] = {
		{
			1,1,0,
			1,0,0,
			1,0,0,
		},
		{
			0,1,1,
			1,1,1,
			1,1,1,
		},
		{
			0,0,0,
			0,1,1,
			0,1,1,
		},

	};


	struct TerrainCorner {

		TerrainCorner() : pattern(-1),X(0),Y(0),lX(0),lY(0),sX(0),sY(0) {}
		TerrainCorner( int pattern, int X, int Y, int lX, int lY, int sX, int sY ) : pattern(pattern), X(X), Y(Y), lX(lX), lY(lY), sX(sX), sY(sY) {}

		int pattern;

		int X, Y;
		int lX, lY;
		int sX, sY;

		int cX() const { 
			return (2*X + 4 - (sX+lX))/2;
		}
		int cY() const {
			return (2*Y + 4 - (sY+lY))/2;
		}

		int bX() const {
			return (2*X + 3 - 2*(sX+lX))/2;
		}
		int bY() const {
			return (2*Y + 3 - 2*(sY+lY))/2;
		}

	};

	struct DistI {
		DistI() {}
		DistI( int x, int y, float mx, float my, float ox, float oy ) : x(x), y(y), mx(mx), my(my), ox(ox), oy(oy) {}
		int x;
		int y;
		float mx;
		float my;
		float ox;
		float oy;
		float d() {
			return sqrtf((mx-ox)*(mx-ox)+(my-oy)*(my-oy));
		}
	};


	void TerrainMesh::updateMesh( int minX, int minY, int maxX, int maxY ) {
		genMesh( true );
	}

	TerrainMesh::TerrainMesh( SpaceTerrain * terrain, SpaceEngine * engine ) : m_engine(engine), m_terrain(terrain),
		m_distField((terrain->getWidth()+1)*(terrain->getHeight()+1)),
		map( 4*terrain->getWidth()*terrain->getHeight()/3 )
	{
		genMesh(false);
	}


	void TerrainMesh::genMesh( bool incremental ) {

		POOP_LOG( "Generating terrain mesh" );

		corners.clear();

		int w = m_terrain->getWidth();
		int h = m_terrain->getHeight();

		//DynamicArray<TerrainVertex> verts;
		//DynamicArray<Face> faces;
		verts.clear();
		faces.clear();
		map = HashMap<VoxelCorner, VoxelEntry>( 4*w*h/3 );
		//HashMap<VoxelCorner, VoxelEntry> map( 4*w*h/3 );
		

		POOP_LOG( "Generating vertices" );
		// do vertices
		for ( int y = 0; y < h; y++ ) {
			for ( int x = 0; x < w; x++ ) {
				if ( m_terrain->getValByMapPos(x,y) ) {
					VoxelCorner c0( x,y );
					VoxelCorner c1( x+1,y );
					VoxelCorner c2( x,y+1 );
					VoxelCorner c3( x+1,y+1 );
					if ( !map.contains( c0 ) ) {
						Vector2 wp = m_terrain->mapToWorld((float)x-0.5f,(float)y-0.5f);
						verts.append( TerrainVertex( Vector3(wp.x, wp.y, 0.0f), VoxelCorner(x,y) ) );
						map.add( VoxelCorner(x,y), VoxelEntry(verts.getLength()-1) );
					}
					if ( !map.contains( c1 ) ) {
						Vector2 wp = m_terrain->mapToWorld((float)x+0.5f,(float)y-0.5f);
						verts.append( TerrainVertex( Vector3(wp.x, wp.y, 0.0f), VoxelCorner(x+1,y) ) );
						map.add( VoxelCorner(x+1,y), VoxelEntry(verts.getLength()-1) );
					}
					if ( !map.contains( c2 ) ) {
						Vector2 wp = m_terrain->mapToWorld((float)x-0.5f,(float)y+0.5f);
						verts.append( TerrainVertex( Vector3(wp.x, wp.y, 0.0f), VoxelCorner(x,y+1) ) );
						map.add( VoxelCorner(x,y+1), VoxelEntry(verts.getLength()-1) );
					}
					if ( !map.contains( c3 ) ) {
						Vector2 wp = m_terrain->mapToWorld((float)x+0.5f,(float)y+0.5f);
						verts.append( TerrainVertex( Vector3(wp.x, wp.y, 0.0f), VoxelCorner(x+1,y+1) ) );
						map.add( VoxelCorner(x+1,y+1), VoxelEntry(verts.getLength()-1) );
					}
				}
			}
		}
		
		POOP_LOG( "Detecting corners" );
		// Find corners
		for ( int Y = 0; Y < h-3; Y++ ) {
			for ( int X = 0; X < w-3; X++ ) {
				TerrainCorner * tc = matchCorner(X,Y);				
				if ( tc ) {
					bool skip = false;
					for ( int i = 0; i < corners.getLength(); i++ ) {
						if ( corners[i].cX() == tc->cX() && corners[i].cY() == tc->cY() ) {
							if ( tc->pattern < corners[i].pattern ) {
								corners.remove(i);
								i--;
							} else {
								skip = true;
								break;
							}
						}
					}
					if ( !skip ) {
						corners.append( *tc );
					}
				}
			}
		}

		POOP_LOG( "Smoothing corners" );
		HashMap<VoxelCorner, VoxelEntry>::KeyValuePair j;
		for ( int i = 0; i < corners.getLength(); i++ ) {
			TerrainCorner & c = corners[i];
			int px0, py0, px1, py1;
			if ( c.pattern == 0 ) {
				px0 = c.cX() + c.lX; py0 = c.cY() + c.lY;
				px1 = c.cX() + c.sX; py1 = c.cY() + c.sY;
			} else if ( c.pattern == 1 ) {
				px0 = c.cX() - c.lX; py0 = c.cY() - c.lY;
				px1 = c.cX() - c.sX; py1 = c.cY() - c.sY;
			} else {
				continue;
			}

			//POOP_ASSERT( map.contains( VoxelCorner(px0,py0) ) );
			//POOP_ASSERT( map.contains( VoxelCorner(px1,py1) ) );
			if ( !map.contains( VoxelCorner(px0,py0) ) || !map.contains( VoxelCorner(px1,py1) ) ) {
				POOP_WARNING( "Unknown corner found :I" );
				continue;
			}

			VoxelEntry & v0 = map[VoxelCorner(px0,py0)];
			VoxelEntry & v1 = map[VoxelCorner(px1,py1)];

			//if ( v0.merged || v1.merged ) {
			//	continue;
			//}

			v0.merged = true;
			v1.merged = true;
				
			Vector3 npos = (verts[v0.index].pos + verts[v1.index].pos) * 0.5f;

			int v0i = v0.index;
			int v1i = v1.index;
			verts.remove( v0.index );
			verts.append( TerrainVertex( Vector3( npos.x, npos.y, 0 ), VoxelCorner(px0,py0) ) );
			map.resetIteration();
			while( map.iterateNext( &j ) ) {
				if ( j.value->index > v0i ) j.value->index--;
				else if ( j.value->index == v0i ) j.value->index = verts.getLength()-1;
			}
			if ( v1i != v0i ) {
				if ( v1i > v0i ) v1i--;
				verts.remove( v1.index );
				map.resetIteration();
				while( map.iterateNext( &j ) ) {
					if ( j.value->index > v1i ) j.value->index--;
					else if ( j.value->index == v1i ) j.value->index = verts.getLength()-1;				
				}
			}

		}

		POOP_LOG( "Generating faces" );
		// do faces
		for ( int y = 0; y < h; y++ ) {
			for ( int x = 0; x < w; x++ ) {
				if ( m_terrain->getValByMapPos(x,y) ) {
					VoxelEntry c3 = map[VoxelCorner(x,y)];
					VoxelEntry c2 = map[VoxelCorner(x+1,y)];
					VoxelEntry c1 = map[VoxelCorner(x,y+1)];
					VoxelEntry c0 = map[VoxelCorner(x+1,y+1)];
					faces.append( Face( Face::V(c0.index), Face::V(c1.index), Face::V(c3.index), Face::V(c2.index) ) );
				}
			}
		}


		{
			MeshData * ret = new MeshData();
			ret->vertices = Array<Vertex>(verts.getLength());
			for( int i = 0; i < verts.getLength(); i++ ) {
				ret->vertices[i].position = verts[i].pos;
			}
			ret->faces = faces.ToArray();
			ret->exportOBJ( "terrain0.obj" );
		}

		if ( !incremental ) {
			computeDistanceField();
		} 
		
		POOP_LOG( "Deforming vertices" );
		for ( int y = 0; y < h; y++ ) {
			for ( int x = 0; x < w; x++ ) {
				VoxelCorner vc( x,y );
				if ( map.contains( vc ) ) {
					VoxelEntry ve = map[vc];
					Vector3 & v = verts[ve.index].pos;
					float d = m_distField[y*(w+1)+x];
					v.z = -sqrtf(d) * 1.5f;
					//v.z = -sqrtf(maxdist*maxdist - (d-maxdist)*(d-maxdist)) * 0.1f;
					//v.z = sin(v.x) + cos(v.y);
					//v.z = -d * 0.01f;
				}
			}
		}

		generateClosingFaces();
		computeNormals();


		//ret->computeNormals();
		//for ( int i = 0; i < ret->vertices.getLength(); i++ ) {
		//	ret->vertices[i].position = ret->vertices[i].position + Vector3( RND(-1,1), RND(-1,1), RND(-1,1) ) * 0.2f;
			//ret->vertices[i].position = ret->vertices[i].position + ret->vertices[i].normal * RND(-1,1) * 0.2f;
		//}

		//POOP_LOG( "Subdivide #1" );
		//ret->subdivide( true );

		//ret->exportOBJ( "terrain3.obj" );


		//ret->computeNormals();
		//for ( int i = 0; i < ret->vertices.getLength(); i++ ) {
		//	ret->vertices[i].position = ret->vertices[i].position + Vector3( RND(-1,1), RND(-1,1), RND(-1,1) ) * 0.1f;
			//ret->vertices[i].position = ret->vertices[i].position + ret->vertices[i].normal * RND(-1,1) * 0.1f;
		//}

		//POOP_LOG( "Subdivide #2" );
		//ret->subdivide( true );
		//ret->exportOBJ( "terrain4.obj" );

		//ret->computeNormals();

		//m_terrainMesh = ret->createMesh( m_engine->getRenderer(), m_engine->getResources()->vlayout_basic );

		POOP_ASSERT( (m_terrain->getWidth() * m_terrain->getHeight()) % ( BLOCK_SIZE * BLOCK_SIZE ) == 0 );

		// Create meshes

		m_meshes = Array<Mesh*>( m_terrain->getWidth() * m_terrain->getHeight() / ( BLOCK_SIZE * BLOCK_SIZE ) );

		for ( int y = 0; y < m_terrain->getHeight() / BLOCK_SIZE; y++ ) {
			for ( int x = 0; x < m_terrain->getWidth() / BLOCK_SIZE; x++ ) {
				generateMesh(x,y);
			}
		}

	}

	void TerrainMesh::computeNormals() {
		for( uint v = 0; v < verts.getLength(); v++ ) {
			verts[v].normal = Vector3(0,0,0);
		}
		for( uint i = 0; i < faces.getLength(); i++ ) {
			Face & f = faces[i];
			TerrainVertex & v0 = verts[f.v[0].i];
			TerrainVertex & v1 = verts[f.v[1].i];
			TerrainVertex & v2 = verts[f.v[2].i];
			Vector3 e0 = v0.pos - v1.pos;
			Vector3 e1 = v0.pos - v2.pos;
			Vector3 n = e0.cross(e1).normalized();
			v0.normal += n;
			v1.normal += n;
			v2.normal += n;
			if ( f.v[3].i != -1 ) {
				TerrainVertex & w0 = verts[f.v[2].i];
				TerrainVertex & w1 = verts[f.v[3].i];
				TerrainVertex & w2 = verts[f.v[0].i];
				Vector3 e2 = w0.pos - w1.pos;
				Vector3 e3 = w0.pos - w2.pos;
				Vector3 n = e2.cross(e3).normalized();
				w0.normal += n;
				w1.normal += n;
				w2.normal += n;
			}
		}
		for( uint v = 0; v < verts.getLength(); v++ ) {
			verts[v].normal.normalize();
		}	
	}

	void TerrainMesh::generateClosingFaces() {
		int w = m_terrain->getWidth();
		int h = m_terrain->getHeight();

		POOP_LOG( "Generating closing vertices." );
		Array<int> closingVertices(verts.getLength());
		closingVertices.fill(-1);
		for ( int y = 0; y < h; y++ ) {
			for ( int x = 0; x < w; x++ ) {
				bool c0 = map.contains( VoxelCorner( x,y ) );
				bool c1 = map.contains( VoxelCorner( x+1,y ) );
				bool c2 = map.contains( VoxelCorner( x,y+1 ) );
				bool c3 = map.contains( VoxelCorner( x-1,y ) );
				bool c4 = map.contains( VoxelCorner( x,y-1 ) );
				bool c5 = map.contains( VoxelCorner( x+1,y+1 ) );
				bool c6 = map.contains( VoxelCorner( x-1,y+1 ) );
				bool c7 = map.contains( VoxelCorner( x-1,y-1 ) );
				bool c8 = map.contains( VoxelCorner( x+1,y-1 ) );
				if ( c0 && (!c1 || !c2 || !c3 || !c4 || !c5 || !c6 || !c7 || !c8) ) {
					float d = m_distField[y*w+x];
					VoxelEntry & ve = map[VoxelCorner(x,y)];
					Vector3 &v = verts[ve.index].pos;
					if ( closingVertices[ve.index] == -1 && fabsf(v.z) > 0.01f ) {
						VoxelEntry & ve = map[ VoxelCorner(x,y) ];
						Vector3 & op = verts[ve.index].pos;
						verts.append( TerrainVertex( Vector3( op.x, op.y, 0 ), verts[ve.index].vc ) );
						closingVertices[ve.index] = verts.getLength() - 1;
					}
				}
			}
		}
		for ( int y = 0; y <= h; y++ ) {
			for ( int x = 0; x <= w; x++ ) {
				if ( map.contains( VoxelCorner( x,y ) ) ) { 
					map[VoxelCorner( x,y )].closingVertex = closingVertices[map[VoxelCorner( x,y )].index];
				}
			}
		}

		POOP_LOG( "Generating closing faces." );
		int of = faces.getLength();
		for ( int i = 0; i < of; i++ ) {
			int ec = faces[i].v[3].i == -1 ? 3 : 4;
			for ( int e = 0; e < ec; e++ ) {
				VoxelCorner & a = verts[faces[i].v[e].i].vc;
				VoxelCorner & b = verts[faces[i].v[(e+1)%ec].i].vc;
				VoxelEntry & ae = map[a], &be = map[b];
				bool ac = ae.closingVertex != -1, bc = be.closingVertex != -1;
				if ( ac || bc ) {
					if ( !bc ) {
						faces.append( Face( ae.closingVertex, be.index, ae.index ) );
					} else if ( !ac ) {
						faces.append( Face( be.index, ae.index, be.closingVertex ) );
					} else {
						faces.append( Face( be.closingVertex, be.index, ae.index, ae.closingVertex ) );
					}
				}
			}
		}
	}

	void TerrainMesh::computeDistanceField() {
		int w = m_terrain->getWidth();
		int h = m_terrain->getHeight();

		POOP_LOG( "Creating distance field" );
		m_distField.fill(100000.0f);
		float maxdist = 0.0f;
		for ( int y = 0; y < h; y++ ) {
			for ( int x = 0; x < w; x++ ) {
				bool c0 = map.contains( VoxelCorner( x,y ) );
				bool c1 = map.contains( VoxelCorner( x+1,y ) );
				bool c2 = map.contains( VoxelCorner( x,y+1 ) );
				bool c3 = map.contains( VoxelCorner( x-1,y ) );
				bool c4 = map.contains( VoxelCorner( x,y-1 ) );
				if ( c0 && (!c1 || !c2 || !c3 || !c4) ) {
					Queue<DistI> bfs;
					VoxelEntry e = map[VoxelCorner(x,y)];
					Vector3 v = verts[e.index].pos;
					bfs.push( DistI(x,y, v.x,v.y, v.x,v.y) );
					while( bfs.getLength() > 0 ) {
						DistI i = bfs.pop();
						Vector3 p = verts[map[VoxelCorner(i.x,i.y)].index].pos;
						i.mx = p.x;
						i.my = p.y;
						if ( m_distField[i.y*(w+1)+i.x] > i.d() ) {
							m_distField[i.y*(w+1)+i.x] = i.d();
							if ( maxdist < i.d() ) {
								maxdist = i.d();
							}
							if ( map.contains(VoxelCorner(i.x-1,i.y)) ) bfs.push( DistI(i.x-1,i.y, i.ox, i.oy, i.ox, i.oy) );
							if ( map.contains(VoxelCorner(i.x,i.y-1)) ) bfs.push( DistI(i.x,i.y-1, i.ox, i.oy, i.ox, i.oy) );
							if ( map.contains(VoxelCorner(i.x+1,i.y)) ) bfs.push( DistI(i.x+1,i.y, i.ox, i.oy, i.ox, i.oy) );
							if ( map.contains(VoxelCorner(i.x,i.y+1)) ) bfs.push( DistI(i.x,i.y+1, i.ox, i.oy, i.ox, i.oy) );

							if ( map.contains(VoxelCorner(i.x-1,i.y-1)) ) bfs.push( DistI(i.x-1,i.y-1, i.ox, i.oy, i.ox, i.oy) );
							if ( map.contains(VoxelCorner(i.x-1,i.y+1)) ) bfs.push( DistI(i.x-1,i.y+1, i.ox, i.oy, i.ox, i.oy) );
							if ( map.contains(VoxelCorner(i.x+1,i.y-1)) ) bfs.push( DistI(i.x+1,i.y-1, i.ox, i.oy, i.ox, i.oy) );
							if ( map.contains(VoxelCorner(i.x+1,i.y+1)) ) bfs.push( DistI(i.x+1,i.y+1, i.ox, i.oy, i.ox, i.oy) );
						}
					}
				}
			}
		}
		/*
		Array<float> ndistField((h+1)*(w+1));
		for ( int y = 0; y < h; y++ ) {
			for ( int x = 0; x < w; x++ ) {
				float avg = 0.0f;
				int n = 0;
				for ( int ox = x-1; ox <=x+1; ox++ ) {
					for ( int oy = y-1; oy <=y+1; oy++ ) {
						if ( ox >= 0 && ox <= w && oy >= 0 && oy <= h && m_distField[oy*(w+1)+ox] < 10000.0f ) {
							avg += m_distField[oy*(w+1)+ox];
							n++;
						}
					}
				}
				if ( n > 0 ){// && m_distField[y*(w+1)+x] > 0.0f ) {
					ndistField[y*(w+1)+x] = avg/n;
				} else {
					ndistField[y*(w+1)+x] = 0.0f;
				}
			}
		}
		m_distField = ndistField;
		*/
	}

	void TerrainMesh::generateMesh( int x, int y ) {

		int meshes_x = m_terrain->getWidth() / BLOCK_SIZE;

		int mapMinY  = y * m_terrain->getHeight() / BLOCK_SIZE;
		int mapMaxY  = (y+1) * m_terrain->getHeight() / BLOCK_SIZE;
		int mapMinX  = x * m_terrain->getHeight() / BLOCK_SIZE;
		int mapMaxX  = (x+1) * m_terrain->getHeight() / BLOCK_SIZE;
		Vector2 worldMin = m_terrain->mapToWorld(mapMinX,mapMinY);
		Vector2 worldMax = m_terrain->mapToWorld(mapMaxX,mapMaxY);

		MeshData blockMData;

		Array<int> added(verts.getLength());
		added.fill( -1 );
		DynamicArray<Vertex> blockVerts;
		DynamicArray<Face> blockFaces;

		for ( int i = 0; i < faces.getLength(); i++ ) {
			Face & f = faces[i];
			Vector3 centroid;
			if ( f.v[3].i != -1 ) centroid = (verts[f.v[0].i].pos + verts[f.v[1].i].pos + verts[f.v[2].i].pos + verts[f.v[3].i].pos) * 0.25f;
			else centroid = (verts[f.v[0].i].pos + verts[f.v[1].i].pos + verts[f.v[2].i].pos) * 0.333333f;
			if ( centroid.x >= worldMin.x && centroid.x < worldMax.x && centroid.y >= worldMin.y && centroid.y < worldMax.y ) {
				if ( added[f.v[0].i] == -1 ) { blockVerts.append( Vertex( verts[f.v[0].i].pos, verts[f.v[0].i].normal ) ); added[f.v[0].i] = blockVerts.getLength()-1; }
				if ( added[f.v[1].i] == -1 ) { blockVerts.append( Vertex( verts[f.v[1].i].pos, verts[f.v[1].i].normal ) ); added[f.v[1].i] = blockVerts.getLength()-1; }
				if ( added[f.v[2].i] == -1 ) { blockVerts.append( Vertex( verts[f.v[2].i].pos, verts[f.v[2].i].normal ) ); added[f.v[2].i] = blockVerts.getLength()-1; }
				if ( f.v[3].i != -1 && added[f.v[3].i] == -1 ) { blockVerts.append( Vertex( verts[f.v[3].i].pos, verts[f.v[3].i].normal ) ); added[f.v[3].i] = blockVerts.getLength()-1; }
				Face nf = f;
				nf.v[0].i = added[f.v[0].i];
				nf.v[1].i = added[f.v[1].i];
				nf.v[2].i = added[f.v[2].i];
				if ( f.v[3].i != -1 ) nf.v[3].i = added[f.v[3].i];
				blockFaces.append( nf );
			}
		}

		if ( blockVerts.getLength() > 0 && blockFaces.getLength() > 0 ) {
			blockMData.faces = blockFaces.ToArray();
			blockMData.vertices = blockVerts.ToArray();

			POOP_LOG( String("Creating mesh of ") + blockMData.vertices.getLength() + String(" vertices and ") + blockMData.faces.getLength() + String(" faces @ (") + x + String(", ") + y + String(")") );

			m_meshes[y*meshes_x+x] = blockMData.createMesh( m_engine->getRenderer(), m_engine->getResources()->vlayout_basic );
		} else {
			m_meshes[y*meshes_x+x] = NULL;
		}

	}

	TerrainCorner * TerrainMesh::matchCorner( int X, int Y ) {
		TerrainCorner tcs[] = {
			TerrainCorner( 0, X,Y,  0, 1,  1, 0 ),
			TerrainCorner( 0, X,Y,  0,-1,  1, 0 ),
			TerrainCorner( 0, X,Y,  1, 0,  0, 1 ),
			TerrainCorner( 0, X,Y, -1, 0,  0, 1 ),
			TerrainCorner( 0, X,Y,  0, 1, -1, 0 ),
			TerrainCorner( 0, X,Y,  0,-1, -1, 0 ),
			TerrainCorner( 0, X,Y,  1, 0,  0,-1 ),
			TerrainCorner( 0, X,Y, -1, 0,  0,-1 ),
			
			TerrainCorner( 1, X,Y, 1, 0,  0, 1 ),
			TerrainCorner( 1, X,Y, 0, 1, -1, 0 ),
			TerrainCorner( 1, X,Y,-1, 0,  0,-1 ),
			TerrainCorner( 1, X,Y, 0,-1,  1, 0 ),
			
			//TerrainCorner( 2, X,Y, 1, 0,  0, 1 ),
			//TerrainCorner( 2, X,Y, 0, 1, -1, 0 ),
			//TerrainCorner( 2, X,Y,-1, 0,  0,-1 ),
			//TerrainCorner( 2, X,Y, 0,-1,  1, 0 ),
		};

		TerrainCorner * tc = 0;
		for ( int i = 0; i < sizeof(tcs)/sizeof(TerrainCorner); i++ ) {
			if ( matchesCorner(&tcs[i]) ) {
				tc = new TerrainCorner();
				*tc = tcs[i];
				break;
			}
		}

		return tc;
	}

	bool TerrainMesh::matchesCorner( const TerrainCorner * tc ) {

		char * pattern = patterns[tc->pattern];

		for ( int y = 0; y < 3; y++ ) {
			for ( int x = 0; x < 3; x++ ) {
				int bX = tc->bX();
				int bY = tc->bY();
				// b + l * y + s * x
				if ( (m_terrain->getValByMapPos( bX + tc->lX*y + tc->sX*x, bY + tc->lY*y + tc->sY*x ) != 0) != (pattern[3*y+x] != 0) ) {
					return false;
				}
			}
		}
		return true;
	}


}

#endif

