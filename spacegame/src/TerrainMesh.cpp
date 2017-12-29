
#include "SpaceEngine.h"
#include "SpaceTerrain.h"
#include "TerrainMesh.h"


namespace space {

	struct TerrainVertex {

		TerrainVertex() : cx(-1), cy(-1) {}
		TerrainVertex( const Vector3 & pos, int cx, int cy ) : pos(pos), cx(cx), cy(cy) {}

		Vector3 pos;
		Vector3 norm;
		int cx, cy;

	};

	struct TerrainFace {

		TerrainFace() : a(0), b(0), c(0), d(0) {}
		TerrainFace( TerrainVertex *a, TerrainVertex *b, TerrainVertex *c, TerrainVertex *d ) : a(a), b(b), c(c), d(d) {}
		TerrainFace( TerrainVertex *a, TerrainVertex *b, TerrainVertex *c ) : a(a), b(b), c(c), d(0) {}

		TerrainVertex *a, *b, *c, *d;
		int tx, ty;

		TerrainVertex * get(int u, int v) {
			if ( u == 0 && v == 0 ) return a;
			if ( u == 1 && v == 0 ) return b;
			if ( u == 1 && v == 1 ) return c;
			if ( u == 0 && v == 1 ) return d;
			return 0;
		}

		Vector3 computeCentroid() {
			if ( a && b && c && d ) {
				return (a->pos + b->pos + c->pos + d->pos) / 4.0f;
			} else {
				return (a->pos + b->pos + c->pos) / 3.0f;
			}
		}

	};

	struct VertexMergeGroup {

		VertexMergeGroup() : n(0) {}

		void addVertex( TerrainVertex* vert ) {
			verts.append( vert );
			sum = sum + vert->pos;
			n++;
		}

		void removeVertex( TerrainVertex * vert ) {
			for ( int i = 0; i < verts.getLength(); i++ ) {
				if ( verts[i] == vert ) {
					verts.remove( i );
					sum = sum - vert->pos;
					n--;
					break;
				}
			}
		}

		void combine( VertexMergeGroup * vmg ) {
			for ( int i = 0; i < vmg->verts.getLength(); i++ ) {
				verts.append( vmg->verts[i] );
			}
			sum = sum + vmg->sum;
			n += vmg->n;
		}

		Vector3 getPosition() {
			return sum / (float)n;
		}

		TerrainVertex * getMergedVertex() {
			v.pos = getPosition();
			return &v;
		}

		int getVertexCount() const {
			return n;
		}

		DynamicArray<TerrainVertex*> verts;
	private:

		TerrainVertex v;

		Vector3 sum;
		int n;

	};

	TerrainMesh::TerrainMesh( SpaceTerrain * terrain, SpaceEngine * engine ) : m_terrain(terrain), m_engine(engine) {
		m_blocksX = (terrain->getWidth() + BLOCK_SIZE - 1) / BLOCK_SIZE;
		m_blocksY = (terrain->getHeight() + BLOCK_SIZE - 1) / BLOCK_SIZE;

		m_meshes = Array<Mesh*>( m_blocksX * m_blocksY );
		m_meshdatas = Array<MeshData*>( m_blocksX * m_blocksY );
		m_meshdatasNonMerged = Array<MeshData*>( m_blocksX * m_blocksY );
		m_meshes.fill( 0 );
		m_meshdatas.fill( 0 );
		m_meshdatasNonMerged.fill( 0 );
		blockFaces = Array< HashSet<TerrainFace*> >( m_blocksX * m_blocksY );

		tw = m_terrain->getWidth();
		th = m_terrain->getHeight();
		voxelToFace = Array<TerrainFace*>(tw*th);
		cornerToVert = Array<TerrainVertex*>( (tw+1)*(th+1) );
		cornerToVert.fill(0);
		voxelToFace.fill(0);

		/*
		HashMap<int,int> hm(16);
		HashSet<int> added;
		Array<int> a(100000);
		Array<int> b(100000);
		int n = 0;
		for( int j = 0; j < 1000; j++ ) {
			for ( int i = 0; i < 100; i ++ ){
				do {
					a[n] = rand();
				} while( added.contains(a[n]) );
				do {
					b[n] = rand();
				} while ( b[n] == -1 );
				hm.add( a[n], b[n] );
				added.add( a[n] );
				n++;
			}
			hm.remove(a[n-50]);
			b[n-50] = -1;
		}
		for ( int i = 0; i < 100000; i++ ){
			POOP_ASSERT( (b[i] != -1 && hm.contains(a[i]) && hm[a[i]] == b[i]) || (b[i] == -1 && !hm.contains(a[i])) );
		}
		*/

		createMesh();
	}

