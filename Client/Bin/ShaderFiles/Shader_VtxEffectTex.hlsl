
#include "Shader_Client_Defines.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_DepthTexture;
texture2D		g_DiffuseTexture[5];
texture2D		g_MaskTexture[5];

// Type
int		g_TextureRenderType, g_BlendType;

int		g_SeparateWidth, g_SeparateHeight;
uint	g_iTotalDTextureComCnt, g_iTotalMTextureComCnt;

float   g_WidthFrame, g_HeightFrame;
float4  g_vColor;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	/*SV_TARGET0 : 모든 정보가 결정된 픽셀이다. AND 0번째 렌더타겟에 그리기위한 색상이다. */
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (g_iTotalDTextureComCnt == 1)
	{
		vector albedo = g_DiffuseTexture[0].Sample(LinearSampler, In.vTexUV);
		if (albedo.a < 0.1f)
			discard;
		Out.vColor = albedo;
	}
	if (g_iTotalDTextureComCnt == 2)
	{
		//float4 albedo = g_DiffuseTexture[0].Sample(LinearSampler, In.vTexUV);
		//float4 albedo1 = g_DiffuseTexture[1].Sample(LinearSampler, In.vTexUV);

		//float  FinalAlpha = (albedo.a * g_vColor.a);
		//float3 emissive = ((albedo.rgb * g_vColor.rgb) + albedo1.rgb)*FinalAlpha;
		//float3 finalColor = emissive;
		//Out.vColor = float4(finalColor, FinalAlpha);
		//float4 vmixcolor = mix(albedo, albedo1);
	}
	if (g_iTotalDTextureComCnt == 3)
	{
		vector albedo0 = g_DiffuseTexture[0].Sample(LinearSampler, In.vTexUV);
		vector albedo1 = g_DiffuseTexture[1].Sample(LinearSampler, In.vTexUV);
		vector albedo2 = g_DiffuseTexture[2].Sample(LinearSampler, In.vTexUV);

		Out.vColor = albedo0;
			//Out.vColor = albedo0 * albedo1* albedo2 + albedo0 * (1.f - albedo1 *albedo2);
	}

	/////////

	//if (g_iTotalMTextureComCnt == 1)
	//{
	//	vector vMaskTex = g_MaskTexture[0].Sample(LinearSampler, In.vTexUV);

	//	if (g_BlendType == 2)
	//		Out.vColor = Out.vColor * vMaskTex * g_vColor;
	//	else
	//		Out.vColor = (Out.vColor * vMaskTex) * g_vColor + (Out.vColor * vMaskTex) * (1.f - g_vColor);

	//}
	//else if (g_iTotalMTextureComCnt == 2)
	//{
	//	vector maskTex0 = g_MaskTexture[0].Sample(LinearSampler, In.vTexUV);
	//	vector maskTex1 = g_MaskTexture[1].Sample(LinearSampler, In.vTexUV);
	//	//	Out.vColor = Out.vColor * masktex * g_vColor;
	//	vector masktex = maskTex0 * maskTex1;

	//	//if (g_BlendType == 2)
	//	//	Out.vColor = Out.vColor;
	//	//else
	//	//	Out.vColor = (Out.vColor * masktex) * g_vColor + (Out.vColor * masktex) * (1.f - g_vColor);
	//}
	//else if (g_iTotalMTextureComCnt == 3)
	//{
	//	vector vMaskTex[3];
	//	vMaskTex[0] = g_MaskTexture[0].Sample(LinearSampler, In.vTexUV);
	//	vMaskTex[1] = g_MaskTexture[1].Sample(LinearSampler, In.vTexUV);
	//	vMaskTex[2] = g_MaskTexture[2].Sample(LinearSampler, In.vTexUV);

	//	vector masktex = vMaskTex[0] * vMaskTex[1] * vMaskTex[2];
	//	if (g_BlendType == 2)
	//		Out.vColor = Out.vColor * masktex * g_vColor;
	//	else
	//		Out.vColor = (Out.vColor * masktex) * g_vColor + (Out.vColor * masktex) * (1.f - g_vColor);
	//}


	////
	//if (g_TextureRenderType == 0) // Single
	//{
	//	float2		vTexUV;
	//	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	//	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	//	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, vTexUV);

	//	float		fOldViewZ = vDepthDesc.y * 300.f;
	//	float		fViewZ = In.vProjPos.w;

	//	Out.vColor.a = Out.vColor.a * (saturate(fOldViewZ - fViewZ) * 2.5f);
	//}
	//else // Sprite
	//{
	//	In.vTexUV.x = In.vTexUV.x * g_WidthFrame / g_SeparateWidth;
	//	In.vTexUV.y = In.vTexUV.y + g_HeightFrame / g_SeparateHeight;

	//	float2		vTexUV;
	//	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	//	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	//	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, vTexUV);

	//	float		fOldViewZ = vDepthDesc.y * 300.f;
	//	float		fViewZ = In.vProjPos.w;

	//	Out.vColor.a = Out.vColor.a * (saturate(fOldViewZ - fViewZ) * 2.5f);
	//}

	return Out;
}

technique11 DefaultTechnique
{
	pass Effect_Dafalut // 0
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
	pass Effect_Alpha // 1
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

	pass Effect_Black // 2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_One, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

}
