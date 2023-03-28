
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
bool	  g_IsTrail;

matrix    g_WispInfoMatrix[300];
texture2D g_WispflowTexture;
texture2D g_WispOutTexture;

matrix    g_KenaInfoMatrix[300];
texture2D g_FlowTexture;
texture2D g_TypeTexture;

matrix    g_RotInfoMatrix[300];
matrix    g_InfoMatrix[100];

float   g_InfoSize;
float	g_fWidth = 1.f;
float	g_fLife;
bool    g_bDistanceAlpha;
float   g_fAlpha;

float2  g_UV;
float   g_Time;
/* ~Trail */

/* Dissolve */
bool  g_bDissolve;
float g_fDissolveTime;
float _DissolveSpeed = 0.2f;
float _FadeSpeed = 1.5f;
/* Dissolve */

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;

	row_major float4x4	Matrix : WORLD;
};

struct VS_OUT
{
	float3		vPosition : POSITION;
	float3		vCenterPosition : TEXCOORD0;
	float3		vRightScale : TEXCOORD1;
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
	float		fWidth : TEXCOORD1;
	row_major   float4x4    Matrix  : WORLD;
	uint        InstanceID  : SV_InstanceID;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	vector		vPosition = mul(float4(In.vPosition, 1.f), In.Matrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix).xyz;
	Out.vCenterPosition = matrix_postion(g_WorldMatrix);
	Out.vRightScale = matrix_right(In.Matrix);
	Out.vPSize = In.vPSize;

	return Out;
}

