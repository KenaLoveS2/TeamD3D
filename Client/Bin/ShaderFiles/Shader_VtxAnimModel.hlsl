
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

float								g_fHairLength = 1.f;
float								g_fHairThickness = 1.f;

float								g_fLashDensity = 0.5f;
float								g_fLashWidth = 0.5f;
float								g_fLashIntensity = 0.5f;

float4							g_vAmbientEyeColor = float4(1.f, 1.f, 1.f, 1.f);
float4							g_vAmbientColor = float4(1.f, 1.f, 1.f, 1.f);
float4							g_vSSSColor = float4(1.f, 0.f, 0.f, 1.f);
float								g_fSSSAmount = 1.f;

float4 SSS(float3 position, float3 normal, float3 dir, float4 color, float2 vUV, float amount, Texture2D<float4> Texturediffuse, Texture2D<float4> sssMask)
{
	float4 result = 0;
	// Calculate the distance to the surface
	float surfaceDistance = distance(position, g_vCamPosition.xyz);
	// Calculate the distance that light travels through the material
	float scatterDistance = sqrt(surfaceDistance) * amount;
	// Calculate the diffuse term for the subsurface scattering
	float diffuse = saturate(dot(normal, -dir)); // 여기서 디렉션이 조명 방향이였음 그러므로 이것도 디퍼드로 바뀌어야함.
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
	Out.vAmbient = (vector)1.f;

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

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
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
	Out.vAmbient = (vector)1.f;

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
	Out.vAmbient = (vector)1.f;

	return Out;
}//7

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
}

