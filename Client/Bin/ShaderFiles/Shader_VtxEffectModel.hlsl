#include "Shader_Client_Defines.h"

/**********Constant Buffer*********/
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4			g_WorldCamPosition;
float			g_fFar = 500.f;
/**********************************/

/**********Common Texture*********/
texture2D		g_DepthTexture, g_NormalTexture;
texture2D		g_MaskTexture, g_ReamTexture, g_DiffuseTexture;

texture2D		g_DTexture_0, g_DTexture_1, g_DTexture_2, g_DTexture_3, g_DTexture_4;
texture2D		g_MTexture_0, g_MTexture_1, g_MTexture_2, g_MTexture_3, g_MTexture_4;
/**********************************/

/**********Common Option*********/
int		g_TextureRenderType, g_BlendType;
bool    g_IsUseMask, g_IsUseNormal;
int		g_SeparateWidth, g_SeparateHeight;
uint	g_iTotalDTextureComCnt, g_iTotalMTextureComCnt;
float   g_WidthFrame, g_HeightFrame, g_Time;
float4  g_vColor;
/**********************************/

/**********Dissolve*********/
bool       g_bDissolve;
float      g_fDissolveTime;
texture2D  g_DissolveTexture;
/***************************/

/* Arrow */
float	g_WaveHeight;
float	g_Speed;
float	g_WaveFrequency;
float	g_UVSpeed;
/* ~Arrow */

/* HpRatio */
float   g_HpRatio;
float	g_DamageDurationTime;
uint	g_PulseState;
/* HpRatio */

bool	g_bPulseRecive = false;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float3      vWorldNormal : TEXCOORD2;
	float3      vViewDir : TEXCOORD3;
	float4      vWorldPosition : TEXCOORD4;
	float4		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
	Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));

	Out.vWorldNormal = normalize(mul(In.vNormal, (float3x3)g_WorldMatrix));

	Out.vWorldPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vViewDir = normalize(Out.vWorldPosition.xyz - g_WorldCamPosition.xyz);

	return Out;
}

VS_OUT VS_ARROW(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float		fCosTime = g_WaveHeight * cos(g_Time * g_Speed + (In.vTexUV.x + In.vTexUV.y) * 0.5f * g_WaveFrequency);
	In.vPosition.xy *= fCosTime;

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV + float2(g_Time * g_UVSpeed * -1.f, 0.f);
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
	Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));

	Out.vWorldNormal = normalize(mul(In.vNormal, (float3x3)g_WorldMatrix));

	Out.vWorldPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vViewDir = normalize(Out.vWorldPosition.xyz - g_WorldCamPosition.xyz);

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float3      vWorldNormal : TEXCOORD2;
	float3      vViewDir : TEXCOORD3;
	float4      vWorldPosition : TEXCOORD4;
	float4		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
};

struct PS_OUT
{
	float4		vDiffuse  : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth   : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float time = frac(g_Time * 0.2f);

	float2 OffsetUV = TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(0.f, -time));

	float4 normalmap = g_NormalTexture.Sample(LinearSampler, float2(OffsetUV.x, OffsetUV.y - 0.5f * time));

	float4 fresnelColor = float4(0.5, 0.5, 0.5, 0.1f);
	float4 fresnel_color = g_DTexture_2.Sample(LinearSampler, float2(OffsetUV.x * 2.5f, OffsetUV.y* 1.5f));

	// fresnel_glow(굵기(클수록 얇음), )
	float4 fresnel = float4(fresnel_glow(3, 3.5, fresnel_color.rgb, In.vNormal.rgb, -In.vViewDir), 1.f);

	float4 BackPulseColor = g_vColor;
	float4 fresnel_Pulse = float4(fresnel_glow(4.5f, 2.5f, fresnelColor.rgb, In.vNormal.rgb, -In.vViewDir.rgb), fresnelColor.a) + fresnel;

	float4 mask = g_DTexture_1.Sample(LinearSampler, float2(OffsetUV.x, OffsetUV.y - 0.5f * time));
	float4 glow = g_DTexture_0.Sample(LinearSampler, In.vTexUV);

	Out.vDiffuse = saturate((mask + glow) * BackPulseColor * 2.f) * fresnel_Pulse + BackPulseColor;
	Out.vDiffuse.a = (g_vColor.r * 5.f + 0.5f) * 0.2f;

	Out.vDiffuse.rgb = Out.vDiffuse.rgb * 2.f;
	Out.vNormal = vector(In.vNormal.rgb * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);

	return Out;
}

