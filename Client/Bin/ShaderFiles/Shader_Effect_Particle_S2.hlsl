#include "Shader_Client_Defines.h"

vector			g_vCamPosition;
//texture2D		g_DepthTexture;
Texture2D<float4>		g_DiffuseTexture;

float4			g_vColor;
float			g_fAlpha;
float			g_fHDRItensity;

/* Option */
bool			g_IsSpriteAnim = false, g_IsUVAnim = false;

/* UV Animation */
float			g_fUVSpeedX = 0.f, g_fUVSpeedY = 0.f;

/* Sprite Animation */
int				g_XFrames = 1, g_YFrames = 1;
int				g_XFrameNow = 0, g_YFrameNow = 0;

struct VS_IN
{
	float3		vPosition		: POSITION;
	float2		vPSize			: PSIZE;

	row_major float4x4	Matrix	: WORLD;
};

struct VS_OUT
{
	float3		vPosition		: POSITION;
	float2		vPSize			: PSIZE;
	float		fLife			: TEXCOORD0;
	float3		vCenterPosition	: TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	float4x4	Matrix = In.Matrix;
	/* Option */
	Out.fLife = In.Matrix[3][3];

	/* Return to Original Matrix */
	/* It means... we can make 4 options.. */
	/* Matrix[3][3] is used for Life */

	Matrix[0][3] = 0.f;
	Matrix[1][3] = 0.f;
	Matrix[2][3] = 0.f;
	Matrix[3][3] = 1.f;

	vector		vPosition = mul(float4(In.vPosition, 1.f), Matrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix).xyz;
	Out.vPSize = In.vPSize;
	Out.vCenterPosition = matrix_postion(g_WorldMatrix);

	return Out;
}


struct GS_IN
{
	float3		vPosition		: POSITION;
	float2		vPSize			: PSIZE;
	float       fLife			: TEXCOORD0;
	float3		vCenterPosition	: TEXCOORD1;
};

struct GS_OUT
{
	float4		vPosition		: SV_POSITION;
	float2		vTexUV			: TEXCOORD0;
	float       fLife			: TEXCOORD1;
};

