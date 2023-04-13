#include "Shader_Client_Defines.h"

vector			g_vCamPosition;
//texture2D		g_DepthTexture;
Texture2D<float4>		g_DiffuseTexture;
Texture2D<float4>		g_MaskTexture;
Texture2D<float4>		g_DissolveTexture;

float4			g_vColor = { 1.f, 1.f, 1.f, 1.f };
float4			g_vMaskColor = { 1.f ,1.f, 1.f, 1.f };
float4			g_vDissolveColor = { 1.f, 1.f, 1.f, 1.f };

float			g_fDissolveAlpha = 0.f;
float			g_fHDRItensity;

float			g_fCutY;			 /* For. discard Y Range option */

/* Option */
bool			g_IsSpriteAnim = false, g_IsUVAnim = false;

/* UV Animation */
float			g_fUVSpeedX = 0.f, g_fUVSpeedY = 0.f;

/* Sprite Animation */
int				g_XFrames = 1, g_YFrames = 1;
int				g_XFrameNow = 0, g_YFrameNow = 0;

/* UV Scale */
float		g_UVScaleX = 1.f;
float		g_UVScaleY = 1.f;

struct VS_IN
{
	float3		vPosition	: POSITION;
	float3		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
	float3		vTangent	: TANGENT;
};

struct VS_OUT
{
	float4		vPosition		: SV_POSITION;
	float4		vNormal			: NORMAL;
	float2		vTexUV			: TEXCOORD0;
	float4		vProjPos		: TEXCOORD1;
	float3		vWorldNormal	: TEXCOORD2;
	float3		vViewDir		: TEXCOORD3;
	float4		vWorldPosition	: TEXCOORD4;
	float4		vTangent		: TANGENT;
	float3		vBinormal		: BINORMAL;
	float4		vInPosition		: TEXCOORD5;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;
	matrix		matWV, matWVP;

	matWV				= mul(g_WorldMatrix, g_ViewMatrix);
	matWVP				= mul(matWV, g_ProjMatrix);

	Out.vInPosition		= vector(In.vPosition, 1.f);
	Out.vPosition		= mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal			= normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV			= In.vTexUV;
	Out.vProjPos		= Out.vPosition;
	Out.vTangent		= normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
	Out.vBinormal		= normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));

	Out.vWorldNormal	= normalize(mul(In.vNormal, (float3x3)g_WorldMatrix));
	Out.vWorldPosition	= mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vViewDir		= normalize(Out.vWorldPosition.xyz - g_vCamPosition.xyz);

	return Out;
}

struct PS_IN
{
	float4		vPosition		: SV_POSITION;
	float4		vNormal			: NORMAL;
	float2		vTexUV			: TEXCOORD0;
	float4		vProjPos		: TEXCOORD1;
	float3      vWorldNormal	: TEXCOORD2;
	float3      vViewDir		: TEXCOORD3;
	float4      vWorldPosition	: TEXCOORD4;
	float4		vTangent		: TANGENT;
	float3		vBinormal		: BINORMAL;
	float4		vInPosition		: TEXCOORD5;
};

struct PS_OUT
{
	float4		vDiffuse		: SV_TARGET0;
	float4		vNormal			: SV_TARGET1;
	float4		vDepth			: SV_TARGET2;
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
		In.vTexUV.x *= g_float2_1.x;
		In.vTexUV.x += g_float2_0.x;

		In.vTexUV.y *= g_float2_1.y;
		In.vTexUV.y += g_float2_0.y;
	}


	Out.vDiffuse	= g_tex_0.Sample(LinearSampler, In.vTexUV);
	Out.vNormal.rgb = In.vNormal.rgb;
	Out.vDepth		= vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.0f, 0.0f);

	Out.vDiffuse		*= g_float4_0;
	Out.vDiffuse.rgb	*= g_float_0;
	//Out.vDiffuse		= CalcHDRColor(Out.vDiffuse, g_float_0);

	Out.vDiffuse.a		*= abs(g_float_1 - abs(In.vInPosition.y)) / g_float_1;

	if (In.vInPosition.y > g_float_1)
		discard;

	return Out;
}

PS_OUT PS_MAIN_ONLYCOLOR(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse		= CalcHDRColor(g_float4_0, g_float_0);
	Out.vNormal.rgb		= In.vNormal.rgb;
	Out.vDepth			= vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.0f, 0.0f);

	if (In.vInPosition.y > g_float_1)
		discard;

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
		In.vTexUV.x *= g_float2_1.x;
		In.vTexUV.x += g_float2_0.x;

		In.vTexUV.y *= g_float2_1.y;
		In.vTexUV.y += g_float2_0.y;
	}

	float4	vMask		= g_tex_1.Sample(LinearSampler, In.vTexUV);
	vMask *= g_float4_1;

	Out.vDiffuse	= vDiffuse;
	Out.vDiffuse.a	= vMask.r;

	Out.vDiffuse	*= g_float4_0;
	//Out.vDiffuse	= CalcHDRColor(Out.vDiffuse, g_float_0);
	Out.vDiffuse.rgb *= g_float_0;

	Out.vDiffuse.a	*= abs(g_float_1 - abs(In.vInPosition.y)) / g_float_1;

	if (In.vInPosition.y > g_float_1)
		discard;

	return Out;
}

