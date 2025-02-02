#include "Shader_Client_Defines.h"

/**********Constant Buffer*********/
float4			g_WorldCamPosition;
float			g_fFar = 500.f;
float			g_fHDRValue;
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
float2  g_fUV;
bool	g_bTimer;
/**********************************/

/**********Dissolve*********/
bool       g_bDissolve;
float      g_fDissolveTime;
texture2D  g_DissolveTexture;
float	   _DissolveSpeed = 0.2f;
float	   _FadeSpeed = 1.5f;
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
	float3		vWorldNormal : TEXCOORD2;
	float3		vViewDir : TEXCOORD3;
	float4		vWorldPosition : TEXCOORD4;
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

VS_OUT VS_MAIN_SOCKET(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	vector		vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_SocketMatrix);

	vector		vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
	vNormal = mul(vNormal, g_SocketMatrix);

	vector		vTangent = mul(float4(In.vTangent, 0.f), g_WorldMatrix);
	vTangent = mul(vTangent, g_SocketMatrix);

	Out.vPosition = mul(float4(vPosition.xyz, 1.f), matVP);
	Out.vNormal = normalize(vNormal);
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(vTangent);
	Out.vBinormal = normalize(cross(vNormal.xyz, vTangent.xyz));

	Out.vWorldNormal = vNormal.xyz;
	Out.vWorldPosition = float4(vPosition.xyz, 1.f);
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

//struct PS_OUT
//{
//	float4		vDiffuse  : SV_TARGET0;
//	float4		vNormal : SV_TARGET1;
//	float4		vDepth   : SV_TARGET2;
//};

struct PS_OUT
{
	float4		vDiffuse  : SV_TARGET0;
	float4		vDepth : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float time = frac(g_Time * 0.2f);
	float2 OffsetUV = TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(0.f, -time));

	float4 normalmap = g_NormalTexture.Sample(LinearSampler, float2(OffsetUV.x, OffsetUV.y - 0.5f * time));

	float4 fresnelColor = float4(0.5, 0.5, 0.5, 0.1f);
	float4 fresnel_color = g_DTexture_2.Sample(LinearSampler, float2(OffsetUV.x * 2.5f, OffsetUV.y* 1.5f));

	float4 fresnel = float4(fresnel_glow(3, 3.5, fresnel_color.rgb, In.vNormal.rgb, -In.vViewDir), 1.f);

	float4 BackPulseColor = g_vColor;
	float4 fresnel_Pulse = float4(fresnel_glow(4.5f, 2.5f, fresnelColor.rgb, In.vNormal.rgb, -In.vViewDir.rgb), fresnelColor.a) + fresnel;

	float4 mask = g_DTexture_1.Sample(LinearSampler, float2(OffsetUV.x, OffsetUV.y - 0.5f * time));
	float4 glow = g_DTexture_0.Sample(LinearSampler, In.vTexUV);

	Out.vDiffuse = saturate((mask + glow) * BackPulseColor * 2.f) * fresnel_Pulse + BackPulseColor;
	Out.vDiffuse.a = (g_vColor.r * 5.f + 0.5f) * 0.2f;

	Out.vDiffuse = CalcHDRColor(Out.vDiffuse, g_fHDRValue);
	//Out.vNormal = vector(In.vNormal.rgb * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);

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

		if (1.f < fTime)  
			vColor = vColor * (2.f - fTime);
		else 
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
	Out.vDiffuse = CalcHDRColor(finalcolor, g_fHDRValue);
	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, g_fHDRValue, 0.f);
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
	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, g_fHDRValue, 0.f);

	return Out;
}

