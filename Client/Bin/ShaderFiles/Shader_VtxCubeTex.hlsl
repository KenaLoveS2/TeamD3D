#include "Shader_Client_Defines.h"

textureCUBE		g_Texture;
float				g_fColorIntensity;
bool				g_bFog;
float4			g_vFogColor;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;


	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
	float4		vAmbient : SV_TARGET3;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector FinalColor = g_SkyTexture.Sample(LinearSampler, In.vTexUV);

	if(!g_bFog)
	{
		FinalColor.rgb *= g_fColorIntensity;
	}
	else
	{
		FinalColor.rgb = g_vFogColor.rgb;
	}

	Out.vColor = FinalColor;

	return Out;
}

technique11 DefaultTechnique
{
	pass Rect
	{
		SetRasterizerState(RS_CW);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);		
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);	

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
