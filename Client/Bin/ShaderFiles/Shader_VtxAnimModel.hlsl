
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
Texture2D<float4>		g_AO_R_MTexture;
Texture2D<float4>		g_EmissiveTexture;
Texture2D<float4>		g_EmissiveMaskTexture;
Texture2D<float4>		g_MaskTexture;
Texture2D<float4>		g_SSSMaskTexture;

Texture2D<float4>		g_HairDepthTexture;
Texture2D<float4>		g_HairAlphaTexture;
Texture2D<float4>		g_HairRootTexture;

Texture2D<float4>		g_DetailNormal;

/* Kena Bow_String Texture */
Texture2D		g_NoiseTexture;
Texture2D		g_SwipeTexture;
Texture2D		g_GradientTexture;
/* Kena Bow_String Texture */

float								g_fHairLength = 1.f;
float								g_fHairThickness = 1.f;

float								g_fLashDensity = 0.5f;
float								g_fLashWidth = 0.5f;
float								g_fLashIntensity = 0.5f;

float4							g_vAmbientEyeColor = float4(1.f, 1.f, 1.f, 1.f);
float4							g_vAmbientColor = float4(1.f, 1.f, 1.f, 1.f);
float4							g_vSSSColor = float4(1.f, 0.f, 0.f, 1.f);
float								g_fSSSAmount = 1.f;

float g_Time;

float4 SSS(float3 position, float3 normal, float3 dir, float4 color, float2 vUV, float amount, Texture2D<float4> Texturediffuse, Texture2D<float4> sssMask)
{
	float4 result = 0;
	// Calculate the distance to the surface
	float surfaceDistance = distance(position, g_vCamPosition.xyz);
	// Calculate the distance that light travels through the material
	float scatterDistance = sqrt(surfaceDistance) * amount;
	// Calculate the diffuse term for the subsurface scattering
	float diffuse = saturate(dot(normal, -dir)); // ���⼭ �𷺼��� ���� �����̿��� �׷��Ƿ� �̰͵� ���۵��?�ٲ�����.
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

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = g_vAmbientEyeColor;

	return Out;
}//1

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

	FinalColor = vDiffuse + vEmissive;

	if (0.1f > vDiffuse.a)
		discard;

	Out.vDiffuse = vector(FinalColor.rgb, 1.f);
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = vAO_R_M;

	return Out;
}//2

PS_OUT PS_MAIN_KENA_MAINOUTFIT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2		vTexUV = In.vTexUV;

	vector		vAO_R_M			 = g_AO_R_MTexture.Sample(LinearSampler, vTexUV);
	vector		vMask		 		 = g_MaskTexture.Sample(LinearSampler, vTexUV);
	vector		vSSSMask		 	 = g_SSSMaskTexture.Sample(LinearSampler, vTexUV);
	vector		vEmissiveMask = g_EmissiveMaskTexture.Sample(LinearSampler, vTexUV);
	vector		vDiffuse			 = g_DiffuseTexture.Sample(LinearSampler, vTexUV);
	vector		vEmissive			 = g_EmissiveTexture.Sample(LinearSampler, vTexUV);
	vector		vNormalDesc	 = g_NormalTexture.Sample(LinearSampler, vTexUV);

	float3		vNormal = (vNormalDesc.xyz) * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	float4		FinalColor = float4(1, 1, 1, 1);

	FinalColor = vDiffuse + vEmissive * vEmissiveMask.r;

	Out.vDiffuse = vector(FinalColor.rgb, 1.f);
	Out.vNormal  = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth     = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = vAO_R_M;

	return Out;
}//3

PS_OUT PS_MAIN_FACE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vAO_R_M	  = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);
	vector		vSSSMask	  = g_SSSMaskTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	float4		FinalColor = float4(0, 0, 0, 1);
	FinalColor = vDiffuse;

	Out.vDiffuse = vector(FinalColor.rgb, 1.f);
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = vAO_R_M;

	return Out;
}//4

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

	Out.vDiffuse = vector(FinalColor.rgb, 1.f);
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = vAO_R_M;

	return Out;
}//5

PS_OUT PS_MAIN_HAIR(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
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

	float fFinalAlpha = fAlpha * saturate((fDepth - rootPosition.z)/(1.f - rootPosition.z));
	fFinalAlpha = 1.f - fFinalAlpha;

	Out.vDiffuse = float4(vDiffuse.rgb, fFinalAlpha);
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f , 0.f);
	Out.vAmbient = Out.vDiffuse;

	return Out;
}//6

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

	float4 finalColor = float4(lerp(vAlbedo, vLashColor, distance * g_fLashIntensity));

	Out.vDiffuse = finalColor;
	Out.vNormal = (vector)1.f;
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = Out.vDiffuse;

	return Out;
}//7

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

	Out.vDiffuse.rgb = Out.vDiffuse.rgb * 4.f;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = Out.vDiffuse;

	return Out;
}//8

PS_OUT PS_MAIN_STAFF_BOWSTRING(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	/* Texture */
	vector vMask  = g_MaskTexture.Sample(LinearSampler, float2(In.vTexUV.x + 1.78, In.vTexUV.y));
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
	mask_texture.rgb = mask_texture.rgb * 1.5f;
	mask_texture = mask_texture + MaskColor;
	/* Mask */

	Out.vDiffuse = mask_texture;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = Out.vDiffuse;

	return Out;
}//9

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
	float4 TextureColor = vector(37.0f,210.f, 255.f, 50.f) / 255.f;
	float4 vColor = vector(0.0f, 194.f, 221.f, 0.0f) / 255.f;
	float4 FinalDiffuseColor = vector(38.0f, 108.f, 145.f, 228.f) / 255.f;

	float4   fresnel = float4(fresnel_glow(4.5, 2.5, vColor.rgb, In.vNormal.rgb, In.vViewDir), vColor.a);

	vector vNoise = g_NoiseTexture.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y - time / OffsetUV.y));
	vector vSwipe = g_SwipeTexture.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y - time / OffsetUV.y));
	float  fAlpha = 1.f - (abs(0.5f - In.vTexUV.y) * 2.f);

	Out.vDiffuse = lerp(vNoise, vSwipe, 1.f - fAlpha);
	Out.vDiffuse = saturate(Out.vDiffuse * FinalDiffuseColor * 2.f) * fresnel + FinalDiffuseColor;
	Out.vDiffuse.a = Out.vDiffuse.a*fAlpha * 0.5f;

	Out.vDiffuse.rgb = Out.vDiffuse.rgb * 2.f;
	if (Out.vDiffuse.a < 0.33)
		discard;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	Out.vAmbient = Out.vDiffuse;

	return Out;
}//10

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
	}

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
	}

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
	}

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
	}

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
	}

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
	}

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
	}
	// 8
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
	}
	// 9
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
	}
	// 10
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
	}
}
