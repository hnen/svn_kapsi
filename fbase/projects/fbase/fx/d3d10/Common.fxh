
matrix world;
matrix view;
matrix proj;

matrix worldView;
matrix viewProj;
matrix worldViewProj;

Texture2D diffuse;

SamplerState sampler_linear {
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

BlendState blend_alpha{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
	BlendOp = ADD;
};

DepthStencilState depth_enable {
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
	DepthFunc = LESS_EQUAL;
};
DepthStencilState depth_disable {
	DepthEnable = FALSE;
	DepthWriteMask = 0;
};
