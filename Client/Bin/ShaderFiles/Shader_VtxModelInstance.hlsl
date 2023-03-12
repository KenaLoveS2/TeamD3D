#include "Shader_Client_Defines.h"

/**********Constant Buffer*********/
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_SocketMatrix;
float				g_fFar = 300.f;
float				g_fStonePulseIntensity = 0.f;
/**********************************/

Texture2D<float4>		g_DiffuseTexture;
Texture2D<float4>		g_NormalTexture;
Texture2D<float4>		g_EmissiveTexture;

Texture2D<float4>		g_HRAOTexture;
Texture2D<float4>		g_MRAOTexture;
Texture2D<float4>		g_ERAOTexture;

Texture2D<float4>		g_BlendDiffuseTexture;
Texture2D<float4>		g_DetailNormalTexture;
Texture2D<float4>		g_MaskTexture;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;

	float4		vRight : TEXCOORD1;
	float4		vUp : TEXCOORD2;
	float4		vLook : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;
};

struct VS_OUT_TESS
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vTangent : TANGENT;
	float3		vBinormal	: BINORMAL;
};

VS_OUT_TESS VS_MAIN_TESS(VS_IN In)
{
	VS_OUT_TESS		Out = (VS_OUT_TESS)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float4x4	Transform = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector		vPosition = mul(float4(In.vPosition, 1.f), Transform);
	vector		vNormal = mul(float4(In.vNormal, 0.f), Transform);

	Out.vPosition = vPosition.xyz;
	Out.vNormal = vNormal.xyz;
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), Transform));
	Out.vBinormal = normalize(cross(vNormal.xyz, Out.vTangent.xyz));

	//Out.vProjPos = Out.vPosition;
	//Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
	//Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));

	return Out;
}

struct PatchTess
{
	float EdgeTess[3] : SV_TessFactor;
	float InsideTess : SV_InsideTessFactor;

	// Geometry cubic generated control points
	float3 fOne    : POSITION3;
	float3 fTwo    : POSITION4;
	float3 fThree    : POSITION5;
	float3 fFour    : POSITION6;
	float3 fFive    : POSITION7;
	float3 fSix    : POSITION8;
	float3 fCenter    : CENTER;

	// Normal quadratic generated control points
	float3 vecNormal_1    : NORMAL3;
	float3 vecNormal_2    : NORMAL4;
	float3 vecNormal_3    : NORMAL5;
};

PatchTess ConstantHS(InputPatch<VS_OUT_TESS, 3> Patch, uint PatchID : SV_PrimitiveID)
{
	PatchTess		pt;

	pt.EdgeTess[0] = 4;
	pt.EdgeTess[1] = 4;
	pt.EdgeTess[2] = 4;

	pt.InsideTess = (pt.EdgeTess[0] + pt.EdgeTess[1] + pt.EdgeTess[2]) / 3.f;
	// Assign Positions
	float3 AssingPos_0 = Patch[0].vPosition;
	float3 AssingPos_1 = Patch[1].vPosition;
	float3 AssingPos_2 = Patch[2].vPosition;
	// And Normals
	float3 Assing_Normal_0 = Patch[0].vNormal;
	float3 Assing_Normal_1 = Patch[1].vNormal;
	float3 Assing_Normal_2 = Patch[2].vNormal;

	// Compute the cubic geometry control points
	// Edge control points
	pt.fOne = ((2.0f * AssingPos_0) + AssingPos_1 - (dot((AssingPos_1 - AssingPos_0), Assing_Normal_0) * Assing_Normal_0)) / 3.0f;
	pt.fTwo = ((2.0f * AssingPos_1) + AssingPos_0 - (dot((AssingPos_0 - AssingPos_1), Assing_Normal_1) * Assing_Normal_1)) / 3.0f;
	pt.fThree = ((2.0f * AssingPos_1) + AssingPos_2 - (dot((AssingPos_2 - AssingPos_1), Assing_Normal_1) * Assing_Normal_1)) / 3.0f;
	pt.fFour = ((2.0f * AssingPos_2) + AssingPos_1 - (dot((AssingPos_1 - AssingPos_2), Assing_Normal_2) * Assing_Normal_2)) / 3.0f;
	pt.fFive = ((2.0f * AssingPos_2) + AssingPos_0 - (dot((AssingPos_0 - AssingPos_2), Assing_Normal_2) * Assing_Normal_2)) / 3.0f;
	pt.fSix = ((2.0f * AssingPos_0) + AssingPos_2 - (dot((AssingPos_2 - AssingPos_0), Assing_Normal_0) * Assing_Normal_0)) / 3.0f;

	// Center control point
	float3 f3E = (pt.fOne + pt.fTwo + pt.fThree + pt.fFour + pt.fFive + pt.fSix) / 6.0f;
	float3 f3V = (AssingPos_0 + AssingPos_1 + AssingPos_2) / 3.0f;
	pt.fCenter = f3E + ((f3E - f3V) / 2.0f);

	// Compute the quadratic normal control points, and rotate into world space
	float fV12 = 2.0f * dot(AssingPos_1 - AssingPos_0, Assing_Normal_0 + Assing_Normal_1) / dot(AssingPos_1 - AssingPos_0, AssingPos_1 - AssingPos_0);
	pt.vecNormal_1 = normalize(Assing_Normal_0 + Assing_Normal_1 - fV12 * (AssingPos_1 - AssingPos_0));
	float fV23 = 2.0f * dot(AssingPos_2 - AssingPos_1, Assing_Normal_1 + Assing_Normal_2) / dot(AssingPos_2 - AssingPos_1, AssingPos_2 - AssingPos_1);
	pt.vecNormal_2 = normalize(Assing_Normal_1 + Assing_Normal_2 - fV23 * (AssingPos_2 - AssingPos_1));
	float fV31 = 2.0f * dot(AssingPos_0 - AssingPos_2, Assing_Normal_2 + Assing_Normal_0) / dot(AssingPos_0 - AssingPos_2, AssingPos_0 - AssingPos_2);
	pt.vecNormal_3 = normalize(Assing_Normal_2 + Assing_Normal_0 - fV31 * (AssingPos_0 - AssingPos_2));

	return pt;
}