/* Trail */
VS_TRAILOUT VS_TRAILMAIN(VS_TRAILIN In)
{
	VS_TRAILOUT  Out = (VS_TRAILOUT)0;

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
	float3		vPosition : POSITION;
	float3		vCenterPosition : TEXCOORD0;
	float3		vRightScale : TEXCOORD1;
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
	float		fWidth : TEXCOORD1;
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
	float3		vDir = normalize(In[0].vPosition - In[0].vCenterPosition);
	float3		vRight = normalize(cross(vDir, vLook)) * In[0].vPSize.x * 0.5f;
	float3		vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f * In[0].vRightScale.x;

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
void GS_DEFAULT(point GS_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
	GS_OUT		Out[4];

	float3		vLook = g_vCamPosition.xyz - In[0].vPosition;
	float3		vRight = normalize(cross(float3(0.0f, 1.0f, 0.0f), vLook)) * In[0].vPSize.x * 0.5f;
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
void GS_RECTTRAIL(point GS_TRAILIN In[1], inout TriangleStream<GS_TRAILOUT> Vertices)
{
	GS_TRAILOUT		Out[4];

	matrix      matVP = mul(g_ViewMatrix, g_ProjMatrix);
	float4x4    WorldMatrix = In[0].Matrix;

	float3      vUp = matrix_up(WorldMatrix) * In[0].vPSize.y * In[0].fWidth;
	float3		vRight = matrix_right(WorldMatrix)* In[0].vPSize.x * In[0].fWidth;
	float3		vPosition = matrix_postion(In[0].Matrix);

	float3 vResultPos;
	vResultPos = vPosition - vRight + vUp;
	Out[0].vPosition = mul(vector(vResultPos, 1.f), matVP);
	Out[0].vTexUV = float2(0.f, 0.f);
	Out[0].fLife = In[0].fLife / g_fLife;

	vResultPos = vPosition + vRight + vUp;
	Out[1].vPosition = mul(vector(vResultPos, 1.f), matVP);
	Out[1].vTexUV = float2(1.f, 0.f);
	Out[1].fLife = In[0].fLife / g_fLife;

	vResultPos = vPosition + vRight - vUp;
	Out[2].vPosition = mul(vector(vResultPos, 1.f), matVP);
	Out[2].vTexUV = float2(1.f, 1.f);
	Out[2].fLife = In[0].fLife / g_fLife;

	vResultPos = vPosition - vRight - vUp;
	Out[3].vPosition = mul(vector(vResultPos, 1.f), matVP);
	Out[3].vTexUV = float2(0.f, 1.f);
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

[maxvertexcount(6)]
void GS_KENATRAIL(point GS_TRAILIN In[1], inout TriangleStream<GS_TRAILOUT> Vertices)
{
	GS_TRAILOUT		Out[4] = 
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

		vResultPos = vPosition + vUp;
		Out[2].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[2].vTexUV = float2(1.0f, 1.f);
		Out[2].fLife = In[0].fLife / g_fLife;

		vResultPos = vPosition - vUp;
		Out[3].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[3].vTexUV = float2(0.f, 1.f);
		Out[3].fLife = In[0].fLife / g_fLife;
	}
	else
	{
		matrix      PreWorldMatrix = g_KenaInfoMatrix[In[0].InstanceID - 1];
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

[maxvertexcount(6)]
void GS_WISPTRAIL(point GS_TRAILIN In[1], inout TriangleStream<GS_TRAILOUT> Vertices)
{
	GS_TRAILOUT		Out[4] =
	{
		{ { 0.0f, 0.0f, 0.0f,0.0f },{ 0.0f,0.0f },{ 0.f } },
		{ { 0.0f, 0.0f, 0.0f,0.0f },{ 0.0f,0.0f },{ 0.f } },
		{ { 0.0f, 0.0f, 0.0f,0.0f },{ 0.0f,0.0f },{ 0.f } },
		{ { 0.0f, 0.0f, 0.0f,0.0f },{ 0.0f,0.0f },{ 0.f } }
	};

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);
	float		fCurWidth = g_fWidth * (In[0].fLife / g_fLife) * 0.5f;
	float4x4    WorldMatrix = In[0].Matrix;
	float3      vUp = matrix_up(WorldMatrix)* fCurWidth;
	float3		vPosition = matrix_postion(WorldMatrix);

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

		vResultPos = vPosition + vUp;
		Out[2].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[2].vTexUV = float2(1.0f, 1.f);
		Out[2].fLife = In[0].fLife / g_fLife;

		vResultPos = vPosition - vUp;
		Out[3].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[3].vTexUV = float2(0.f, 1.f);
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
		matrix PreWorldMatrix = g_WispInfoMatrix[In[0].InstanceID - 1];
		matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

		float3 vPreRight = matrix_right(PreWorldMatrix) * fCurWidth;
		float3 vPrelook = matrix_look(PreWorldMatrix);
		float3 vPreUp = matrix_up(PreWorldMatrix)* fCurWidth;

		float3 vRight = matrix_right(WorldMatrix) * fCurWidth;
		float3 vlook = matrix_look(WorldMatrix);
		
		float3 vResultPos; 
		vResultPos = matrix_postion(PreWorldMatrix) + vPreUp;
		Out[0].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[0].fLife = (g_WispInfoMatrix[In[0].InstanceID - 1])[3][3] / g_fLife;

		vResultPos = matrix_postion(WorldMatrix) + vUp;
		Out[1].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[1].fLife = In[0].fLife / g_fLife;

		vResultPos = matrix_postion(WorldMatrix) - vUp;
		Out[2].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[2].fLife = In[0].fLife / g_fLife;

		vResultPos = matrix_postion(PreWorldMatrix) - vPreUp;
		Out[3].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[3].fLife = (g_WispInfoMatrix[In[0].InstanceID - 1])[3][3] / g_fLife;

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

[maxvertexcount(6)]
void GS_ROTTRAIL(point GS_TRAILIN In[1], inout TriangleStream<GS_TRAILOUT> Vertices)
{
	GS_TRAILOUT		Out[4] =
	{
		{ { 0.0f, 0.0f, 0.0f,0.0f },{ 0.0f,0.0f },{ 0.f } },
		{ { 0.0f, 0.0f, 0.0f,0.0f },{ 0.0f,0.0f },{ 0.f } },
		{ { 0.0f, 0.0f, 0.0f,0.0f },{ 0.0f,0.0f },{ 0.f } },
		{ { 0.0f, 0.0f, 0.0f,0.0f },{ 0.0f,0.0f },{ 0.f } }
	};

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);
	float		fCurWidth = g_fWidth * (In[0].fLife / g_fLife) * 0.5f;
	float4x4    WorldMatrix = In[0].Matrix;
	float3      vUp = matrix_up(WorldMatrix)* fCurWidth;
	float3		vPosition = matrix_postion(WorldMatrix);

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

		vResultPos = vPosition + vUp;
		Out[2].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[2].vTexUV = float2(1.0f, 1.f);
		Out[2].fLife = In[0].fLife / g_fLife;

		vResultPos = vPosition - vUp;
		Out[3].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[3].vTexUV = float2(0.f, 1.f);
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
		matrix PreWorldMatrix = g_RotInfoMatrix[In[0].InstanceID - 1];
		matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

		float3 vPreRight = matrix_right(PreWorldMatrix) * fCurWidth;
		float3 vPrelook = matrix_look(PreWorldMatrix);
		float3 vPreUp = matrix_up(PreWorldMatrix)* fCurWidth;

		float3 vRight = matrix_right(WorldMatrix) * fCurWidth;
		float3 vlook = matrix_look(WorldMatrix);

		float3 vResultPos;
		vResultPos = matrix_postion(PreWorldMatrix) + vPreUp;
		Out[0].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[0].fLife = (g_RotInfoMatrix[In[0].InstanceID - 1])[3][3] / g_fLife;

		vResultPos = matrix_postion(WorldMatrix) + vUp;
		Out[1].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[1].fLife = In[0].fLife / g_fLife;

		vResultPos = matrix_postion(WorldMatrix) - vUp;
		Out[2].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[2].fLife = In[0].fLife / g_fLife;

		vResultPos = matrix_postion(PreWorldMatrix) - vPreUp;
		Out[3].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[3].fLife = (g_RotInfoMatrix[In[0].InstanceID - 1])[3][3] / g_fLife;

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

[maxvertexcount(6)]
void GS_ROTBOMBTRAIL(point GS_TRAILIN In[1], inout TriangleStream<GS_TRAILOUT> Vertices)
{
	GS_TRAILOUT		Out[4] = 
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
		vResultPos = vPosition + vUp;
		Out[0].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[0].vTexUV = float2(0.f, 0.f);
		Out[0].fLife = In[0].fLife / g_fLife;

		vResultPos = vPosition + vUp;
		Out[1].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[1].vTexUV = float2(1.0f, 0.f);
		Out[1].fLife = In[0].fLife / g_fLife;

		vResultPos = vPosition - vUp;
		Out[2].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[2].vTexUV = float2(1.0f, 1.f);
		Out[2].fLife = In[0].fLife / g_fLife;

		vResultPos = vPosition - vUp;
		Out[3].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[3].vTexUV = float2(0.f, 1.f);
		Out[3].fLife = In[0].fLife / g_fLife;
	}
	else
	{
		matrix      PreWorldMatrix = g_KenaInfoMatrix[In[0].InstanceID - 1];
		matrix      matVP = mul(g_ViewMatrix, g_ProjMatrix);

		float3		vPrelook = matrix_look(PreWorldMatrix);
		float3		vPreUp = matrix_up(PreWorldMatrix)* fCurWidth;

		float3 vResultPos;
		vResultPos = matrix_postion(PreWorldMatrix) + vPreUp;
		Out[0].vPosition = vector(vResultPos, 1.f);
		Out[0].vPosition = mul(Out[0].vPosition, matVP);
		Out[0].fLife = PreWorldMatrix[3][3] / g_fLife;

		vResultPos = vPosition + vUp;
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

PS_OUT PS_DOT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (g_TextureRenderType == 1) // Sprite
	{
		In.vTexUV.x = In.vTexUV.x + g_WidthFrame;
		In.vTexUV.y = In.vTexUV.y + g_HeightFrame;

		In.vTexUV.x = In.vTexUV.x / g_SeparateWidth;
		In.vTexUV.y = In.vTexUV.y / g_SeparateHeight;
	}

	/* Diffuse */
	vector Diffuse = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	Diffuse.a = Diffuse.r;

	float4 finalcolor = Diffuse;
	if (finalcolor.a < 0.7f)
		discard;
	else
		finalcolor.rgb = finalcolor.rgb * 2.f;

	float fTIme = min(g_Time, 1.f);
	if (0.5f < fTIme)   // 내려가야함
		finalcolor.a = finalcolor.a * (1.f - fTIme);

	Out.vColor = finalcolor;
	return Out;
}

//PS_HIT
PS_OUT PS_HIT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	/* Diffuse */
	vector Diffuse = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	Diffuse.a = Diffuse.r;

	float4 vColor = float4(204.f, 210.f, 131.f, 255.f) / 255.f;
	Out.vColor = Diffuse * vColor;
	Out.vColor.rgb = Out.vColor.rgb * 2.5f;
	Out.vColor.a = Out.vColor.r;

	if (Out.vColor.a < 0.3f)
		discard;

	return Out;
}

// PS_HEAVYATTACK
PS_OUT PS_HEAVYATTACK(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	/* Sprite */
	if (g_TextureRenderType == 1)
	{
		In.vTexUV.x = In.vTexUV.x + g_WidthFrame;
		In.vTexUV.y = In.vTexUV.y + g_HeightFrame;

		In.vTexUV.x = In.vTexUV.x / g_SeparateWidth;
		In.vTexUV.y = In.vTexUV.y / g_SeparateHeight;
	}

	/* Diffuse */
	vector Diffuse = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	Diffuse.a = Diffuse.r;

	Out.vColor = Diffuse * g_vColor;
	Out.vColor.rgb = Out.vColor.rgb * 2.5f;
	Out.vColor.a = Out.vColor.r;

	if (Out.vColor.a < 0.5f)
		discard;

	return Out;
}

PS_OUT PS_TRAILMAIN(PS_TRAILIN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector   type = g_TypeTexture.Sample(LinearSampler, In.vTexUV);
	vector	 flow = g_FlowTexture.Sample(LinearSampler, In.vTexUV);

	float    fAlpha = 1.f - (abs(0.5f - In.vTexUV.y) * 2.f);

	float4 vColor = g_vColor;
	//float4 vColor = vector(0.0f, 195.f, 255.f, 255.f) / 255.f;
	Out.vColor = flow + vColor;
	Out.vColor.a = Out.vColor.r * In.fLife;
	Out.vColor.rgb = Out.vColor.rgb * 1.4f;
	
	if (g_bDistanceAlpha == true)
		Out.vColor.a = Out.vColor.a * fAlpha;

	return Out;
}
//BombTrail
PS_OUT PS_BOMBTRAIL(PS_TRAILIN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector   type = g_TypeTexture.Sample(LinearSampler, In.vTexUV);
	vector	 flow = g_FlowTexture.Sample(LinearSampler, In.vTexUV);
	flow.a = flow.r;

	float4 vColor = float4(206.f, 180.f, 225.f, 225.f) / 225.f;

	if (flow.r >= 0.5f)
		flow.rgb = vColor.rgb;
	else if(flow.r > 0.7f)
		flow.rgb = vColor.rgb*2.f;
	else
		discard;

	float  fAlpha = min(In.vTexUV.x, 1.f);

	float4 vRedColor = float4(242.f, 151.f, 53.f, 225.f) / 225.f;
	float  fRedAlpha = 1.f - (abs(0.5f - In.vTexUV.x) * 2.f);
	vRedColor.a = flow.a * fRedAlpha;
	flow.a = flow.a * fAlpha;

	Out.vColor = flow + fRedAlpha;
//	Out.vColor.a = Out.vColor.a * fAlpha;
	return Out;
}

PS_OUT PS_ENEMYWISP(PS_TRAILIN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float fLife = 2.0f;

	vector   vWispTrailTexture = g_WispflowTexture.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y /1.f));
	vWispTrailTexture.a = vWispTrailTexture.r * fLife;

	float4 finalcolor = vWispTrailTexture;
	float3 TrailColor = float3(255.f, 97.f, 0.f) / 255.f;

	if (vWispTrailTexture.a < 0.7f)
		finalcolor.rgb = finalcolor.rgb + TrailColor + float3(1.0f, 0.0, 0.0f) * 2.f;
	else
		finalcolor.rgb = finalcolor.rgb + TrailColor * 2.f;

	if (g_bDissolve)
	{
		float fDissolveAmount = g_fDissolveTime;

		// sample noise texture
		float noiseSample = vWispTrailTexture.r;

		float  _ColorThreshold = 1.0f;
		float4 _DissolveColor = float4(194.f, 0.0f, 0.0f, 1.0f) / 255.f;  //red

		// add edge colors
		float thresh = fDissolveAmount * _ColorThreshold;
		float useDissolve = noiseSample - thresh < 0;
		finalcolor = (1 - useDissolve)* finalcolor + useDissolve * _DissolveColor;

		// determine deletion threshold
		float threshold = fDissolveAmount *_DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}
	Out.vColor = finalcolor;

	return Out;
}

PS_OUT PS_ROT(PS_TRAILIN In)
{
   PS_OUT         Out = (PS_OUT)0;
   float fLife = 2.0f;

   vector   vRotrailTexture = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
   vRotrailTexture.a = vRotrailTexture.r * fLife;

   float4 finalcolor = vRotrailTexture;
   float4 vColor = vector(92.0f, 141.f, 226.f, 255.f) / 255.f;
   finalcolor.rgb = finalcolor.rgb + vColor.rgb * 2.f;

   Out.vColor = finalcolor;
   return Out;
}

PS_OUT PS_FLOWERPARTICLE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector	 flower = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	flower.a = flower.r;
	flower.rgb = flower.rgb + 1.f;

	float3 flowetcolor = float3(98.f, 98.f, 98.f) / 255.f;

	if (flower.a < 0.5f)
		flower.rgb = flower.rgb + g_vColor.rgb + float3(1.f, 0.0f, 0.0f);
	else
		flower.rgb = (float3)1.f;

	Out.vColor = flower;

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

//PS_FRONTVIEWBLINK
PS_OUT PS_FRONTVIEWBLINK(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (g_TextureRenderType == 1)
	{
		In.vTexUV.x = In.vTexUV.x + g_WidthFrame;
		In.vTexUV.y = In.vTexUV.y + g_HeightFrame;

		In.vTexUV.x = In.vTexUV.x / g_SeparateWidth;
		In.vTexUV.y = In.vTexUV.y / g_SeparateHeight;

	}

	vector	 vDiffuse = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	vDiffuse.a = vDiffuse.r;

	vDiffuse.rgb = vDiffuse.rgb * (float3)0.f + g_vColor.rgb;
	vDiffuse.a = vDiffuse.a * 0.4f;

	Out.vColor = vDiffuse * 1.5f;

	float fTime = min(g_Time, 2.f);

	if (1.f < fTime)   // 내려가야함
		Out.vColor.a = Out.vColor.a * (2.f - fTime);

	return Out;
}

//PS_SAPLING
PS_OUT PS_SAPLING(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector Diffuse = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	Diffuse.a = Diffuse.r;

	Out.vColor = Diffuse * g_vColor * 10.f;
	return Out;
}

PS_OUT PS_RECTTRAIL(PS_TRAILIN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vDiffuse = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	vDiffuse.a = vDiffuse.r;
	if (vDiffuse.a < 0.1f)
		discard;
	Out.vColor = vDiffuse + g_vColor;
	Out.vColor.a = Out.vColor.a * In.fLife;
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
	pass Effect_Dot // 1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DOT();
	}
	pass Effect_Black // 2
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
	pass Effect_HeavyAttack // 3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_HEAVYATTACK();
	}

	/* Trail Pass */
	pass Trail_Dafalut // 4
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_TRAILMAIN();
		GeometryShader = compile gs_5_0 GS_KENATRAIL();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_TRAILMAIN();
	}

	// PS_ENEMYWISP, 5
	pass Trail_EnemyWisp // 5
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_TRAILMAIN();
		GeometryShader = compile gs_5_0 GS_WISPTRAIL();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ENEMYWISP();
	}

	pass FlowerParticle // 6
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DEFAULT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FLOWERPARTICLE();
	}

	//GS_ROTTRAIL
	pass Trail_Rot // 7
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_TRAILMAIN();
		GeometryShader = compile gs_5_0 GS_ROTTRAIL();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ROT();
	}

	pass Kena_Hit // 8
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_HIT();
	}
	pass Sapling // 9
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SAPLING();
	}

	// GS_KENANOBILLTRAIL
	pass RotBombTrail // 10
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_TRAILMAIN();
		GeometryShader = compile gs_5_0 GS_ROTBOMBTRAIL();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BOMBTRAIL();
	}

	pass RectTrail // 11
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_TRAILMAIN();
		GeometryShader = compile gs_5_0 GS_RECTTRAIL();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_RECTTRAIL();
	}

	pass frontviewblink // 12
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DEFAULT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FRONTVIEWBLINK();
	}
}
