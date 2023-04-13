#include "Shader_Engine_Defines.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_ProjMatrixInv, g_ViewMatrixInv;
matrix			g_LightViewMatrix, g_LightProjMatrix;
matrix			g_DynamicLightViewMatrix;

vector			g_vLightDir;
vector			g_vLightPos;
float				g_fLightRange;
float				g_fFar = 500.f; // 카메라의 FAR
vector			g_vLightDiffuse;
vector			g_vLightAmbient;
vector			g_vLightSpecular;

vector			g_vCamPosition;

float				g_fTexcelSizeX;
float				g_fTexcelSizeY;

vector			g_vMtrlAmbient = (vector)1.f;
vector			g_vMtrlSpecular = (vector)1.f;

bool				g_bShadow = true;
bool				g_bSSAO = true;

Texture2D<float4>		g_Texture; /* 디버그용텍스쳐*/

Texture2D<float4>		g_NormalTexture;
Texture2D<float4>		g_DepthTexture;
Texture2D<float4>		g_DiffuseTexture;
Texture2D<float4>		g_ShadeTexture;
Texture2D<float4>		g_SpecularTexture;

Texture2D<float4>		g_ShadowTexture;
Texture2D<float4>		g_StaticShadowTexture;
Texture2D<float4>		g_MtrlAmbientTexture;
Texture2D<float4>		g_SSAOTexture;

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

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	
	return Out;
}

struct PS_OUT_LIGHT
{
	float4		vShade : SV_TARGET0;	
	float4		vSpecular : SV_TARGET1;
};

float3 CalcAmbientOcclusion(float3 worldPos, float3 normal, Texture2D<float4> g_AO_R_MTexture, float2 UV)
{
	float ao = g_AO_R_MTexture.Sample(LinearSampler, UV).r;
	return lerp(float3(1.0f, 1.0f, 1.0f), ao, normal);
}

float3 CalcSpecular(float3 worldPos, float3 normal, float3 viewDir, float3 lightDir, float3 albedo, float3 reflectDir, Texture2D<float4> g_AO_R_MTexture, float2 UV)
{
	float metallic = g_AO_R_MTexture.Sample(LinearSampler, UV).b;
	float roughness = g_AO_R_MTexture.Sample(LinearSampler, UV).g;

	float3 halfwayDir = normalize(reflectDir + viewDir);

	float3 F0 = lerp(float3(0.04f, 0.04f, 0.04f), albedo, metallic);
	float3 F90 = float3(1.0f, 1.0f, 1.0f);

	float roughnessFactor = roughness * roughness;

	float NoV = max(dot(normal, viewDir), 0.0f);
	float NoL = max(dot(normal, lightDir), 0.0f);
	float LoH = max(dot(lightDir, halfwayDir), 0.0f);

	float D = DistributionGGX(normal, halfwayDir, roughnessFactor);
	float G = GeometrySchlickGGX(NoV, roughnessFactor) * GeometrySchlickGGX(NoL, roughnessFactor);
	float3 F = F0 + (1.0f - F0) * pow(1.0f - dot(viewDir, halfwayDir), 5.0f);
	float3 specular = F * G * D * g_vLightSpecular.rgb / (4 * NoV * NoL);
	float3 diffuse = (1.0f - metallic) * (1.0f - F);
	return specular + diffuse;
}

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT			Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc   = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vDepthDesc     = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	vector		vAmbientDesc = g_MtrlAmbientTexture.Sample(LinearSampler, In.vTexUV);

	float			fViewZ = vDepthDesc.y * g_fFar;
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	/* For Specular */
	vector		vWorldPos;
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
	vWorldPos.w = 1.0f;
	vWorldPos *= fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
	vector		vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));
	vector		vLook = normalize(vWorldPos - g_vCamPosition);
	float ao = g_MtrlAmbientTexture.Sample(LinearSampler, In.vTexUV).r;
	Out.vShade = g_vLightDiffuse * saturate(saturate(dot(normalize(g_vLightDir) * -1.f, normalize(vNormal))) + (g_vLightAmbient * g_vMtrlAmbient * ao));
	Out.vShade.a = 1.f;

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vLook) * -1.f, normalize(vReflect))), 16.f);
	Out.vSpecular.rgb *= CalcSpecular(vWorldPos.xyz, vNormal.xyz, -vLook.xyz, g_vLightDir.xyz, Out.vShade.rgb, vReflect.xyz, g_MtrlAmbientTexture, In.vTexUV);
	Out.vSpecular.a = 0.f;

	return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT_LIGHT			Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	vector		vAmbientDesc = g_MtrlAmbientTexture.Sample(LinearSampler, In.vTexUV);

	float		fViewZ = vDepthDesc.y * g_fFar;

	/* 0 ~ 1 => -1 ~ 1 */
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	/* 화면에 그려지고 있는 픽셀들의 투영스페이스 상의 위치. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / z */
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

	vector		vLightDir = vWorldPos - g_vLightPos;

	float		fDistance = length(vLightDir);

	float		fAtt = max((g_fLightRange - fDistance), 0.f) / g_fLightRange;

	Out.vShade = g_vLightDiffuse * saturate(saturate(dot(normalize(vLightDir) * -1.f, normalize(vNormal))) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
	Out.vShade.rgb *= CalcAmbientOcclusion(vWorldPos.xyz, vNormal.xyz, g_MtrlAmbientTexture, In.vTexUV);
	Out.vShade.a = 1.f;	

	vector		vReflect = reflect(normalize(vLightDir), normalize(vNormal));
	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vLook) * -1.f, normalize(vReflect))), 16.f) * fAtt;
	Out.vSpecular.rgb *= CalcSpecular(vWorldPos.xyz, vNormal.xyz, -vLook.xyz, vLightDir.xyz, Out.vShade.rgb, vReflect.xyz, g_MtrlAmbientTexture, In.vTexUV);
	Out.vSpecular.a = 0.f;

	return Out;
}

