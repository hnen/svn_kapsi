#pragma once

#include "poopie/base/String.h"
#include "poopie/gfx/Mesh.h"
#include "poopie/gfx/MeshData.h"
#include "poopie/base/DynamicArray.h"
#include "MeshGen.h"

using namespace poopie::gfx;
using namespace poopie;

	class Quadio
	{
	public:
		~Quadio();
		Quadio();

		class Planoid
		{
		public:
			Planoid()
			{
				m_parent = NULL;
				m_away = false;
				extrudeAmount = 0;
				m_normal = Vector3(0,0,0);
			}
			void clear()
			{
				if ( childs.getLength() > 0  )
				{
					for ( uint a=0; a<childs.getLength(); a++ )
					{
						childs[a]->clear();
						delete childs[a];
					}
					childs.resize(0);
				}
			}
			int childExtruded()
			{
				int e = 0;
				for ( int a=0; a<childs.getLength(); a++ )
					if ( childs[a]->extrudeAmount > 0 ) e++;
				return e;
			}

			void addChild( Planoid* p )
			{
				p->m_parent = this;
				childs.Append( p );
			}

			void away()
			{
				if ( m_parent != NULL )
					m_away = true;
			}

			bool m_away;
			Planoid* m_parent;
			DynamicArray<Planoid*> childs;
			float extrudeAmount;
			Vector3 m_normal;
		};

		class KoomaMesh
		{
		public:
			DynamicArray<poopie::gfx::Vertex> vertices;
			DynamicArray<int> indices;
		};


		Planoid me;
		KoomaMesh* mesh;


		void basic()
		{
			DynamicArray<Vertex> vertices(4);
			DynamicArray<int> indices(0);

			vertices[0].position = Vector3(-0.5f, 0,-0.5f );
			vertices[1].position = Vector3( 0.5f, 0,-0.5f );
			vertices[2].position = Vector3( 0.5f, 0, 0.5f );
			vertices[3].position = Vector3(-0.5f, 0, 0.5f );
			
			mesh = new KoomaMesh();

			(mesh->vertices) = vertices;
			(mesh->indices) = indices;

		}

		void clear()
		{
			if ( mesh != NULL ) 
				delete mesh;
/*			if ( me.childs.getLength() > 0 )
			{
				me.clear();
			}*/
		}


		Vector3 getNormal( int i1, int i2, int i3, int i4, KoomaMesh* m );

		static int maxStep;
		static int maxStepLast;

		int growPos;

		poopie::gfx::MeshData* get( int i1 = 0, int i2 = 0, int i3 = 0, int i4 = 0, KoomaMesh* m = NULL, int max = 2, Planoid* pla = NULL, int step = 0, bool extru = true )
		{
			bool first = false;
			if ( m == NULL )
			{
				clear();
				basic();
				m = mesh;
				i1 = 0;
				i2 = 1;
				i3 = 2;
				i4 = 3;
				pla = &me;
				first = true;
				maxStepLast = maxStep;
				if ( (rand()%100) == 10 )
					growPos = maxStep;
//					growPos = rand()%(maxStep+1);
				if ( growPos >= 0 )
				{
					String kj = String("step: ") + String(growPos) + " / " + String(maxStep);
					POOP_WARNING( kj );
				}
				maxStep = 1;
			}

			if ( step > maxStep ) maxStep = step;



			if ( step == growPos && pla->extrudeAmount==0 )
			{
				pla->extrudeAmount = 0.01f;
				growPos = -1;
			}
				

			if ( first )
			{
				if ( me.extrudeAmount <= 0 )
					me.extrudeAmount = 0.01f;
			}
/*			else
			{
				if ( extru )
					if ( rand()%20000 < step )
						if ( pla->extrudeAmount== 0 )
							pla->extrudeAmount = 0.01f;
			}*/

			if ( pla->m_away )
			{
				if ( pla->extrudeAmount > 0 ) 
				{
					pla->extrudeAmount -= 0.01f;
					if ( pla->extrudeAmount <= 0 )
					{
						pla->m_away = false;
						pla->extrudeAmount = 0;
					}
				}
			}
			else
				if ( pla->extrudeAmount>0 && pla->extrudeAmount < 1 ) 
					pla->extrudeAmount += 0.01f;

//			if ( pla->extrudeAmount == 0  )
			{
				m->indices.Append( i2 );
				m->indices.Append( i1 );
				m->indices.Append( i3 );
				m->indices.Append( i4 );
				m->indices.Append( i3 );
				m->indices.Append( i1 );
			}
//			else
			if ( pla->extrudeAmount > 0  )
			{
				Vertex v[4];
				Vector3 normal = getNormal( i1, i2, i3, i4, m );
				Vector3 p1 = m->vertices[i1].position;
				Vector3 p2 = m->vertices[i2].position;
				Vector3 p3 = m->vertices[i3].position;
				Vector3 p4 = m->vertices[i4].position;
				Vector3 center = (p1+p2+p3+p4) / 4.0f;
/*				v[0].position =  (p1+normal)*pla->extrudeAmount + center*(1.0f-pla->extrudeAmount);
				v[1].position =  (p2+normal)*pla->extrudeAmount + center*(1.0f-pla->extrudeAmount);
				v[2].position =  (p3+normal)*pla->extrudeAmount + center*(1.0f-pla->extrudeAmount);
				v[3].position =  (p4+normal)*pla->extrudeAmount + center*(1.0f-pla->extrudeAmount);*/
/*				v[0].position =  (p1*pla->extrudeAmount+center*(1.0f-pla->extrudeAmount)) + (normal * pla->extrudeAmount);
				v[1].position =  (p2*pla->extrudeAmount+center*(1.0f-pla->extrudeAmount)) + (normal * pla->extrudeAmount);
				v[2].position =  (p3*pla->extrudeAmount+center*(1.0f-pla->extrudeAmount)) + (normal * pla->extrudeAmount);
				v[3].position =  (p4*pla->extrudeAmount+center*(1.0f-pla->extrudeAmount)) + (normal * pla->extrudeAmount);*/
				v[0].position =  m->vertices[i1].position + (normal * pla->extrudeAmount);
				v[1].position =  m->vertices[i2].position + (normal * pla->extrudeAmount);
				v[2].position =  m->vertices[i3].position + (normal * pla->extrudeAmount);
				v[3].position =  m->vertices[i4].position + (normal * pla->extrudeAmount);
				int m1 = m->vertices.Append( v[0] );
				int m2 = m->vertices.Append( v[1] );
				int m3 = m->vertices.Append( v[2] );
				int m4 = m->vertices.Append( v[3] );

				if ( pla->childs.getLength()  == 0 )
				{
					pla->addChild( new Planoid() );
					pla->addChild( new Planoid() );
					pla->addChild( new Planoid() );
					pla->addChild( new Planoid() );
					pla->addChild( new Planoid() );
				}

				bool ex = pla->childExtruded() > 1 || pla->extrudeAmount < 0.6f ? false : true;
				
				int z = 0;
				get( i2, i1, m1, m2, m, max-1, pla->childs[z++], step+1, ex );
				get( i3, i2, m2, m3, m, max-1, pla->childs[z++], step+1, ex );
				get( i4, i3, m3, m4, m, max-1, pla->childs[z++], step+1, ex );
				get( i1, i4, m4, m1, m, max-1, pla->childs[z++], step+1, ex );

				get( m1, m2, m3, m4, m, max-1, pla->childs[z++], step+1, true );
			}
			if ( first )
			{
				if ( pla->m_away )
				{
					int kos = 2;
				}
				MeshData *m_ = new MeshData();
				m_->indices = mesh->indices;
				m_->vertices = mesh->vertices;
				m_->flipFaces();
				m_->computeNormals();
				return m_;
			}
			return NULL;
		}


	};


