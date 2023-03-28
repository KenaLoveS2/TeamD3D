
#include "Shader_Client_Defines.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector			g_vCamPosition;
//texture2D		g_DepthTexture;
texture2D		g_DiffuseTexture;
texture2D		g_MaskTexture;
texture2D		g_DissolveTexture;

float4			g_vColor = { 1.f, 1.f, 1.f, 1.f };
float4			g_vMaskColor = { 1.f ,1.f, 1.f, 1.f };
float4			g_vDissolveColor = { 1.f, 1.f, 1.f, 1.f };

float			g_fDissolveAlpha = 0.f;
float			g_fHDRItensity;

float			g_fCutY;			 /* For. discard Y Range option */

float			g_fUVSpeedX = 0.f;
float			g_fUVSpeedY = 0.f;


struct VS_IN
{
	float3		vPosition	: POSITION;
	float3		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
	float3		vTangent	: TANGENT;
};

struct VS_OUT
{
	float4		vPosition		: SV_POSITION;
	float4		vNormal			: NORMAL;
	float2		vTexUV			: TEXCOORD0;
	float4		vProjPos		: TEXCOORD1;
	float3		vWorldNormal	: TEXCOORD2;
	float3		vViewDir		: TEXCOORD3;
	float4		vWorldPosition	: TEXCOORD4;
	float4		vTangent		: TANGENT;
	float3		vBinormal		: BINORMAL;
	float4		vInPosition		: TEXCOORD5;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;
	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vInPosition = vector(In.vPosition, 1.f);
	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
	Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));

	Out.vWorldNormal = normalize(mul(In.vNormal, (float3x3)g_WorldMatrix));
	Out.vWorldPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vViewDir = normalize(Out.vWorldPosition.xyz - g_vCamPosition.xyz);

	return Out;
}

struct PS_IN
{
	float4		vPosition		: SV_POSITION;
	float4		vNormal			: NORMAL;
	float2		vTexUV			: TEXCOORD0;
	float4		vProjPos		: TEXCOORD1;
	float3      vWorldNormal	: TEXCOORD2;
	float3      vViewDir		: TEXCOORD3;
	float4      vWorldPosition	: TEXCOORD4;
	float4		vTangent		: TANGENT;
	float3		vBinormal		: BINORMAL;
	float4		vInPosition		: TEXCOORD5;
};

struct PS_OUT
{
	float4		vDiffuse		: SV_TARGET0;
	float4		vNormal			: SV_TARGET1;
	float4		vDepth			: SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vNormal.rgb = In.vNormal.rgb;
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.0f, 0.0f);

	Out.vDiffuse *= g_vColor;
	Out.vDiffuse.rgb *= g_fHDRItensity;

	if (In.vInPosition.y > g_fCutY)
		discard;

	return Out;
}

PS_OUT PS_MAIN_ONLYCOLOR(PS_IN In)
{

	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse = g_fHDRItensity * g_vColor;
	Out.vNormal.rgb = In.vNormal.rgb;
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.0f, 0.0f);

	if (In.vInPosition.y > g_fCutY)
		discard;

	return Out;
}

PS_OUT PS_MAIN_MASK(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4	vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	float4	vMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);
	vMask *= g_vMaskColor;

	Out.vDiffuse = vDiffuse;
	Out.vDiffuse.a = vMask.r;

	Out.vDiffuse *= g_vColor;
	Out.vDiffuse.rgb *= g_fHDRItensity;

	if (In.vInPosition.y > g_fCutY)
		discard;

	return Out;
}

PS_OUT PS_MAIN_DISSOLVE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4	vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	float4	vMask = g_DissolveTexture.Sample(LinearSampler, In.vTexUV);
	vMask *= g_vMaskColor;

	Out.vDiffuse = vDiffuse;
	Out.vDiffuse.a = vMask.r;

	Out.vDiffuse *= g_vColor;
	Out.vDiffuse.rgb *= g_fHDRItensity;

	if (In.vInPosition.y > g_fCutY)
		discard;

	if (Out.vDiffuse.a < g_fDissolveAlpha)
		discard;

	return Out;
}

PS_OUT PS_MAIN_HunterString(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2	texUV		= In.vTexUV;
	texUV.x += g_fUVSpeedX;
	texUV.y += g_fUVSpeedY;
	float4	vDiffuse	= g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	float4	vMask		= g_MaskTexture.Sample(LinearSampler, texUV);

	float	vDiffuseR = vDiffuse.r;
	vMask *= g_vMaskColor;

	Out.vDiffuse = vDiffuse;
	Out.vDiffuse.a = vMask.r;

	Out.vDiffuse *= g_vColor;
	Out.vDiffuse.rgb *= g_fHDRItensity;

	if (In.vInPosition.y > g_fCutY)
		discard;

	if (vDiffuseR < g_fDissolveAlpha)
		discard;

	return Out;
}

technique11 DefaultTechnique
{
	pass Default //0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass OnlyColor // 1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ONLYCOLOR();
	}

	pass DefaultMask // 2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MASK();
	}

	pass Dissolve // 3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
	}

	pass HunterString // 4
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HunterString();
	}
}