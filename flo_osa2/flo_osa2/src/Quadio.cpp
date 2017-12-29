
#include "Quadio.h"


		int Quadio::maxStep;
		int Quadio::maxStepLast;

Quadio::Quadio()
{
	mesh = NULL;
	maxStep = 1;
	maxStepLast = 1;
}


Quadio::~Quadio()
{
}


Vector3 Quadio::getNormal( int i1, int i2, int i3, int i4, KoomaMesh* m )
{
	Vector3 p1 = m->vertices[i1].position;
	Vector3 p2 = m->vertices[i2].position;
	Vector3 p3 = m->vertices[i3].position;
	Vector3 p4 = m->vertices[i4].position;
	Vector3 normal(0,0,0);		// Newell was here
/*	normal.x = (p1.y-p2.y) * (p1.z + p2.z)	+ (p2.y-p3.y) * (p2.z + p3.z)	+ (p3.y-p4.y) * (p3.z + p4.z)	+ (p4.y-p1.y) * (p4.z + p1.z);
	normal.y = (p1.z-p2.z) * (p1.x + p2.x)	+ (p2.z-p3.z) * (p2.x + p3.x)	+ (p3.z-p4.z) * (p3.x + p4.x)	+ (p4.z-p1.z) * (p4.x + p1.x);
	normal.z = (p1.x-p2.x) * (p1.y + p2.y)	+ (p2.x-p3.x) * (p2.y + p3.y)	+ (p3.x-p4.x) * (p3.y + p4.y)	+ (p4.x-p1.x) * (p4.y + p1.y);*/
	normal = p1 - p2;
	normal = normal.cross( Vector3( p1-p3 ) );
	normal.normalize();
	return normal;
}

