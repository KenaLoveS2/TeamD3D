#ifndef __SHADER_CLIENT_DEFINES__
#define __SHADER_CLIENT_DEFINES__

#include "Shader_Client_Utilty.h"

sampler LinearSampler = sampler_state
{
	filter = min_mag_mip_linear;
AddressU = wrap;
AddressV = wrap;
};

sampler PointSampler = sampler_state
{
	filter = min_mag_mip_Point;
	AddressU = wrap;
	AddressV = wrap;
};

sampler BorderSampler = sampler_state
{
	filter = min_mag_mip_linear;
AddressU = BORDER;
AddressV = BORDER;
};

sampler EyeSampler = sampler_state
{
	filter = min_mag_mip_Point;
AddressU = clamp;
AddressV = clamp;
};

sampler SkySampler = sampler_state
{
	filter = min_mag_mip_linear;
AddressU = clamp;
AddressV = clamp;
};

sampler MirrorSampler = sampler_state
{
	filter = min_mag_mip_Linear;
AddressU = mirror;
AddressV = mirror;
};

RasterizerState RS_Default
{
	FillMode = Solid;
CullMode = Back;
FrontCounterClockwise = false;
};

RasterizerState RS_Wireframe
{
	FillMode = wireframe;
CullMode = Back;
FrontCounterClockwise = false;
};

RasterizerState RS_CW
{
	CullMode = Front;
FrontCounterClockwise = false;
};

RasterizerState RS_CULLNONE
{
FillMode = Solid;
CullMode = none;
FrontCounterClockwise = false;

};

DepthStencilState DS_Default
{
	DepthEnable = true;
DepthWriteMask = all;
DepthFunc = less_equal;
};

DepthStencilState DS_TEST
{
	DepthEnable = false;
DepthWriteMask = all;
DepthFunc = less_equal;
};

DepthStencilState DS_ZEnable_ZWriteEnable_FALSE
{
	DepthEnable = false;
DepthWriteMask = zero;
};

DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};


BlendState BS_Default
{
	BlendEnable[0] = false;


//bool BlendEnable;
//D3D11_BLEND SrcBlend;
//D3D11_BLEND DestBlend;
//D3D11_BLEND_OP BlendOp;
//D3D11_BLEND SrcBlendAlpha;
//D3D11_BLEND DestBlendAlpha;
//D3D11_BLEND_OP BlendOpAlpha;
//UINT8 RenderTargetWriteMask;
};

BlendState BS_AlphaBlend
{
	BlendEnable[0] = true;
SrcBlend[0] = SRC_ALPHA;
DestBlend[0] = INV_SRC_ALPHA;
BlendOp[0] = Add;
};

BlendState BS_One
{
	BlendEnable[0] = true;
SrcBlend = ONE;
DestBlend = ONE;
BlendOp = Add;
};

BlendState BS_HDR
{
BlendEnable[0] = true;
SrcBlendAlpha = ONE;
DestBlendAlpha = SRC_ALPHA;
BlendOpAlpha = Add;
};

#endif