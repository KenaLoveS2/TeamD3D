#include "Shader_Client_Defines.h"

/**********Constant Buffer*********/
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_SocketMatrix;
float				g_fFar = 300.f;
float4			g_vCamPosition;
/**********************************/

Texture2D<float4>		g_DiffuseTexture;
Texture2D<float4>		g_NormalTexture;
Texture2D<float4>		g_MasterBlendDiffuseTexture;

Texture2D<float4> g_AO_R_MTexture;
Texture2D<float4> g_OpacityTexture;
Texture2D<float4> g_EmissiveTexture;

float4			g_EmissiveColor = (float4)1.f;
float				g_fHDRIntensity = 0.f;

texture2D		g_DissolveTexture;
bool				g_bDissolve;
float				g_fDissolveTime;
float				g_DissolveSpeed = 0.2f;
float				g_FadeSpeed = 1.5f;

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

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);
	
	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
	Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));

	return Out;
}

VS_OUT VS_MAIN_SOCKET(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	vector		vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_SocketMatrix);

	vector		vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
	vNormal = mul(vNormal, g_SocketMatrix);

	vector		vTangent = mul(float4(In.vTangent, 0.f), g_WorldMatrix);
	vTangent = mul(vTangent, g_SocketMatrix);

	Out.vPosition = mul(vPosition, matVP); 
	Out.vNormal = normalize(vNormal);
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(vTangent);
	Out.vBinormal = normalize(cross(vNormal.xyz, vTangent.xyz));

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
	/*SV_TARGET0 : ∏µÁ ¡§∫∏∞° ∞·¡§µ» «»ºø¿Ã¥Ÿ. AND 0π¯¬∞ ∑ª¥ı≈∏∞Ÿø° ±◊∏Æ±‚¿ß«— ªˆªÛ¿Ã¥Ÿ. */
	float4		vDiffuse  : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth   : SV_TARGET2;
	float4		vAmbient   : SV_TARGET3;			// ∏µ®_«¡∏Æ∫‰øÎ∑ª¥ı≈∏ƒœ
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	/* ≈∫¡®∆ÆΩ∫∆‰¿ÃΩ∫ */
	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = (float4)1.f;
	return Out;
}

//OPACITY
PS_OUT PS_MAIN_AO_R_M_O(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vAO_R_MDesc = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);
	vector		vOpacityDesc = g_OpacityTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float4 FinalColor = float4(0.f, 0.f, 0.f, 1.f);

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	float fOpacity = vOpacityDesc.r;

	FinalColor = float4(vDiffuse.rgb, fOpacity * vDiffuse.a);

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = vAO_R_MDesc;

	return Out;
}//4

PS_OUT PS_MAIN_DISSOLVE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	//vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	float4 albedo0 = g_MasterBlendDiffuseTexture.Sample(LinearSampler, In.vTexUV);
	float4 albedo1 = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	float4 vColor = albedo1;			//albedo0 * albedo1 * 2.0f;
	vColor = saturate(vColor);

	if (0.1f > vColor.a) 
		discard;

	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	
	/* ≈∫¡®∆ÆΩ∫∆‰¿ÃΩ∫ */
	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));
		
	if (g_bDissolve)
	{
		float fDissolveAmount = g_fDissolveTime * 5.f;

		// sample noise texture
		float noiseSample = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;

		float  _ColorThreshold1 = 1.0f;
		float4 _DissolveColor1 = float4(194.f, 0.0f, 0.0f, 1.0f) / 255.f;  //red

		float  _ColorThreshold2 = 0.4f;
		float4 _DissolveColor2 = float4(255.f, 163.f, 44.f, 1.0f) / 255.f; //orange

																		   // add edge colors0
		float thresh1 = fDissolveAmount * _ColorThreshold1;
		float useDissolve1 = noiseSample - thresh1 < 0;
		vColor = (1 - useDissolve1)* vColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		vColor = (1 - useDissolve2)* vColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount *g_DissolveSpeed * g_FadeSpeed;
		clip(noiseSample - threshold);
	}

	Out.vDiffuse = float4(vColor.rgb, 1.f);
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = (float4)1.f;
	return Out;
}

PS_OUT PS_MAIN_AO_R_M(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector      vAO_R_MDesc = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float4 FinalColor = float4(0.f, 0.f, 0.f, 1.f);

	float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	FinalColor = vDiffuse;

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = vAO_R_MDesc;

	return Out;
}

PS_OUT PS_MAIN_AO_R_M_E(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector      vAO_R_MDesc = g_AO_R_MTexture.Sample(LinearSampler, In.vTexUV);
	vector      vEmissiveDesc = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	float4 FinalColor = float4(0.f, 0.f, 0.f, 1.f);

	float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	FinalColor = vDiffuse + (vDiffuse * vEmissiveDesc * g_EmissiveColor);

	Out.vDiffuse = FinalColor;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, length(vEmissiveDesc) + g_fHDRIntensity, 0.f);
	Out.vAmbient = vAO_R_MDesc;

	return Out;
}//8

struct VS_IN_SHADOW
{
	float3	vPosition : POSITION;
};

struct VS_OUT_SHADOW
{
	float4	vPosition : SV_POSITION;
	float4	vProjPos  : TEXCOORD0;
};

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN_SHADOW In)
{
	VS_OUT_SHADOW		Out = (VS_OUT_SHADOW)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vProjPos = Out.vPosition;

	return Out;
}

VS_OUT_SHADOW VS_MAIN_SHADOW_WEAPON(VS_IN_SHADOW In)
{
	VS_OUT_SHADOW		Out = (VS_OUT_SHADOW)0;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);
	vector		vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_SocketMatrix);

	Out.vPosition = mul(vPosition, matVP);
	Out.vProjPos = Out.vPosition;
	return Out;
}

struct PS_IN_SHADOW
{
	float4			vPosition : SV_POSITION;
	float4			vProjPos : TEXCOORD0;
};

struct PS_OUT_SHADOW
{
	vector			vLightDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
	PS_OUT_SHADOW		Out = (PS_OUT_SHADOW)0;

	Out.vLightDepth.r = In.vProjPos.w / g_fFar;

	Out.vLightDepth.a = 1.f;

	return Out;
}

technique11 DefaultTechnique
{
	pass Default		//0
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

	pass Socket//1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_SOCKET();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Shadow//2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
	}

	pass WireFrame//3
	{
		SetRasterizerState(RS_Wireframe);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Weapon_Shadow //4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_SHADOW_WEAPON();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
	}

	pass Default_Dissolve		//5
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
	}

	pass Socket_AO_R_M_O//6
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_SOCKET();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_AO_R_M_O();
	}

	pass AO_R_M//7
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_AO_R_M();
	}

	pass AO_R_M_E//8
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_AO_R_M_E();
	}
}