struct HullOut
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
};

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(9.0f)]
HullOut HS_MAIN(InputPatch<VS_OUT_TESS, 3> p, uint i : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
	HullOut		hout;

	hout.vPosition = p[i].vPosition;
	hout.vNormal = p[i].vNormal;
	hout.vTexUV = p[i].vTexUV;

	hout.vProjPos = p[i].vProjPos;
	hout.vTangent = p[i].vTangent;
	hout.vBinormal = p[i].vBinormal;
		
	return hout;
}

struct DomainOut
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
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
		PatchTess.fOne * fWW3 * fU +
		PatchTess.fTwo * fW * fUU3 +
		PatchTess.fSix * fWW3 * fV +
		PatchTess.fThree * fUU3 * fV +
		PatchTess.fFive * fW * fVV3 +
		PatchTess.fFour * fU * fVV3 +
		PatchTess.fCenter * 6.0f * fW * fU * fV;

	// Compute normal from quadratic control points and barycentric coords
	float3 n = tri[0].vNormal * fWW +
		tri[1].vNormal * fUU +	
		tri[2].vNormal * fVV +
		PatchTess.vecNormal_1 * fW * fU +
		PatchTess.vecNormal_2 * fU * fV +
		PatchTess.vecNormal_3 * fW * fV;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	vector vProjPos = (tri[0].vProjPos + tri[1].vProjPos + tri[2].vProjPos) / 3.f;
	vector vTangent = (tri[0].vTangent + tri[1].vTangent + tri[2].vTangent) / 3.f;
	float3 vBinormal = (tri[0].vBinormal + tri[1].vBinormal + tri[2].vBinormal) / 3.f;

	Out.vPosition = mul(vector(p, 1.f), matWVP);
	Out.vTexUV = tri[0].vTexUV * fW + tri[1].vTexUV * fU + tri[2].vTexUV * fV;
	Out.vNormal = normalize(mul(vector(n, 0.f), g_WorldMatrix));
	Out.vProjPos = vProjPos;
	Out.vTangent = vTangent;
	Out.vBinormal = vBinormal.xyz;
	return Out;
}

struct PS_IN_TESS
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
};

struct PS_OUT_TESS
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
	float4		vAmbient : SV_TARGET3;
};

PS_OUT_TESS PS_MAIN_TESS(PS_IN_TESS In)
{
	PS_OUT_TESS		Out = (PS_OUT_TESS)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	/* ź��Ʈ�����̽� */
	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = (vector)1.f;
	return Out;
}//1

PS_OUT_TESS PS_MAIN_H_R_AO(PS_IN_TESS In)
{
	PS_OUT_TESS		Out = (PS_OUT_TESS)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vHRAODesc = g_HRAOTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));
	float4 vAORM = float4(vHRAODesc.b, vHRAODesc.g, vHRAODesc.r, vHRAODesc.a);

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = vAORM;
	return Out;
}//2

