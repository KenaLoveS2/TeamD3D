#include "Shader_Client_Defines.h"
/**********Constant Buffer***********/
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
//vector			g_vBrushPos;
//float			g_fBrushRange = 5.f;
/*************************************/

/* 재질정보 */
Texture2D<float4>		g_DiffuseTexture;

///* 지형 셰이딩 */
//Texture2D<float4>		g_BrushTexture;
//Texture2D<float4>		g_FilterTexture;
//

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


struct VS_OUT_TESS
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT_TESS VS_MAIN_TESS(VS_IN In)
{
	VS_OUT_TESS		Out = (VS_OUT_TESS)0;

	matrix		matWV, matWVP;

	Out.vPosition = In.vPosition;
	Out.vNormal = In.vNormal;
	Out.vTexUV = In.vTexUV;

	return Out;
}


// 헐 세이더의 패치 상수 위상에 대한 출력 매개변수
struct PatchTess		//CONSTANT_HS_OUT
{
	float EdgeTess[3] : SV_TessFactor;			// 패치 가장자리의 테셀레이션 개수
	float InsideTess : SV_InsideTessFactor;	// 패치 내부의 테셀레이션 계수

	// Geometry cubic generated control points
	float3 f3B210    : POSITION3;		// 추가할 제어점
	float3 f3B120    : POSITION4;		// 추가 제어점
	float3 f3B021    : POSITION5;		// 추가할 제어점
	float3 f3B012    : POSITION6;
	float3 f3B102    : POSITION7;
	float3 f3B201    : POSITION8;
	float3 f3B111    : CENTER;

	// Normal quadratic generated control points
	float3 f3N110    : NORMAL3;
	float3 f3N011    : NORMAL4;
	float3 f3N101    : NORMAL5;
};

// 제어점 만드들기
PatchTess ConstantHS(InputPatch<VS_OUT_TESS, 3> Patch, uint PatchID : SV_PrimitiveID)
{
	PatchTess		pt;

	pt.EdgeTess[0] = 4;
	pt.EdgeTess[1] = 4;
	pt.EdgeTess[2] = 4;

	pt.InsideTess = (pt.EdgeTess[0] + pt.EdgeTess[1] + pt.EdgeTess[2]) / 3.f;

	// Assign Positions
	float3 f3B003 = Patch[0].vPosition;
	float3 f3B030 = Patch[1].vPosition;
	float3 f3B300 = Patch[2].vPosition;
	// And Normals
	float3 f3N002 = Patch[0].vNormal;
	float3 f3N020 = Patch[1].vNormal;
	float3 f3N200 = Patch[2].vNormal;

	// Compute the cubic geometry control points		//입방체 기하 제어점 계산
	// Edge control points
	pt.f3B210 = ((2.0f * f3B003) + f3B030 - (dot((f3B030 - f3B003), f3N002) * f3N002)) / 3.0f;
	pt.f3B120 = ((2.0f * f3B030) + f3B003 - (dot((f3B003 - f3B030), f3N020) * f3N020)) / 3.0f;
	pt.f3B021 = ((2.0f * f3B030) + f3B300 - (dot((f3B300 - f3B030), f3N020) * f3N020)) / 3.0f;
	pt.f3B012 = ((2.0f * f3B300) + f3B030 - (dot((f3B030 - f3B300), f3N200) * f3N200)) / 3.0f;
	pt.f3B102 = ((2.0f * f3B300) + f3B003 - (dot((f3B003 - f3B300), f3N200) * f3N200)) / 3.0f;
	pt.f3B201 = ((2.0f * f3B003) + f3B300 - (dot((f3B300 - f3B003), f3N002) * f3N002)) / 3.0f;

	// Center control point
	float3 f3E = (pt.f3B210 + pt.f3B120 + pt.f3B021 + pt.f3B012 + pt.f3B102 + pt.f3B201) / 6.0f;
	float3 f3V = (f3B003 + f3B030 + f3B300) / 3.0f;
	pt.f3B111 = f3E + ((f3E - f3V) / 2.0f);

	// Compute the quadratic normal control points, and rotate into world space
	float fV12 = 2.0f * dot(f3B030 - f3B003, f3N002 + f3N020) / dot(f3B030 - f3B003, f3B030 - f3B003);
	pt.f3N110 = normalize(f3N002 + f3N020 - fV12 * (f3B030 - f3B003));
	float fV23 = 2.0f * dot(f3B300 - f3B030, f3N020 + f3N200) / dot(f3B300 - f3B030, f3B300 - f3B030);
	pt.f3N011 = normalize(f3N020 + f3N200 - fV23 * (f3B300 - f3B030));
	float fV31 = 2.0f * dot(f3B003 - f3B300, f3N200 + f3N002) / dot(f3B003 - f3B300, f3B003 - f3B300);
	pt.f3N101 = normalize(f3N200 + f3N002 - fV31 * (f3B003 - f3B300));

	return pt;
}

