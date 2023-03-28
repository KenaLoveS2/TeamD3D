#include "Shader_Client_Defines.h"

/***********Constant Buffers***********/
matrix g_BoneMatrices[800];
matrix g_WorldMatrix;
matrix g_ViewMatrix;
matrix g_ProjMatrix;
float      g_fFar = 300.f;
float4 g_vCamPosition;
/**************************************/
Texture2D<float4>      g_DiffuseTexture;
Texture2D<float4>      g_NormalTexture;

Texture2D<float4>      g_GlowTexture;
Texture2D<float4>      g_AO_R_MTexture;
Texture2D<float4>      g_EmissiveTexture;
Texture2D<float4>      g_EmissiveMaskTexture;
Texture2D<float4>      g_OpacityTexture;

Texture2D<float4>      g_AOTexture;
Texture2D<float4>      g_RoughnessTexture;
Texture2D<float4>      g_MaskTexture;

float4			g_EmissiveColor = (float4)1.f;
float				g_fHDRIntensity = 0.f;

/* EnemyWisp Texture */
texture2D      g_NoiseTexture;
texture2D      g_ReamTexture;
texture2D      g_LineTexture;
texture2D      g_SmoothTexture;
texture2D      g_ShapeMaskTexture;
float4         g_vColor;
/* ~EnemyWisp Texture */

/* Dissolve */
texture2D      g_DissolveTexture;
bool         g_bDissolve;
float         g_fDissolveTime;
float _DissolveSpeed = 0.2f;
float _FadeSpeed = 1.5f;
/* ~Dissolve */


struct VS_IN
{
	float3      vPosition : POSITION;
	float3      vNormal : NORMAL;
	float2      vTexUV : TEXCOORD0;
	float3      vTangent : TANGENT;
	uint4         vBlendIndex : BLENDINDEX;
	float4      vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
	float4      vPosition : SV_POSITION;
	float4      vNormal : NORMAL;
	float4      vTangent : TANGENT;
	float2      vTexUV : TEXCOORD0;
	float4      vProjPos : TEXCOORD1;
	float3      vViewDir : TEXCOORD2;
	float3      vBinormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT      Out = (VS_OUT)0;

	matrix      matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float4 worldPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vViewDir = normalize(worldPosition.xyz - g_vCamPosition.xyz);

	float         fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix         BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices[In.vBlendIndex.w] * fWeightW;

	vector         vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
	vector         vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
	vector         vTangent = mul(float4(In.vTangent, 0.f), BoneMatrix);

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
	float4      vPosition : SV_POSITION;
	float4      vNormal : NORMAL;
	float4      vTangent : TANGENT;
	float2      vTexUV : TEXCOORD0;
	float4      vProjPos : TEXCOORD1;
	float3      vViewDir : TEXCOORD2;
	float3      vBinormal : BINORMAL;
};

