#include "Shader_Client_Defines.h"
/***********Constant Buffers***********/
matrix g_BoneMatrices[800];
float  g_fFar = 500.f;
float4 g_vCamPosition;
/**************************************/
Texture2D<float4>		g_DiffuseTexture;
Texture2D<float4>		g_NormalTexture;
Texture2D<float4>		g_AO_R_MTexture;
Texture2D<float4>		g_EmissiveTexture;
Texture2D<float4>		g_EmissiveMaskTexture;
Texture2D<float4>		g_MaskTexture;
Texture2D<float4>		g_SSSMaskTexture;
Texture2D<float4>		g_HairDepthTexture;
Texture2D<float4>		g_HairAlphaTexture;
Texture2D<float4>		g_HairRootTexture;
Texture2D<float4>		g_DetailNormal;
Texture2D<float4>		g_AlphaTexture;
/* Kena Bow_String Texture */
Texture2D		g_NoiseTexture;
Texture2D		g_SwipeTexture;
Texture2D		g_GradientTexture;
float				g_BowDurationTime;
/* Kena Bow_String Texture */

bool					g_Hit = false;
bool					g_Parry = false;
bool					g_Dash = false;

float					g_fHairLength = 1.f;
float					g_fHairThickness = 1.f;
float					g_fLashDensity = 0.5f;
float					g_fLashWidth = 0.5f;
float					g_fLashIntensity = 0.5f;
float					g_fSSSAmount = 1.f;
float					g_HitRimIntensity = 0.f;
float					g_ParryRimIntensity = 0.f;
float					g_DashRimIntensity = 0.f;
float					g_Time;

float4					g_vAmbientEyeColor = float4(1.f, 1.f, 1.f, 1.f);
float4					g_vAmbientColor = float4(1.f, 1.f, 1.f, 1.f);
float4					g_vSSSColor = float4(1.f, 0.f, 0.f, 1.f);


Texture2D<float4>      g_GlowTexture;
Texture2D<float4>      g_OpacityTexture;
Texture2D<float4>      g_AOTexture;
Texture2D<float4>      g_RoughnessTexture;

float4					g_EmissiveColor = (float4)1.f;
float						g_fHDRIntensity = 0.f;

/* EnemyWisp Texture */
Texture2D<float4>     g_ReamTexture;
Texture2D<float4>     g_LineTexture;
Texture2D<float4>     g_SmoothTexture;
Texture2D<float4>     g_ShapeMaskTexture;
float4			  g_vColor;
/* ~EnemyWisp Texture */

/* Dissolve */
Texture2D<float4>	g_DissolveTexture;
bool							g_bDissolve;
float							g_fDissolveTime;
float							_DissolveSpeed = 0.2f;
float							_FadeSpeed = 1.5f;
/* ~Dissolve */

/* Options */
float			g_fUVSpeedX = 0.f;
float			g_fUVSpeedY = 0.f;
/* ~Options */

/* For. EnemyHunter */
float			g_fStringDissolve;
float			g_fStringHDR;


float4 SSS(float3 position, float3 normal, float3 dir, float4 color, float2 vUV, float amount, Texture2D<float4> Texturediffuse, Texture2D<float4> sssMask)
{
	float4 result = 0;
	// Calculate the distance to the surface
	float surfaceDistance = distance(position, g_vCamPosition.xyz);
	// Calculate the distance that light travels through the material
	float scatterDistance = sqrt(surfaceDistance) * amount;
	// Calculate the diffuse term for the subsurface scattering
	float diffuse = saturate(dot(normal, -dir));
	vector vDiffuse = Texturediffuse.Sample(LinearSampler, vUV);
	// Calculate the subsurface scattering term
	float4 scattering = (1 - exp(-scatterDistance)) * vDiffuse * diffuse;
	scattering *= sssMask.Sample(LinearSampler, vUV).r;
	// Add the subsurface scattering term to the input color
	result = color + scattering;
	return result;
}

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
	float4		vNormal  : SV_TARGET1;
	float4		vDepth	 : SV_TARGET2;
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
	Out.vAmbient = (vector)1.f;

	return Out;
}

