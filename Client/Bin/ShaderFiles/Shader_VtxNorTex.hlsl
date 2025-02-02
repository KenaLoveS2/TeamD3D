#include "Shader_Client_Defines.h"

/**********Constant Buffer***********/
vector			g_vBrushPos;
float				g_fBrushRange = 5.f;
float				g_fFar = 500.f;
/*************************************/
/* 재질정보 */

Texture2D<float4>		g_BaseTexture;
Texture2D<float4>		g_DiffuseTexture_0;
Texture2D<float4>		g_DiffuseTexture_1;
Texture2D<float4>		g_DiffuseTexture_2;

/* 지형 셰이딩 */
Texture2D<float4>		g_BrushTexture;
Texture2D<float4>		g_FilterTexture[3];
Texture2D<float4>		g_NormalTexture;

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
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
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
	Out.vTexUV = In.vTexUV;

	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
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
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float4		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
};

struct PS_OUT
{
	/*SV_TARGET0 : 모든 정보가 결정된 픽셀이다. AND 0번째 렌더타겟에 그리기위한 색상이다. */
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
	float4		vAmbient : SV_TARGET3;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV * 50.f);
	float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	vector		vSourDiffuse = g_BaseTexture.Sample(LinearSampler, In.vTexUV * 100.f);
	vector		vDestDiffuse0 = g_DiffuseTexture_0.Sample(LinearSampler, In.vTexUV * 100.f);
	vector		vDestDiffuse1 = g_DiffuseTexture_1.Sample(LinearSampler, In.vTexUV * 100.f);
	vector		vDestDiffuse2 = g_DiffuseTexture_2.Sample(LinearSampler, In.vTexUV * 100.f);

	vector		vFilter = g_FilterTexture[0].Sample(LinearSampler, In.vTexUV);			// 원본
	vector		vFilter1 = g_FilterTexture[1].Sample(LinearSampler, In.vTexUV);
	vector		vFilter2 = g_FilterTexture[2].Sample(LinearSampler, In.vTexUV);

	vector		vMtrlDiffuse;
	vector		vSour = (vSourDiffuse* vFilter.r * vFilter1.r) + vDestDiffuse0 * (1.f - vFilter.r) + vDestDiffuse1 * (1.f - vFilter1.r);
	vector		vDest = (vSourDiffuse* vFilter1.r * vFilter2.r) + vDestDiffuse1 * (1.f - vFilter1.r) + vDestDiffuse2 * (1.f - vFilter2.r);
	vector		vTemp = (vSourDiffuse* vFilter.r * vFilter2.r) + vDestDiffuse0 * (1.f - vFilter.r) + vDestDiffuse2 * (1.f - vFilter2.r);

	vector vBrush = (vector)0.f;
	/* 브러쉬의 영역 내부인지 확인 */
	if (g_fBrushRange >= abs(In.vWorldPos.x - g_vBrushPos.x) && g_fBrushRange >= abs(In.vWorldPos.z - g_vBrushPos.z))
	{
		/* 브러쉬 영역 내부에서의 정점의 상대위치 / 길이 => 정규화 LT(0,0)~RB(1,1) UV좌표 */
		float2	vUV;
		float	fFullRange = g_fBrushRange * 2.f;
		vUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / fFullRange;
		vUV.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / fFullRange;
		vBrush = g_BrushTexture.Sample(LinearSampler, vUV);
	}
	
	vMtrlDiffuse = saturate((vSourDiffuse * vFilter.r * vFilter1.r * vFilter2.r) + vDestDiffuse0 * (1.f - vFilter.r) + vDestDiffuse1 * (1.f - vFilter1.r) + vDestDiffuse2 * (1.f - vFilter2.r)) + vBrush;
	vMtrlDiffuse.a = 1.f;

	Out.vDiffuse = vMtrlDiffuse;
	Out.vDiffuse.a = 1.f;
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.2f, 0.f);
	Out.vAmbient = (vector)1.f;

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