PS_OUT_TESS PS_MAIN_MRAO_E(PS_IN_TESS In)
{
	PS_OUT_TESS		Out = (PS_OUT_TESS)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vMRAODesc = g_MRAOTexture.Sample(LinearSampler, In.vTexUV);
	vector		vEmissiveDesc = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));
	float4 vAORM = float4(vMRAODesc.b, vMRAODesc.g, vMRAODesc.r, vMRAODesc.a);

	float4	FinalColor = vDiffuse + (vEmissiveDesc * g_fStonePulseIntensity);
	FinalColor.a = vDiffuse.a;
	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, length(vEmissiveDesc), 0.f);
	Out.vAmbient = vAORM;
	return Out;
}//3

PS_OUT_TESS PS_MAIN_HRAO_E(PS_IN_TESS In)
{
	PS_OUT_TESS		Out = (PS_OUT_TESS)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vHRAODesc = g_HRAOTexture.Sample(LinearSampler, In.vTexUV);
	vector		vEmissiveDesc = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));
	float4 vAORM = float4(vHRAODesc.b, vHRAODesc.g, vHRAODesc.r, vHRAODesc.a);

	float4	FinalColor = vDiffuse + (vEmissiveDesc * g_fStonePulseIntensity);
	FinalColor.a = vDiffuse.a;
	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, length(vEmissiveDesc), 0.f);
	Out.vAmbient = vAORM;
	return Out;
}//4

// EMPTY
PS_OUT_TESS PS_MAIN_ERAO(PS_IN_TESS In)
{
	PS_OUT_TESS		Out = (PS_OUT_TESS)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vERAODesc = g_ERAOTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));
	float4 vAORM = float4(vERAODesc.b, vERAODesc.g, vERAODesc.r, vERAODesc.a);

	float4	FinalColor = vDiffuse;
	FinalColor.a = vDiffuse.a;
	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = vAORM;
	return Out;
}//5

PS_OUT_TESS PS_MAIN_MRAO(PS_IN_TESS In)
{
	PS_OUT_TESS		Out = (PS_OUT_TESS)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vMRAODesc = g_MRAOTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));
	float4 vAORM = float4(vMRAODesc.b, vMRAODesc.g, vMRAODesc.r, vMRAODesc.a);

	float4	FinalColor = vDiffuse;
	FinalColor.a = vDiffuse.a;
	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = vAORM;
	return Out;
}//6

PS_OUT_TESS PS_MAIN_LEAF_MRAO(PS_IN_TESS In)
{
	PS_OUT_TESS		Out = (PS_OUT_TESS)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vMRAODesc = g_MRAOTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	if (vDiffuse.r == 1.f && vDiffuse.g == 1.f && vDiffuse.b == 1.f)
		discard;

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));
	float4 vAORM = float4(vMRAODesc.b, vMRAODesc.g, vMRAODesc.r, vMRAODesc.a);

	float4	FinalColor = vDiffuse;
	FinalColor.a = vDiffuse.a;
	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = vAORM;
	return Out;
}//7

PS_OUT_TESS PS_MAIN_RUBBLE(PS_IN_TESS In)
{
	PS_OUT_TESS		Out = (PS_OUT_TESS)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vHRAODesc = g_HRAOTexture.Sample(LinearSampler, In.vTexUV);
	vector		vBlendDiffuse = g_BlendDiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vMaskDesc = g_MaskTexture.Sample(LinearSampler, In.vTexUV);
	vector		vDetailNormalDesc = g_DetailNormalTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	float3		vDetailNormal = vDetailNormalDesc.xyz * 2.f - 1.f;
	vDetailNormal = normalize(mul(vDetailNormal, WorldMatrix));

	vNormal += vDetailNormal;
	vNormal = normalize(vNormal);
	
	float4 vAORM = float4(vHRAODesc.b, vHRAODesc.g, vHRAODesc.r, vHRAODesc.a);

	float4	FinalColor = vDiffuse * (1.f - vMaskDesc.r) + vBlendDiffuse * vMaskDesc.r;
	FinalColor.a = vDiffuse.a;

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = vAORM;
	return Out;
}//8

PS_OUT_TESS PS_MAIN_HRAO_DETAILNORMAL(PS_IN_TESS In)
{
	PS_OUT_TESS		Out = (PS_OUT_TESS)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vHRAODesc = g_HRAOTexture.Sample(LinearSampler, In.vTexUV);
	vector		vDetailNormalDesc = g_DetailNormalTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	float3		vDetailNormal = vDetailNormalDesc.xyz * 2.f - 1.f;
	vDetailNormal = normalize(mul(vDetailNormal, WorldMatrix));

	vNormal += vDetailNormal;
	vNormal = normalize(vNormal);

	float4 vAORM = float4(vHRAODesc.b, vHRAODesc.g, vHRAODesc.r, vHRAODesc.a);

	float4	FinalColor = vDiffuse;
	FinalColor.a = vDiffuse.a;

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = vAORM;
	return Out;
}//9