//PS_EFFECT_PULSEOBJECT
PS_OUT PS_EFFECT_PULSEOBJECT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 outglowcolor = float4(0.0f, 62.f, 102.f, 50.f) / 255.f;
	float4 outglow = float4(fresnel_glow(5, 3.5, g_vColor.rgb, In.vNormal.rgb, -In.vViewDir), 1.f);

	float4 vDiffuse = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	float4 fresnel = float4(fresnel_glow(5, 3.5, vDiffuse.rgb, In.vNormal.rgb, -In.vViewDir), vDiffuse.a);
	fresnel.a = g_vColor.a;

	Out.vDiffuse = saturate(fresnel + outglow);
	Out.vDiffuse.a = fresnel.a;
	Out.vDiffuse = CalcHDRColor(Out.vDiffuse, g_fHDRValue);
	if (true == g_bPulseRecive)
		Out.vDiffuse.b *= 10.f;

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
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
	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, g_fHDRValue, 0.f);

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
	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, g_fHDRValue, 0.f);
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
	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, g_fHDRValue, 0.f);
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
	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, g_fHDRValue, 0.f);
	return Out;
}

PS_OUT PS_MAGEBULLET(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);	//T_Deadzone_REAM
	float4 vNoise = g_ReamTexture.Sample(LinearSampler, In.vTexUV);		// t_fur_noise_02
	float4 vSmooth = g_MaskTexture.Sample(LinearSampler, In.vTexUV);	// T_GR_Cloud_Noise_A
	vNoise = vNoise + float4(53.f, 25.f, 0.0f, 0.0f) / 255.f;

	/* */
	vector vfresnelcolor = float4(255.f, 40.f, 6.0f, 0.0f) / 255.f;
	float4 vfinalblendColor = (vNoise + vSmooth + vfresnelcolor);
	vector texturefresnel = vector(fresnel_glow(-1.0f, 0.64f, vfinalblendColor.rgb, In.vNormal.rgb, In.vViewDir), 1.f);
	texturefresnel.a = texturefresnel.r;

	vector InnerColor = vector(0.0f, 255.f, 255.f, 0.0f) / 255.f;
	vector Inner = vector(fresnel_glow(-0.55f, -0.1f, InnerColor.rgb, In.vNormal.rgb, -In.vViewDir), InnerColor.a);
	/* */

	vfinalblendColor = saturate(texturefresnel) + Inner;

	if (g_bDissolve)
	{
		float fDissolveAmount = g_fDissolveTime + 0.4f;

		// sample noise texture
		float noiseSample = g_DTexture_0.Sample(LinearSampler, In.vTexUV).r;

		float  _ColorThreshold1 = 1.0f;
		float4 _DissolveColor1 = float4(194.f, 0.0f, 0.0f, 1.0f) / 255.f;  //red

		float  _ColorThreshold2 = 0.4f;
		float4 _DissolveColor2 = float4(255.f, 163.f, 44.f, 1.0f) / 255.f; //orange

		// add edge colors0
		float thresh1 = fDissolveAmount * _ColorThreshold1;
		float useDissolve1 = noiseSample - thresh1 < 0;
		vfinalblendColor = (1 - useDissolve1) * vfinalblendColor + useDissolve1 * _DissolveColor1;

		// add edge colors1
		float thresh2 = fDissolveAmount * _ColorThreshold2;
		float useDissolve2 = noiseSample - thresh2 < 0;
		vfinalblendColor = (1 - useDissolve2) * vfinalblendColor + useDissolve2 * _DissolveColor2;

		// determine deletion threshold
		float threshold = fDissolveAmount * _DissolveSpeed * _FadeSpeed;
		clip(noiseSample - threshold);
	}

	Out.vDiffuse = CalcHDRColor(vfinalblendColor, g_fHDRValue);
	return Out;
}

