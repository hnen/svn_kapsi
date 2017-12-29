
#ifndef _SYNC
#define _SYNC

#include <Windows.h>

#include "Demo.h"
#include "poopie.h"
#include <poopie\ui\win32\Window.h>

using namespace poopie;
using namespace poopie::gfx;

class Sync
{
public:
	Sync()
	{
		count=32;
		data = new float*[count];
	}
	~Sync()
	{
		delete[] data;
	}

	float getValue( int value )
	{
		float d = 0;
		for ( int a=0; a<min(num,count); a++ )
			d += data[a][value];
		return d;
	}

	float getValue( int rangeBegin, int rangeEnd )
	{
		float d = 0;
		for ( int a=0; a<min(num,count); a++ )
		{
			float* c = data[a];
			for ( int b=rangeBegin; b<rangeEnd; b++ )
				d += c[a][b];
		}
		return d;
	}

	void update()
	{
		Demo& demo = Demo::get();
		DemoResources& res = DemoResources::get();
		data[current] = res.music->getSpectrum(64);
		current = (current+1)%count;
		num++;
	}

	float** data;
	int current;
	int count;
	int num;
};



#endif
