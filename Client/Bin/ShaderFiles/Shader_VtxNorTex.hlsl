#include "Shader_Client_Defines.h"

/**********Constant Buffer***********/
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector			g_vBrushPos;
float			g_fBrushRange = 5.f;
/*************************************/
/* 재질정보 */

//Texture2D<float4>		g_DiffuseTexture;
//
//Texture2D<float4>		g_FilterOneTexture;
//Texture2D<float4>		g_FilterTwoTexture;
//Texture2D<float4>		g_FilterThreeTexture;


Texture2D<float4>		g_DiffuseTexture[7];

/* 지형 셰이딩 */
Texture2D<float4>		g_BrushTexture;

Texture2D<float4>		g_FilterTexture[3];

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct PS_OUT
{
	/*SV_TARGET0 : 모든 정보가 결정된 픽셀이다. AND 0번째 렌더타겟에 그리기위한 색상이다. */
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
	vector		vSourDiffuse = g_DiffuseTexture[3].Sample(LinearSampler, In.vTexUV * 30.f);
	vector		vDestDiffuse0 = g_DiffuseTexture[0].Sample(LinearSampler, In.vTexUV * 30.f);

	vector vDestDiffuse1 = g_DiffuseTexture[1].Sample(LinearSampler, In.vTexUV * 30.f);
	vector vDestDiffuse2 = g_DiffuseTexture[4].Sample(LinearSampler, In.vTexUV * 30.f);

	vector		vFilter = g_FilterTexture[0].Sample(LinearSampler, In.vTexUV);			// 원본
	vector		vFilter1 = g_FilterTexture[1].Sample(LinearSampler, In.vTexUV);
	vector		vFilter2 = g_FilterTexture[2].Sample(LinearSampler, In.vTexUV);

	vector		vMtrlDiffuse;
	vector		vSour = (vSourDiffuse* vFilter.r * vFilter1.r) + vDestDiffuse0 * (1.f - vFilter.r) + vDestDiffuse1 * (1.f - vFilter1.r);
	vector		vDest = (vSourDiffuse* vFilter1.r * vFilter2.r) + vDestDiffuse1 * (1.f - vFilter1.r) + vDestDiffuse2 * (1.f - vFilter2.r);
	vector		vTemp = (vSourDiffuse* vFilter.r * vFilter2.r) + vDestDiffuse0 * (1.f - vFilter.r) + vDestDiffuse2 * (1.f - vFilter2.r);

	vMtrlDiffuse = (vSourDiffuse * vFilter.r * vFilter1.r* vFilter2.r)
		+ vDestDiffuse0 * (1.f - vFilter.r) + vDestDiffuse1 * (1.f - vFilter1.r)+vDestDiffuse2 * (1.f - vFilter2.r);

	//vMtrlDiffuse = (vSourDiffuse * vFilter.r * vFilter1.r* vFilter2.r)		//원본
	//	+ vDestDiffuse0 * (1.f - vFilter.r) + vDestDiffuse1 * (1.f - vFilter1.r) ;

	//if (vMtrlDiffuse.a > 1.f)
	//	vMtrlDiffusea =0.5f;

	if (vSour.a > 1.f)
	{
		vMtrlDiffuse = vSourDiffuse;
	}

	//if (vDest.a > 1.f)
	//{
	//	vMtrlDiffuse = vSourDiffuse;
	//}

	if (vTemp.a > 1.f)
	{
		vMtrlDiffuse = vSourDiffuse;
	}

	Out.vDiffuse = vMtrlDiffuse;
	Out.vDiffuse.a = 1.f;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.f, 0.f);

	return Out;
}

technique11 DefaultTechnique
{
	pass Terrain
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
}
