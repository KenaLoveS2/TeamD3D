#include "Shader_Engine_Defines.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

matrix			g_LightCamWorldMatrix,	g_CamViewMatrix, g_CamProjMatrix;

float4			g_vLightCamLook , g_vCamLook;
float4			g_vLightCamPos;
float4			g_vCamPosition;
matrix			g_ReflectViewMatrix;

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
Texture2D<float4>		g_PrevFrameTexture;

float g_Time;
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
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;	
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);	

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT VS_REFLECT(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ReflectViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vDiffuse = g_LDRTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor = vDiffuse;
	
	return Out;
}

float2 Distortion(float2 uv, float time, Texture2D<float4> distortionTex)
{
	float2 displacementCoords = uv * 1.5f + time * 0.1f;
	float4 displacementColor = distortionTex.Sample(LinearSampler, displacementCoords);
	float2 displacementVector = (displacementColor.rg - 0.5f) * distortionAmount;

	uv += displacementVector;

	return  uv;
}

PS_OUT PS_DISTORTION(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vDiffuse = float4(0.f,0.f,0.f,0.f);
	float4 vEffect = g_EffectTexture.Sample(PointSampler, In.vTexUV);

	float4 FinalColor = float4(0.f, 0.f, 0.f, 0.f);

	if (vEffect.a <= 0.5f)
	{
		float2 distort = Distortion(In.vTexUV, g_Time, g_DistortionTexture);
		vDiffuse = g_LDRTexture.Sample(LinearSampler, distort);
		//vEffect = g_EffectTexture.Sample(PointSampler, distort);
		FinalColor = vDiffuse;
	}
	else
	{
		vDiffuse = g_LDRTexture.Sample(LinearSampler, In.vTexUV);
		FinalColor = vDiffuse;
	}

	Out.vColor = FinalColor;
	return Out;
}

PS_OUT PS_GRAYSCALE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 color = g_LDRTexture.Sample(LinearSampler, In.vTexUV);
	float luminance = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));
	Out.vColor = float4(luminance, luminance, luminance, color.a);

	return Out;
}

PS_OUT PS_MOTIONBLUR(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 CurFrameColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV);
	float4 PrevFrameColor = g_PrevFrameTexture.Sample(LinearSampler, In.vTexUV);
	float4 blurColor = lerp(CurFrameColor, PrevFrameColor, 0.5);

	Out.vColor = saturate(blurColor);

	return Out;
}

float3 cc(float3 color, float factor, float factor2) // color modifier
{
	float w = color.x + color.y + color.z;
	return lerp(color, (float3) w*factor, w*factor2);
}

PS_OUT PS_FLARE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	float4 CurColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV);

	/* Dot */
	float3 lightDir = normalize(g_vLightCamLook.xyz);
	float3 camDir = normalize(g_vCamLook.xyz);
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float dotProduct = dot(lightDir, camDir);
	float verticalDotProduct = dot(camDir, up);
	float verticalAngle = acos(verticalDotProduct);
	float falloff = saturate(1.0f - dotProduct - pow(verticalAngle, 3.0f));
	float falloffIntensity = pow(falloff, 3.0f);
	/* Dot */

	float4 lightpos = /*g_vLightCamPos;*/ float4(-10.f ,10.f, -10.f,1.f);
	matrix matVP;
	matVP = mul(g_CamViewMatrix, g_CamProjMatrix);
	lightpos = mul(vector(lightpos.xyz, 1.f), matVP);

	float2	vNewUV;
	vNewUV.x = (lightpos.x / lightpos.w) * 0.5f + 0.5f;
	vNewUV.y = (lightpos.y / lightpos.w) * -0.5f + 0.5f;

	float lightDepth = lightpos.z / lightpos.w;
	float depth = g_DepthTexture.Sample(LinearSampler, In.vTexUV).r;

	if(depth > lightDepth)
	{
		Out.vColor = CurColor;
	}
	else
	{
		float4 FlareColor = g_FlareTexture.Sample(LinearSampler, vNewUV);
		FlareColor *= falloffIntensity;
		Out.vColor = CurColor + FlareColor * 0.3f;
		// Streak
		float3 StreakColor = float3(0.1f, 0.1f, 0.05f) * falloffIntensity;
		float StreakLength = pow(falloff, 0.25f) * 1.0f;
		float2 StreakDir = normalize(float2(vNewUV)-In.vTexUV);
		float2 StreakUV = (StreakDir + 1.0f) / 2.0f;
		float StreakMask = 1.0f - saturate((length(float2(vNewUV)-In.vTexUV) - StreakLength) / StreakLength);
		float3 Streak = g_LDRTexture.Sample(LinearSampler, StreakUV).rgb * StreakColor * StreakMask;
		Out.vColor.rgb += Streak;
	}
	// Streak

	return Out;
}