PS_OUT PS_MAIN_KENA_EYE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2		vTexUV = In.vTexUV;
	vTexUV.x -= 0.345f;
	vTexUV.y -= 0.38f;

	vector		vDiffuse = g_DiffuseTexture.Sample(EyeSampler, vTexUV * 3.4f);
	vector		vNormalDesc = g_NormalTexture.Sample(EyeSampler, vTexUV * 3.4f);

	if (0.1f > vDiffuse.a)
		discard;

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	float4		FinalColor = float4(0, 0, 0, 1);

	if (g_Hit)
	{
		vector		vRimColor = float4(1.f, 0.f, 0.f, 0.f);
		FinalColor = (vDiffuse)+(vRimColor * g_HitRimIntensity);
	}
	else if (g_Parry)
	{
		vector		vRimColor = float4(0.f, 0.1f, 0.4f, 0.f);
		FinalColor = (vDiffuse)+(vRimColor * g_ParryRimIntensity);
	}
	else if (g_Dash)
	{
		vector		vRimColor = float4(0.8f, 0.8f, 1.0f, 1.f);
		FinalColor = (vDiffuse)+(vRimColor * g_DashRimIntensity);
	}
	else
	{
		FinalColor = (vDiffuse);
	}

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = (vector)1.f;

	return Out;
}

PS_OUT PS_MAIN_KENA_BODY(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vAO_R_M = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	float4		FinalColor = float4(0, 0, 0, 1);

	if (g_Hit)
	{
		vector		vRimColor = float4(1.f, 0.f, 0.f, 0.f);
		FinalColor = (vDiffuse + vEmissive) + vRimColor * g_HitRimIntensity;
	}
	else if (g_Parry)
	{
		vector		vRimColor = float4(0.f, 0.1f, 0.4f, 0.f);
		FinalColor = (vDiffuse + vEmissive) + vRimColor * g_ParryRimIntensity;
	}
	else if (g_Dash)
	{
		vector		vRimColor = float4(0.8f, 0.8f, 1.0f, 1.f);
		FinalColor = (vDiffuse + vEmissive) + vRimColor * g_DashRimIntensity;
	}
	else
	{
		FinalColor = (vDiffuse + vEmissive);
	}

	if (0.1f > vDiffuse.a)
		discard;

	Out.vDiffuse = vector(FinalColor.rgb, 1.f);
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = vAO_R_M;

	return Out;
}

PS_OUT PS_MAIN_KENA_MAINOUTFIT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2		vTexUV = In.vTexUV;

	vector		vAO_R_M = g_AO_R_MTexture.Sample(LinearSampler, vTexUV);
	vector		vMask = g_MaskTexture.Sample(LinearSampler, vTexUV);
	vector		vSSSMask = g_SSSMaskTexture.Sample(LinearSampler, vTexUV);
	vector		vEmissiveMask = g_EmissiveMaskTexture.Sample(LinearSampler, vTexUV);
	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, vTexUV);
	vector		vEmissive = g_EmissiveTexture.Sample(LinearSampler, vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, vTexUV);

	float3		vNormal = (vNormalDesc.xyz) * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	float4		FinalColor = float4(1, 1, 1, 1);

	if (g_Hit)
	{
		vector		vRimColor = float4(1.f, 0.f, 0.f, 0.f);
		FinalColor = (vDiffuse + vEmissive * vEmissiveMask.r) + vRimColor * g_HitRimIntensity;
	}
	else if (g_Parry)
	{
		vector		vRimColor = float4(0.f, 0.1f, 0.4f, 0.f);
		FinalColor = (vDiffuse + vEmissive * vEmissiveMask.r) + vRimColor * g_ParryRimIntensity;
	}
	else if (g_Dash)
	{
		vector		vRimColor = float4(0.8f, 0.8f, 1.0f, 1.f);
		FinalColor = (vDiffuse + vEmissive * vEmissiveMask.r) + vRimColor * g_DashRimIntensity;
	}
	else
	{
		FinalColor = (vDiffuse + vEmissive * vEmissiveMask.r);
	}

	if (0.1f > vDiffuse.a)
		discard;

	Out.vDiffuse = vector(FinalColor.rgb, 1.f);
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = vAO_R_M;

	return Out;
}