PS_OUT PS_MAIN_DISSOLVE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

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
		In.vTexUV.x *= g_float2_1.x;
		In.vTexUV.x += g_float2_0.x;

		In.vTexUV.y *= g_float2_1.y;
		In.vTexUV.y += g_float2_0.y;
	}

	float4	vMask		= g_tex_2.Sample(LinearSampler, In.vTexUV);
	vMask *= g_float4_2;

	Out.vDiffuse	= vDiffuse;
	Out.vDiffuse.a	= vMask.r;

	Out.vDiffuse	*= g_float4_0;
	//Out.vDiffuse	= CalcHDRColor(Out.vDiffuse, g_float_0);
	Out.vDiffuse.rgb *= g_float_0;

	if (In.vInPosition.y > g_float_1)
		discard;

	if (Out.vDiffuse.a < g_float_2)
		discard;

	return Out;
}

//PS_OUT PS_MAIN_DefaultMask_V2(PS_IN In)
//{
//	PS_OUT		Out = (PS_OUT)0;
//
//	float2 MoveUV = In.vTexUV;
//	MoveUV.x += g_float2_0.x;
//	MoveUV.y += g_float2_0.y;
//
//	float4	vDiffuse	= g_tex_0.Sample(LinearSampler, In.vTexUV);
//	float4	vMask		= g_tex_1.Sample(LinearSampler, MoveUV);
//
//	float	vDiffuseR	= vDiffuse.r;
//	vMask *= g_float4_1;
//
//	Out.vDiffuse		= vDiffuse;
//	Out.vDiffuse.a		= vMask.r;
//
//	Out.vDiffuse		*= g_float4_0;
//	//Out.vDiffuse.rgb	*= g_float_0;
//	Out.vDiffuse = CalcHDRColor(Out.vDiffuse, g_float_0);
//
//	if (In.vInPosition.y > g_float_1)
//		discard;
//
//	if (vDiffuseR < g_float_2)
//		discard;
//
//	return Out;
//}

PS_OUT PS_MAIN_DefaultMask_V2(PS_IN In)
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
		In.vTexUV.x *= g_float2_1.x;
		In.vTexUV.x += g_float2_0.x;

		In.vTexUV.y *= g_float2_1.y;
		In.vTexUV.y += g_float2_0.y;
	}

	float4	vMask = g_tex_1.Sample(LinearSampler, In.vTexUV);
	vMask *= g_float4_1;

	Out.vDiffuse = vDiffuse;
	Out.vDiffuse.a = vMask.r;

	Out.vDiffuse *= g_float4_0;
	Out.vDiffuse	= CalcHDRColor(Out.vDiffuse, g_float_0);
	//Out.vDiffuse.rgb *= g_float_0;

	Out.vDiffuse.a *= abs(g_float_1 - abs(In.vInPosition.y)) / g_float_1;

	if (In.vInPosition.y > g_float_1)
		discard;

	return Out;
}

PS_OUT PS_MAIN_MASK_DiffuseMove(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 UVOrignial = In.vTexUV;

	if (g_bool_0)
	{
		In.vTexUV.x = In.vTexUV.x + g_int_2;
		In.vTexUV.y = In.vTexUV.y + g_int_3;

		In.vTexUV.x = In.vTexUV.x / g_int_0;
		In.vTexUV.y = In.vTexUV.y / g_int_1;
	}

	if (g_bool_1)
	{
		In.vTexUV.x *= g_float2_1.x;
		In.vTexUV.x += g_float2_0.x;

		In.vTexUV.y *= g_float2_1.y;
		In.vTexUV.y += g_float2_0.y;
	}

	float4	vDiffuse	= g_tex_0.Sample(LinearSampler, In.vTexUV);

	float4	vMask		= g_tex_1.Sample(LinearSampler, UVOrignial);
	vMask *= g_float4_1;

	Out.vDiffuse		= vDiffuse;
	Out.vDiffuse.a		= vMask.r;

	Out.vDiffuse		*= g_float4_0;
	Out.vDiffuse = CalcHDRColor(Out.vDiffuse, g_float_0);
	//Out.vDiffuse.rgb *= g_float_0;

	Out.vDiffuse.a *= abs(g_float_1 - abs(In.vInPosition.y)) / g_float_1;

	if (In.vInPosition.y > g_float_1)
		discard;

	return Out;
}

technique11  DefaultTechnique
{
	pass Default //0
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass OnlyColor // 1
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ONLYCOLOR();
	}

	pass DefaultMask // 2
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MASK();
	}

	pass Dissolve // 3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
	}

	pass DefaultMask_V2 // 4
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DefaultMask_V2();
	}

	pass DefaultMask_DiffuseMove // 5
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MASK_DiffuseMove();
	}
}