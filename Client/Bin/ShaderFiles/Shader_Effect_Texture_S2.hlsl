#include "Shader_Client_Defines.h"

//Texture2D<float4>		g_DiffuseTexture;
//Texture2D<float4>		g_MaskTexture;

//float			g_fHDRItensity;

//float4			g_vColor = { 1.f, 1.f, 1.f, 1.f };
//float4			g_vMaskColor = { 1.f ,1.f, 1.f, 1.f };

/* Option */
//bool			g_IsSpriteAnim = false, g_IsUVAnim = false;

/* UV Animation */
//float			g_fUVSpeedX = 0.f, g_fUVSpeedY = 0.f;

/* Sprite Animation */
//int				g_XFrames = 1, g_YFrames = 1;
//int				g_XFrameNow = 0, g_YFrameNow = 0;


/* Old (remove later) */
//texture2D		g_Texture;
//texture2D		g_DepthTexture;
//texture2D		g_NoiseTexture;
//int		g_XFrameNow = 0, g_YFrameNow = 0;	/* Current Sprite frame */
//int		g_XFrames = 1, g_YFrames = 1;		/* the number of sprites devided each side */
//float	g_fSpeedX = 0.f, g_fSpeedY = 0.f;	/* UV Move Speed */
//
//float4	g_vMinColor = { 0.f, 0.f, 0.f,0.f };
//
//float	g_fAlpha = 1.f;
//float	g_fAmount = 1.f; /* Guage Data (normalized) */
//float	g_fEnd = 0.f;

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
	float4		vColor : SV_TARGET0;
};

/* Common Var */
// g_tex_0		: DiffuseTexture
// g_tex_1		: MaskTexture
// g_tex_2		: DissolveTexture

// g_float_0	: fHDRIntensity
// g_float_1	: fCutY
// g_float_2	: fDissolveAlpha

// g_bool_0		: isSpriteAnim
// g_bool_1		: isUVAnim

// g_float2_0	: vUVMove
// g_float2_1	: vUVScale

// g_int_0		: XFrames
// g_int_1		: YFrames
// g_int_2		: XFrameNow
// g_int_3		: YFrameNow

// g_float4_0	: DiffuseColor
// g_float4_1	: MaskColor
// g_float4_2	: DissolveColor

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (g_bool_0)
	{
		In.vTexUV.x = In.vTexUV.x + g_int_2;
		In.vTexUV.y = In.vTexUV.y + g_int_3;

		In.vTexUV.x = In.vTexUV.x / g_int_0;
		In.vTexUV.y = In.vTexUV.y / g_int_1;
	}

	if (g_bool_1)
	{
		In.vTexUV.x += g_float2_0.x;
		In.vTexUV.y += g_float2_0.y;
	}

	float4	vDiffuse = g_tex_0.Sample(LinearSampler, In.vTexUV);
	Out.vColor = vDiffuse;
	Out.vColor *= g_float4_0;

	Out.vColor.rgb *= g_float_0;

	return Out;
}

PS_OUT PS_MAIN_MASK(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4	vDiffuse = g_tex_0.Sample(LinearSampler, In.vTexUV);

	if (g_bool_0)
	{
		In.vTexUV.x = In.vTexUV.x + g_int_2;
		In.vTexUV.y = In.vTexUV.y + g_int_3;

		In.vTexUV.x = In.vTexUV.x / g_int_0;
		In.vTexUV.y = In.vTexUV.y / g_int_1;
	}

	if (g_bool_1)
	{
		In.vTexUV.x += g_float2_0.x;
		In.vTexUV.y += g_float2_0.y;
	}

	float4	vMask = g_tex_1.Sample(LinearSampler, In.vTexUV);
	vMask *= g_float4_1;

	Out.vColor = vDiffuse;
	Out.vColor.a = vMask.r;

	Out.vColor *= g_float4_0;
	Out.vColor.rgb *= g_float_0;

	return Out;
}

technique11 DefaultTechnique
{
	pass Default // 0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass DefaultMask // 1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MASK();
	}

}