PS_OUT PS_MAIN_FACE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vAO_R_M = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);
	vector		vSSSMask = g_SSSMaskTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	float4		FinalColor = float4(0.f, 0.f, 0.f, 1.f);

	if (g_Hit)
	{
		vector		vRimColor = float4(1.f, 0.f, 0.f, 0.f);
		FinalColor = (vDiffuse)+vRimColor * g_HitRimIntensity;
	}
	else if (g_Parry)
	{
		vector		vRimColor = float4(0.f, 0.1f, 0.4f, 0.f);
		FinalColor = (vDiffuse)+vRimColor * g_ParryRimIntensity;
	}
	else if (g_Dash)
	{
		vector		vRimColor = float4(0.8f, 0.8f, 1.0f, 1.f);
		FinalColor = (vDiffuse)+ vRimColor * g_DashRimIntensity;
	}
	else
	{
		FinalColor = (vDiffuse);
	}

	Out.vDiffuse = vector(FinalColor.rgb, 1.f);
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = vAO_R_M;

	return Out;
}

PS_OUT PS_MAIN_STAFF(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vAO_R_M = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	float4		FinalColor = float4(0, 0, 0, 1);
	FinalColor = vDiffuse + vEmissive;

	Out.vDiffuse = vector(FinalColor.rgb, vDiffuse.a);
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = vAO_R_M;

	return Out;
}

PS_OUT PS_MAIN_HAIR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vAlpha = g_HairAlphaTexture.Sample(LinearSampler, In.vTexUV);
	vector		vRoot = g_HairRootTexture.Sample(LinearSampler, In.vTexUV);
	vector		vDepth = g_HairDepthTexture.Sample(LinearSampler, In.vTexUV);

	float fAlpha = vAlpha.r;

	if (fAlpha < 0.5f)
		discard;

	float3 viewDirection = -In.vViewDir;
	float3 rootPosition = vRoot.rgb;

	float fDepth = vDepth.r;

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	float3 pixelPosition = float3(In.vPosition.xy, In.vTexUV.r) * g_fHairLength;
	float3 hairDirection = normalize(pixelPosition - rootPosition);

	float viewAngle = dot(hairDirection, viewDirection);

	float thickness = saturate(g_fHairThickness / (1.0f + viewAngle));

	vDiffuse.rgb *= thickness;

	float fFinalAlpha = fAlpha * saturate((fDepth - rootPosition.z) / (1.f - rootPosition.z));
	fFinalAlpha = 1.f - fFinalAlpha;

	float4		FinalColor = float4(0, 0, 0, 1);

	if (g_Hit)
	{
		vector		vRimColor = float4(1.f, 0.f, 0.f, 0.f);
		FinalColor = (vDiffuse)+vRimColor * g_HitRimIntensity;
	}
	else if (g_Parry)
	{
		vector		vRimColor = float4(0.f, 0.1f, 0.4f, 0.f);
		FinalColor = (vDiffuse)+vRimColor * g_ParryRimIntensity;
	}
	else if (g_Dash)
	{
		vector		vRimColor = float4(0.8f, 0.8f, 1.0f, 1.f);
		FinalColor = (vDiffuse)+vRimColor * g_DashRimIntensity;
	}
	else
	{
		FinalColor = (vDiffuse);
	}

	Out.vDiffuse = float4(FinalColor.rgb, fFinalAlpha);
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = (vector)1.f;

	return Out;
}

