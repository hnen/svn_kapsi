
#ifndef __D3D10ENUMS
#define __D3D10ENUMS

#include <D3D10.h>

class D3dFormat {
public:
	static const D3dFormat RGBA_BYTE;
	static const D3dFormat RGBA_FLOAT;
	static const D3dFormat FLOAT;
	static const D3dFormat DEPTH_FLOAT;

public:
	const DXGI_FORMAT dxgiFormat;
	const int channelSize;
	const int numChannels;

private:

	D3dFormat(DXGI_FORMAT dxgiFormat, int channelSize, int numChannels) : dxgiFormat(dxgiFormat), channelSize(channelSize), numChannels(numChannels) {}

};


#endif