struct HullOut
{
	float3	vPosition : POSITION;
	float3	vNormal : NORMAL;
	float2	vTexUV : TEXCOORD0;
};


[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(9.0f)]

HullOut HS_MAIN(InputPatch<VS_OUT_TESS, 3> p,		//삼각형은 3개
	uint i : SV_OutputControlPointID,
	uint patchID : SV_PrimitiveID)
{
	HullOut		hout;

	hout.vPosition = p[i].vPosition;
	hout.vNormal = p[i].vNormal;
	hout.vTexUV = p[i].vTexUV;

	return hout;
}

struct DomainOut
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

[domain("tri")]
DomainOut DS_MAIN(PatchTess PatchTess, float3 uvw : SV_DomainLocation,
	const OutputPatch<HullOut, 3> tri)
{
	// The barycentric coordinates
	float fU = uvw.x;
	float fV = uvw.y;
	float fW = uvw.z;

	// Precompute squares and squares * 3 
	float fUU = fU * fU;
	float fVV = fV * fV;
	float fWW = fW * fW;
	float fUU3 = fUU * 3.0f;
	float fVV3 = fVV * 3.0f;
	float fWW3 = fWW * 3.0f;

	DomainOut		Out = (DomainOut)0;

	float3 p = tri[0].vPosition * fWW * fW +
		tri[1].vPosition * fUU * fU +
		tri[2].vPosition * fVV * fV +
		PatchTess.f3B210 * fWW3 * fU +
		PatchTess.f3B120 * fW * fUU3 +
		PatchTess.f3B201 * fWW3 * fV +
		PatchTess.f3B021 * fUU3 * fV +
		PatchTess.f3B102 * fW * fVV3 +
		PatchTess.f3B012 * fU * fVV3 +
		PatchTess.f3B111 * 6.0f * fW * fU * fV;

	// Compute normal from quadratic control points and barycentric coords
	float3 n = tri[0].vNormal * fWW +
		tri[1].vNormal * fUU +
		tri[2].vNormal * fVV +
		PatchTess.f3N110 * fW * fU +
		PatchTess.f3N011 * fU * fV +
		PatchTess.f3N101 * fW * fV;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(p, 1.f), matWVP);
	Out.vTexUV = tri[0].vTexUV * fW + tri[1].vTexUV * fU + tri[2].vTexUV * fV;
	Out.vNormal = normalize(mul(vector(n, 0.f), g_WorldMatrix));
	Out.vWorldPos = (vector)0.f;
	Out.vProjPos = (vector)0.f;

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
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vSourDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV * 30.f);
	//vector		vDestDiffuse = g_DiffuseTexture[1].Sample(LinearSampler, In.vTexUV * 30.f);
	//vector		vFilter = g_FilterTexture.Sample(LinearSampler, In.vTexUV);

	//vector		vBrush = (vector)0.f;

	/*if (g_vBrushPos.x - g_fBrushRange <= In.vWorldPos.x && In.vWorldPos.x < g_vBrushPos.x + g_fBrushRange &&
		g_vBrushPos.z - g_fBrushRange <= In.vWorldPos.z && In.vWorldPos.z < g_vBrushPos.z + g_fBrushRange)
	{
		float2		vUV;

		vUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);
		vUV.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);

		vBrush = g_BrushTexture.Sample(LinearSampler, vUV);
	}*/

	/*Out.vDiffuse = vSourDiffuse * vFilter.r +
		vDestDiffuse * (1.f - vFilter.r) + vBrush;*/

	Out.vDiffuse = vSourDiffuse;
	Out.vDiffuse.a = 1.f;

	/* -1 ~ 1 => 0 ~ 1 */
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	//x, y, z * 투영행렬. 
	//z == near -> 0
	//z == far -> far

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.f, 0.f);

	return Out;
}

technique11 DefaultTechnique
{
	pass DefaultPass
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

	pass TerrainTess
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_TESS();
		GeometryShader = NULL;
		HullShader = compile hs_5_0 HS_MAIN();
		DomainShader = compile ds_5_0 DS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

}