PS_OUT_TESS PS_MAIN_NOAO_DETAILNORMAL(PS_IN_TESS In)
{
	PS_OUT_TESS		Out = (PS_OUT_TESS)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vDetailNormalDesc = g_DetailNormalTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	float3		vDetailNormal = vDetailNormalDesc.xyz * 2.f - 1.f;
	vDetailNormal = normalize(mul(vDetailNormal, WorldMatrix));

	vNormal += vDetailNormal;
	vNormal = normalize(vNormal);

	float4 vAORM = (float4)1.f;

	float4	FinalColor = vDiffuse;
	FinalColor.a = vDiffuse.a;

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = vAORM;
	return Out;
}//10

PS_OUT_TESS PS_MAIN_NOAO_DN_BM(PS_IN_TESS In)
{
	PS_OUT_TESS		Out = (PS_OUT_TESS)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vDetailNormalDesc = g_DetailNormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vBlendDiffuse = g_BlendDiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vMaskDesc = g_MaskTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	float3		vDetailNormal = vDetailNormalDesc.xyz * 2.f - 1.f;
	vDetailNormal = normalize(mul(vDetailNormal, WorldMatrix));

	vNormal += vDetailNormal;
	vNormal = normalize(vNormal);

	float4 vAORM = (float4)1.f;

	float4	FinalColor = vDiffuse * (1.f - vMaskDesc.r) + vBlendDiffuse * vMaskDesc.r;
	FinalColor.a = vDiffuse.a;

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = vAORM;
	return Out;
}//11

technique11 DefaultTechnique
{
	pass MESH_INStancing
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_TESS();
		GeometryShader = NULL;
		HullShader = compile hs_5_0 HS_MAIN();
		DomainShader = compile ds_5_0 DS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_TESS();
	}//0

	pass MESH_INStancing_Lod//
	{
		SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_TESS();
		GeometryShader = NULL;
		HullShader = compile hs_5_0 HS_MAIN();
		DomainShader = compile ds_5_0 DS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_TESS();
	}//1

	pass Comp_H_R_AO
	{
		SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_TESS();
		GeometryShader = NULL;
		HullShader = compile hs_5_0 HS_MAIN();
		DomainShader = compile ds_5_0 DS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_H_R_AO();
	}//2

	pass Comp_M_R_AO_E
	{
		SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_TESS();
		GeometryShader = NULL;
		HullShader = compile hs_5_0 HS_MAIN();
		DomainShader = compile ds_5_0 DS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_MRAO_E();
	}//3

	pass Comp_H_R_AO_E
	{
		SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_TESS();
		GeometryShader = NULL;
		HullShader = compile hs_5_0 HS_MAIN();
		DomainShader = compile ds_5_0 DS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_HRAO_E();
	}//4

	 // EMPTY
	pass E_R_AO
	{
		SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_TESS();
		GeometryShader = NULL;
		HullShader = compile hs_5_0 HS_MAIN();
		DomainShader = compile ds_5_0 DS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_ERAO();
	}//5

	pass M_R_AO
	{
		SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_TESS();
		GeometryShader = NULL;
		HullShader = compile hs_5_0 HS_MAIN();
		DomainShader = compile ds_5_0 DS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_MRAO();
	}//6

	pass LEAF_M_R_AO
	{
		SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_TESS();
		GeometryShader = NULL;
		HullShader = compile hs_5_0 HS_MAIN();
		DomainShader = compile ds_5_0 DS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_LEAF_MRAO();
	}//7

	pass RUBBLE
	{
		SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_TESS();
		GeometryShader = NULL;
		HullShader = compile hs_5_0 HS_MAIN();
		DomainShader = compile ds_5_0 DS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_RUBBLE();
	}//8

	pass HRAO_DETAILNORMAL
	{
		SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_TESS();
		GeometryShader = NULL;
		HullShader = compile hs_5_0 HS_MAIN();
		DomainShader = compile ds_5_0 DS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_HRAO_DETAILNORMAL();
	}//9

	pass NOAO_DETAILNORMAL
	{
		SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_TESS();
		GeometryShader = NULL;
		HullShader = compile hs_5_0 HS_MAIN();
		DomainShader = compile ds_5_0 DS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_NOAO_DETAILNORMAL();
	}//10

	pass NOAO_DN_BM
	{
		SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_TESS();
		GeometryShader = NULL;
		HullShader = compile hs_5_0 HS_MAIN();
		DomainShader = compile ds_5_0 DS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_NOAO_DN_BM();
	}//10
}