PS_OUT PS_EFFECT_PULSE_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float  time = frac(g_Time * 0.05f);
	float2 OffsetUV = TilingAndOffset(float2(In.vTexUV.x / 2.f, In.vTexUV.y / 4.f), float2(1.f, 1.f), float2(0.f, -time));

	float4 vBase = g_DTexture_0.Sample(LinearSampler, In.vTexUV);

	float4 vMovingDot = g_DTexture_1.Sample(LinearSampler, OffsetUV);
	float4 vMovingDot_fresnel = float4(fresnel_glow(2.f, 3.5f, vMovingDot.rgb, In.vNormal.rgb, -In.vViewDir), 1.f);
	vMovingDot_fresnel.a = vMovingDot_fresnel.r * 0.1f;

	float4 vMovingMask = g_DTexture_2.Sample(LinearSampler, OffsetUV);
	float4 vMovingMask_fresnel = float4(fresnel_glow(2.f, 3.5f, vMovingMask.rgb, In.vNormal.rgb, -In.vViewDir), 1.f);
	vMovingMask_fresnel.a = vMovingMask_fresnel.r * 0.15f;

	float4 fresnelColor = (float4)1.0f;
	float4 fresnel = float4(fresnel_glow(8.f, 3.5f, fresnelColor.rgb, In.vNormal.rgb, -In.vViewDir), fresnelColor.a);
	
	/* Base Color */
	float4 vBaseColor = float4(20.f, 44.f, 90.f, 15.f) / 255.f;
	vBaseColor.rgb = fresnel_glow(0.5f, 2.5f, vBase.rgb, In.vNormal.rgb, In.vViewDir) * vBaseColor.rgb + fresnel.rgb * float3(1.0f, 1.0f, 1.0f) ;

	float4 vGreencolor = float4(25.f, 283.f, 189.f, 255.f) / 255.f;
	vBaseColor.rgb = vBaseColor.rgb * vGreencolor.rgb;

	float4 vfinaladdcolor = float4(8.0f, 155.f, 125.f, 255.f) / 255.f;
	vfinaladdcolor.rgb = vfinaladdcolor.rgb * 2.f;

	float4 finalcolor = vBaseColor + vMovingDot_fresnel + vMovingMask_fresnel * vfinaladdcolor * float4(117.f, 94.f, 215.f, 212.f) / 255.f;

	float	fHpRatio = max(0.f, g_HpRatio);
	float	fColorRatio;
	vector	vColor;

	/************************** Hp **************************/
	if (fHpRatio > 0.7f)
	{
		vColor = float4(0.f, 0.f, 0.f, 0.f);
	}
	else if (fHpRatio > 0.3f && fHpRatio <= 0.7f)
	{
		fColorRatio = 1.f - ((fHpRatio - 0.3f) / 0.4f);
		vColor = float4(fColorRatio, 0.f, fColorRatio, 0.f) * 2.f;
	}
	else
	{
		fColorRatio = fHpRatio / 0.3f;
		vColor = float4(1.f, 0.f, fColorRatio, 0.f) * 2.f;
	}
	finalcolor = finalcolor + vColor;

	if (g_PulseState == 1) //Default2Damage
	{
		vector vColor = float4(255.f, 0.f, 255.f, 0.0f) / 255.f;
		float fTime = min(g_DamageDurationTime, 2.f);

		if (1.f < fTime)   // 1.0 이상 컬러값이 내려가야함
			vColor = vColor * (2.f - fTime);
		else // 1.0 이하 컬러값이 올라가야함
			vColor = vColor * fTime;

		finalcolor = finalcolor + vColor;
	}
	/************************** Hp **************************/

	if (g_bDissolve)
	{
		float fDissolveAmount = g_fDissolveTime * 2.f;
		float vDissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;

		float useDissolve = vDissolve - fDissolveAmount < 0;

		if (g_HpRatio <= 0.0f)
		{
			finalcolor = finalcolor + useDissolve;
			float threshold = fDissolveAmount * 0.3f * 1.5f;

			clip(vDissolve - threshold);
		}
		else
		{
			finalcolor = finalcolor + useDissolve*(1 - useDissolve);
			float threshold = fDissolveAmount * 0.7f;
			clip(threshold - vDissolve);
		}
	}

	Out.vDiffuse = finalcolor;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
	return Out;
}