PS_OUT PS_MAGEBULLETCOVER(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float  fDissolveAmount = 0.5f;
	float  time = frac(g_Time * 1.8f);
	float2 OffsetUV = TilingAndOffset(In.vTexUV, float2(1.0f, 1.0f), float2(time, time));

	float4 vDissolve = g_MaskTexture.Sample(LinearSampler, OffsetUV);
	half   dissolve_value = vDissolve.r;

	if (dissolve_value <= fDissolveAmount)
		discard;

	else if (dissolve_value <= fDissolveAmount && fDissolveAmount != 0)
	{
		if (vDissolve.a != 0.0f)
			vDissolve = float4(float3(1.0f, 0.0f, 0.0f) * step(dissolve_value + fDissolveAmount, 0.2f), vDissolve.a);
	}

	float4 vColor = float4(255.f, 136.f, 98.f, 255.f) / 255.f;
	float4 finalcolor = vDissolve * vColor;

	if (g_fDissolveTime != 0.0f)
	{
		float fDissolveAmount = g_fDissolveTime;

		float4 vDissolve = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
		half dissolve_value = vDissolve.r;

		if (dissolve_value <= fDissolveAmount)
			discard;

		else if (dissolve_value <= fDissolveAmount && fDissolveAmount != 0)
		{
			if (finalcolor.a != 0.0f)
				finalcolor = float4(float3(1.0f, 0.0f, 0.0f) * step(dissolve_value + fDissolveAmount, 0.2f), finalcolor.a);
		}
	}

	Out.vDiffuse = CalcHDRColor(finalcolor, g_fHDRValue);
	return Out;
}

//PS_PULSECOVER
PS_OUT PS_PULSECOVER(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float  fDissolveAmount = 0.7f;

	float  time = frac(g_Time * 0.6f);
	float2 OffsetUV = TilingAndOffset(In.vTexUV, float2(1.0f, 1.0f), float2(0.0f, time));

	float4 vDiffuse = g_DTexture_0.Sample(LinearSampler, OffsetUV);
	half   dissolve_value = vDiffuse.r;
	if (dissolve_value <= fDissolveAmount)
		discard;

	float4 vfinalcolor = vDiffuse + g_vColor;
	vfinalcolor = saturate(vfinalcolor);
	if (g_fDissolveTime != 0.0f)
	{
		float fDissolveAmount = g_fDissolveTime;

		float4 vDissolve = g_MTexture_0.Sample(LinearSampler, In.vTexUV);
		half dissolve_value = vDissolve.r;

		if (dissolve_value <= fDissolveAmount)
			discard;

		else if (dissolve_value <= fDissolveAmount && fDissolveAmount != 0)
		{
			if (vfinalcolor.a != 0.0f)
				vfinalcolor = float4(float3(1.0f, 0.0f, 0.0f) * step(dissolve_value + fDissolveAmount, 0.2f), Out.vDiffuse.a);
		}
	}
	Out.vDiffuse = CalcHDRColor(vfinalcolor, 1.f);
	return Out;
}

//PS_PULSEINNER
PS_OUT PS_PULSEINNER(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float  time = frac(g_Time * 0.6f);
	float2 OffsetUV = TilingAndOffset(In.vTexUV, float2(1.0f, 1.0f), float2(0.0f, time));

	float4 tex_0 = g_DTexture_0.Sample(LinearSampler, OffsetUV);
	float4 tex_1 = g_DTexture_1.Sample(LinearSampler, OffsetUV);
	tex_0.a = tex_0.r;

	float4 fresnelColor = float4(0.0f, 36.f, 145.f, 255.f) / 255.f;
	float4 fresnel = float4(fresnel_glow(3.0f, 1.5f, fresnelColor.rgb, In.vNormal.rgb, In.vViewDir), fresnelColor.a);

	float4 finalcolor = saturate(lerp(tex_0, tex_1, 0.5f)) * fresnel;
	finalcolor.a = (tex_0 * tex_1).a;

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
	Out.vDiffuse = CalcHDRColor(finalcolor, g_fHDRValue);
	return Out;
}


