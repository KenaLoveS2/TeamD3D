#include "Shader_Client_Defines.h"

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
	float		fLife : TEXCOORD0;
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
	float       fLife : TEXCOORD0;
	float3		vCenterPosition	: TEXCOORD1;
};

struct GS_OUT
{
	float4		vPosition	: SV_POSITION;
	float2		vTexUV		: TEXCOORD0;
	float       fLife : TEXCOORD1;
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
	float       fLife : TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (g_IsSpriteAnim)
	{
		In.vTexUV.x = In.vTexUV.x + g_XFrameNow;
		In.vTexUV.y = In.vTexUV.y + g_YFrameNow;

		In.vTexUV.x = In.vTexUV.x / g_XFrames;
		In.vTexUV.y = In.vTexUV.y / g_YFrames;
	}

	Out.vColor = g_DiffuseTexture.Sample(PointSampler, In.vTexUV);

	Out.vColor *= g_vColor;
	Out.vColor.a *= In.fLife;

	if (Out.vColor.a < 0.01f)
		discard;

	Out.vColor.rgb *= g_fHDRItensity;

	return Out;
}

PS_OUT PS_MAIN_BLACK(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (g_IsSpriteAnim)
	{
		In.vTexUV.x = In.vTexUV.x + g_XFrameNow;
		In.vTexUV.y = In.vTexUV.y + g_YFrameNow;

		In.vTexUV.x = In.vTexUV.x / g_XFrames;
		In.vTexUV.y = In.vTexUV.y / g_YFrames;
	}

	Out.vColor = g_DiffuseTexture.Sample(PointSampler, In.vTexUV);

	Out.vColor.a = Out.vColor.r;

	Out.vColor *= g_vColor;

	if (Out.vColor.a < 0.01f)
		discard;

	Out.vColor.rgb *= g_fHDRItensity;

	return Out;
}

PS_OUT PS_MAIN_SPREAD(PS_IN In)
{
	/* Ref : Shader_VtxEffectPointInstance : PS_FRONTVIEWBLINK */

	PS_OUT		Out = (PS_OUT)0;

	if (g_IsSpriteAnim)
	{
		In.vTexUV.x = In.vTexUV.x + g_XFrameNow;
		In.vTexUV.y = In.vTexUV.y + g_YFrameNow;

		In.vTexUV.x = In.vTexUV.x / g_XFrames;
		In.vTexUV.y = In.vTexUV.y / g_YFrames;
	}

	vector vDiffuse = g_DiffuseTexture.Sample(PointSampler, In.vTexUV);

	Out.vColor = vDiffuse;
	Out.vColor.rgb *= g_vColor.rgb;
	Out.vColor.a *= (1 - In.fLife);
	//vDiffuse.a = vDiffuse.r;

	//vDiffuse.rgb = g_vColor.rgb;
	//vDiffuse.a *= g_vColor.rgb;

	//if (Out.vColor.a == 0.0f)
	//	discard;

	//Out.vColor.rgb *= g_fHDRItensity;

	return Out;
}

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
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_HAZE();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SPREAD();
	}
}