struct PS_OUT
{
	float4      vDiffuse : SV_TARGET0;
	float4      vNormal : SV_TARGET1;
	float4      vDepth : SV_TARGET2;
	float4      vAmbient : SV_TARGET3;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	if (g_bDissolve)
	{
		float fDissolveAmount = g_fDissolveTime * 5.f;

		// sample noise texture
		float noiseSample = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;

		float  _ColorThreshold1 = 1.0f;
		float4 _DissolveColor1 = float4(194.f, 0.0f, 0.0f, 1.0f) / 255.f;  //red

		float  _ColorThreshold2 = 0.4f;
		float4 _DissolveColor2 = float4(255.f, 163.f, 44.f, 1.0f) / 255.f; //orange

																		   // add edge colors0
		float thresh1 = fDissolveAmount * _ColorThreshold1;
		float useDissolve1 = noiseSample - thresh1 < 0;
		vDiffuse = (1 - useDissolve1)* vDiffuse + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		vDiffuse = (1 - useDissolve2)* vDiffuse + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount *_DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = (vector)1.f;

	return Out;
}//0

PS_OUT PS_MAIN_AO_R_M(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector      vAO_R_MDesc = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float4 FinalColor = float4(0.f, 0.f, 0.f, 1.f);

	float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	FinalColor = vDiffuse;

	if (g_bDissolve)
	{
		float fDissolveAmount = g_fDissolveTime * 5.f;

		// sample noise texture
		float noiseSample = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;

		float  _ColorThreshold1 = 1.0f;
		float4 _DissolveColor1 = float4(194.f, 0.0f, 0.0f, 1.0f) / 255.f;  //red

		float  _ColorThreshold2 = 0.4f;
		float4 _DissolveColor2 = float4(255.f, 163.f, 44.f, 1.0f) / 255.f; //orange

																		   // add edge colors0
		float thresh1 = fDissolveAmount * _ColorThreshold1;
		float useDissolve1 = noiseSample - thresh1 < 0;
		FinalColor = (1 - useDissolve1)* FinalColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		FinalColor = (1 - useDissolve2)* FinalColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount *_DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = vAO_R_MDesc;

	return Out;
}//1

PS_OUT PS_MAIN_AO_R_M_E(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector      vAO_R_MDesc = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);
	vector      vEmissiveDesc = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float4 FinalColor = float4(0.f, 0.f, 0.f, 1.f);

	float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	FinalColor = vDiffuse +	(vDiffuse * vEmissiveDesc * g_EmissiveColor);

	if (g_bDissolve)
	{
		float fDissolveAmount = g_fDissolveTime * 5.f;

		// sample noise texture
		float noiseSample = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;

		float  _ColorThreshold1 = 1.0f;
		float4 _DissolveColor1 = float4(194.f, 0.0f, 0.0f, 1.0f) / 255.f;  //red

		float  _ColorThreshold2 = 0.4f;
		float4 _DissolveColor2 = float4(255.f, 163.f, 44.f, 1.0f) / 255.f; //orange

																		   // add edge colors0
		float thresh1 = fDissolveAmount * _ColorThreshold1;
		float useDissolve1 = noiseSample - thresh1 < 0;
		FinalColor = (1 - useDissolve1)* FinalColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		FinalColor = (1 - useDissolve2)* FinalColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount *_DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, length(vEmissiveDesc) + g_fHDRIntensity, 0.f);
	Out.vAmbient = vAO_R_MDesc;

	return Out;
}//2

 // GLOW
PS_OUT PS_MAIN_AO_R_M_G(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector      vAO_R_MDesc = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);
	vector      vGlowDesc = g_GlowTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float4 FinalColor = float4(0.f, 0.f, 0.f, 1.f);

	float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	if (0.1f > vGlowDesc.a)
		FinalColor = vDiffuse;
	else
		FinalColor = vDiffuse + vGlowDesc;

	if (g_bDissolve)
	{
		float fDissolveAmount = g_fDissolveTime * 5.f;

		// sample noise texture
		float noiseSample = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;

		float  _ColorThreshold1 = 1.0f;
		float4 _DissolveColor1 = float4(194.f, 0.0f, 0.0f, 1.0f) / 255.f;  //red

		float  _ColorThreshold2 = 0.4f;
		float4 _DissolveColor2 = float4(255.f, 163.f, 44.f, 1.0f) / 255.f; //orange

		 // add edge colors0
		float thresh1 = fDissolveAmount * _ColorThreshold1;
		float useDissolve1 = noiseSample - thresh1 < 0;
		FinalColor = (1 - useDissolve1)* FinalColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		FinalColor = (1 - useDissolve2)* FinalColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount *_DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, length(vGlowDesc), 0.f);
	Out.vAmbient = vAO_R_MDesc;

	return Out;
}//3

 //OPACITY
PS_OUT PS_MAIN_AO_R_M_O(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector      vAO_R_MDesc = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);
	vector      vOpacityDesc = g_OpacityTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float4 FinalColor = float4(0.f, 0.f, 0.f, 1.f);

	float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	float fOpacity = vOpacityDesc.r;

	FinalColor = float4(vDiffuse.rgb, fOpacity * vDiffuse.a);

	if (g_bDissolve)
	{
		float fDissolveAmount = g_fDissolveTime * 5.f;

		// sample noise textures
		float noiseSample = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;

		float  _ColorThreshold1 = 1.0f;
		float4 _DissolveColor1 = float4(194.f, 0.0f, 0.0f, 1.0f) / 255.f;  //red

		float  _ColorThreshold2 = 0.4f;
		float4 _DissolveColor2 = float4(255.f, 163.f, 44.f, 1.0f) / 255.f; //orange

																		   // add edge colors0
		float thresh1 = fDissolveAmount * _ColorThreshold1;
		float useDissolve1 = noiseSample - thresh1 < 0;
		FinalColor = (1 - useDissolve1)* FinalColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		FinalColor = (1 - useDissolve2)* FinalColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount *_DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}


	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = vAO_R_MDesc;

	return Out;
}//4

 //EMISSIVEMASK