PS_OUT PS_EFFECT_HIT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 PulseColor = float4(122.f, 122.f, 122.f, 255.f) / 255.f;
	float4 fresnel = float4(fresnel_glow(4, 4.5, PulseColor.rgb, In.vNormal.rgb, -In.vViewDir), 1.f);

	float4 vDiffuse = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	vDiffuse.rgb = vDiffuse.rgb *4.f;

	Out.vDiffuse = vDiffuse * fresnel + PulseColor;
	Out.vDiffuse.a = (PulseColor.r * 5.f + 0.5f) * 0.05f;

	return Out;
}

//PS_EFFECT_PULSEOBJECT
PS_OUT PS_EFFECT_PULSEOBJECT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 outglowcolor = float4(0.0f, 62.f, 102.f, 50.f) / 255.f;
	float4 outglow = float4(fresnel_glow(8, 3.5, g_vColor.rgb, In.vNormal.rgb, -In.vViewDir), 1.f);

	float4 vDiffuse = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
//	vDiffuse.rgb = vDiffuse.rgb *4.f;

	Out.vDiffuse = vDiffuse * outglow * 4.f;
	Out.vDiffuse.rgb = Out.vDiffuse.rgb * 6.f;

	if (true == g_bPulseRecive)
		Out.vDiffuse.b *= 10.f;

	Out.vDiffuse.a = (outglowcolor.r * 5.f + 0.5f) * 0.05f;

	return Out;
}

// PS_EFFECT_DEADZONE
PS_OUT PS_EFFECT_DEADZONE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float  time     = frac(g_Time * 0.35f);
	float2 OffsetUV = TilingAndOffset(In.vTexUV, float2(1.0f, 1.0f), float2(time, 0.0f));

	float4 vDiffuse = g_DTexture_0.Sample(LinearSampler, float2(In.vTexUV.x - time, In.vTexUV.y));
	float4 vMask = g_MTexture_0.Sample(LinearSampler, In.vTexUV);
	float4 vMask1 = g_MTexture_1.Sample(LinearSampler, float2(In.vTexUV.x + time, In.vTexUV.y - time));
	vDiffuse.a = vDiffuse.r;
	vMask.a = vMask.r;
	vMask1.a = vMask1.r;

	float4 LeapMask = lerp(vMask, vMask1, 0.5f);
	float3 fresnelColor = float3(0.f, 0.0f, 0.0f);

	float4 fresnel = float4(fresnel_glow(3.5, 2.5, fresnelColor, In.vNormal.rgb, -In.vViewDir), 0.2f);

	float4 finalcolor = saturate(vDiffuse * LeapMask);

	if (vDiffuse.a < 0.4f)
		finalcolor.rgb = finalcolor.rgb + float3(200.f, 21.f, 0.0f) / 255.f  * 4.f;
	else
		finalcolor.rgb = finalcolor.rgb + float3(255.f,54.f, 0.0f) / 255.f * 4.f;

	finalcolor.rgb = finalcolor.rgb + fresnel.rgb;
	Out.vDiffuse = finalcolor * float4(255.f, 192.f, 163.f, 255.f) / 255.f;

	if (g_bDissolve)
	{
		float fDissolveAmount = g_fDissolveTime;

		float4 vDissolve = g_MTexture_2.Sample(LinearSampler, In.vTexUV);
		half dissolve_value = vDissolve.r;

		if (dissolve_value >= fDissolveAmount)
			discard;

		else if (dissolve_value >= fDissolveAmount && fDissolveAmount != 0)
		{
			if (Out.vDiffuse.a != 0.0f)
				Out.vDiffuse = float4(float3(1.0f, 0.0f, 0.0f) * step(dissolve_value + fDissolveAmount, 0.2f), Out.vDiffuse.a);
		}
	}

	return Out;
}

//PS_SPRITARROW
PS_OUT PS_SPRITARROW(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float  time = frac(g_Time * 4.5f);
	float2 OffsetUV = TilingAndOffset(In.vTexUV, float2(1.0f, 1.0f), float2(0.5f, time));

	float4 outglowcolor = float4(31.0f, 158.f, 191.f, 255.f) / 255.f;
	float4 outglow = float4(fresnel_glow(2.f, 3.5f, outglowcolor.rgb, In.vNormal.rgb, In.vViewDir), 1.f);

	float4 vDiffuse = g_DTexture_0.Sample(LinearSampler, In.vTexUV * 3.f);
	Out.vDiffuse.rgb = vDiffuse.rgb + outglow.rgb;
	if (Out.vDiffuse.a < 0.2f)
		Out.vDiffuse.rgb = Out.vDiffuse.rgb * (float3)1.f;
	Out.vDiffuse.a = (outglowcolor.b * 5.f) * 0.2f;
	if (Out.vDiffuse.a < 0.2f)
		discard;
	return Out;
}

