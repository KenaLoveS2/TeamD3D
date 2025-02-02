#include "Shader_Engine_Defines.h"

matrix					g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D<float4>		g_LDRTexture;

Texture2D<float4>		g_NormalTexture;
Texture2D<float4>		g_DepthTexture;
Texture2D<float4>		g_DiffuseTexture;
Texture2D<float4>		g_ShadeTexture;
Texture2D<float4>		g_SpecularTexture;

Texture2D<float4>		g_LightDepthTexture;
Texture2D<float4>		g_FlareTexture;
Texture2D<float4>		g_EffectTexture;
Texture2D<float4>		g_DistortionTexture;

float  g_Time;
float2 distortionAmount = float2(0.01f, 0.01f);
float2 ScreenSize = float2(1600.f, 900.f);

#define FXAA_QUALITY_SUBPIX  0.75f
#define FXAA_QUALITY_EDGE_THRESHOLD 0.125f
#define FXAA_QUALITY_EDGE_THRESHOLD_MIN 0.0312f
#define FXAA_QUALITY_EDGE_THRESHOLD_MAX 0.25f
#define FXAA_SUBPIX_FLOOR  0.125f
#define FXAA_QUALITY_CORNER_SHARPNESS 1.f

float2 DistortUV(float2 uv, float time)
{
	float2 offset = distortionAmount * sin(time * uv.y * ScreenSize.x / 10.0);
	return uv + offset;
}

float2 RadialDistorUV(float2 uv, float time)
{
	// Calculate the displacement vector from the center position and time
	float2 displacementVector = uv - float2(0.5, 0.5f);
	float distanceFromCenter = length(displacementVector);
	float displacementAmount = sin(distanceFromCenter * 2.f + time) * 3.5f;
	displacementVector *= displacementAmount;

	// Calculate the new UV coordinates by adding the displacement vector
	uv += displacementVector;

	return uv;
}

float3 FilmicTonemapping(float3 color)
{
	// Filmic tonemapping curve parameters
	const float A = 0.22f;
	const float B = 0.30f;
	const float C = 0.10f;
	const float D = 0.20f;
	const float E = 0.01f;
	const float F = 0.30f;

	color *= 2.f;

	// Apply the filmic tonemapping curve
	color = ((color*(A*color + C*B) + D*E) / (color*(A*color + B) + D*F)) - E / F;

	return color;
}

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

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

float2 Distortion(float2 uv, float time, Texture2D<float4> distortionTex)
{
	float2 displacementCoords = uv * 1.5f + time * 0.1f;
	float4 displacementColor = distortionTex.Sample(LinearSampler, displacementCoords);
	float2 displacementVector = (displacementColor.rg - 0.5f) * distortionAmount;

	uv += displacementVector;

	return  uv;
}

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vDiffuse = g_LDRTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor = vDiffuse;
	return Out;
}

PS_OUT PS_DISTORTION(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float3 tsNormal = normalize(g_DistortionTexture.Sample(LinearSampler, In.vTexUV).xyz * 2.0 - 1.0);
	tsNormal = normalize(In.vTangent.rgb * tsNormal.x + In.vBinormal * tsNormal.y + In.vNormal.rgb * tsNormal.z);

	float  distortionAmount = g_Time;
	float2 distortion = tsNormal.xy * distortionAmount;
	float2 timeOffset = float2(cos(g_Time), sin(g_Time)) * g_Time;
	float2 distortedTexCoord = In.vTexUV + distortion + timeOffset;

	float4 vDiffuse = g_LDRTexture.Sample(LinearSampler, distortedTexCoord);
	float4 vEffect = g_EffectTexture.Sample(PointSampler, distortedTexCoord);

	Out.vColor = vDiffuse;
	return Out;
}

technique11 DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	} //0

	pass Distortion
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION();
	} //1
}