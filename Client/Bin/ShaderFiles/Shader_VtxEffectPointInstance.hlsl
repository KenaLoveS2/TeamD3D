
#include "Shader_Client_Defines.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector			g_vCamPosition;
texture2D		g_DepthTexture;
texture2D		g_DTexture_0, g_DTexture_1, g_DTexture_2, g_DTexture_3, g_DTexture_4;
texture2D		g_MTexture_0, g_MTexture_1, g_MTexture_2, g_MTexture_3, g_MTexture_4;

// Type
int		g_TextureRenderType, g_BlendType;

bool    g_IsUseMask;

int		g_SeparateWidth, g_SeparateHeight;
uint	g_iTotalDTextureComCnt, g_iTotalMTextureComCnt;

float   g_WidthFrame, g_HeightFrame;
float4  g_vColor;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;

	row_major float4x4	Matrix : WORLD;	
};

struct VS_OUT
{
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;
};

/* 추가적으로 정점을 생성할 수 있는 상황을 준비해준다. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	vector		vPosition = mul(float4(In.vPosition, 1.f), In.Matrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix).xyz;	
	Out.vPSize = In.vPSize;

	return Out;
}

struct GS_IN
{
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;
};


struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

[maxvertexcount(6)]
void GS_MAIN( point GS_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
	GS_OUT		Out[4];

	// 빌보드 수정해야함 
	float3		vLook = g_vCamPosition.xyz - In[0].vPosition;
	float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
	float3		vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	float3		vPosition;

	vPosition = In[0].vPosition + vRight + vUp;
	Out[0].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[0].vTexUV = float2(0.f, 0.f);

	vPosition = In[0].vPosition - vRight + vUp;
	Out[1].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[1].vTexUV = float2(1.f, 0.f);

	vPosition = In[0].vPosition - vRight - vUp;
	Out[2].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[2].vTexUV = float2(1.f, 1.f);

	vPosition = In[0].vPosition + vRight - vUp;
	Out[3].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[3].vTexUV = float2(0.f, 1.f);

	Vertices.Append(Out[0]);
	Vertices.Append(Out[1]);
	Vertices.Append(Out[2]);
	Vertices.RestartStrip();

	Vertices.Append(Out[0]);
	Vertices.Append(Out[2]);
	Vertices.Append(Out[3]);
	Vertices.RestartStrip();

}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (g_TextureRenderType == 1) // Sprite
	{
		In.vTexUV.x = In.vTexUV.x + g_WidthFrame;
		In.vTexUV.y = In.vTexUV.y + g_HeightFrame;

		In.vTexUV.x = In.vTexUV.x / g_SeparateWidth;
		In.vTexUV.y = In.vTexUV.y / g_SeparateHeight;
	}

	// DTexture
	if (g_iTotalDTextureComCnt == 1)
	{
		vector albedo = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
		if (albedo.a < 0.1f)
			discard;
		Out.vColor = albedo;
	}
	if (g_iTotalDTextureComCnt == 2)
	{
		float4 albedo0 = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
		float4 albedo1 = g_DTexture_1.Sample(LinearSampler, In.vTexUV);

		Out.vColor = albedo0 * albedo1 * 2.0f;
		Out.vColor = saturate(Out.vColor);
	}
	if (g_iTotalDTextureComCnt == 3)
	{
		vector albedo0 = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
		vector albedo1 = g_DTexture_1.Sample(LinearSampler, In.vTexUV);
		vector albedo2 = g_DTexture_2.Sample(LinearSampler, In.vTexUV);

		Out.vColor = albedo0 * albedo1 * albedo2 * 2.0f;
		Out.vColor = saturate(Out.vColor);
	}
	if (g_iTotalDTextureComCnt == 4)
	{
		vector albedo0 = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
		vector albedo1 = g_DTexture_1.Sample(LinearSampler, In.vTexUV);
		vector albedo2 = g_DTexture_2.Sample(LinearSampler, In.vTexUV);
		vector albedo3 = g_DTexture_3.Sample(LinearSampler, In.vTexUV);

		Out.vColor = albedo0 * albedo1 * albedo2 * albedo3* 2.0f;
		Out.vColor = saturate(Out.vColor);
	}
	if (g_iTotalDTextureComCnt == 5)
	{
		vector albedo0 = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
		vector albedo1 = g_DTexture_1.Sample(LinearSampler, In.vTexUV);
		vector albedo2 = g_DTexture_2.Sample(LinearSampler, In.vTexUV);
		vector albedo3 = g_DTexture_3.Sample(LinearSampler, In.vTexUV);
		vector albedo4 = g_DTexture_4.Sample(LinearSampler, In.vTexUV);

		Out.vColor = albedo0 * albedo1 * albedo2 * albedo3* albedo4*2.0f;
		Out.vColor = saturate(Out.vColor);
	}
	if (g_BlendType == 2 || g_BlendType == 3)
		Out.vColor = Out.vColor * g_vColor;
	else
		Out.vColor = (Out.vColor * g_vColor) + (Out.vColor * (1.f - g_vColor));

	// MTexture
	if (g_IsUseMask == true)
	{
		if (g_iTotalMTextureComCnt == 1)
		{
			vector maskTex0 = g_MTexture_0.Sample(LinearSampler, In.vTexUV);
			Out.vColor = Out.vColor * maskTex0* g_vColor;
		}
		if (g_iTotalMTextureComCnt == 2)
		{
			vector maskTex0 = g_MTexture_0.Sample(LinearSampler, In.vTexUV);
			vector maskTex1 = g_MTexture_1.Sample(LinearSampler, In.vTexUV);

			vector maskTex = saturate(maskTex0 * maskTex1* 2.0f);
			Out.vColor = Out.vColor * maskTex * g_vColor;
		}
		if (g_iTotalMTextureComCnt == 3)
		{
			vector maskTex0 = g_MTexture_0.Sample(LinearSampler, In.vTexUV);
			vector maskTex1 = g_MTexture_1.Sample(LinearSampler, In.vTexUV);
			vector maskTex2 = g_MTexture_2.Sample(LinearSampler, In.vTexUV);

			vector maskTex = saturate(maskTex0 * maskTex1* maskTex2* 2.0f);
			Out.vColor = Out.vColor * maskTex * g_vColor;
		}
		if (g_iTotalMTextureComCnt == 4)
		{
			vector maskTex0 = g_MTexture_0.Sample(LinearSampler, In.vTexUV);
			vector maskTex1 = g_MTexture_1.Sample(LinearSampler, In.vTexUV);
			vector maskTex2 = g_MTexture_2.Sample(LinearSampler, In.vTexUV);
			vector maskTex3 = g_MTexture_3.Sample(LinearSampler, In.vTexUV);

			vector maskTex = saturate(maskTex0 * maskTex1* maskTex2* maskTex3* 2.0f);
			Out.vColor = Out.vColor * maskTex * g_vColor;
		}
		if (g_iTotalMTextureComCnt == 5)
		{
			vector maskTex0 = g_MTexture_0.Sample(LinearSampler, In.vTexUV);
			vector maskTex1 = g_MTexture_1.Sample(LinearSampler, In.vTexUV);
			vector maskTex2 = g_MTexture_2.Sample(LinearSampler, In.vTexUV);
			vector maskTex3 = g_MTexture_3.Sample(LinearSampler, In.vTexUV);
			vector maskTex4 = g_MTexture_4.Sample(LinearSampler, In.vTexUV);

			vector maskTex = saturate(maskTex0 * maskTex1* maskTex2* maskTex3* maskTex4* 2.0f);
			Out.vColor = Out.vColor * maskTex * g_vColor;
		}
	}
	else
		Out.vColor = Out.vColor * g_vColor;

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
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Effect_Alpha // 1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Effect_Black // 2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Effect_Mix // 3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

}