PS_OUT PS_MAIN_EYELASH(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	vector		vLashColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vAlbedo = vLashColor;

	float alpha = vLashColor.r;

	float distance = 1.0;
	for (float i = 0.0; i < g_fLashDensity; i += 1.0)
	{
		float2 offset = float2(sin(i) * g_fLashWidth, cos(i) * g_fLashWidth);
		float4 lashColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV + offset);
		float lashAlpha = lashColor.a * alpha;
		distance = min(distance, lashAlpha);
	}

	float4 FinalColor = float4(lerp(vAlbedo, vLashColor, distance * g_fLashIntensity));

	if (g_Hit)
	{
		vector		vRimColor = float4(1.f, 0.f, 0.f, 0.f);
		FinalColor += vRimColor * g_HitRimIntensity;
	}
	else if (g_Parry)
	{
		vector		vRimColor = float4(0.f, 0.1f, 0.4f, 0.f);
		FinalColor += vRimColor * g_ParryRimIntensity;
	}
	else if (g_Dash)
	{
		vector		vRimColor = float4(0.8f, 0.8f, 1.0f, 1.f);
		FinalColor += vRimColor * g_DashRimIntensity;
	}

	Out.vDiffuse = FinalColor;
	Out.vNormal = (vector)1.f;
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = (vector)1.f;

	return Out;
}

PS_OUT PS_MAIN_STAFF_BOWTRAIL(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float  time = frac(g_Time * 0.7f);
	float2 OffsetUV = TilingAndOffset(In.vTexUV, float2(2.f, 1.f), float2(0.f, -time));

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vMask = g_MaskTexture.Sample(LinearSampler, float2(OffsetUV.x, OffsetUV.y));

	float4 vColor = vector(29.0f, 142.f, 176.f, 255.f) / 255.f;
	Out.vDiffuse = (vDiffuse * vMask) * 2.f + vColor;
	Out.vDiffuse.a = Out.vDiffuse.r;
	Out.vDiffuse = saturate(Out.vDiffuse.r * vColor) * 4.5f;
	if (Out.vDiffuse.a < 0.01)
		discard;

	if (g_BowDurationTime > 4.f)
		Out.vDiffuse.rgb = Out.vDiffuse.rgb * 4.f;
	else
		Out.vDiffuse.rgb = Out.vDiffuse.rgb * g_BowDurationTime;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = (vector)1.f;

	return Out;
}

PS_OUT PS_MAIN_STAFF_BOWSTRING(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	/* Texture */
	vector vMask = g_MaskTexture.Sample(LinearSampler, float2(In.vTexUV.x + 1.78, In.vTexUV.y));
	if (vMask.g < 7 / 255.f)
		discard;
	vMask.r = 0.0f;
	vMask.b = 0.0f;

	/* Color */		// R=0, G=6.49374, B=10, A=1
	float4 MaskColor = vector(40.f, 85.f, 92.f, 155.f) / 255.f;

	/* Mask */
	float  mask_alpha = vMask.g;
	float  mask_distance = min(1.f, mask_alpha);

	float4 mask_texture = float4(lerp(vMask, mask_alpha, mask_distance));

	mask_texture = saturate(mask_texture * 2.f * MaskColor);

	mask_texture.a = mask_texture.r;
	if (mask_texture.a < 0.01)
		discard;

	mask_texture.rgb = mask_texture.rgb * g_BowDurationTime;
	mask_texture = mask_texture + MaskColor;

	/* Mask */
	Out.vDiffuse = mask_texture;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = (vector)1.f;

	return Out;
}

PS_OUT PS_MAIN_STAFF_BOWSTRING_PART2(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	/* UV Moving */
	float  time = frac(g_Time * 0.5f);
	float2 OffsetUV = TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(0.0f, -time));

	/* Texture */
	vector vGradient = g_GradientTexture.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y + 1.9f));
	vGradient.a = vGradient.r;
	if (vGradient.a < 0.3f)
		discard;
	vGradient = vector(0.25f, 0.25f, 0.25f, 0.25f);

	/* Color */
	float4 TextureColor = vector(37.0f, 210.f, 255.f, 50.f) / 255.f;
	float4 vColor = vector(0.0f, 194.f, 221.f, 0.0f) / 255.f;
	float4 FinalDiffuseColor = vector(38.0f, 108.f, 145.f, 228.f) / 255.f;

	float4   fresnel = float4(fresnel_glow(4.5, 2.5, vColor.rgb, In.vNormal.rgb, In.vViewDir), vColor.a);

	vector vNoise = g_NoiseTexture.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y - time / OffsetUV.y));
	vector vSwipe = g_SwipeTexture.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y - time / OffsetUV.y));
	float  fAlpha = 1.f - (abs(0.5f - In.vTexUV.y) * 2.f);

	Out.vDiffuse = lerp(vNoise, vSwipe, 1.f - fAlpha);
	Out.vDiffuse = saturate(Out.vDiffuse * FinalDiffuseColor * 2.f) * fresnel + FinalDiffuseColor;
	Out.vDiffuse.a = Out.vDiffuse.a * fAlpha * 0.5f;

	Out.vDiffuse.rgb = Out.vDiffuse.rgb * 2.f;
	if (Out.vDiffuse.a < 0.33)
		discard;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = (vector)1.f;

	return Out;
}

