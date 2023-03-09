
#include "Shader_Client_Defines.h"

/***********Constant Buffers***********/
matrix g_BoneMatrices[800];
matrix g_WorldMatrix;
matrix g_ViewMatrix;
matrix g_ProjMatrix;
float	   g_fFar = 300.f;
float4 g_vCamPosition;
/**************************************/
Texture2D<float4>		g_DiffuseTexture;
Texture2D<float4>		g_NormalTexture;

Texture2D<float4>		g_GlowTexture;
Texture2D<float4>		g_AO_R_MTexture;
Texture2D<float4>		g_EmissiveTexture;
Texture2D<float4>		g_EmissiveMaskTexture;
Texture2D<float4>		g_OpacityTexture;

Texture2D<float4>		g_AOTexture;
Texture2D<float4>		g_RoughnessTexture;
Texture2D<float4>		g_MaskTexture;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
	uint4			vBlendIndex : BLENDINDEX;
	float4		vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float4		vTangent : TANGENT;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float3		vViewDir : TEXCOORD2;
	float3		vBinormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float4 worldPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vViewDir = normalize(worldPosition.xyz - g_vCamPosition.xyz);

	float			fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix			BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices[In.vBlendIndex.w] * fWeightW;

	vector			vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
	vector			vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
	vector			vTangent = mul(float4(In.vTangent, 0.f), BoneMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vTangent = normalize(mul(vTangent, g_WorldMatrix));
	Out.vProjPos = Out.vPosition;
	Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float4		vTangent : TANGENT;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float3		vViewDir : TEXCOORD2;
	float3		vBinormal : BINORMAL;
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
	float4		vAmbient : SV_TARGET3;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = vDiffuse;

	return Out;
}//0

PS_OUT PS_MAIN_AO_R_M(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vAO_R_MDesc = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float4 FinalColor = float4(0.f, 0.f, 0.f, 1.f);

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	FinalColor = vDiffuse;

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = vAO_R_MDesc;

	return Out;
}//1

PS_OUT PS_MAIN_AO_R_M_E(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vAO_R_MDesc = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);
	vector		vEmissiveDesc  = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float4 FinalColor = float4(0.f, 0.f, 0.f, 1.f);

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	FinalColor = vDiffuse +	(vDiffuse * vEmissiveDesc);

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, length(vEmissiveDesc), 0.f);
	Out.vAmbient = vAO_R_MDesc;

	return Out;
}//2

// GLOW
PS_OUT PS_MAIN_AO_R_M_G(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vAO_R_MDesc = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);
	vector		vGlowDesc = g_GlowTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float4 FinalColor = float4(0.f, 0.f, 0.f, 1.f);

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	if (0.1f > vGlowDesc.a)
		FinalColor = vDiffuse;
	else
		FinalColor = vDiffuse + vGlowDesc;

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, length(vGlowDesc), 0.f);
	Out.vAmbient = vAO_R_MDesc;

	return Out;
}//3

//OPACITY
PS_OUT PS_MAIN_AO_R_M_O(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vAO_R_MDesc = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);
	vector		vOpacityDesc = g_OpacityTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float4 FinalColor = float4(0.f, 0.f, 0.f, 1.f);

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	float fOpacity = vOpacityDesc.r;

	FinalColor = float4(vDiffuse.rgb, fOpacity * vDiffuse.a);

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = vAO_R_MDesc;

	return Out;
}//4

 //EMISSIVEMASK
PS_OUT PS_MAIN_AO_R_M_EEM(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vAO_R_MDesc = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);
	vector		vEmissiveDesc = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);
	vector		vEmissiveMaskDesc = g_EmissiveMaskTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float4 FinalColor = float4(0.f, 0.f, 0.f, 1.f);

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	FinalColor = vDiffuse + vEmissiveDesc /** vEmissiveMaskDesc.r*/;

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, length(vEmissiveDesc /** vEmissiveMaskDesc.r*/), 0.f);
	Out.vAmbient = vAO_R_MDesc;

	return Out;
}//5

 //SPEARATE
PS_OUT PS_MAIN_SEPARATE_AO_R_M_E(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vAODesc = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);
	vector		vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexUV);
	vector		vEmissiveDesc = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float4 FinalColor = float4(0.f, 0.f, 0.f, 1.f);
	float  fMetalic = 1.f - vRoughness.r;
	float4 AO_R_M = float4(vAODesc.r, vRoughness.r, fMetalic, 1.f);

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	FinalColor = vDiffuse + vEmissiveDesc;

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, length(vEmissiveDesc), 0.f);
	Out.vAmbient = AO_R_M;

	return Out;
}//6

PS_OUT PS_MAIN_MASK(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float4 FinalColor = float4(0.f, 0.f, 0.f, 1.f);
	float4 AO_R_M = float4(0.f, 0.f, 0.f, 1.f);

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	FinalColor = vDiffuse * vMask.r;

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = AO_R_M;

	return Out;
}//7

// ALPHA, AO
PS_OUT PS_MAIN_ALPHA_AO_R_M(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);
	vector		vAO_R_M = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float4 FinalColor = float4(0.f, 0.f, 0.f, 1.f);

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	FinalColor = float4(vDiffuse.rgb, vMask.r);

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = vAO_R_M;

	return Out;
}//8

technique11 DefaultTechnique
{
	pass Default
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

	pass AO_R_M
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_AO_R_M();
	}

	pass AO_R_M_E
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_AO_R_M_E();
	}

	pass AO_R_M_G
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_AO_R_M_G();
	}

	pass AO_R_M_O
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_AO_R_M_O();
	}

	pass AO_R_M_EEM
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_AO_R_M_EEM();
	}

	pass SEPARATE_AO_R_M_E
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SEPARATE_AO_R_M_E();
	}

	pass MASK
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MASK();
	}

	pass ALPHA_AO_R_M
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA_AO_R_M();
	}
}