//PS_SWIPES_CHARGED
PS_OUT PS_SWIPES_CHARGED(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float  time = frac(g_Time * 0.5f);
	float2 OffsetUV = TilingAndOffset(float2(In.vTexUV.x, In.vTexUV.y * 2.5f), float2(0.8f, 1.0f), float2(0.0f, time));
	float4 swipes_charged_color = float4(255.f, 0.0f, 0.0f, 0.0f) / 255.f;

	vector vRedRim = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	vector vIcenoise = g_DTexture_1.Sample(LinearSampler, In.vTexUV);

	vector vSwipe05 = g_DTexture_2.Sample(LinearSampler, OffsetUV);
	vSwipe05.a = vSwipe05.r;

	/* */
	float4 finalcolor = vRedRim + vIcenoise;
	vector texturefresnel = vector(fresnel_glow(5.f, 5.5f, finalcolor.rgb, In.vNormal.rgb, -In.vViewDir), 1.f);
	texturefresnel.a = texturefresnel.r ;
	/* */

	vector fresnel = vector(fresnel_glow(1.0f, 0.03f, finalcolor.rgb, In.vNormal.rgb, -In.vViewDir), g_vColor.a);
	finalcolor = finalcolor + vSwipe05 + texturefresnel + fresnel + swipes_charged_color;
	finalcolor = saturate(finalcolor);
	finalcolor.a = 0.3f;

	Out.vDiffuse = CalcHDRColor(finalcolor, g_fHDRValue);
	return Out;
}

PS_OUT PS_MAIN_EFFECTSHAMAN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vFinalColor = vDiffuse + g_vColor * 3.f;

	Out.vDiffuse = CalcHDRColor(vFinalColor, g_fHDRValue);
	return Out;
}

//PS_FIRE_SWIPES
PS_OUT PS_FIRE_SWIPES(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float  time = frac(g_Time * 0.4f);
	float2 OffsetUV = TilingAndOffset(In.vTexUV * 1.5f, float2(0.5f, 1.0f), float2(0.0f, time));
	float4 swipes_charged_color = float4(1.f, 0.166602f, 0.419517f, 0.514f);

	vector customNoise = g_DTexture_0.Sample(LinearSampler, OffsetUV);
	vector t_fur_noise = g_DTexture_1.Sample(LinearSampler, OffsetUV);
	vector T_ramp04 = g_DTexture_2.Sample(LinearSampler, float2(In.vTexUV.x + time, In.vTexUV.y + g_fUV.y));
	vector SmokeTiled3 = g_DTexture_4.Sample(LinearSampler, OffsetUV);

	float4 finalcolor = (customNoise + t_fur_noise + SmokeTiled3);
	finalcolor = saturate(finalcolor + swipes_charged_color);
	//finalcolor.a = T_ramp04 * In.vTexUV.y;
	finalcolor.a = T_ramp04.r * In.vTexUV.y;

	Out.vDiffuse = CalcHDRColor(finalcolor, 1.5f);
	return Out;
}

// PS_FIRE_SWIPES_INNER
PS_OUT PS_FIRE_SWIPES_INNER(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float  time = frac(g_Time * 0.4f);
	float2 OffsetUV = TilingAndOffset(In.vTexUV, float2(1.0f, 1.0f), float2(0.0f, time));

	/* main color */
	float4 swipes_charged_color = float4(1.f, 0.166602f, 0.419517f, 0.514f);

	/* alpha */
	vector T_swipe05 = g_DTexture_3.Sample(LinearSampler, OffsetUV);
	vector T_ramp04 = g_DTexture_2.Sample(LinearSampler, float2(In.vTexUV.x + g_fUV.x, In.vTexUV.y + 0.36f));
	T_swipe05.a = T_swipe05.r;

	float4 finalcolor = T_swipe05;
	finalcolor = saturate(finalcolor + swipes_charged_color);
	finalcolor.a = T_ramp04.r * In.vTexUV.y;

	Out.vDiffuse = CalcHDRColor(finalcolor, 1.f);
	return Out;
}
 
// PS_ROOT
PS_OUT PS_ROOT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float  time = frac(g_Time * 0.3f);

	/* main color */
	float4 swipes_charged_color = float4(1.f, 0.166602f, 0.419517f, 0.514f);

	/* alpha */
	vector vDiffuse = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	vector Gradient = g_DTexture_1.Sample(LinearSampler, float2(In.vTexUV.x , In.vTexUV.y + g_Time));

	float4 finalcolor = vDiffuse + Gradient;
	float4 Realfinal = saturate(finalcolor * swipes_charged_color) ;

	Out.vDiffuse = CalcHDRColor(Realfinal, g_fHDRValue);
	return Out;
}

