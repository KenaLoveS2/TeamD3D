#include "Shader_Client_Defines.h"

/**********Constant Buffer*********/
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_SocketMatrix;
//matrix			g_ReflectedViewMatrix;
float				g_fFar = 300.f;
float4			g_vCamPosition;
float				g_fTime;
/**********************************/

Texture2D<float4>		g_DiffuseTexture;
Texture2D<float4>		g_NormalTexture;
Texture2D<float4>		g_ReflectTexture;

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
//float4       vReflectionMapSamplingPos : TEXCOORD2;
//float4		vRefractionMapSamplingPos : TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;//, matRVP, matWRVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

//matRVP = mul(g_ReflectedViewMatrix, g_ProjMatrix);
//matWRVP = mul(g_WorldMatrix, matRVP);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vPosition.y += sin(In.vPosition.x + In.vPosition.z + g_fTime * 5.0f) * 0.1f;
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
	Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));
//Out.vReflectionMapSamplingPos = mul(float4(In.vPosition, 1.f), matWRVP);
//Out.vRefractionMapSamplingPos = mul(float4(In.vPosition, 1.f), matRVP);
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
	//float4       vReflectionMapSamplingPos : TEXCOORD2;
	//float4		vRefractionMapSamplingPos : TEXCOORD3;
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

	/* Sampling Texture */
	/*float2  vProjectedTexCoord;
	vProjectedTexCoord.x = In.vReflectionMapSamplingPos.x / In.vReflectionMapSamplingPos.w / 2.f + 0.5f;
	vProjectedTexCoord.y = -In.vReflectionMapSamplingPos.y / In.vReflectionMapSamplingPos.w / 2.f + 0.5f;
	float4  vReflectiveColor = g_ReflectTexture.Sample(LinearSampler, vProjectedTexCoord);*/

	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	float4 		vDiffuse = float4(0.f, 0.5f, 1.f, 1.f);

	/* 탄젠트스페이스 */
	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	//float4 FinalColor = lerp(vDiffuse, vReflectiveColor, 0.5f);

	Out.vDiffuse = float4(vDiffuse.rgb, 0.3f);
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
}