bool  g_bFog = false;
float4 g_FogColor = float4(1.f, 1.f, 1.f, 0.f);
float  g_FogStart = 0.f;
float  g_FogRange = 50.f;

PS_OUT PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	             
	vector		vDiffuse			 = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vShade				 = g_ShadeTexture.Sample(LinearSampler, In.vTexUV);
	vector		vDepthDesc		 = g_DepthTexture.Sample(DepthSampler, In.vTexUV);
	vector		vSpecular			 = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);
	vector		vSSAODesc		 = g_SSAOTexture.Sample(LinearSampler, In.vTexUV);

	if(g_bSSAO)
		Out.vColor =	CalcHDRColor(vDiffuse * vShade + vSpecular, vDepthDesc.b)  * vSSAODesc.r;
	else
		Out.vColor = CalcHDRColor(vDiffuse * vShade + vSpecular, vDepthDesc.b);

	if (Out.vColor.a == 0.0f)
		discard;

	if(g_bShadow)
	{
		/* For. Shadow */
		float		fViewZ = vDepthDesc.y * g_fFar;

		/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / z */
		vector		vPosition;
		vPosition.x = In.vTexUV.x * 2.f - 1.f;
		vPosition.y = In.vTexUV.y * -2.f + 1.f;
		vPosition.z = vDepthDesc.x; /* 0 ~ 1 */
		vPosition.w = 1.0f;

		/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
		vPosition *= fViewZ;

		// 뷰 상
		vPosition = mul(vPosition, g_ProjMatrixInv);

		// 월드 상
		vPosition = mul(vPosition, g_ViewMatrixInv);

		vector vDynamicPos = mul(vPosition, g_DynamicLightViewMatrix);
		vector  vDynamicUVPos = mul(vDynamicPos, g_LightProjMatrix);

		vPosition = mul(vPosition, g_LightViewMatrix);
		vector	vUVPos = mul(vPosition, g_LightProjMatrix);

		float2	vNewUV, vDynamicUV;
		vNewUV.x = (vUVPos.x / vUVPos.w) * 0.5f + 0.5f;
		vNewUV.y = (vUVPos.y / vUVPos.w) * -0.5f + 0.5f;

		vDynamicUV.x = (vDynamicUVPos.x / vDynamicUVPos.w) * 0.5f + 0.5f;
		vDynamicUV.y = (vDynamicUVPos.y / vDynamicUVPos.w) * -0.5f + 0.5f;

		float2 TexcelSize = float2(1.0f /g_fTexcelSizeX, 1.0f / g_fTexcelSizeY);
		float fShadowRate = 0.f;

		for (int y = -1; y <= 1; ++y)
		{
			for (int x = -1; x <= 1; ++x)
			{
				float2 offset = float2(x, y) * TexcelSize;
				vector	vShadowDesc = g_ShadowTexture.Sample(LinearSampler, vDynamicUV + offset);
				vector  vStaticShadowDesc = g_StaticShadowTexture.Sample(LinearSampler, vNewUV + offset);
				if (vPosition.z - 0.1f > (vShadowDesc.r * vStaticShadowDesc.r) * g_fFar)
					fShadowRate += 1.f;
			}
		}

		fShadowRate /= 9.f;
		fShadowRate *= 0.3f;
		
		Out.vColor *= (1.f - fShadowRate);
	}

	if (g_bFog)
	{
		float fViewZ = vDepthDesc.y * g_fFar;
		vector vWorldPos;

		vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
		vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
		vWorldPos.z = vDepthDesc.x;
		vWorldPos.w = 1.f;
		vWorldPos *= fViewZ;
		vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
		vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

		// 투영좌표까지 올라와 있는거 월드 좌표까지 내려야함
		float fDist = length(vWorldPos.xyz - g_vCamPosition.xyz);
		float fogFactor = saturate((fDist - g_FogStart) / g_FogRange);

		float4 FinalColor = Out.vColor;
		Out.vColor.rgb = lerp(FinalColor.rgb, g_FogColor.rgb, fogFactor);
	}

	return Out;
}

technique11 DefaultTechnique
{
	pass Debug
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}

	pass Light_Directional
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_One, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	pass Light_Point
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_One, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	pass Blend
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}
}