PS_OUT PS_MAIN_CINE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float3		vNormal = In.vNormal.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));
	float4 vAORM = (float4)1.f;

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = vAORM;
	return Out;
}

PS_OUT PS_PULSEPLATEANIM(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vERAODesc = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = vector(vERAODesc.b, vERAODesc.g, 1.f, 1.f);

	return Out;
}

struct PS_OUT_SHADOW
{
	vector			vLightDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN In)
{
	PS_OUT_SHADOW		Out = (PS_OUT_SHADOW)0;

	Out.vLightDepth.r = In.vProjPos.w / g_fFar;
	Out.vLightDepth.a = 1.f;

	return Out;
}

PS_OUT PS_MAIN_MON(PS_IN In)
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
		vDiffuse = (1 - useDissolve1) * vDiffuse + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		vDiffuse = (1 - useDissolve2) * vDiffuse + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount * _DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = (vector)1.f;

	return Out;
}

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
		FinalColor = (1 - useDissolve1) * FinalColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		FinalColor = (1 - useDissolve2) * FinalColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount * _DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = vAO_R_MDesc;

	return Out;
}

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

	FinalColor = vDiffuse + (vDiffuse * vEmissiveDesc * g_EmissiveColor);

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
		FinalColor = (1 - useDissolve1) * FinalColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		FinalColor = (1 - useDissolve2) * FinalColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount * _DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, length(vEmissiveDesc) + g_fHDRIntensity, 0.f);
	Out.vAmbient = vAO_R_MDesc;

	return Out;
}

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
		FinalColor = (1 - useDissolve1) * FinalColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		FinalColor = (1 - useDissolve2) * FinalColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount * _DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, length(vGlowDesc), 0.f);
	Out.vAmbient = vAO_R_MDesc;

	return Out;
}

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

		// sample noise texture
		float noiseSample = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;

		float  _ColorThreshold1 = 1.0f;
		float4 _DissolveColor1 = float4(194.f, 0.0f, 0.0f, 1.0f) / 255.f;  //red

		float  _ColorThreshold2 = 0.4f;
		float4 _DissolveColor2 = float4(255.f, 163.f, 44.f, 1.0f) / 255.f; //orange

																		   // add edge colors0
		float thresh1 = fDissolveAmount * _ColorThreshold1;
		float useDissolve1 = noiseSample - thresh1 < 0;
		FinalColor = (1 - useDissolve1) * FinalColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		FinalColor = (1 - useDissolve2) * FinalColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount * _DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}


	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = vAO_R_MDesc;

	return Out;
}

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
		FinalColor = (1 - useDissolve1) * FinalColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		FinalColor = (1 - useDissolve2) * FinalColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount * _DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}


	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, length(vEmissiveDesc /** vEmissiveMaskDesc.r*/), 0.f);
	Out.vAmbient = vAO_R_MDesc;

	return Out;
}

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
		FinalColor = (1 - useDissolve1) * FinalColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		FinalColor = (1 - useDissolve2) * FinalColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount * _DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}


	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, length(vEmissiveDesc), 0.f);
	Out.vAmbient = AO_R_M;

	return Out;
}

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
		FinalColor = (1 - useDissolve1) * FinalColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		FinalColor = (1 - useDissolve2) * FinalColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount * _DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}


	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = AO_R_M;

	return Out;
}

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

	// fresnel_glow(????(????? ????), )
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
		FinalColor = (1 - useDissolve1) * FinalColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		FinalColor = (1 - useDissolve2) * FinalColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount * _DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}


	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 2.f, 0.f);
	Out.vAmbient = (vector)1.f;

	return Out;
} 

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
		FinalColor = (1 - useDissolve1) * FinalColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		FinalColor = (1 - useDissolve2) * FinalColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount * _DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = vAO_R_M;

	return Out;
}