	Sphere TerrainMesh::getBounds( int i ) {
		int x = i%m_blocksX;
		int y = i/m_blocksX;
		Vector2 c = blockToWorld( x, y );
		return Sphere( Vector3( c.x, c.y, 0 ), 1.42f * BLOCK_SIZE * 0.1f * 0.5f );
	}

	TerrainVertex * TerrainMesh::getRealVertex( TerrainVertex * v ) {
		if ( vertexToMergeGroup.contains( v ) ) {
			return vertexToMergeGroup[v]->getMergedVertex();
		} else {
			return v;
		}
	}

	void TerrainMesh::merge( TerrainVertex * a, TerrainVertex * b ) {
		bool aic = vertexToMergeGroup.contains( a );
		bool bic = vertexToMergeGroup.contains( b );
		if ( aic && bic ) {
			// combine 2 merge groups.
			VertexMergeGroup * vmg0 = vertexToMergeGroup[a];
			VertexMergeGroup * vmg1 = vertexToMergeGroup[b];
			if ( vmg0 != vmg1 ) {
				vmg0->combine( vmg1 );
				mergeGroupCollection.remove( vmg1 );
				for( int i = 0; i < vmg1->verts.getLength(); i++ ) {
					vertexToMergeGroup[vmg1->verts[i]] = vmg0;
				}
				delete vmg1;
			}
 		} else if ( aic && !bic ) {
			VertexMergeGroup * vmg = vertexToMergeGroup[a];
			vmg->addVertex( b );
			vertexToMergeGroup.add( b, vmg );
		} else if ( !aic && bic ) {
			VertexMergeGroup * vmg = vertexToMergeGroup[b];
			vmg->addVertex( a );
			vertexToMergeGroup.add( a, vmg );
		} else if ( !aic && !bic ) {
			VertexMergeGroup * vmg = new VertexMergeGroup();
			mergeGroupCollection.add( vmg );
			vmg->addVertex( a );
			vmg->addVertex( b );
			vertexToMergeGroup.add( a, vmg );
			vertexToMergeGroup.add( b, vmg );
		}
	}