PS_OUT PS_MAIN_AO_R_M_EEM(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector      vAO_R_MDesc = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);
	vector      vEmissiveDesc = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);
	vector      vEmissiveMaskDesc = g_EmissiveMaskTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float4 FinalColor = float4(0.f, 0.f, 0.f, 1.f);

	float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	FinalColor = vDiffuse + vEmissiveDesc /** vEmissiveMaskDesc.r*/;

	if (g_bDissolve)
	{
		float fDissolveAmount = g_fDissolveTime * 5.f;

		// sample noise texture
		float noiseSample = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;

		float  _ColorThreshold1 = 1.0f;
		float4 _DissolveColor1 = float4(194.f, 0.0f, 0.0f, 1.0f) / 255.f;  //red

		float  _ColorThreshold2 = 0.4f;
		float4 _DissolveColor2 = float4(255.f, 163.f, 44.f, 1.0f) / 255.f; //orange

																		   // add edge colors0
		float thresh1 = fDissolveAmount * _ColorThreshold1;
		float useDissolve1 = noiseSample - thresh1 < 0;
		FinalColor = (1 - useDissolve1)* FinalColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		FinalColor = (1 - useDissolve2)* FinalColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount *_DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}


	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, length(vEmissiveDesc /** vEmissiveMaskDesc.r*/), 0.f);
	Out.vAmbient = vAO_R_MDesc;

	return Out;
}//5

 //SPEARATE
PS_OUT PS_MAIN_SEPARATE_AO_R_M_E(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector      vAODesc = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);
	vector      vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexUV);
	vector      vEmissiveDesc = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float4 FinalColor = float4(0.f, 0.f, 0.f, 1.f);
	float  fMetalic = 1.f - vRoughness.r;
	float4 AO_R_M = float4(vAODesc.r, vRoughness.r, fMetalic, 1.f);

	float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	FinalColor = vDiffuse + vEmissiveDesc;

	if (g_bDissolve)
	{
		float fDissolveAmount = g_fDissolveTime * 5.f;

		// sample noise texture
		float noiseSample = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;

		float  _ColorThreshold1 = 1.0f;
		float4 _DissolveColor1 = float4(194.f, 0.0f, 0.0f, 1.0f) / 255.f;  //red

		float  _ColorThreshold2 = 0.4f;
		float4 _DissolveColor2 = float4(255.f, 163.f, 44.f, 1.0f) / 255.f; //orange

																		   // add edge colors0
		float thresh1 = fDissolveAmount * _ColorThreshold1;
		float useDissolve1 = noiseSample - thresh1 < 0;
		FinalColor = (1 - useDissolve1)* FinalColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		FinalColor = (1 - useDissolve2)* FinalColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount *_DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}


	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, length(vEmissiveDesc), 0.f);
	Out.vAmbient = AO_R_M;

	return Out;
}//6

PS_OUT PS_MAIN_MASK(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector      vMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float4 FinalColor = float4(0.f, 0.f, 0.f, 1.f);
	float4 AO_R_M = float4(0.f, 0.f, 0.f, 1.f);

	float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	FinalColor = vDiffuse * vMask.r;

	if (g_bDissolve)
	{
		float fDissolveAmount = g_fDissolveTime * 5.f;

		// sample noise texture
		float noiseSample = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;

		float  _ColorThreshold1 = 1.0f;
		float4 _DissolveColor1 = float4(194.f, 0.0f, 0.0f, 1.0f) / 255.f;  //red

		float  _ColorThreshold2 = 0.4f;
		float4 _DissolveColor2 = float4(255.f, 163.f, 44.f, 1.0f) / 255.f; //orange

																		   // add edge colors0
		float thresh1 = fDissolveAmount * _ColorThreshold1;
		float useDissolve1 = noiseSample - thresh1 < 0;
		FinalColor = (1 - useDissolve1)* FinalColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		FinalColor = (1 - useDissolve2)* FinalColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount *_DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}


	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = AO_R_M;

	return Out;
}//7