//PS_WIND
PS_OUT PS_WIND(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float  time = frac(g_Time  * 0.1f);
	float4 vDiffuse = g_DTexture_0.Sample(LinearSampler, In.vTexUV) * 2.f;

	float fTIme = min(time, 1.f);
	if (In.vTexUV.y < fTIme)
		vDiffuse.a *= (1.f - fTIme);

	Out.vDiffuse = vDiffuse;
	return Out;
}

//PS_SPRITARROW_GRAB
PS_OUT PS_SPRITARROW_GRAB(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float  time = frac(g_Time * 0.4f);
	float2 OffsetUV = TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(time, time));

	float4 outglowcolor = float4(2.0f, 6.f, 10.f, 0.f) / 255.f;
	float4 outglow = float4(fresnel_glow(8, 3.5, outglowcolor.rgb, In.vNormal.rgb, In.vViewDir), 1.f);

	float4 vDiffuse = g_DTexture_0.Sample(LinearSampler, OffsetUV);
	
	Out.vDiffuse = vDiffuse * outglow;
	Out.vDiffuse.a = outglowcolor.r * 0.05f;
	return Out;
}

PS_OUT PS_MAGEBULLET(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	//T_Deadzone_REAM
	float4 vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	//t_fur_noise_02
	float4 vNoise = g_ReamTexture.Sample(LinearSampler, In.vTexUV);
	// T_GR_Cloud_Noise_A
	float4 vSmooth = g_MaskTexture.Sample(LinearSampler, In.vTexUV);

	float4 vblendColor = lerp(vSmooth, vColor, 0.4f);
	float4 finalcolor = lerp(vblendColor, vNoise, vNoise.r) * float4(81.f, 12.f, 0.f, 255.f) / 255.f;

	// fresnel_glow(굵기(클수록 얇음), )
	float4 fresnelcolor = float4(255.f, 37.f, 0.f, 255.f) / 255.f;
	float4 fresnel = float4(fresnel_glow(3.5, 2.5, fresnelcolor.rgb, In.vNormal.rgb, In.vViewDir.rgb), fresnelcolor.a);

	float4 vfinalblendColor = lerp(finalcolor, fresnel, finalcolor.r);
	Out.vDiffuse = vfinalblendColor;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);

	return Out;
}

PS_OUT PS_MAGEBULLETCOVER(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float  fDissolveAmount = 0.5f;

	float4 vDissolve = g_MaskTexture.Sample(LinearSampler, In.vTexUV);
	half   dissolve_value = vDissolve.r;

	if (dissolve_value <= fDissolveAmount)
		discard;

	else if (dissolve_value <= fDissolveAmount && fDissolveAmount != 0)
	{
		if (Out.vDiffuse.a != 0.0f)
			Out.vDiffuse = float4(float3(1.0f, 0.0f, 0.0f) * step(dissolve_value + fDissolveAmount, 0.2f), Out.vDiffuse.a);
	}

	float4 vColor = float4(255.f, 136.f, 98.f, 255.f) / 255.f;
	Out.vDiffuse = vDissolve * vColor * 3.f;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);

	return Out;
}

//PS_PULSECOVER
PS_OUT PS_PULSECOVER(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float  fDissolveAmount = 0.8f;

	float4 vDiffuse = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	half   dissolve_value = vDiffuse.r;

	if (dissolve_value <= fDissolveAmount)
		discard;

	float4 vColor = g_vColor;
	Out.vDiffuse = vDiffuse * vColor * 3.f;
	Out.vDiffuse.a = 0.3f;

	if (g_bDissolve)
	{
		float fDissolveAmount = g_fDissolveTime;

		float4 vDissolve = g_MTexture_0.Sample(LinearSampler, In.vTexUV);
		half dissolve_value = vDissolve.r;

		if (dissolve_value <= fDissolveAmount)
			discard;

		else if (dissolve_value <= fDissolveAmount && fDissolveAmount != 0)
		{
			if (Out.vDiffuse.a != 0.0f)
				Out.vDiffuse = float4(float3(1.0f, 0.0f, 0.0f) * step(dissolve_value + fDissolveAmount, 0.2f), Out.vDiffuse.a);
		}
	}

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);

	return Out;
}

