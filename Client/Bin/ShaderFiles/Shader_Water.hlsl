#include "Shader_Client_Defines.h"

/**********Constant Buffer*********/
//matrix			g_ReflectedViewMatrix;
float				g_fFar = 500.f;
float4			g_vCamPosition;
float				g_fTime;
/**********************************/

Texture2D<float4>		g_DiffuseTexture;
Texture2D<float4>		g_NormalTexture;
Texture2D<float4>		g_NoiseTexture;
Texture2D<float4>		g_MaskTexture;
Texture2D<float4>		g_DepthTexture;

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

float3 WaterDistortion(float3 position, float time)
{
	// Calculate distortion based on position and time
	float3 distortion = float3(
		sin(position.x * 10.0f + time * 0.1f) * 0.05f + sin(position.y * 5.0f + time * 0.05f) * 0.02f,
		cos(position.y * 10.0f + time * 0.2f) * 0.1f,
		0.f);
	return distortion;
}

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	vector vNoiseDesc = g_NoiseTexture.SampleLevel(LinearSampler, In.vTexUV * 5.f, 0);

	In.vPosition.x += vNoiseDesc.r * WaterDistortion(In.vPosition, g_fTime * 10.f).x;
	In.vPosition.y += vNoiseDesc.g * WaterDistortion(In.vPosition, g_fTime * 10.f).y;

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
	/*SV_TARGET0 : 모든 정보가 결정된 픽셀이다. AND 0번째 렌더타겟에 그리기위한 색상이다. */
	float4		vDiffuse  : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth   : SV_TARGET2;
	float4		vAmbient   : SV_TARGET3;			
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 texCoord = float2(In.vTexUV.x  , In.vTexUV.y + g_fTime * 0.01f);

	//vector		vMaskDesc = g_MaskTexture.Sample(LinearSampler, texCoord);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, texCoord);
	float4 		vDiffuse = float4(0.f, 0.5f, 1.f, 1.f);

	/* 탄젠트스페이스 */
	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	float3 toCamera = normalize(g_vCamPosition.xyz - In.vPosition.xyz);
	float3 reflected = reflect(toCamera, vNormal);
	float specular = pow(saturate(dot(reflected, toCamera)), 30.0f);
	vDiffuse += specular;

	float2		vTexUV;
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, vTexUV);
	float		fOldViewZ = vDepthDesc.y * g_fFar; // 카메라의 far
	float		fViewZ = In.vProjPos.w;

	Out.vDiffuse = float4(vDiffuse.rgb, 0.3f);
	Out.vDiffuse.a = Out.vDiffuse.a * (saturate(fOldViewZ - fViewZ));

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);

	//if (vMaskDesc.r <= 0.1f)
		//discard;
	Out.vAmbient = (float4)1.f;
	return Out;
}

PS_OUT PS_PORTALMAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	/* 탄젠트스페이스 */
	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	float3	FinalColor = lerp(vDiffuse.rgb, float3(0.8f, 0.f, 0.8f), 0.2f);

	Out.vDiffuse = float4(FinalColor, 0.6f);
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = (float4)1.f;
	return Out;
}

technique11 DefaultTechnique
{
	pass Default		//0
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

	pass Portal //1
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_PORTALMAIN();
	}
}