PS_OUT PS_MAIN_BOSS_AO_R_M(PS_IN In)
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
		float4 _DissolveColor1 = float4(255.f, 255.f, 255.f, 255.f) / 255.f;

		float  _ColorThreshold2 = 0.4f;
		float4 _DissolveColor2 = float4(255.f, 127.f, 255.f, 255.f) / 255.f;

		// add edge colors0
		float thresh1 = fDissolveAmount * _ColorThreshold1;
		float useDissolve1 = noiseSample - thresh1 < 0;
		FinalColor = (1 - useDissolve1) * FinalColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		FinalColor = (1 - useDissolve2) * FinalColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount * _DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = vAO_R_MDesc;

	return Out;
}

PS_OUT PS_MAIN_BOSS_AO_R_M_E(PS_IN In)
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

	FinalColor = vDiffuse + (vDiffuse * vEmissiveDesc * g_EmissiveColor);

	if (g_bDissolve)
	{
		float fDissolveAmount = g_fDissolveTime * 5.f;

		// sample noise texture
		float noiseSample = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;

		float  _ColorThreshold1 = 1.0f;
		float4 _DissolveColor1 = float4(255.f, 255.f, 255.f, 255.f) / 255.f;

		float  _ColorThreshold2 = 0.4f;
		float4 _DissolveColor2 = float4(255.f, 127.f, 255.f, 255.f) / 255.f;

		// add edge colors0
		float thresh1 = fDissolveAmount * _ColorThreshold1;
		float useDissolve1 = noiseSample - thresh1 < 0;
		FinalColor = (1 - useDissolve1) * FinalColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		FinalColor = (1 - useDissolve2) * FinalColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount * _DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 
		length(vEmissiveDesc) + (g_fHDRIntensity + vEmissiveDesc.r * 2.f), 0.f);
	Out.vAmbient = vAO_R_MDesc;

	return Out;
}

PS_OUT PS_MAIN_HUNTER_STRING(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2	texUV = In.vTexUV;
	texUV.x += g_fUVSpeedX;
	//texUV.y += g_fUVSpeedY;
	float2	texUV2 = In.vTexUV;
	texUV2 -= g_fUVSpeedX;
	float4	vDiffuse = g_DiffuseTexture.Sample(LinearSampler, texUV2);
	float4	vMask = g_MaskTexture.Sample(LinearSampler, texUV);

	float	vDiffuseR = vDiffuse.r;

	Out.vDiffuse = vDiffuse;
	Out.vDiffuse.a = vMask.r;
	if (0.1f > Out.vDiffuse.a)
		discard;
	vector      vAO_R_M = (vector)1.f;
	Out.vDiffuse.rgb *= g_vColor.rgb;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f,
		g_fStringHDR, 0.f);
	Out.vAmbient = vAO_R_M;

	if (vDiffuseR < g_fStringDissolve)
		discard;

	return Out;
} 

PS_OUT PS_MAIN_AO_R_M_DEFAULT(PS_IN In)
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

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = vAO_R_MDesc;

	return Out;
}

PS_OUT PS_MAIN_AO_R_M_E_DEFAULT(PS_IN In)
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

	FinalColor = vDiffuse + (vDiffuse * vEmissiveDesc * g_EmissiveColor);

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, length(vEmissiveDesc), 0.f);
	Out.vAmbient = vAO_R_MDesc;

	return Out;
}

PS_OUT PS_MAIN_SAIYA_EYE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2		vTexUV = In.vTexUV;
	vTexUV.x -= 0.3f;
	vTexUV.y -= 0.37f;

	vector		vDiffuse = g_DiffuseTexture.Sample(EyeSampler, vTexUV * 3.f);
	vector		vNormalDesc = g_NormalTexture.Sample(EyeSampler, vTexUV * 3.f);

	if (0.1f > vDiffuse.a)
		discard;

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = (vector)1.f;

	return Out;
}