	void TerrainMesh::createMesh() {
		verts.clear();
		faces.clear();

		for ( int y = 0; y < m_terrain->getHeight(); y++ ) {
			for ( int x = 0; x < m_terrain->getWidth(); x++ ) {
				char v = m_terrain->getValByMapPos( x, y );
				if ( v ) {
					TerrainVertex *p00 = 0, *p10 = 0, *p01 = 0, *p11 = 0;
					if ( !cornerToVert[y*(tw+1)+x] ) {
						p00 = new TerrainVertex( m_terrain->mapToWorld( (float)x - 0.5f, (float)y - 0.5f ).xy0(), x, y );
						verts.add( p00 );
						cornerToVert[y*(tw+1)+x] = p00;
					} else p00 = cornerToVert[y*(tw+1)+x];

					if ( !cornerToVert[y*(tw+1)+x+1] ) {
						p10 = new TerrainVertex( m_terrain->mapToWorld( (float)x + 0.5f, (float)y - 0.5f ).xy0(), x+1, y );
						verts.add( p10 );
						cornerToVert[y*(tw+1)+x+1] = p10;
					} else p10 = cornerToVert[y*(tw+1)+x+1];

					if ( !cornerToVert[(y+1)*(tw+1)+x] ) {
						p01 = new TerrainVertex( m_terrain->mapToWorld( (float)x - 0.5f, (float)y + 0.5f ).xy0(), x, y+1 );
						verts.add( p01 );
						cornerToVert[(y+1)*(tw+1)+x] = p01;
					} else p01 = cornerToVert[(y+1)*(tw+1)+x];

					if ( !cornerToVert[(y+1)*(tw+1)+x+1] ) {
						p11 = new TerrainVertex( m_terrain->mapToWorld( (float)x + 0.5f, (float)y + 0.5f ).xy0(), x+1, y+1 );
						verts.add( p11 );
						cornerToVert[(y+1)*(tw+1)+x+1] = p11;
					} else p11 = cornerToVert[(y+1)*(tw+1)+x+1];

					//faces.append( new TerrainFace( p00, p10, p11, p01 ) );
					addFace( new TerrainFace( p00, p10, p11, p01 ), x, y );
				}
			}
		}

		// merge corners
		for ( int y = 0; y < m_terrain->getHeight()-1; y++ ) {
			for ( int x = 0; x < m_terrain->getWidth()-1; x++ ) {
				detectAndMergeCorner( x, y );
			}
		}

		generateDistanceField();
		setDepths();
		computeNormals();

		POOP_LOG( "meshes" );
		for ( int my = 0; my < m_blocksY; my++ ) {
			for ( int mx = 0; mx < m_blocksX; mx++ ) {
				createMeshBlock( mx, my );
			}
		}

		//exportTerrain();

	}

	struct DistF {
		DistF( int cx, int cy, Vector2 startPos ) : cx(cx), cy(cy), startPos(startPos) {}
		DistF() {}
		Vector2 startPos;
		int cx, cy;
	};

	void TerrainMesh::generateDistanceField() {
		POOP_LOG( "distance field.." );
		m_distanceField = Array<float>( (tw+1)*(th+1) );
		m_distanceField.fill( -1 );
		Queue<DistF> dfq( (tw+1)*(th+1) );
		for ( int cy = 1; cy < th; cy++ ) {
			for ( int cx = 1; cx < tw; cx++ ) {
				if ( isBorderVertex( cx,cy ) ) {
					TerrainVertex * tv0 = cornerToVert[cy*(tw+1)+cx];
					if ( tv0 == 0 ) continue;
					TerrainVertex * tv = getRealVertex(tv0);
					if ( tv == 0 ) continue;
					Vector3 p = tv->pos;
					dfq.push( DistF(cx,cy, Vector2(p.x,p.y) ) );
				}
			}
		}

		while( dfq.getLength() > 0 ) {
			DistF df = dfq.pop();
			if ( df.cx < 0 || df.cy < 0 || df.cx > tw || df.cy > th ) continue;
			if ( !cornerToVert[df.cy*(tw+1)+df.cx] ) continue;
			Vector3 cpf = getRealVertex( cornerToVert[df.cy*(tw+1)+df.cx] )->pos;
			Vector2 cp = Vector2( cpf.x, cpf.y );
			float p = (cp - df.startPos).length();
			float & d = m_distanceField[df.cy*(tw+1)+df.cx];
			if ( d < 0 || d > p ) {
				d = p;
				dfq.push( DistF( df.cx+1, df.cy, df.startPos ) );
				dfq.push( DistF( df.cx-1, df.cy, df.startPos ) );
				dfq.push( DistF( df.cx, df.cy+1, df.startPos ) );
				dfq.push( DistF( df.cx, df.cy-1, df.startPos ) );
				dfq.push( DistF( df.cx+1, df.cy-1, df.startPos ) );
				dfq.push( DistF( df.cx-1, df.cy+1, df.startPos ) );
				dfq.push( DistF( df.cx+1, df.cy+1, df.startPos ) );
				dfq.push( DistF( df.cx-1, df.cy-1, df.startPos ) );
			}
		}
	}

