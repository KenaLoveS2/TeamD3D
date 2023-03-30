
#include "Shader_Client_Defines.h"

/**********Constant Buffer*********/
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_DepthTexture;
vector			g_vCamPosition;
float			g_fHDRValue;
/**********************************/

/**********Trail Option_1*********/
bool	g_IsTrail, g_IsUseMask;
int		g_TextureRenderType, g_BlendType;
int		g_SeparateWidth, g_SeparateHeight;
uint	g_iTotalDTextureComCnt, g_iTotalMTextureComCnt;
float   g_WidthFrame, g_HeightFrame;
float4  g_vColor;
/**********************************/
/********** Trail Option_2*********/
float   g_InfoSize;
float	g_fWidth = 1.f;
float	g_fLife;
bool    g_bDistanceAlpha;
float   g_fAlpha;
float   g_Time;
/*********************************/

/**********************************/
matrix    g_BossTrailInfoMatrix[300];
texture2D g_FlowTexture;
texture2D g_TypeTexture;
/**********************************/

struct VS_IN
{
	float3      vPosition   : POSITION;
	float2      vPSize     : PSIZE;
	row_major   float4x4    Matrix  : WORLD;
	uint        InstanceID  : SV_InstanceID;
};

struct VS_OUT
{
	float4      vPosition   : POSITION;
	float2      vPSize      : PSIZE;
	float       fLife : TEXCOORD0;
	float		fWidth : TEXCOORD1;
	row_major   float4x4    Matrix  : WORLD;
	uint        InstanceID  : SV_InstanceID;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT  Out = (VS_OUT)0;

	float4x4    Matrix = In.Matrix;
	Out.fLife = In.Matrix[3][3];
	Out.fWidth = In.Matrix[2][3];

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
	float4      vPosition   : POSITION;
	float2      vPSize      : PSIZE;
	float       fLife : TEXCOORD0;
	float		fWidth : TEXCOORD1;
	row_major   float4x4    Matrix  : WORLD;
	uint        InstanceID  : SV_InstanceID;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float		fLife : TEXCOORD1;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
	GS_OUT		Out[4] =
	{
		{ { 0.0f, 0.0f, 0.0f,0.0f },{ 0.0f,0.0f },{ 0.f } },
		{ { 0.0f, 0.0f, 0.0f,0.0f },{ 0.0f,0.0f },{ 0.f } },
		{ { 0.0f, 0.0f, 0.0f,0.0f },{ 0.0f,0.0f },{ 0.f } },
		{ { 0.0f, 0.0f, 0.0f,0.0f },{ 0.0f,0.0f },{ 0.f } }
	};

	matrix      matVP = mul(g_ViewMatrix, g_ProjMatrix);
	float       fCurWidth = g_fWidth * (In[0].fLife / g_fLife) * 0.5f;
	float4x4    WorldMatrix = In[0].Matrix;

	float3      vUp = matrix_up(WorldMatrix) * fCurWidth;
	float3		vLook = matrix_look(WorldMatrix);
	float3		vPosition = matrix_postion(In[0].Matrix);

	if (In[0].InstanceID == 0) 
	{
		float3  vResultPos;
		vResultPos = vPosition;
		Out[0].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[0].vTexUV = float2(0.f, 0.f);
		Out[0].fLife = In[0].fLife / g_fLife;

		vResultPos = vPosition;
		Out[1].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[1].vTexUV = float2(1.0f, 0.f);
		Out[1].fLife = In[0].fLife / g_fLife;

		vResultPos = vPosition;
		Out[2].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[2].vTexUV = float2(1.0f, 1.f);
		Out[2].fLife = In[0].fLife / g_fLife;

		vResultPos = vPosition;
		Out[3].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[3].vTexUV = float2(0.f, 1.f);
		Out[3].fLife = In[0].fLife / g_fLife;
	}
	else
	{
		matrix      PreWorldMatrix = g_BossTrailInfoMatrix[In[0].InstanceID - 1];
		matrix      matVP = mul(g_ViewMatrix, g_ProjMatrix);

		float3		vPrelook = matrix_look(PreWorldMatrix);
		float3		vPreUp = matrix_up(PreWorldMatrix)* fCurWidth;

		float3 vResultPos;
		vResultPos = matrix_postion(PreWorldMatrix);
		Out[0].vPosition = vector(vResultPos, 1.f);
		Out[0].vPosition = mul(Out[0].vPosition, matVP);
		Out[0].fLife = PreWorldMatrix[3][3] / g_fLife;

		vResultPos = vPosition;
		Out[1].vPosition = vector(vResultPos, 1.f);
		Out[1].vPosition = mul(Out[1].vPosition, matVP);
		Out[1].fLife = In[0].fLife / g_fLife;

		vResultPos = vPosition - vUp;
		Out[2].vPosition = vector(vResultPos, 1.f);
		Out[2].vPosition = mul(Out[2].vPosition, matVP);
		Out[2].fLife = In[0].fLife / g_fLife;

		vResultPos = matrix_postion(PreWorldMatrix) - vPreUp;
		Out[3].vPosition = vector(vResultPos, 1.f);
		Out[3].vPosition = mul(Out[3].vPosition, matVP);
		Out[3].fLife = PreWorldMatrix[3][3] / g_fLife;

		Out[0].vTexUV = float2(1.f - ((In[0].InstanceID - 1.f) / g_InfoSize), 0.f);
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
	float       fLife : TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector	 flow = g_FlowTexture.Sample(LinearSampler, In.vTexUV);
	vector   type = g_TypeTexture.Sample(LinearSampler, In.vTexUV);
	// R = 1, G = 0.2, B = 1, A = 1

	float4 finalcolor = flow + g_vColor;
	Out.vColor = finalcolor * type;
	Out.vColor.a = Out.vColor.r * In.fLife;
	Out.vColor.rgb = Out.vColor.rgb * 1.4f;
	return Out;
}

//PS_SHAMAN_TRAIL
PS_OUT PS_SHAMAN_TRAIL(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector	 Flow = g_FlowTexture.Sample(LinearSampler, In.vTexUV);
	vector   Type = g_TypeTexture.Sample(LinearSampler, In.vTexUV);
	// R = 1, G = 0.2, B = 1, A = 1
	vector   vTrailColor = vector(225.f, 236.f, 255.f, 255.f) / 255.f;

	float4 finalcolor = Flow ;
	finalcolor.rgb = finalcolor.rgb + vTrailColor.rgb;
	Out.vColor = finalcolor * Type;
	Out.vColor.a = Out.vColor.r * In.fLife;
	return Out;
}

technique11 DefaultTechnique
{
	pass Warrior_Trail // 0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Shaman_Trail // 1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SHAMAN_TRAIL();
	}
}
