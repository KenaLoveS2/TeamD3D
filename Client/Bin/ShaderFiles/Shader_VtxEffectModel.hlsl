#include "Shader_Client_Defines.h"

/**********Constant Buffer*********/
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
/**********************************/

texture2D		g_DepthTexture, g_NormalTexture;
texture2D		g_DTexture_0, g_DTexture_1, g_DTexture_2, g_DTexture_3, g_DTexture_4;
texture2D		g_MTexture_0, g_MTexture_1, g_MTexture_2, g_MTexture_3, g_MTexture_4;

// Type
int		g_TextureRenderType, g_BlendType;
bool    g_IsUseMask, g_IsUseNormal;

int		g_SeparateWidth, g_SeparateHeight;
uint	g_iTotalDTextureComCnt, g_iTotalMTextureComCnt;

float   g_WidthFrame, g_HeightFrame, g_Time;
float4  g_vColor;

float4  g_WorldCamPosition;
// ~Type

uniform float emission_amount= 5.0f;
uniform float rim_steepness  = 3.0f; //higher values mean a smaller rim.

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
	float3      vWorldNormal : TEXCOORD2;
	float3      vViewDir : TEXCOORD3;
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

	float4 worldPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vViewDir = normalize(worldPosition.xyz - g_WorldCamPosition.xyz);

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
	float4		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
};

struct PS_OUT
{
	float4		vDiffuse  : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth   : SV_TARGET2;
};

float3 fresnel_glow(float amount, float intensity, float3 color, float3 normal, float3 view)
{
	return pow((1.0 - dot(normalize(normal), normalize(view))), amount) * color * intensity;
}

uniform float guruguru_power = 10.0;
uniform float vacuum_power = 0.1;
uniform float radius : hint_range = 0.8;
uniform float4 albedo : hint_color = float4(0.5, 0.5, 0.5, 1.0);

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	// this code
	//float2		refractTexCoord;
	//////vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	//////vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;
	//refractTexCoord.x = (In.vProjPos.x / In.vProjPos.w) / 2.0f + 0.5f;
	//refractTexCoord.y = -(In.vProjPos.y / In.vProjPos.w) / 2.0f + 0.5f;

	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.0f, 0.0f);
	////
	//// Sample the normal from the normal map texture.
	//float4 normalmap = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	//float3 normal = (normalmap.xyz * 2.0f) - 1.0f;

	//refractTexCoord = refractTexCoord + (normal.xy * 2.f);
	//// Sample the texture pixel from the refraction texture using the perturbed texture coordinates.
	//vector refractionColor = g_DTexture_0.Sample(LinearSampler, refractTexCoord);

	//// Sample the texture pixel from the glass color texture.
	//vector textureColor = g_DTexture_0.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y - 0.5f * g_Time));
	//Out.vDiffuse = lerp(refractionColor, textureColor, 0.5f);

	//// 이 코드만 열면 됨 
//	float4 d = g_DTexture_0.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y - 0.5f * g_Time * 1.f));   //검정
//	vector albedo = g_DTexture_0.Sample(LinearSampler, In.vTexUV + d.r);
////	vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, vTexUV);
//
//	// DTexture == 40
//	// Blend_Mix
//	float4 fresnel_color = float4(0, 0.399, 0.801, 1);
//	float4 fresnel = float4(fresnel_glow(3.0, 4.5, fresnel_color.rgb, In.vNormal.rgb, -In.vViewDir), 0.2f);
//	Out.vDiffuse = albedo * fresnel;
	////

	//
	float2 vTexUV;
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	float4 d = g_NormalTexture.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y - 0.5f * g_Time * 1.f));   //검정
	vector albedo = g_DTexture_0.Sample(LinearSampler, In.vTexUV + d.r);
	vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, vTexUV);

	// DTexture == 40
	// Blend_Mix
	float4 fresnel_color = float4(0, 0.399, 0.801, 1);
	float4 fresnel = float4(fresnel_glow(3.0, 4.5, fresnel_color.rgb, In.vNormal.rgb, -In.vViewDir), 0.2f);

	//
	float   fOldViewZ = vDepthDesc.y * 300.f;
	float   fViewZ = In.vProjPos.w;

	Out.vDiffuse = albedo *2.f;
	Out.vDiffuse = saturate(Out.vDiffuse) * fresnel;
	Out.vDiffuse.a = Out.vDiffuse.a * (saturate(fOldViewZ - fViewZ) * 2.5f);
	//
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.0f, 0.0f);

	//
	//
	//float   fOldViewZ = vDepthDesc.y * 300.f;
	//float   fViewZ = In.vProjPos.w;

	//Out.vDiffuse.a = Out.vDiffuse.a * (saturate(fOldViewZ - fViewZ) * 2.5f);
	

	// ~this code

	//float4 base_color = float4(1, 1, 1, 1);
	//float4 fresnel_color = float4(g_vColor);
	//fresnel_color.a = 0.0f;
	//float4 fresnel = float4(fresnel_glow(4.0, 4.5, fresnel_color, In.vNormal, -In.vViewDir), 0.4f);
	//Out.vDiffuse = base_color * fresnel;

	return Out;
}

technique11 DefaultTechnique
{
	pass Default //0
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_TEST, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Effect_Alpha // 1
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

	pass Effect_Black // 2
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_One, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Effect_Mix // 3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Mix, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