float  g_fFar = 500.f;
matrix g_ProjMatrixInv;
matrix g_ViewMatrixInv;

//PS_OUT PS_LIGHTSHAFT(PS_IN In)
//{
//	PS_OUT Out = (PS_OUT)0;
//
//	float4 FinalColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV);
//	float4 vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
//
//	float fViewZ = vDepthDesc.y * g_fFar;
//	vector vWorldPos;
//
//	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
//	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
//	vWorldPos.z = vDepthDesc.x;
//	vWorldPos.w = 1.f;
//	vWorldPos *= fViewZ;
//
//	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
//	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
//	// 투영좌표까지 올라와 있는거 월드 좌표까지 내려야함
//	float fDist = length(vWorldPos.xyz - g_vCamPosition.xyz);
//	float fogFactor = saturate((fDist - g_FogStart) / g_FogRange);
//
//	Out.vColor.rgb = lerp(FinalColor.rgb, g_FogColor.rgb, fogFactor);
//	Out.vColor.a = FinalColor.a;
//
//	return Out;
//}

float4 g_LightShaftValue = float4(0.5f, 0.9f, 0.5f, 1.f);
// x= Density
// y = Decay
// z = Weight
// w = Exposure

#define NUM_SAMPLES 64

PS_OUT PS_LIGHTSHAFT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	// 광원의 위치
	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	float		fViewZ = vDepthDesc.y * g_fFar;
	vector		vWorldPos;
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
	vWorldPos.w = 1.0f;
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
	vWorldPos *= fViewZ;
	/* 로컬위치 * 월드행렬 * 뷰행렬 */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
	/* 로컬위치 * 월드행렬  */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	 // calculate light direction
	float3 lightDir = normalize(float3(1.f,-1.f,1.f) - g_vCamPosition.xyz);
	// calculate distance to player
	float fDistance = length(g_vCamPosition.xyz - vWorldPos.xyz);
	float fAtt = max((50.f - fDistance), 0.f) / 50.f;

	// calculate position of light source in projection space
	float4 lightPos = float4(g_vCamPosition.xyz + lightDir * 175.f, 1.f);
	matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
	lightPos = mul(lightPos, matVP);

	// calculate delta texture coordinate
	float2 dist = abs(In.vTexUV - lightPos.xy);
	float2 texCoord = In.vTexUV;
	float2 DeltaTexCoord = (In.vTexUV - lightPos.xy);
	DeltaTexCoord *= 1.f / NUM_SAMPLES * g_LightShaftValue.x * (1.f - fAtt);

	//matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
	//lightPos = mul(lightPos, matVP);
	//float2 dist = abs(In.vTexUV - lightPos.xy);
	//float2 texCoord = In.vTexUV;
	//float2 DeltaTexCoord = (In.vTexUV - lightPos.xy);
	//// Scale DeltaTexCoord based on distance from light source
	//DeltaTexCoord *= 1.f / NUM_SAMPLES * g_LightShaftValue.x * fAtt;

	float4 FinalColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV);

	float illuminationDecay =1.f;

	for (int i = 0; i < NUM_SAMPLES; ++i)
	{
		texCoord -= DeltaTexCoord;
		float4 vSample = g_LDRTexture.Sample(LinearSampler, texCoord);
		vSample *= illuminationDecay * g_LightShaftValue.z * (1.f - fAtt);
		FinalColor += vSample * 0.3f;
		illuminationDecay *= g_LightShaftValue.y ;
	}

	Out.vColor = saturate(FinalColor * g_LightShaftValue.w);

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

	pass Distortion_P
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

	pass GrayScale
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_GRAYSCALE();
	} //2

	pass Reflect
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_REFLECT();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	} //3

	pass MotionBlur
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MOTIONBLUR();
	} //4

	pass Flare
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FLARE();
	} //5

	pass LightShaft
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_LIGHTSHAFT();
	} //6
}