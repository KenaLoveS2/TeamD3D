#include "Shader_Client_Defines.h"

/**********Constant Buffer*********/
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4			g_WorldCamPosition;
float			g_fFar = 500.f;
float			g_fHDRValue;
/**********************************/

/**********Common Texture*********/
texture2D		g_DiffuseTexture, g_NormalTexture, g_DepthTexture;
texture2D		g_ShamanTexture;

texture2D		g_DTexture_0, g_DTexture_1, g_DTexture_2, g_DTexture_3, g_DTexture_4;
texture2D		g_MTexture_0, g_MTexture_1, g_MTexture_2, g_MTexture_3, g_MTexture_4;
/**********************************/

/**********Common Option*********/
int		        g_TextureRenderType, g_BlendType;
int		        g_SeparateWidth, g_SeparateHeight;
uint	        g_iTotalDTextureComCnt, g_iTotalMTextureComCnt;
bool            g_IsUseMask, g_IsUseNormal;
float           g_WidthFrame, g_HeightFrame, g_Time;
float4          g_vColor;
/**********************************/

/**********Dissolve*********/
bool            g_bDissolve;
float           g_fDissolveTime;
texture2D       g_DissolveTexture;
/***************************/

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float3		vWorldNormal : TEXCOORD2;
	float3		vViewDir : TEXCOORD3;
	float4		vWorldPosition : TEXCOORD4;
	float4		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
	Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));

	Out.vWorldNormal = normalize(mul(In.vNormal, (float3x3)g_WorldMatrix));
	Out.vWorldPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vViewDir = normalize(Out.vWorldPosition.xyz - g_WorldCamPosition.xyz);

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float3      vWorldNormal : TEXCOORD2;
	float3      vViewDir : TEXCOORD3;
	float4      vWorldPosition : TEXCOORD4;
	float4		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
};

struct PS_OUT
{
	float4		vDiffuse  : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth   : SV_TARGET2;
};

PS_OUT PS_SHAMANMAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse = vDiffuse * g_vColor * g_fHDRValue;
	if (Out.vDiffuse.a < 0.01f)
		discard;

	float fTime = min(g_Time, 1.f);
	Out.vDiffuse = Out.vDiffuse * (fTime / 1.f);

	Out.vNormal = vector(In.vNormal.rgb * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);

	return Out;
}

PS_OUT PS_SHAMANTHUNDER(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	In.vTexUV.x = In.vTexUV.x + g_WidthFrame;
	In.vTexUV.y = In.vTexUV.y + g_HeightFrame;

	In.vTexUV.x = In.vTexUV.x / 4.f;
	In.vTexUV.y = In.vTexUV.y / 4.f;

	float time = frac(g_Time* 0.9f);
	float2 OffsetUV = TilingAndOffset(In.vTexUV, float2(4.5f, 4.f), float2(-time, time));

	float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, OffsetUV);
	Out.vDiffuse = vDiffuse * g_vColor * g_fHDRValue;

	float fTime = min(g_Time, 1.f);
	Out.vDiffuse.a = Out.vDiffuse.a * (fTime / 1.f);

	Out.vNormal = vector(In.vNormal.rgb * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);

	return Out;
}

technique11 DefaultTechnique
{
	pass ShamanMain //0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SHAMANMAIN();
	}

	pass ShamanMain_Thunder // 1
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SHAMANTHUNDER();
	}
}
