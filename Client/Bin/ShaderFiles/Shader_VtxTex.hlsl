
#include "Shader_Client_Defines.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_Texture;
texture2D		g_DepthTexture;
texture2D		g_MaskTexture;

int		g_XFrameNow = 0, g_YFrameNow = 0; /* Current Sprite frame */
int		g_XFrames = 1, g_YFrames = 1; /* the number of sprites devided each side */

float	g_fSpeedX = 0.f, g_fSpeedY = 0.f;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;


	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	/*SV_TARGET0 : 모든 정보가 결정된 픽셀w이다. AND 0번째 렌더타겟에 그리기위한 색상이다. */
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_MAIN_EFFECT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	float2		vTexUV; 

	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, vTexUV);

	float		fOldViewZ = vDepthDesc.y * 300.f;
	float		fViewZ = In.vProjPos.w;

	Out.vColor.a = Out.vColor.a * (saturate(fOldViewZ - fViewZ) * 2.5f);

	return Out;
}

PS_OUT PS_MAIN_AlphaBlend(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_MAIN_MASKMAP(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vDiffuse	= g_Texture.Sample(LinearSampler, In.vTexUV);
	vector vMask	= g_MaskTexture.Sample(LinearSampler, In.vTexUV);

	vDiffuse.a = vMask.r;

	Out.vColor = vDiffuse;

	return Out;
}

PS_OUT PS_MAIN_HPBar(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (In.vTexUV.x > 0.01 && In.vTexUV.x < 0.99)
		In.vTexUV.x = (In.vTexUV.x - 0.5) / 3 + 0.5;
	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_MAIN_SPRITE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	In.vTexUV.x = In.vTexUV.x + g_XFrameNow;
	In.vTexUV.y = In.vTexUV.y + g_YFrameNow;

	In.vTexUV.x = In.vTexUV.x / g_XFrames;
	In.vTexUV.y = In.vTexUV.y / g_YFrames;


	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_MAIN_UVMOVE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	In.vTexUV.x += g_fSpeedX;
	In.vTexUV.y += g_fSpeedY;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	return Out;
}

technique11 DefaultTechnique
{
	pass Rect // 0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass DiffuseAlphaBlend // 1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_AlphaBlend();
	}

	pass MaskMap // 2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MASKMAP();
	}

	pass HPBar // 3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HPBar();
	}

	pass AlphaBlendSprites // 4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SPRITE();
	}

	pass DefaultUVMove
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_UVMOVE();
	}
}