	bool TerrainMesh::isBorderVertex( int cx, int cy ) {
		//return faceCounts[cornerToVert[cy*(tw+1)+cx]] <= 2;
		return cornerToVert[cy*(tw+1)+cx] && (
			!cornerToVert[(cy)*(tw+1)+(cx+1)] ||
			!cornerToVert[(cy)*(tw+1)+(cx-1)] ||
			!cornerToVert[(cy-1)*(tw+1)+(cx)] ||
			!cornerToVert[(cy+1)*(tw+1)+(cx)]  );
	}
	
	void TerrainMesh::setDepths() {
		for ( int y = 0; y <= th; y++  ){
			for ( int x = 0; x <= tw; x++ ) {
				TerrainVertex * tv0 = cornerToVert[y*(tw+1)+x];
				if ( !tv0 ) continue;
				TerrainVertex * tv = getRealVertex( tv0 );
				if ( !tv ) continue;
				tv->pos = Vector3( tv->pos.x, tv->pos.y, -GetVertZ(x,y) );
			}
		}
	}

	void TerrainMesh::computeNormals() {

		verts.resetIteration();
		TerrainVertex * v;
		while ( verts.iterateNext( &v ) ) {
			v = getRealVertex(v);
			v->norm = Vector3(0,0,0);
		}

		faces.resetIteration();
		TerrainFace * f;
		while ( faces.iterateNext( &f ) ) {

			TerrainVertex * a = getRealVertex( f->a );
			TerrainVertex * b = getRealVertex( f->b );
			TerrainVertex * c = getRealVertex( f->c );
			TerrainVertex * d = getRealVertex( f->d );

			Vector3 norm = (c->pos - a->pos).cross(c->pos - b->pos);
			norm.normalize();
			a->norm = a->norm + norm;
			b->norm = b->norm + norm;
			c->norm = c->norm + norm;
			d->norm = d->norm + norm;
		}

		verts.resetIteration();
		while ( verts.iterateNext( &v ) ) {
			v->norm.normalize();
		}

	}

	void TerrainMesh::detectAndMergeCorner( int x, int y ) {
		// is a corner?
		bool empty = false;
		bool valid = false;
		int oX = -1, oY = -1;
		for( int u = 0; u < 2; u++ ) {
			for ( int v = 0; v < 2; v++ ) {
				if ( !m_terrain->getValByMapPos(x+u, y+v) ) {
					if ( !empty ) {
						valid = true;
						empty = true;
						oX = u;
						oY = v;
					} else {
						valid = false;
					}
				}
			}
		}
		if ( empty && valid ) {
			POOP_ASSERT( (oX == 0 || oX == 1) && (oY == 0 || oY == 1) );
			int c0x = x + oX * 2, c0y = y + 1;
			int c1x = x + 1, c1y = y + oY * 2;
			TerrainVertex * a = cornerToVert[c0y*(tw+1)+c0x];
			TerrainVertex * b = cornerToVert[c1y*(tw+1)+c1x];
			merge( a, b );
		}
	}

	void TerrainMesh::exportTerrain() {

		POOP_LOG( "Merging for export.." );

		MeshData e;
		e.append( m_meshdatas );
		POOP_LOG( "And de export.." );
		e.exportOBJ( "terr.obj" );

		MeshData enm;
		enm.append( m_meshdatasNonMerged );
		POOP_LOG( "And de export.." );
		enm.exportOBJ( "terr_nonm.obj" );
	}

	float TerrainMesh::GetVertZ( int cx, int cy ) {
		float d = m_distanceField[cy*(tw+1)+cx];
		return powf(d, 0.5f) * 1.5f;
		//return ;
	}

