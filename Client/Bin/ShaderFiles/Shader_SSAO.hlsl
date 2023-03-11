#include "Shader_Engine_Defines.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_ProjMatrixInv, g_ViewMatrixInv;

Texture2D<float4>		g_NormalTexture;
Texture2D<float4>		g_DepthTexture;

#define TAP_SIZE 0.02
#define NUM_TAPS 16
#define THRESHOLD 0.1
#define SCALE 1.0

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

static const float3 taps[16] = { float3(-0.364452, -0.014985, -0.513535),
												float3(0.004669, -0.445692, -0.165899),
												float3(0.607166, -0.571184, 0.377880),
												float3(-0.607685, -0.352123, -0.663045),
												float3(-0.235328, -0.142338, 0.925718),
												float3(-0.023743, -0.297281, -0.392438),
												float3(0.918790, 0.056215, 0.092624),
												float3(0.608966, -0.385235, -0.108280),
												float3(-0.802881, 0.225105, 0.361339),
												float3(-0.070376, 0.303049, -0.905118),
												float3(-0.503922, -0.475265, 0.177892),
												float3(0.035096, -0.367809, -0.475295),
												float3(-0.316874, -0.374981, -0.345988),
												float3(-0.567278, -0.297800, -0.271889),
												float3(-0.123325, 0.197851, 0.626759),
												float3(0.852626, -0.061007, -0.144475) };

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float start_Z = g_DepthTexture.Sample(LinearSampler, In.vTexUV).r;
	float start_Y = 1.f - In.vTexUV.y;
	float3 start_Pos = float3(In.vTexUV.x, start_Y, start_Z);
	float3 ndc_Pos = (2.0 * start_Pos) - 1.0;
	float4 unproject = mul(float4(ndc_Pos.x, ndc_Pos.y, ndc_Pos.z, 1.0), g_ProjMatrixInv);
	float3 viewPos = unproject.xyz / unproject.w;
	float3 viewNorm = g_NormalTexture.Sample(LinearSampler, In.vTexUV).xyz;

	float total = 0.0;

	for (uint i = 0; i < NUM_TAPS; i++) {
		float3 offset = TAP_SIZE * taps[i];
		float2 offTex = In.vTexUV + float2(offset.x, -offset.y);

		float off_start_Z = g_DepthTexture.Sample(LinearSampler, offTex).r;
		float3 off_start_Pos = float3(offTex.x, start_Y + offset.y, off_start_Z);
		float3 off_ndc_Pos = (2.0f * off_start_Pos) - 1.0f;
		float4 off_unproject = mul(float4(off_ndc_Pos.x, off_ndc_Pos.y, off_ndc_Pos.z, 1.0f), g_ProjMatrixInv);
		float3 off_viewPos = off_unproject.xyz / off_unproject.w;

		float3 diff = off_viewPos.xyz - viewPos.xyz;
		float distance = length(diff);
		float3 diffnorm = normalize(diff);
		float dotx = diffnorm.x * viewNorm.x;

		float occlusion = max(0.0, dot(viewNorm, diffnorm));// * SCALE / (1.0 + distance);
		total += (1.0 - occlusion);
	}

	total /= NUM_TAPS;

	Out.vColor = float4(total, total, total, 1.0);

	return Out;
}

technique11 DefaultTechnique
{
	pass SSAO
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