PS_OUT PS_MAIN_BENI_EYE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2		vTexUV = In.vTexUV;
	vTexUV.x -= 0.33f;
	vTexUV.y -= 0.37f;

	vector		vDiffuse = g_DiffuseTexture.Sample(EyeSampler, vTexUV * 2.8f);
	vector		vNormalDesc = g_NormalTexture.Sample(EyeSampler, vTexUV * 2.8f);

	if (0.1f > vDiffuse.a)
		discard;

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = (vector)1.f;

	return Out;
}

PS_OUT PS_MAIN_ROTHAIR(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vAlphaDesc = g_AlphaTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float4	vBaseColor = float4(0.018114f, 0.016204f, 0.020833f, 1.f);
	Out.vDiffuse = vBaseColor;
	Out.vNormal  = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = vector(1.f, 1.f, 1.f, 1.f);

	return Out;
}

PS_OUT PS_MAIN_NONNORMAL(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = (vector)1.f;

	return Out;
}

PS_OUT PS_MAIN_EMISSIVE(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector      vAO_R_MDesc = (vector)1.f;
	vector      vEmissiveDesc = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float4 FinalColor = float4(0.f, 0.f, 0.f, 1.f);

	float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	FinalColor = vDiffuse + (vDiffuse * vEmissiveDesc * g_EmissiveColor);

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, length(vEmissiveDesc), 0.f);
	Out.vAmbient = vAO_R_MDesc;

	return Out;
}

PS_OUT PS_MAIN_HUNTER_HAIR(PS_IN In)
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

	FinalColor = vDiffuse + (vDiffuse * vEmissiveDesc * g_EmissiveColor);

	if (g_bDissolve)
	{
		float fDissolveAmount = g_fDissolveTime * 5.f;

		// sample noise texture
		float noiseSample = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;

		float  _ColorThreshold1 = 1.0f;
		float4 _DissolveColor1 = float4(255.f, 255.f, 255.f, 255.f) / 255.f;

		float  _ColorThreshold2 = 0.4f;
		float4 _DissolveColor2 = float4(255.f, 127.f, 255.f, 255.f) / 255.f;

		// add edge colors0
		float thresh1 = fDissolveAmount * _ColorThreshold1;
		float useDissolve1 = noiseSample - thresh1 < 0;
		FinalColor = (1 - useDissolve1) * FinalColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		FinalColor = (1 - useDissolve2) * FinalColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount * _DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);

	if (vEmissiveDesc.r < 0.0001f)
	{
		Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar,
			length(vEmissiveDesc) + (g_fHDRIntensity), 0.f);
	}
	else
	{
		if (In.vTexUV.y < 0.5f)
		{
			Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar,
				length(vEmissiveDesc) + (g_fHDRIntensity + vEmissiveDesc.r * 4.f), 0.f);
		}
		else
		{
			if (In.vTexUV.x > 0.2f)
			{
				float4 vColor = float4(240, 33, 168, 1) / 255;
				Out.vDiffuse.rgb *= vColor.rgb;

				Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar,
					length(vEmissiveDesc) + (g_fHDRIntensity + vEmissiveDesc.r * 30.f), 0.f);
			}
			else
			{
				float4 vColor = float4(237, 152, 196, 1) / 255;
				Out.vDiffuse.rgb *= vColor.rgb;

				if (vEmissiveDesc.r < 0.2f)
				{
					Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar,
						length(vEmissiveDesc) + 0.5f, 0.f);
				}
				else
				{
					Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar,
						length(vEmissiveDesc) + 4.f, 0.f);
				}
			}
		}
	}
	//if (In.vTexUV.y < 0.5f)
	//{
	//	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar,
	//		length(vEmissiveDesc) + (g_fHDRIntensity + vEmissiveDesc.r * 4.f), 0.f);
	//}
	//else
	//{
	//	if (In.vTexUV.y < 0.1f)

	//}

	//if (vEmissiveDesc.r < 0.01f)
	//{
	//	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar,
	//		length(vEmissiveDesc) + g_fHDRIntensity, 0.f);
	//}
	//else if (vEmissiveDesc.r < 0.2f)
	//{
	//	if (In.vTexUV.y > 0.5f)
	//	{
	//		float4 vColor = float4( 240, 104, 175, 1.f ) / 255;
	//		Out.vDiffuse.rgb *= vColor.rgb;

	//		Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar,
	//			length(vEmissiveDesc) + (g_fHDRIntensity + vEmissiveDesc.r * 20.f), 0.f);
	//	}
	//	else
	//	{
	//		Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar,
	//			length(vEmissiveDesc) + (g_fHDRIntensity + vEmissiveDesc.r * 4.f), 0.f);
	//	}
	//}



	Out.vAmbient = vAO_R_MDesc;

	return Out;
} // 34

