#include "Shader_Client_Defines.h"

/**********Constant Buffer*********/
matrix g_BoneMatrices[800];
matrix g_WorldMatrix;
matrix g_ViewMatrix;
matrix g_ProjMatrix;
float  g_fFar = 300.f;
float4 g_WorldCamPosition;
/**********************************/

texture2D		g_DepthTexture, g_NormalTexture;

/* EnemyWisp Texture */
texture2D		g_DiffuseTexture;
texture2D		g_ReamTexture;
texture2D		g_LineTexture;
texture2D		g_ShapeTexture;
texture2D		g_SmoothTexture;
/* ~EnemyWisp Texture */

// Type
int		g_TextureRenderType, g_BlendType;
int		g_SeparateWidth, g_SeparateHeight;
uint	g_iTotalDTextureComCnt, g_iTotalMTextureComCnt;
bool    g_IsUseMask, g_IsUseNormal;
float   g_WidthFrame, g_HeightFrame, g_Time;
float4  g_vColor;
float2  g_UV;
// ~Type

// Dissolve
bool  g_bDissolve;
float g_fDissolveTime;
float _DissolveSpeed = 0.2f;
float _FadeSpeed = 1.5f;
// ~Dissolve

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
	uint4		vBlendIndex : BLENDINDEX;
	float4		vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float4		vTangent : TANGENT;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float3		vViewDir : TEXCOORD2;
	float3		vBinormal : BINORMAL;
	float3      vWorldNormal : TEXCOORD3;
	float4      vWorldPosition : TEXCOORD4;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float4 worldPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vViewDir = normalize(worldPosition.xyz - g_WorldCamPosition.xyz);

	float			fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix			BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices[In.vBlendIndex.w] * fWeightW;

	vector			vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
	vector			vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
	vector			vTangent = mul(float4(In.vTangent, 0.f), BoneMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vTangent = normalize(mul(vTangent, g_WorldMatrix));
	Out.vProjPos = Out.vPosition;
	Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));
	Out.vWorldNormal = normalize(mul(In.vNormal, (float3x3)g_WorldMatrix));
	Out.vWorldPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float4		vTangent : TANGENT;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float3		vViewDir : TEXCOORD2;
	float3		vBinormal : BINORMAL;
	float3      vWorldNormal : TEXCOORD3;
	float4      vWorldPosition : TEXCOORD4;
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
	float4		vAmbient : SV_TARGET3;
};

PS_OUT PS_EFFECT_ENEMYWISP(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	/* 
	texture2D		g_DiffuseTexture; == NOISE
	texture2D		g_ReamTexture; == RED
	texture2D		g_LineTexture; == BLACK
	texture2D		g_ShapeTexture; == MASK SHAPE
	*/

	float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	float4 vColor = g_ReamTexture.Sample(LinearSampler, In.vTexUV);
	float4 vSmooth = g_SmoothTexture.Sample(LinearSampler, In.vTexUV);
	float4 vblendColor = lerp(vColor, vSmooth, vSmooth.r);
	float4 finalcolor = lerp(vblendColor, vDiffuse, vDiffuse.r) * float4(76.f, 25.f, 12.f, 195.f) / 255.f;

	float4 vLine = g_LineTexture.Sample(LinearSampler, In.vTexUV);
	float4 vShapeMask = g_ShapeTexture.Sample(LinearSampler, float2(In.vTexUV.x / 4.86, -In.vTexUV.y / 2.1));
	vShapeMask = saturate(vShapeMask);
	half dissolve_value = vShapeMask.r;

	if (dissolve_value > 0.6f)
		discard;

	// fresnel_glow(±½±â(Å¬¼ö·Ï ¾ãÀ½), )
	float4 fresnelcolor = float4(255.f, 122.f, 180.f, 255.f) / 255.f;
	float4 fresnel = float4(fresnel_glow(3, 3, fresnelcolor.rgb, In.vNormal.rgb, In.vViewDir.rgb), fresnelcolor.a);

	// rim
	float  rim = dot(In.vNormal.rgb, In.vViewDir.rgb);
	float4 rimcolor = float4(66.f, 9.f, 0.f, 118.f) / 255.f;
	float4 vOutline = rimcolor * pow(1.f - rim, 2.f);

	float4 vfinalblendColor = lerp(finalcolor, fresnel, finalcolor.r);
	vfinalblendColor = vfinalblendColor * vOutline;

	if (g_bDissolve)
	{
		float fDissolveAmount = g_fDissolveTime;

		// sample noise texture
		float noiseSample = vShapeMask.r;

		float  _ColorThreshold = 1.0f;
		float4 _DissolveColor = float4(194.f, 0.0f, 0.0f, 1.0f) / 255.f;  //red
		
		// add edge colors
		float thresh = fDissolveAmount * _ColorThreshold;
		float useDissolve = noiseSample - thresh < 0;
		vfinalblendColor = (1 - useDissolve)* vfinalblendColor + useDissolve * _DissolveColor;

		// determine deletion threshold
		float threshold = fDissolveAmount *_DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}

	Out.vDiffuse = vfinalblendColor;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
	Out.vAmbient = (vector)1.f;

	return Out;
}

technique11 DefaultTechnique
{
	pass Effect_EnemyWisp // 0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EFFECT_ENEMYWISP();
	}
}