PS_OUT PS_PLANEROOT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float  time = frac(g_Time * 1.1f);

	/* main color */
	float4 swipes_charged_color = float4(1.f, 0.166602f, 0.419517f, 0.514f);

	/* alpha */
	vector vPlaneRoot = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	vPlaneRoot.a = vPlaneRoot.r * 0.2f;
	vPlaneRoot.rgb = (float3)0.0f;

	vector vColor = g_DTexture_1.Sample(LinearSampler, In.vTexUV);

	float3 finalcolor = vPlaneRoot.rgb + swipes_charged_color.rgb * 35.f;

	Out.vDiffuse = float4(finalcolor, vPlaneRoot.a);

	float fTime = min(time, 1.f);
	if (0.5f < fTime)  
		Out.vDiffuse = Out.vDiffuse * (1.f - fTime);
	else 
		Out.vDiffuse = Out.vDiffuse * fTime;

	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, g_fHDRValue, 0.f);

	return Out;
}

//PS_GRONDSHOCK
PS_OUT PS_GRONDSHOCK(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float  time = frac(g_Time * 1.1f);
	float2 OffsetUV = TilingAndOffset(In.vTexUV, float2(1.0f, 1.0f), float2(time, 0.f));

	vector vDiffuseTexture = g_DTexture_0.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y / 2.f));
	float4 vColor = float4(40.f, 20.f, 7.f, 0.f) / 255.f;

	float  fAlpha = min(1.0f, In.vTexUV.y);

	Out.vDiffuse = vDiffuseTexture + vColor * 1.5f;
	Out.vDiffuse.a = Out.vDiffuse.a * 0.1f;

	float fTime = min(g_Time * 1.2f, 2.f);

	if (1.f < fTime) 
		Out.vDiffuse.a = Out.vDiffuse.a * (2.f - fTime);

	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, g_fHDRValue, 0.f);

	return Out;
}

// PS_GRONDPLANE
PS_OUT PS_GRONDPLANE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vDiffuseTexture = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	vDiffuseTexture.a = vDiffuseTexture.r;

	// float4 vColor = float4(66.f, 23.f, 0.f, 0.f) / 255.f;

	Out.vDiffuse = vDiffuseTexture + g_vColor;
	Out.vDiffuse.a = Out.vDiffuse.a * 0.4f;

	float fTime = min(g_Time * 1.2f, 2.f);

	if (1.f < fTime)  
		Out.vDiffuse.a = Out.vDiffuse.a * (2.f - fTime);

	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, g_fHDRValue, 0.f);

	return Out;
}

//PS_ENRAGE
PS_OUT PS_ENRAGE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	/* Sprite */
	In.vTexUV.x = In.vTexUV.x + g_WidthFrame;
	In.vTexUV.y = In.vTexUV.y + g_HeightFrame;

	In.vTexUV.x = In.vTexUV.x / 1.f;
	In.vTexUV.y = In.vTexUV.y / 4.f;

	In.vTexUV.y = In.vTexUV.y * 10.f;

	float  time = frac(g_Time * 1.5f);
	float2 OffsetUV = TilingAndOffset(In.vTexUV, float2(1.0f, 1.0f), float2(0.0f, time));
	
	vector vDiffuseTexture = g_DTexture_0.Sample(LinearSampler, OffsetUV);
	vDiffuseTexture.a = vDiffuseTexture.r;
	
	Out.vDiffuse = vDiffuseTexture + g_vColor;

	float fTime = min(g_Time, 1.f);
	if (0.5f < fTime)
		vDiffuseTexture.a = vDiffuseTexture.a * (1.f - fTime);
	else
		vDiffuseTexture.a = vDiffuseTexture.a * (fTime / 1.f);

	float4 finalcolor = vDiffuseTexture + g_vColor;
	Out.vDiffuse = finalcolor;
	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, g_fHDRValue, 0.f);

	return Out;
}

