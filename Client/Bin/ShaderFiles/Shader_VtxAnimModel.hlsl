
#include "Shader_Client_Defines.h"

/***********Constant Buffers***********/
matrix g_BoneMatrices[800];
matrix g_WorldMatrix;
matrix g_ViewMatrix;
matrix g_ProjMatrix;
float	   g_fFar = 300.f;
float4 g_vCamPosition;
/**************************************/

Texture2D<float4>		g_DiffuseTexture;
Texture2D<float4>		g_NormalTexture;

Texture2D<float4>		g_AO_R_MTexture;
Texture2D<float4>		g_EmissiveTexture;
Texture2D<float4>		g_EmissiveMaskTexture;
Texture2D<float4>		g_MaskTexture;
Texture2D<float4>		g_SSSMaskTexture;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;

	/* ���� �������� ���������� ����(�ִ� 4��)�� ����� �����. */
	uint4			vBlendIndex : BLENDINDEX;
	float4		vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float4		vTangent : TANGENT;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float3		vViewDir : TEXCOORD2;
	float3		vBinormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float4 worldPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vViewDir = normalize(worldPosition.xyz - g_vCamPosition.xyz);

	float			fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix			BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices[In.vBlendIndex.w] * fWeightW;

	vector			vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
	vector			vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
	vector			vTangent = mul(float4(In.vTangent, 0.f), BoneMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vTangent = normalize(mul(vTangent, g_WorldMatrix));
	Out.vProjPos = Out.vPosition;
	Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float4		vTangent : TANGENT;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float3		vViewDir : TEXCOORD2;
	float3		vBinormal : BINORMAL;
};

struct PS_OUT
{
	/*SV_TARGET0 : ��� ������ ������ �ȼ��̴�. AND 0��° ����Ÿ�ٿ� �׸������� �����̴�. */
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	
	return Out;
}

PS_OUT PS_MAIN_KENA_EYE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2		vTexUV = In.vTexUV;
	vTexUV.x -= 0.345f;
	vTexUV.y -= 0.38f;

	vector		vDiffuse = g_DiffuseTexture.Sample(EyeSampler, vTexUV * 3.4f);
	vector		vNormalDesc = g_NormalTexture.Sample(EyeSampler, vTexUV * 3.4f);

	if (0.1f > vDiffuse.a)
		discard;

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	
	return Out;
}

PS_OUT PS_MAIN_KENA_BODY(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse			  = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vEmissive		      = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc	  = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vAO_R_M			  = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);

	float			fAmbientOcclusion = vAO_R_M.r;
	float			fRoughness			  = vAO_R_M.g;
	float			fMetalic				  = vAO_R_M.b;

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	float3 N = normalize(vNormal);
	float3 V = normalize(-In.vViewDir);
	float3 R = reflect(-V, N);

	float3 F0 = float3(0.04, 0.04, 0.04);
	F0 = lerp(F0, vDiffuse.rgb, fMetalic);

	float3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, fRoughness);

	float3 kS = F;
	float3 kD = (float3)1.0 - kS;
	kD *= 1.0 - fMetalic;

	float		fEmissive = vEmissive.b * 10.f;

	// 따로 설정이 필요할듯
	//vDiffuse.rgb = (kD *	vDiffuse.rgb) * fAmbientOcclusion;

	if (0.1f > vDiffuse.a)
		discard;

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, fEmissive, 0.f);

	return Out;
}

PS_OUT PS_MAIN_KENA_MAINOUTFIT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);
	vector		vSSSMask = g_SSSMaskTexture.Sample(LinearSampler, In.vTexUV);
	vector		vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);
	vector		vAO_R_M = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	float			fAmbientOcclusion = vAO_R_M.r;
	float			fRoughness = vAO_R_M.g;
	float			fMetalic = vAO_R_M.b;

	float3 N = normalize(vNormal.xyz);
	float3 V = normalize(-In.vViewDir);
	float3 R = reflect(-V, N);

	float3 F0 = float3(0.04, 0.04, 0.04);
	F0 = lerp(F0, vDiffuse.rgb, fMetalic);

	float3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, fRoughness);

	float3 kS = F;
	float3 kD = (float3)1.0 - kS;
	kD *= 1.0 - fMetalic;

	float			fEmissive = vEmissive.r * 10.f;

	vDiffuse.rgb = (kD *	vDiffuse.rgb) * fAmbientOcclusion;
	//vDiffuse.a = vMask.r * vSSSMask.r;

	if (0.1f > vDiffuse.a)
		discard;

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, fEmissive, 0.f);

	return Out;
}

PS_OUT PS_MAIN_FACE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vAO_R_M = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);
	vector		vSSSMask = g_SSSMaskTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	float			fAmbientOcclusion = vAO_R_M.r;
	float			fRoughness = vAO_R_M.g;
	float			fMetalic = vAO_R_M.b;

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	float3 N = normalize(vNormal.xyz);
	float3 V = normalize(-In.vViewDir);
	float3 R = reflect(-V, N);

	float3 F0 = float3(0.04, 0.04, 0.04);
	F0 = lerp(F0, vDiffuse.rgb, fMetalic);

	float3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, fRoughness);

	float3 kS = F;
	float3 kD = (float3)1.0 - kS;
	kD *= 1.0 - fMetalic;

	vDiffuse.rgb = (kD *	vDiffuse.rgb) * fAmbientOcclusion;
	//vDiffuse.a = vSSSMask.r;

	if (0.1f > vDiffuse.a)
		discard;

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);

	return Out;
}

technique11 DefaultTechnique
{
	pass Default
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

	pass Kena_Eye
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_KENA_EYE();
	}

	pass Kena_Body
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_KENA_BODY();
	}

	pass Kena_MainOutFit
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_KENA_MAINOUTFIT();
	}

	pass Kena_Face
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FACE();
	}
}