	void TerrainMesh::appendFace( TerrainFace * f, TerrainVertex * a, TerrainVertex * b, TerrainVertex * c, TerrainVertex * d, DynamicArray<Vertex> & nVerts, DynamicArray<Face> & faces, HashMap<TerrainVertex*, int> & vertexIndex, HashMap<TerrainVertex*, int> & closingVertex ) {
		if ( !vertexIndex.contains( a ) ) {
			vertexIndex[a] = nVerts.getLength();
			nVerts.append( Vertex( a->pos, a->norm ) );
		}
		if ( !vertexIndex.contains( b ) ) {
			vertexIndex[b] = nVerts.getLength();
			nVerts.append( Vertex( b->pos, b->norm ) );
		}
		if ( !vertexIndex.contains( c ) ) {
			vertexIndex[c] = nVerts.getLength();
			nVerts.append( Vertex( c->pos, c->norm ) );
		}
		if ( d ) {
			if ( !vertexIndex.contains( d ) ) {
				vertexIndex[d] = nVerts.getLength();
				nVerts.append( Vertex( d->pos, d->norm ) );
			}
		}
		//Face & nv = blockMData->faces[i];
		Face nv;
		nv.v[0].i = vertexIndex[a];
		nv.v[1].i = vertexIndex[b];
		nv.v[2].i = vertexIndex[c];
		if ( d ) {
			nv.v[3].i = vertexIndex[d];
		} else {
			nv.v[3].i = -1;
		}
		faces.append( nv );

		if ( f ) {
			if ( isOpenEdge(f->a,f->b) ) {
				if ( !closingVertex.contains( a ) ) {
					closingVertex.add( a, nVerts.getLength() );
					nVerts.append( Vertex( Vector3( a->pos.x, a->pos.y, 0 ) ) );
					nVerts.append( Vertex( a->pos ) );
				}
				if ( !closingVertex.contains( b ) ) {
					closingVertex.add( b, nVerts.getLength() );
					nVerts.append( Vertex( Vector3( b->pos.x, b->pos.y, 0 ) ) );
					nVerts.append( Vertex( b->pos ) );
				}
				Face f( closingVertex[b]+1, closingVertex[a]+1, closingVertex[a], closingVertex[b] );
				
				Vector3 a = nVerts[f.v[1].i].position - nVerts[f.v[0].i].position, b = nVerts[f.v[2].i].position - nVerts[f.v[0].i].position;
				Vector3 n = a.cross(b).normalized();
				nVerts[f.v[0].i].normal += n;
				nVerts[f.v[1].i].normal += n;
				nVerts[f.v[2].i].normal += n;
				nVerts[f.v[3].i].normal += n;

				faces.append( f );
			}
			if ( isOpenEdge(f->b,f->c) ) {
				if ( !closingVertex.contains( b ) ) {
					closingVertex.add( b, nVerts.getLength() );
					nVerts.append( Vertex( Vector3( b->pos.x, b->pos.y, 0 ) ) );
					nVerts.append( Vertex( b->pos ) );
				}
				if ( !closingVertex.contains( c ) ) {
					closingVertex.add( c, nVerts.getLength() );
					nVerts.append( Vertex( Vector3( c->pos.x, c->pos.y, 0 ) ) );
					nVerts.append( Vertex( c->pos ) );
				}
				Face f( closingVertex[c]+1, closingVertex[b]+1, closingVertex[b], closingVertex[c] );
				
				Vector3 a = nVerts[f.v[1].i].position - nVerts[f.v[0].i].position, b = nVerts[f.v[2].i].position - nVerts[f.v[0].i].position;
				Vector3 n = a.cross(b).normalized();
				nVerts[f.v[0].i].normal += n;
				nVerts[f.v[1].i].normal += n;
				nVerts[f.v[2].i].normal += n;
				nVerts[f.v[3].i].normal += n;

				faces.append( f );
			}
			if ( isOpenEdge(f->c,f->d) ) {
				if ( !closingVertex.contains( c ) ) {
					closingVertex.add( c, nVerts.getLength() );
					nVerts.append( Vertex( Vector3( c->pos.x, c->pos.y, 0 ) ) );
					nVerts.append( Vertex( c->pos ) );
				}
				if ( !closingVertex.contains( d ) ) {
					closingVertex.add( d, nVerts.getLength() );
					nVerts.append( Vertex( Vector3( d->pos.x, d->pos.y, 0 ) ) );
					nVerts.append( Vertex( d->pos ) );
				}
				Face f( closingVertex[d]+1, closingVertex[c]+1, closingVertex[c], closingVertex[d] );
				
				Vector3 a = nVerts[f.v[1].i].position - nVerts[f.v[0].i].position, b = nVerts[f.v[2].i].position - nVerts[f.v[0].i].position;
				Vector3 n = a.cross(b).normalized();
				nVerts[f.v[0].i].normal += n;
				nVerts[f.v[1].i].normal += n;
				nVerts[f.v[2].i].normal += n;
				nVerts[f.v[3].i].normal += n;

				faces.append( f );
			}
			if ( isOpenEdge(f->d,f->a) ) {
				if ( !closingVertex.contains( d ) ) {
					closingVertex.add( d, nVerts.getLength() );
					nVerts.append( Vertex( Vector3( d->pos.x, d->pos.y, 0 ) ) );
					nVerts.append( Vertex( d->pos ) );
				}
				if ( !closingVertex.contains( a ) ) {
					closingVertex.add( a, nVerts.getLength() );
					nVerts.append( Vertex( Vector3( a->pos.x, a->pos.y, 0 ) ) );
					nVerts.append( Vertex( a->pos ) );
				}
				Face f( closingVertex[a]+1, closingVertex[d]+1, closingVertex[d], closingVertex[a] );
				
				Vector3 a = nVerts[f.v[1].i].position - nVerts[f.v[0].i].position, b = nVerts[f.v[2].i].position - nVerts[f.v[0].i].position;
				Vector3 n = a.cross(b).normalized();
				nVerts[f.v[0].i].normal += n;
				nVerts[f.v[1].i].normal += n;
				nVerts[f.v[2].i].normal += n;
				nVerts[f.v[3].i].normal += n;

				faces.append( f );
			}
		}
	}