[maxvertexcount(6)]
void GS_MAIN_HAZE(point GS_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
	GS_OUT		Out[4];

	//float3		vLook	= In[0].fLife * (g_vCamPosition.xyz - In[0].vPosition);
	//float3		vRight	= In[0].fLife * (normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f);
	//float3		vUp		= In[0].fLife * (normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f);

	float3		vLook = In[0].fLife * (g_vCamPosition.xyz - In[0].vPosition);
	float3		vDir = normalize(In[0].vPosition - In[0].vCenterPosition);
	float3		vRight = In[0].fLife * (normalize(cross(vDir, vLook)) * In[0].vPSize.x * 0.5f);
	float3		vUp = In[0].fLife * (normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f);



	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	float3		vPosition;

	vPosition = In[0].vPosition + vRight + vUp;
	Out[0].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[0].vTexUV = float2(0.f, 0.f);
	Out[0].fLife = In[0].fLife;

	vPosition = In[0].vPosition - vRight + vUp;
	Out[1].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[1].vTexUV = float2(1.f, 0.f);
	Out[1].fLife = In[0].fLife;

	vPosition = In[0].vPosition - vRight - vUp;
	Out[2].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[2].vTexUV = float2(1.f, 1.f);
	Out[2].fLife = In[0].fLife;

	vPosition = In[0].vPosition + vRight - vUp;
	Out[3].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[3].vTexUV = float2(0.f, 1.f);
	Out[3].fLife = In[0].fLife;

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
void GS_MAIN_GATHER(point GS_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
	GS_OUT		Out[4];

	float		fLife = 1 - In[0].fLife;
	float3		vLook = fLife * (g_vCamPosition.xyz - In[0].vPosition);
	float3		vDir = normalize(In[0].vPosition - In[0].vCenterPosition);
	float3		vRight = fLife * (normalize(cross(vDir, vLook)) * In[0].vPSize.x * 0.5f);
	float3		vUp = fLife * (normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f);

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	float3		vPosition;

	vPosition = In[0].vPosition + vRight + vUp;
	Out[0].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[0].vTexUV = float2(0.f, 0.f);
	Out[0].fLife = fLife;

	vPosition = In[0].vPosition - vRight + vUp;
	Out[1].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[1].vTexUV = float2(1.f, 0.f);
	Out[1].fLife = In[0].fLife;

	vPosition = In[0].vPosition - vRight - vUp;
	Out[2].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[2].vTexUV = float2(1.f, 1.f);
	Out[2].fLife = In[0].fLife;

	vPosition = In[0].vPosition + vRight - vUp;
	Out[3].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[3].vTexUV = float2(0.f, 1.f);
	Out[3].fLife = In[0].fLife;

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
	float4		vPosition	: SV_POSITION;
	float2		vTexUV		: TEXCOORD0;
	float       fLife		: TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

/* Common Var */
// g_tex_0		: DiffuseTexture
// g_tex_1		: MaskTexture
// g_tex_2		: DissolveTexture

// g_float_0	: fHDRIntensity
// g_float_1	: fCutY
// g_float_2	: fDissolveAlpha

// g_bool_0		: isSpriteAnim
// g_bool_1		: isUVAnim

// g_float2_0	: vUVMove
// g_float2_1	: vUVScale

// g_int_0		: XFrames
// g_int_1		: YFrames
// g_int_2		: XFrameNow
// g_int_3		: YFrameNow

// g_float4_0	: DiffuseColor
// g_float4_1	: MaskColor
// g_float4_2	: DissolveColor

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (g_bool_0)
	{
		In.vTexUV.x = In.vTexUV.x + g_int_2;
		In.vTexUV.y = In.vTexUV.y + g_int_3;

		In.vTexUV.x = In.vTexUV.x / g_int_0;
		In.vTexUV.y = In.vTexUV.y / g_int_1;
	}

	Out.vColor = g_tex_0.Sample(PointSampler, In.vTexUV);

	Out.vColor *= g_float4_0;
	Out.vColor.a *= In.fLife;

	if (Out.vColor.a < 0.01f)
		discard;

	Out.vColor.rgb *= g_float_0;

	return Out;
}

PS_OUT PS_MAIN_BLACK(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (g_bool_0)
	{
		In.vTexUV.x = In.vTexUV.x + g_int_2;
		In.vTexUV.y = In.vTexUV.y + g_int_3;

		In.vTexUV.x = In.vTexUV.x / g_int_0;
		In.vTexUV.y = In.vTexUV.y / g_int_1;
	}

	Out.vColor = g_tex_0.Sample(PointSampler, In.vTexUV);

	Out.vColor.a = Out.vColor.r;

	Out.vColor *= g_float4_0;

	if (Out.vColor.a < 0.01f)
		discard;

	Out.vColor.rgb *= g_float_0;

	return Out;
}

PS_OUT PS_MAIN_SPREAD(PS_IN In)
{
	/* Ref : Shader_VtxEffectPointInstance : PS_FRONTVIEWBLINK */

	PS_OUT		Out = (PS_OUT)0;

	if (g_bool_0)
	{
		In.vTexUV.x = In.vTexUV.x + g_int_2;
		In.vTexUV.y = In.vTexUV.y + g_int_3;

		In.vTexUV.x = In.vTexUV.x / g_int_0;
		In.vTexUV.y = In.vTexUV.y / g_int_1;
	}

	vector vDiffuse = g_tex_0.Sample(PointSampler, In.vTexUV);

	Out.vColor = vDiffuse;
	Out.vColor.rgb *= g_float4_0.rgb;
	Out.vColor.a *= (1 - In.fLife);
	//vDiffuse.a = vDiffuse.r;

	//vDiffuse.rgb = g_vColor.rgb;
	//vDiffuse.a *= g_vColor.rgb;

	//if (Out.vColor.a == 0.0f)
	//	discard;

	//Out.vColor.rgb *= g_fHDRItensity;

	return Out;
}


/***********************************************/
/*					Trail					   */
/***********************************************/

struct VS_TRAILIN
{
	float3				vPosition	: POSITION;
	float2				vPSize		: PSIZE;
	row_major float4x4	Matrix		: WORLD;
	uint				InstanceID	: SV_InstanceID;
};

struct VS_TRAILOUT
{
	float4				vPosition	: POSITION;
	float2				vPSize		: PSIZE;
	float				fLife		: TEXCOORD0;
	row_major float4x4	Matrix		: WORLD;
	uint				InstanceID	: SV_InstanceID;
};

VS_TRAILOUT VS_TRAIL(VS_TRAILIN In)
{
	VS_TRAILOUT  Out = (VS_TRAILOUT)0;

	float4x4	Matrix = In.Matrix;

	/* Option */
	Out.fLife = In.Matrix[3][3];

	Matrix[0][3] = 0.f;
	Matrix[1][3] = 0.f;
	Matrix[2][3] = 0.f;
	Matrix[3][3] = 1.f;

	vector		vPosition = float4(In.vPosition, 1.f);	/* Already World Position.*/

	Out.vPosition	= float4(In.vPosition, 1.f);
	Out.InstanceID	= In.InstanceID;
	Out.vPSize		= In.vPSize;
	Out.Matrix		= Matrix;

	return Out;
}


// constant buffer
cbuffer Constants : register(b0)
{
	float3	vPositionP1;
	float3	vPositionP2;
}

float3 GetPos1() { return vPositionP1; }
float3 GetPos2() { return vPositionP2; }
void SetPos1(float3 x) { vPositionP1 = x; }
void SetPos2(float3 x) { vPositionP2 = x; }



struct GS_TRAILIN
{
	float4				vPosition   : POSITION;
	float2				vPSize      : PSIZE;
	float				fLife		: TEXCOORD0;
	row_major float4x4  Matrix		: WORLD;
	uint				InstanceID  : SV_InstanceID;
};

struct GS_TRAILOUT
{
	float4				vPosition	: SV_POSITION;
	float2				vTexUV		: TEXCOORD0;
	float				fLife		: TEXCOORD1;
};

[maxvertexcount(6)]
void GS_TRAIL(point GS_TRAILIN In[1], inout TriangleStream<GS_TRAILOUT> Vertices)
{
	GS_TRAILOUT		Out[4] = { (GS_TRAILOUT)0, (GS_TRAILOUT)0, (GS_TRAILOUT)0,(GS_TRAILOUT)0 };

	matrix      matVP		= mul(g_ViewMatrix, g_ProjMatrix);
	float4x4    WorldMatrix = In[0].Matrix;

	float3      vUp			= In[0].fLife * matrix_up(WorldMatrix) * In[0].vPSize.y * 0.5f;
	float3		vRight		= In[0].fLife * matrix_right(WorldMatrix) * In[0].vPSize.x * 0.5f;
	float3		vPosition	= matrix_postion(In[0].Matrix);

	float3 vResultPos;
	if (In[0].InstanceID == 0)
	{
		vResultPos = vPosition - vRight + vUp;
		Out[0].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[0].vTexUV = float2(0.f, 0.f);
		Out[0].fLife = In[0].fLife;
		
		vResultPos = vPosition + vRight + vUp;
		vPositionP1 = vResultPos;
		Out[1].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[1].vTexUV = float2(1.f, 0.f);
		Out[1].fLife = In[0].fLife;

		vResultPos = vPosition + vRight - vUp;
		vPositionP2 = vResultPos;
		Out[2].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[2].vTexUV = float2(1.f, 1.f);
		Out[2].fLife = In[0].fLife;

		vResultPos = vPosition - vRight - vUp;
		Out[3].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[3].vTexUV = float2(0.f, 1.f);
		Out[3].fLife = In[0].fLife;

	}
	else
	{
		//vResultPos = vPosition - vRight + vUp;
		vResultPos = vPosition1;
		Out[0].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[0].vTexUV = float2(0.f, 0.f);
		Out[0].fLife = In[0].fLife;

		vResultPos = vPosition + vRight + vUp;
		vPositionP1 = vResultPos;
		Out[1].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[1].vTexUV = float2(1.f, 0.f);
		Out[1].fLife = In[0].fLife;

		vResultPos = vPosition + vRight - vUp;
		vPositionP2 = vResultPos;
		Out[2].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[2].vTexUV = float2(1.f, 1.f);
		Out[2].fLife = In[0].fLife;

		vResultPos = vPosition2;
		Out[3].vPosition = mul(vector(vResultPos, 1.f), matVP);
		Out[3].vTexUV = float2(0.f, 1.f);
		Out[3].fLife = In[0].fLife;
	}

	

	//tInstanceInfo p1;
	//p1.vPosition	= vPosition + vRight + vUp;
	//p1.vTexUV		= Out[1].vTexUV;
	//p1.fLife		= Out[1].fLife;
	//g_WriteBuffer[0] = p1;

	//tInstanceInfo p2;
	//p2.vPosition	= vPosition + vRight - vUp;
	//p2.vTexUV		= Out[2].vTexUV;
	//p2.fLife		= Out[2].fLife;
	//g_WriteBuffer[1] = p2;


	Vertices.Append(Out[0]);
	Vertices.Append(Out[1]);
	Vertices.Append(Out[2]);
	Vertices.RestartStrip();

	Vertices.Append(Out[0]);
	Vertices.Append(Out[2]);
	Vertices.Append(Out[3]);
	Vertices.RestartStrip();

}

struct PS_TRAILIN
{
	float4		vPosition		: SV_POSITION;
	float2		vTexUV			: TEXCOORD0;
	float       fLife			: TEXCOORD1;
};

PS_OUT PS_TRAIL(PS_TRAILIN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vDiffuse = g_tex_0.Sample(LinearSampler, In.vTexUV);
	//
	//vDiffuse.a = vDiffuse.r;
	//if (vDiffuse.a < 0.1f)
	//	discard;
	//Out.vColor = vDiffuse + g_vColor;
	//Out.vColor.a *= In.fLife;

	// Out.vColor.a = Out.vColor.a * In.fLife;

	Out.vColor = float4(1.f, 1.f, 1.f, In.fLife);
	return Out;
}

/***********************************************/
/*					Pass					   */
/***********************************************/
technique11 DefaultTechnique
{
	pass Default_Haze // 0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_HAZE();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Black_Haze // 1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_HAZE();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLACK();
	}

	pass Black_Gather // 2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_GATHER();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLACK();
	}

	pass DefaultSpread // 3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_HAZE();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SPREAD();
	}

	pass DefaultTrail // 4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_TRAIL();
		GeometryShader = compile gs_5_0 GS_TRAIL();

		HullShader = NULL;// compile hs_5_0 HS_TRAIL();

		DomainShader = NULL;// compile ds_5_0 DS_TRAIL();
		PixelShader = compile ps_5_0 PS_TRAIL();
	}
}