//PS_PULSEINNER
PS_OUT PS_PULSEINNER(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float  time = frac(g_Time * 1.8f);
	float2 OffsetUV = TilingAndOffset(In.vTexUV* 5.f, float2(1.0f, 1.0f), float2(0.0f, -time));

	float4 vDiffuse = g_DTexture_0.Sample(LinearSampler, OffsetUV);
	vDiffuse.a = vDiffuse.r;

	if (vDiffuse.a < 0.2f)
		discard;

	float4 fresnelColor = float4(0.0f, 205.f, 205.f, 255.f) / 255.f;
	float4 fresnel = float4(fresnel_glow(5.0f, 1.5f, fresnelColor.rgb, In.vNormal.rgb, In.vViewDir), fresnelColor.a);

	float4 vColor = float4(15.f, 40.f, 79.f, 0.0f) / 255.f;
	float4 vBaseColor = lerp(vDiffuse, vColor, 0.5f);

	Out.vDiffuse = vBaseColor * fresnel * 2.f;
	Out.vDiffuse.a = 0.2f;

	if (g_bDissolve)
	{
		float fDissolveAmount = g_fDissolveTime;

		float4 vDissolve = g_MTexture_0.Sample(LinearSampler, In.vTexUV);
		half dissolve_value = vDissolve.r;

		if (dissolve_value <= fDissolveAmount)
			discard;

		else if (dissolve_value <= fDissolveAmount && fDissolveAmount != 0)
		{
			if (Out.vDiffuse.a != 0.0f)
				Out.vDiffuse = float4(float3(1.0f, 0.0f, 0.0f) * step(dissolve_value + fDissolveAmount, 0.2f), Out.vDiffuse.a);
		}
	}

	return Out;
}

//PS_SWIPES_CHARGED
PS_OUT PS_SWIPES_CHARGED(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float  time = frac(g_Time * 0.5f);
	float2 OffsetUV = TilingAndOffset(In.vTexUV * 1.5f, float2(0.8f, 1.0f), float2(0.0f, time));

	float4 swipes_charged_color = float4(1.f, 0.166602f, 0.419517f, 0.514f);

	vector vRedRim = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	vector vIcenoise = g_DTexture_1.Sample(LinearSampler, In.vTexUV);

	vector vLerpTexture = lerp(vRedRim, vIcenoise, vRedRim.r);
	vLerpTexture.a = vLerpTexture.r;

	vector texturefresnel = vector(fresnel_glow(4.f, 4.5f, vLerpTexture.rgb, In.vNormal.rgb, -In.vViewDir), 1.f);
	texturefresnel.a = texturefresnel.r * 0.1f;

	vector fresnel = vector(fresnel_glow(0.5f, 3.5f, swipes_charged_color.rgb, In.vNormal.rgb, -In.vViewDir), 1.f);
	fresnel.a = fresnel.r * 0.1f;

	vector vSwipe05 = g_DTexture_2.Sample(LinearSampler, OffsetUV);
	vSwipe05.a = vSwipe05.r * 0.1f;

	Out.vDiffuse = texturefresnel + fresnel + vSwipe05 * (swipes_charged_color * 3.f);

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);

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

	pass Effect_Pulse // 1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EFFECT_PULSE_MAIN();
	}

	pass Effect_Hit // 2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EFFECT_HIT();
	}

	pass Effect_PulseObject // 3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EFFECT_PULSEOBJECT();
	}

	pass Effect_DeadZone // 4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EFFECT_DEADZONE();
	}

	pass Effect_SpritArrow // 5
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_ARROW();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SPRITARROW();
	}

	pass Effect_Wind // 6
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_WIND();
	}

	pass Effect_SpritArrowGrab // 7
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SPRITARROW_GRAB();
	}

	pass Effect_Mage // 8
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAGEBULLET();
	}

	pass Effect_MageCover // 9
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAGEBULLETCOVER();
	}

	pass Effect_PulseCover // 10
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_PULSECOVER();
	}

	pass Effect_PulseInner // 11
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_PULSEINNER();
	}

	pass Swipes_Charged // 12
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SWIPES_CHARGED();
	}
}