	void TerrainMesh::createMeshBlock( int bx, int by ) {

		MeshData * blockMData = new MeshData();

		DynamicArray<Vertex> nVerts, nVertsNonMerged;
		HashSet<TerrainFace*> & cachedFaces = blockFaces[by*m_blocksX+bx];
		//blockMData->faces = Array<Face>( cachedFaces.getLength() );
		DynamicArray<Face> faces, facesNonMerged;
		
		HashMap<TerrainVertex*, int> vertexIndex, vertexIndexNonMerged;
		HashMap<TerrainVertex*, int> closingVertex, closingVertexNonMerged;


		cachedFaces.resetIteration();
		TerrainFace * f;
		int vId = 0;
		int i = 0;
		while ( cachedFaces.iterateNext( &f ) ) {
			TerrainVertex * a = getRealVertex( f->a );
			TerrainVertex * b = getRealVertex( f->b );
			TerrainVertex * c = getRealVertex( f->c );
			TerrainVertex * d = f->d ? getRealVertex( f->d ) : 0;

			appendFace( f, a, b, c, d, nVerts, faces, vertexIndex, closingVertex );
			appendFace( 0, f->a, f->b, f->c, f->d, nVertsNonMerged, facesNonMerged, vertexIndexNonMerged, closingVertexNonMerged );
			
			i++;
		}

		for ( int i = 0; i < nVerts.getLength(); i++ ) {
			nVerts[i].normal.normalize();
		}

		blockMData->faces = faces.ToArray();

		if ( blockMData->faces.getLength() > 0 ) {
			blockMData->vertices = nVerts.ToArray();

			//POOP_LOG( String("Creating mesh of ") + blockMData.vertices.getLength() + String(" vertices and ") + blockMData.faces.getLength() + String(" faces @ (") + bx + String(", ") + by + String(")") );
			blockMData->mirrorZ();
			
			m_meshes[by*m_blocksX+bx] = blockMData->createMesh( m_engine->getRenderer(), m_engine->getResources()->vlayout_basic );
			m_meshdatas[by*m_blocksX+bx] = blockMData;

			MeshData * nm = new MeshData();
			nm->faces = facesNonMerged.ToArray();
			nm->vertices = nVertsNonMerged.ToArray();
			nm->mirrorZ();
			m_meshdatasNonMerged[by*m_blocksX+bx] = nm;
			
		} else {
			m_meshes[by*m_blocksX+bx] = NULL;
			m_meshdatas[by*m_blocksX+bx] = NULL;
		}

	}