PS_OUT PS_MAIN_BOMBCHARGEUP(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	float4 vNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexUV);
	//float4 vMask = g_ShapeMaskTexture.Sample(LinearSampler, In.vTexUV);
	//vMask.a = vMask.r;
	float4 vColor = g_ReamTexture.Sample(LinearSampler, In.vTexUV);
	float4 vSmooth = g_SmoothTexture.Sample(LinearSampler, In.vTexUV);
	float4 vblendColor = lerp(vColor, vSmooth, vSmooth.r);
	float4 finalcolor = lerp(vblendColor, vNoise, vNoise.r) * float4(73.f, 24.f, 16.f, 255.f) / 255.f;

	float4 vLine = g_LineTexture.Sample(LinearSampler, In.vTexUV);

	// fresnel_glow(±½±â(Å¬¼ö·Ï ¾ãÀ½), )
	float  base = dot(In.vNormal.rgb, -In.vViewDir.rgb);
	float  exponential = /*vMask * */pow(base, 2.f);
	float4 fresnelcolor = float4(197.f, 57.f, 57.f, 13.f) / 255.f;
	float4 fresnel = exponential + fresnelcolor * (1.0f - exponential);

	// rim
	float  rim = dot(In.vNormal.rgb, In.vViewDir.rgb);
	float4 vOutline = pow(1.f - rim, 5.f);

	float4 FinalColor = finalcolor * vOutline * fresnel;

	if (g_bDissolve)
	{
		float fDissolveAmount = g_fDissolveTime * 5.f;

		// sample noise texture
		float noiseSample = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;

		float  _ColorThreshold1 = 1.0f;
		float4 _DissolveColor1 = float4(194.f, 0.0f, 0.0f, 1.0f) / 255.f;  //red

		float  _ColorThreshold2 = 0.4f;
		float4 _DissolveColor2 = float4(255.f, 163.f, 44.f, 1.0f) / 255.f; //orange

																		   // add edge colors0
		float thresh1 = fDissolveAmount * _ColorThreshold1;
		float useDissolve1 = noiseSample - thresh1 < 0;
		FinalColor = (1 - useDissolve1)* FinalColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		FinalColor = (1 - useDissolve2)* FinalColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount *_DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}


	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 2.f, 0.f);
	Out.vAmbient = (vector)1.f;

	return Out;
} //8

  // ALPHA, AO
PS_OUT PS_MAIN_ALPHA_AO_R_M(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector      vMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);
	vector      vAO_R_M = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float4 FinalColor = float4(0.f, 0.f, 0.f, 1.f);

	float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	FinalColor = float4(vDiffuse.rgb, vMask.r);

	if (g_bDissolve)
	{
		float fDissolveAmount = g_fDissolveTime * 5.f;

		// sample noise texture
		float noiseSample = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;

		float  _ColorThreshold1 = 1.0f;
		float4 _DissolveColor1 = float4(194.f, 0.0f, 0.0f, 1.0f) / 255.f;  //red

		float  _ColorThreshold2 = 0.4f;
		float4 _DissolveColor2 = float4(255.f, 163.f, 44.f, 1.0f) / 255.f; //orange

																		   // add edge colors0
		float thresh1 = fDissolveAmount * _ColorThreshold1;
		float useDissolve1 = noiseSample - thresh1 < 0;
		FinalColor = (1 - useDissolve1)* FinalColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		FinalColor = (1 - useDissolve2)* FinalColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount *_DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = vAO_R_M;

	return Out;
}//9

struct PS_OUT_SHADOW
{
	vector         vLightDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN In)
{
	PS_OUT_SHADOW      Out = (PS_OUT_SHADOW)0;

	Out.vLightDepth.r = In.vProjPos.w / g_fFar;
	Out.vLightDepth.a = 1.f;

	return Out;
}// 10

technique11 DefaultTechnique
{
	// 0
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

	// 1
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
	// 2
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
	// 3
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
	// 4
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

	// 5
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
	// 6
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
	// 7
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

	// 8
	pass Sapling_BombChargeUp
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BOMBCHARGEUP();
	}

	//9
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

	//10
	pass SHADOW
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
	}
}
