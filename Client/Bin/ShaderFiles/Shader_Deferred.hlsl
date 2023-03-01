#include "Shader_Engine_Defines.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_ProjMatrixInv, g_ViewMatrixInv;
matrix			g_LightViewMatrix, g_LightProjMatrix;

vector			g_vLightDir;
vector			g_vLightPos;
float				g_fLightRange;
float				g_fFar = 300.f; // ī�޶��� FAR
vector			g_vLightDiffuse;
vector			g_vLightAmbient;
vector			g_vLightSpecular;

vector			g_vCamPosition;

float				g_fTexcelSizeX;
float				g_fTexcelSizeY;

vector			g_vMtrlAmbient = (vector)1.f;
vector			g_vMtrlSpecular = (vector)1.f;

Texture2D<float4>		g_Texture; /* ����׿��ؽ���*/

Texture2D<float4>		g_NormalTexture;
Texture2D<float4>		g_DepthTexture;
Texture2D<float4>		g_DiffuseTexture;
Texture2D<float4>		g_ShadeTexture;
Texture2D<float4>		g_SpecularTexture;

Texture2D<float4>		g_ShadowTexture;
Texture2D<float4>		g_MtrlAmbientTexture;

float3 CalculateDiffuse(float3 albedo, float3 normal, float3 lightColor, float3 lightDirection)
{
	float ndl = max(0, dot(lightDirection, normal));
	return albedo * lightColor * ndl;
}

float3 CalculateSpecular(float3 viewDirection, float3 lightDirection, float3 normal, float roughness, float metallic, float3 albedo, float3 lightColor)
{
	float roughnessSquared = roughness * roughness;

	// Calculate the reflectance of the material at normal incidence using the Schlick's approximation
	float3 F0 = lerp((float3)0.04, albedo, metallic);

	// Calculate the specular reflectance using the GGX distribution and Smith's shadowing function
	float3 H = normalize(viewDirection + lightDirection);
	float3 N = normalize(normal);
	float3 V = normalize(viewDirection);
	float3 L = normalize(lightDirection);

	float NoH = max(0.001, dot(N, H));

	float3 F = F0 + (1 - F0) * pow(1 - NoH, 5);
	float3 F_Schlick = F + (1 - F) * pow(1 - dot(H, lightDirection), 5);
	float roughnessSquared_Divided_By_PI = roughnessSquared / PI;
	float NdotL = max(0.001, dot(N, L));
	float NdotV = max(0.001, dot(N, V));
	float NdotH = max(0.001, dot(N, H));
	float VdotH = max(0.001, dot(V, H));

	float G = min(1, min(2 * NdotH * NdotV / VdotH, 2 * NdotH * NdotL / VdotH));
	float3 specular = (F_Schlick * G * roughnessSquared_Divided_By_PI) / (NdotL * NdotV);

	return lightColor * specular;
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

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT			Out = (PS_OUT_LIGHT)0;

	vector		vDiffuse		   = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc   = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vDepthDesc     = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	vector		vAmbientDesc = g_MtrlAmbientTexture.Sample(LinearSampler, In.vTexUV);

	float			fViewZ = vDepthDesc.y * g_fFar;
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
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
	// Calculate the specular reflectance using the GGX distribution and Smith's shadowing function

	//float3		Lo = (float3)0.f;
	//float3		P = vWorldPos.xyz;
	//float3		V = normalize(vLook.xyz);
	//float3       N = normalize(vNormal.xyz);
	//float			fAO = vAmbientDesc.r;
	//float			fRoughness = vAmbientDesc.g;
	//float			fMetalic = vAmbientDesc.b;
	//float3		F0 = lerp(float3(0.04f, 0.04f, 0.04f), vDiffuse.rgb, fMetalic);
	//float3 LightColor = g_vLightDiffuse.rgb;
	//LightColor *= min(174.25f / 255.f, length(LightColor));
	//LightColor *= 255.f;
	//float3 L = normalize(g_vLightDir.xyz);
	//float3 H = normalize(V + L);
	//float  cosTheta = max(dot(H, V), 0.0f);
	//float  distance = length(g_vLightPos - vWorldPos); 
	//float attenuation = 1 / (distance * distance);
	//float3 radiance = LightColor * attenuation;
	//float3 F = FresnelSchlick(dot(H, V), F0);
	//float NDF = DistributionGGX(N, H, clamp(fRoughness, 0.3f, 1.f));
	//float G = GeometrySmith(N, V, L, fRoughness);
	//float3 numerator = NDF * G * F;
	//float denomenator = (4 * max(0.0f, dot(N, L)) * max(0.0f, dot(N, V)));
	//float3 specular = numerator / (denomenator + 0.0001f);
	//float3 kS = F;
	//float3 kD = (float3)1.f - kS;
	//kD *= 1.f - fMetalic;
	//float nl = max(0.f, dot(N, L));
	//Lo += (kD * vDiffuse.xyz / PI + specular) * radiance * nl;
	//kS = FresnelSchlick(max(dot(N, V), 0.f), F0);
	//kD = (float3)1.f - kS;
	//float3 irradiance = float3(1.f, 1.f, 1.f); // ȯ���
	//float3 diffuse = irradiance * vDiffuse.xyz;
	//float3 ambient = kD * diffuse;
	//float4 color = float4(ambient + Lo, 1.f);
	//float gamma = 2.2f;
	//color = color / (color + (float4)1.f);
	//color = pow(color, (float4)(1.f / gamma));
	//Out.vShade = color;
	//Out.vShade.a = vDiffuse.a;
	//Out.vSpecular = float4(specular, 0.f);
	//Out.vSpecular.a = 0.f;

	float3 diffuse = CalculateDiffuse(vDiffuse.rgb, vNormal, g_vLightDiffuse.rgb, g_vLightDir);
	float3 specular = CalculateSpecular(vLook.xyz, g_vLightDir.xyz, vNormal.xyz, vAmbientDesc.g, vAmbientDesc.b, vDiffuse.rgb, g_vLightSpecular.rgb);
	Out.vShade = float4(diffuse, vDiffuse.a);
	Out.vSpecular = float4(specular, 0.f);
	return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT_LIGHT			Out = (PS_OUT_LIGHT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	vector		vAmbientDesc = g_MtrlAmbientTexture.Sample(LinearSampler, In.vTexUV);

	float		fViewZ = vDepthDesc.y * g_fFar;

	/* 0 ~ 1 => -1 ~ 1 */
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	/* ȭ�鿡 �׷����� �ִ� �ȼ����� ���������̽� ���� ��ġ. */
	/* ������ġ * ������� * ����� * ������� / z */
	vector		vWorldPos;
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
	vWorldPos.w = 1.0f;

	/* ������ġ * ������� * ����� * ������� */
	vWorldPos *= fViewZ;

	/* ������ġ * ������� * ����� */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* ������ġ * �������  */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector		vLightDir = vWorldPos - g_vLightPos;

	float		fDistance = length(vLightDir);

	float		fAtt = max((g_fLightRange - fDistance), 0.f) / g_fLightRange;

	Out.vShade = g_vLightDiffuse * saturate(saturate(dot(normalize(vLightDir) * -1.f, normalize(vNormal))) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
	Out.vShade.a = 1.f;	

	vector		vReflect = reflect(normalize(vLightDir), normalize(vNormal));
	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vLook) * -1.f, normalize(vReflect))), 30.f) * fAtt;
	Out.vSpecular.a = 0.f;

	return Out;
}