	IntVec2 TerrainMesh::worldToBlock( Vector2 w ) { 
		Vector2 mp = m_terrain->worldToMap( w );
		return IntVec2( (int)(mp.x/BLOCK_SIZE), (int)(mp.y/BLOCK_SIZE) );
	}

	Vector2 TerrainMesh::blockToWorld( int x, int y ) { 
		float mpx = x*BLOCK_SIZE + BLOCK_SIZE*0.5f;
		float mpy = y*BLOCK_SIZE + BLOCK_SIZE*0.5f;
		return m_terrain->mapToWorld( mpx, mpy );
	}

	bool TerrainMesh::isOpenEdge( TerrainVertex * a, TerrainVertex * b ) {
		bool am = vertexToMergeGroup.contains( a );
		bool bm = vertexToMergeGroup.contains( b );
		//if ( a->pos.z == 0 && b->pos.z == 0 ) return false;
		if ( !am && !bm ) {
			return edgeFaceCount[TerrainEdge(a,b)] == 1;
		} else if ( am && !bm ) {
			VertexMergeGroup * vmg = vertexToMergeGroup[a];
			int c = 0;
			for ( int i = 0; i < vmg->verts.getLength(); i++ ) {
				c += edgeFaceCount[TerrainEdge(vmg->verts[i],b)];
				if ( c > 1 ) return false;
			}
			return c==1;
		} else if ( !am && bm ) {
			VertexMergeGroup * vmg = vertexToMergeGroup[b];
			int c = 0;
			for ( int i = 0; i < vmg->verts.getLength(); i++ ) {
				c += edgeFaceCount[TerrainEdge(a,vmg->verts[i])];
				if ( c > 1 ) return false;
			}
			return c==1;
		} else {
			VertexMergeGroup * vmg0 = vertexToMergeGroup[a];
			VertexMergeGroup * vmg1 = vertexToMergeGroup[b];
			//POOP_ASSERT(  );
			if( vmg0 == vmg1 ) return false;
			int c = 0;
			for ( int i = 0; i < vmg0->verts.getLength(); i++ ) {
				for ( int j = 0; j < vmg1->verts.getLength(); j++ ) {
					c += edgeFaceCount[TerrainEdge(vmg0->verts[i],vmg1->verts[j])];
					if ( c > 1 ) return false;
				}
			}
			return c==1;
		}
	}

	void TerrainMesh::addFace( TerrainFace * nface, int tx, int ty ) {
		faces.add( nface );
		nface->tx = tx;
		nface->ty = ty;
		voxelToFace[ty*tw+tx] = nface;
		faceCounts[nface->a]++;
		faceCounts[nface->b]++;
		faceCounts[nface->c]++;
		if ( nface->d ) faceCounts[nface->d]++;

		edgeFaceCount[TerrainEdge(nface->a, nface->b)]++;
		edgeFaceCount[TerrainEdge(nface->b, nface->c)]++;
		edgeFaceCount[TerrainEdge(nface->c, nface->d)]++;
		edgeFaceCount[TerrainEdge(nface->d, nface->a)]++;

		Vector3 c = nface->computeCentroid();
		IntVec2 blockP = worldToBlock( Vector2( c.x, c.y ) );
		blockFaces[ blockP.y * m_blocksX + blockP.x ].add( nface );
	}

	void TerrainMesh::removeMergeGroup( VertexMergeGroup * vmg ) {
		mergeGroupCollection.remove( vmg );
		for ( int i = 0; i < vmg->verts.getLength(); i++ ) {
			vertexToMergeGroup.remove( vmg->verts[i] );
		}
		delete vmg;

	}

