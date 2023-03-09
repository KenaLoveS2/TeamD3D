
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

/* Trail  */
bool	g_IsTrail;
matrix  g_InfoMatrix[300];
float   g_InfoSize;
float	g_fWidth = 1.f;
float	g_fLife;
bool    g_bDistanceAlpha;
float   g_fAlpha;

texture2D g_TrailflowTexture;
texture2D g_TrailTypeTexture;
float	  g_BeforeUV, g_AfterUV;

float2 g_UV;
float  g_Time;
/* ~Trail */

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

/* Trail */
struct VS_TRAILIN
{
	float3      vPosition   : POSITION;
	float2      vPSize     : PSIZE;
	row_major   float4x4    Matrix  : WORLD;
	uint        InstanceID  : SV_InstanceID;
};

struct VS_TRAILOUT
{
	float4      vPosition   : POSITION;
	float2      vPSize      : PSIZE;
	float       fLife : TEXCOORD0;
	row_major   float4x4    Matrix  : WORLD;
	uint        InstanceID  : SV_InstanceID;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	vector		vPosition = mul(float4(In.vPosition, 1.f), In.Matrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix).xyz;
	Out.vPSize = In.vPSize;

	return Out;
}

/* Trail */
VS_TRAILOUT VS_TRAILMAIN(VS_TRAILIN In)
{
	VS_TRAILOUT  Out = (VS_TRAILOUT)0;

	float4x4    Matrix = In.Matrix;
	Out.fLife = In.Matrix[3][3];

	Matrix[0][3] = 0.f;
	Matrix[1][3] = 0.f;
	Matrix[2][3] = 0.f;
	Matrix[3][3] = 1.f;

	Out.vPosition = float4(In.vPosition, 1.f);
	Out.vPSize = In.vPSize;
	Out.Matrix = Matrix;
	Out.InstanceID = In.InstanceID;

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

/* Trail */
struct GS_TRAILIN
{
	float4      vPosition   : POSITION;
	float2      vPSize      : PSIZE;
	float       fLife : TEXCOORD0;
	row_major   float4x4    Matrix  : WORLD;
	uint        InstanceID  : SV_InstanceID;
};

struct GS_TRAILOUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float		fLife : TEXCOORD1;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
	GS_OUT		Out[4];

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

[maxvertexcount(6)]
void GS_TRAILMAIN(point GS_TRAILIN In[1], inout TriangleStream<GS_TRAILOUT> Vertices)
{
	GS_TRAILOUT		Out[4] = 
	{
		{ { 0.0f, 0.0f, 0.0f,0.0f },{ 0.0f,0.0f },{ 0.f } },
		{ { 0.0f, 0.0f, 0.0f,0.0f },{ 0.0f,0.0f },{ 0.f } },
		{ { 0.0f, 0.0f, 0.0f,0.0f },{ 0.0f,0.0f },{ 0.f } },
		{ { 0.0f, 0.0f, 0.0f,0.0f },{ 0.0f,0.0f },{ 0.f } }
	};

	matrix  matVP = mul(g_ViewMatrix, g_ProjMatrix);

	float   fCurWidth = g_fWidth * (In[0].fLife / g_fLife) * 0.5f;
	float3  vUp = normalize(matrix_up(In[0].Matrix));
	float4x4    WorldMatrix = In[0].Matrix;
	vector		vPosition = vector(matrix_postion(In[0].Matrix), 1.f);
	vector      vResultPos;

	if (In[0].InstanceID == 0) // 인스턴스아이디 50전까지 빌보드를 먹임(왜냐면 지금 출력사이즈를 작게했기 때문임) => Test
	{
		vResultPos = vPosition + float4(vUp, 0.f) * fCurWidth;
		Out[0].vPosition = mul(vResultPos, matVP);
		Out[0].fLife = In[0].fLife / g_fLife;

		vResultPos = vPosition + float4(vUp, 0.f) * fCurWidth;
		Out[1].vPosition = mul(vResultPos, matVP);
		Out[1].fLife = In[0].fLife / g_fLife;

		vResultPos = vPosition - float4(vUp, 0.f) * fCurWidth;
		Out[2].vPosition = mul(vResultPos, matVP);
		Out[2].fLife = In[0].fLife / g_fLife;

		vResultPos = vPosition - float4(vUp, 0.f) * fCurWidth;
		Out[3].vPosition = mul(vResultPos, matVP);
		Out[3].fLife = In[0].fLife / g_fLife;

		Vertices.Append(Out[0]);
		Vertices.Append(Out[1]);
		Vertices.Append(Out[2]);
		Vertices.RestartStrip();

		Vertices.Append(Out[0]);
		Vertices.Append(Out[2]);
		Vertices.Append(Out[3]);
		Vertices.RestartStrip();
	}
	else
	{
		float3 vPreU = matrix_look(g_InfoMatrix[In[0].InstanceID - 1]);
		float3 vU = matrix_look(In[0].Matrix);

		vResultPos = float4(matrix_postion(g_InfoMatrix[In[0].InstanceID - 1]), 1.f) + float4(vUp, 0.f) * fCurWidth ;
		Out[0].vPosition = mul(vResultPos, matVP);
		Out[0].fLife = In[0].fLife / g_fLife;

		vResultPos = vPosition + float4(vUp, 0.f) * fCurWidth;
		Out[1].vPosition = mul(vResultPos, matVP);
		Out[1].fLife = In[0].fLife / g_fLife;

		vResultPos = vPosition - float4(vUp, 0.f) * fCurWidth;
		Out[2].vPosition = mul(vResultPos, matVP);
		Out[2].fLife = In[0].fLife / g_fLife;

		vResultPos = float4(matrix_postion(g_InfoMatrix[In[0].InstanceID - 1]), 1.f) - float4(vUp, 0.f) * fCurWidth;
		Out[3].vPosition = mul(vResultPos, matVP);
		Out[3].fLife = In[0].fLife / g_fLife;

		Out[0].vTexUV = float2(1.f - ((In[0].InstanceID - 1.f )/ g_InfoSize), 0.f);
		Out[1].vTexUV = float2(1.f - In[0].InstanceID / g_InfoSize, 0.f);
		Out[2].vTexUV = float2(1.f - In[0].InstanceID / g_InfoSize, 1.f);
		Out[3].vTexUV = float2(1.f - ((In[0].InstanceID - 1.f) / g_InfoSize), 1.f);

		Vertices.Append(Out[0]);
		Vertices.Append(Out[1]);
		Vertices.Append(Out[2]);
		Vertices.RestartStrip();

		Vertices.Append(Out[0]);
		Vertices.Append(Out[2]);
		Vertices.Append(Out[3]);
		Vertices.RestartStrip();
	}
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_TRAILIN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float       fLife : TEXCOORD1;
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
	Out.vColor = Out.vColor * g_vColor;
	Out.vColor.a = Out.vColor.r;
	Out.vColor = saturate(Out.vColor.r* g_vColor) * 4.5f;
	Out.vColor.rgb = Out.vColor.rgb * 4.f;

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

PS_OUT PS_TRAILMAIN(PS_TRAILIN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector   type = g_TrailTypeTexture.Sample(LinearSampler, In.vTexUV);
	vector	 flow = g_TrailflowTexture.Sample(LinearSampler, In.vTexUV);

	float    fAlpha = 1.f - (abs(0.5f - In.vTexUV.y) * 2.f);

	float4 vColor = vector(92.0f, 141.f, 226.f, 255.f) / 255.f;
	//float4 vColor = vector(0.0f, 195.f, 255.f, 255.f) / 255.f;
	Out.vColor = flow + vColor;
	Out.vColor.a = Out.vColor.r * In.fLife;
	Out.vColor.rgb = Out.vColor.rgb * 1.4f;
	
	if (g_bDistanceAlpha == true)
		Out.vColor.a = Out.vColor.a * fAlpha;

	return Out;
}

technique11 DefaultTechnique
{
	pass Effect_Dafalut // 0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Effect_Alpha // 1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_One, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

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
		SetBlendState(BS_One, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

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
		SetBlendState(BS_HDR, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	/* Trail Pass */
	pass Trail_Dafalut // 4
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_TRAILMAIN();
		GeometryShader = compile gs_5_0 GS_TRAILMAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_TRAILMAIN();
	}
}