PS_OUT PS_MAIN_E_R_AO_E_DISSOLVE(PS_IN In)
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

	FinalColor = vDiffuse + (vDiffuse * vEmissiveDesc * g_EmissiveColor);

	if (g_bDissolve)
	{
		float fDissolveAmount = g_fDissolveTime * 5.f;

		// sample noise texture
		float noiseSample = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;

		float  _ColorThreshold1 = 1.0f;
		float4 _DissolveColor1 = float4(120.f, 120.f, 120.f, 255.f) / 255.f;

		float  _ColorThreshold2 = 0.4f;
		float4 _DissolveColor2 = float4(85.f, 215.f, 255.f, 255.f) / 255.f;

		// add edge colors0
		float thresh1 = fDissolveAmount * _ColorThreshold1;
		float useDissolve1 = noiseSample - thresh1 < 0;
		FinalColor = (1 - useDissolve1) * FinalColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		FinalColor = (1 - useDissolve2) * FinalColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount * _DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, length(vEmissiveDesc) + g_fHDRIntensity, 0.f);
	// ERAO
	float4 ERAO = float4(vAO_R_MDesc.b, vAO_R_MDesc.g, 1.f, 1.f);
	Out.vAmbient = ERAO;

	return Out;
}//35

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
	}// 0

	pass Kena_Eye
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_KENA_EYE();
	}// 1

	pass Kena_Body
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_KENA_BODY();
	}// 2

	pass Kena_MainOutFit
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_KENA_MAINOUTFIT();
	}// 3

	pass Kena_Face
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FACE();
	}// 4

	pass Kena_Staff
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_STAFF();
	}// 5

	pass Kena_Hair
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HAIR();
	}// 6

	pass Kena_EyeLash
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EYELASH();
	}// 7

	pass Kena_Staff_BowTrail
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_STAFF_BOWTRAIL();
	} // 8

	pass Kena_Staff_BowString
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_STAFF_BOWSTRING();
	} // 9

	pass Kena_Staff_BowString_Part2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_STAFF_BOWSTRING_PART2();
	} // 10

	pass Shadow
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
	}	//11

	pass CINE
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_CINE();
	} //12

	pass ERAO
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_PULSEPLATEANIM();
	} //13

	pass MonDefault
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MON();
	}//14

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
	}//15

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
	}//16

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
	}//17

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
	}//18

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
	}// 19

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
	}// 20

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
	}//21

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
	}//22

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
	}//23

	pass BOSS_AO_R_M
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BOSS_AO_R_M();
	}//24

	pass BOSS_AO_R_M_E
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BOSS_AO_R_M_E();
	}//25

	pass HUNTER_STRING
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HUNTER_STRING();
	}//26

	pass DEFAULT_AORM
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_AO_R_M_DEFAULT();
	}//27

	pass DEFAULT_AORM_E
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_AO_R_M_E_DEFAULT();
	}//28

	pass BENI_EYE
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BENI_EYE();
	}//29

	pass SAIYA_EYE
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SAIYA_EYE();
	}//30

	pass RotHair
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ROTHAIR();
	}//31

	pass NonNormal
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NONNORMAL();
	}//32

	pass Emissive
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EMISSIVE();
	}//33

	pass HUNTER_HAIR
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HUNTER_HAIR();
	}//34
	pass E_R_AO_E
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_E_R_AO_E_DISSOLVE();
	}//35
};