	void TerrainMesh::removeVertex( TerrainVertex * v ) {
		if ( vertexToMergeGroup.contains(v) ) {

			VertexMergeGroup * vmg = vertexToMergeGroup[v];

			for ( int i = 0 ; i < vmg->verts.getLength(); i++ ) {
				potentialCorner.add( IntVec2( vmg->verts[i]->cx - 1, vmg->verts[i]->cy - 1 ) );

				potentialCorner.add( IntVec2( vmg->verts[i]->cx - 1 - 1, vmg->verts[i]->cy - 1 ) );
				potentialCorner.add( IntVec2( vmg->verts[i]->cx - 1, vmg->verts[i]->cy - 1 - 1 ) );
				potentialCorner.add( IntVec2( vmg->verts[i]->cx - 1 + 1, vmg->verts[i]->cy - 1 ) );
				potentialCorner.add( IntVec2( vmg->verts[i]->cx - 1, vmg->verts[i]->cy - 1 + 1 ) );
			}

			removeMergeGroup( vmg );

		}
		cornerToVert[v->cy*(tw+1)+v->cx] = 0;
		verts.remove( v );
		delete v;
	}


	void TerrainMesh::removeFace( TerrainFace * oface ) {
		faces.remove( oface );
		voxelToFace[oface->ty*tw+oface->tx] = 0;

		faceCounts[oface->a]--;
		faceCounts[oface->b]--;
		faceCounts[oface->c]--;
		if ( oface->d ) faceCounts[oface->d]--;

		edgeFaceCount[TerrainEdge(oface->a, oface->b)]--;
		edgeFaceCount[TerrainEdge(oface->b, oface->c)]--;
		edgeFaceCount[TerrainEdge(oface->c, oface->d)]--;
		edgeFaceCount[TerrainEdge(oface->d, oface->a)]--;

		Vector3 c = oface->computeCentroid();
		IntVec2 blockP = worldToBlock( Vector2(c.x,c.y) );
		blockFaces[blockP.y * m_blocksX + blockP.x].remove( oface );
		
		if ( faceCounts[oface->a] <= 0 ) removeVertex( oface->a );
		if ( faceCounts[oface->b] <= 0 ) removeVertex( oface->b );
		if ( faceCounts[oface->c] <= 0 ) removeVertex( oface->c );
		if ( oface->d && faceCounts[oface->d] <= 0 ) removeVertex( oface->d );

		delete oface;
	}

	void TerrainMesh::updateMesh() {

		HashSet<IntVec2> changedBlocks;
		IntVec2 i;
		removed.resetIteration();
		while ( removed.iterateNext( &i ) ) {
			POOP_LOG( String("Removed voxel ") + i.x + ", " + i.y );
			TerrainFace * f = voxelToFace[i.y*tw+i.x];
			Vector3 c = f->computeCentroid();
			changedBlocks.add( worldToBlock( Vector2(c.x,c.y) ) );
			removeFace( f );
		}
		removed.clear();

		potentialCorner.resetIteration();
		IntVec2 c;
		while( potentialCorner.iterateNext( &c ) ) {
			detectAndMergeCorner( c.x, c.y );
		}
		potentialCorner.clear();

		changedBlocks.resetIteration();
		while ( changedBlocks.iterateNext( &i ) ) {
			createMeshBlock( i.x, i.y );
		}

		//for ( int my = 0; my < m_blocksY; my++ ) {
		//	for ( int mx = 0; mx < m_blocksX; mx++ ) {
		//		createMeshBlock( mx, my );
		//	}
		//}


	}

	void TerrainMesh::markRemoved( int tx, int ty ) {
		potentialCorner.add( IntVec2(tx,ty) );
		potentialCorner.add( IntVec2(tx-1,ty) );
		potentialCorner.add( IntVec2(tx,ty-1) );
		potentialCorner.add( IntVec2(tx-1,ty-1) );

		removed.add( IntVec2(tx,ty) );
	}


}