//PS_DISTORTION
PS_OUT PS_DISTORTION(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vDiffuseTexture = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	vDiffuseTexture.a = vDiffuseTexture.r;

	Out.vDiffuse = vDiffuseTexture * g_vColor;
	Out.vDiffuse.rgb = Out.vDiffuse.rgb * 3.f;

	return Out;
}

//PS_DISTORTION_INTO
PS_OUT PS_DISTORTION_INTO(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vMaskTexture = g_MTexture_0.Sample(LinearSampler, In.vTexUV);
	vMaskTexture.a = vMaskTexture.r;

	/* Sprite */
	In.vTexUV.x = In.vTexUV.x + g_WidthFrame;
	In.vTexUV.y = In.vTexUV.y + g_HeightFrame;

	In.vTexUV.x = In.vTexUV.x / 1.f;
	In.vTexUV.y = In.vTexUV.y / 4.f;

	float  time = frac(g_Time * 0.3f);
	float2 OffsetUV = TilingAndOffset(float2(In.vTexUV.x , In.vTexUV.y / 3.f), float2(1.0f, 1.0f), float2(0.0f, time));

	vector vDiffuseTexture = g_DTexture_0.Sample(LinearSampler, OffsetUV);
	vDiffuseTexture.a = vDiffuseTexture.r;

	//float fTime = min(g_Time * 1.2f, 2.f);
	//if (1.f < fTime)
	//	vDiffuseTexture.a = vDiffuseTexture.a * (2.f - fTime);
	//else
	//	vDiffuseTexture.a = vDiffuseTexture.a * (fTime / 2.f);

	float4 finalcolor = lerp(vDiffuseTexture, vMaskTexture, vDiffuseTexture.r);
	float fAlpha = 1.0f - abs(In.vTexUV.y - 0.5f) * 2.f;

	Out.vDiffuse = finalcolor * g_vColor * 2.f;
	Out.vDiffuse.a = Out.vDiffuse.a * fAlpha;
	if (Out.vDiffuse.a < 0.1f)
		discard;
	Out.vDiffuse = CalcHDRColor(Out.vDiffuse, 1.5f);
	return Out;
}
//PS_KENADASH
PS_OUT PS_KENADASH(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vGradient = g_DTexture_1.Sample(LinearSampler, In.vTexUV);

	float  time = frac(g_Time );
	float2 OffsetUV = TilingAndOffset(float2(In.vTexUV.x, In.vTexUV.y / 3.f), float2(1.0f, 1.0f), float2(time, time));

	vector vDiffuseTexture = g_DTexture_0.Sample(LinearSampler, OffsetUV);
	vDiffuseTexture.a = vDiffuseTexture.r;

	float4 finalcolor = vDiffuseTexture + vGradient;
	finalcolor = saturate(finalcolor) * g_vColor;
	finalcolor.a = (vDiffuseTexture * vGradient).r * In.vTexUV.y;

	Out.vDiffuse = CalcHDRColor(finalcolor, g_fHDRValue);
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

	/***************** MAGE *****************/
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
	/***************** MAGE *****************/

	pass Effect_PulseCover // 10
	{
		SetRasterizerState(RS_CW);
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

	pass Effect_Shaman //12
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_SOCKET();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECTSHAMAN();
	}

	pass Swipes_Charged // 13
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

	/***************** W A R R I O R *****************/

	pass Fire_Swipes // 14
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FIRE_SWIPES();
	}

	pass Fire_Swipes_inner // 15
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FIRE_SWIPES_INNER();
	}

	pass Root // 16
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ROOT();
	}

	pass PlaneRoot // 17
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_PLANEROOT();
	}

	pass GrondShock // 18
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_GRONDSHOCK();
	}

	pass GrondPlane // 19
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_GRONDPLANE();
	}

	pass Enrage // 20
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ENRAGE();
	}
	/***************** W A R R I O R *****************/

	pass Distortion // 21
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION();
	}

	pass EnrageInto // 22
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION_INTO();
	}

	pass KenaDash // 23
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_KENADASH();
	}
}