PS_OUT PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	             
	vector		vDiffuse			 = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vShade				 = g_ShadeTexture.Sample(LinearSampler, In.vTexUV);
	vector		vDepthDesc		 = g_DepthTexture.Sample(DepthSampler, In.vTexUV);
	vector		vSpecular			 = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor =	CalcHDRColor(vShade, vDepthDesc.b) + vSpecular * 0.001f;

	if (Out.vColor.a == 0.0f)
		discard;

	float		fViewZ = vDepthDesc.y * g_fFar;

	/* ������ġ * ������� * ����� * ������� / z */
	vector		vPosition;
	vPosition.x = In.vTexUV.x * 2.f - 1.f;
	vPosition.y = In.vTexUV.y * -2.f + 1.f;
	vPosition.z = vDepthDesc.x; /* 0 ~ 1 */
	vPosition.w = 1.0f;

	/* ������ġ * ������� * ����� * ������� */
	vPosition *= fViewZ;

	// �� ��
	vPosition = mul(vPosition, g_ProjMatrixInv);

	// ���� ��
	vPosition = mul(vPosition, g_ViewMatrixInv);
	vPosition = mul(vPosition, g_LightViewMatrix);

	vector	vUVPos = mul(vPosition, g_LightProjMatrix);
	float2	vNewUV;

	vNewUV.x = (vUVPos.x / vUVPos.w) * 0.5f + 0.5f;
	vNewUV.y = (vUVPos.y / vUVPos.w) * -0.5f + 0.5f;

	float2 TexcelSize = float2(1.0f /g_fTexcelSizeX, 1.0f / g_fTexcelSizeY);
	float fShadowRate = 0.f;

	for (int y = -1; y <= 1; ++y)
	{
		for (int x = -1; x <= 1; ++x)
		{
			float2 offset = float2(x, y) * TexcelSize;
			vector	vShadowDesc = g_ShadowTexture.Sample(LinearSampler, vNewUV + offset);
			if (vPosition.z - 0.1f > vShadowDesc.r * g_fFar)
				fShadowRate += 1.f;
		}
	}

	fShadowRate /= 9.f;
	fShadowRate *= 0.3f;
	
	Out.vColor *= (1.f - fShadowRate);

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
