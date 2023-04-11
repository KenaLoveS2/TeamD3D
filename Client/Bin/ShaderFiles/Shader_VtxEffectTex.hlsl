#include "Shader_Client_Defines.h"

/**********Constant Buffer*********/
float4			g_WorldCamPosition;
float			g_fHDRValue;
/**********************************/

/**********Common Texture*********/
texture2D		g_DepthTexture;
texture2D		g_DTexture_0, g_DTexture_1, g_DTexture_2, g_DTexture_3, g_DTexture_4;
texture2D		g_MTexture_0, g_MTexture_1, g_MTexture_2, g_MTexture_3, g_MTexture_4;
/**********************************/

/**********Common Option*********/
int		g_TextureRenderType, g_BlendType;
bool    g_IsUseMask;
int		g_SeparateWidth, g_SeparateHeight;
uint	g_iTotalDTextureComCnt, g_iTotalMTextureComCnt;
float   g_WidthFrame, g_HeightFrame;
float4  g_vColor;
float   g_Time;
float2  g_fUV;
bool	g_bTimer;
/**********************************/

/* HpRatio */
float   g_HpRatio;
float	g_DamageDurationTime;
uint	g_PulseState;
/* HpRatio */


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
	float3      vViewDir : TEXCOORD2;
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

	float4 WorldPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vViewDir = normalize(WorldPosition.xyz - g_WorldCamPosition.xyz);

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float3      vViewDir : TEXCOORD2;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 UV = In.vTexUV;
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
	//if (g_BlendType == 2 || g_BlendType == 3)
	//	Out.vColor = Out.vColor * g_vColor;
	//else
	//	Out.vColor = (Out.vColor * g_vColor) + (Out.vColor * (1.f - g_vColor));

	Out.vColor = Out.vColor * g_vColor;
	Out.vColor.a = Out.vColor.r;
	Out.vColor = saturate(Out.vColor.r* g_vColor) * 4.5f;
	Out.vColor.rgb = Out.vColor.rgb * 4.f;

	if (g_IsUseMask == true)
	{
		if (g_iTotalMTextureComCnt == 1)
		{
			vector maskTex0 = g_MTexture_0.Sample(LinearSampler, UV);
			Out.vColor = Out.vColor * maskTex0;
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

			vector maskTex = saturate(maskTex0 * maskTex1* maskTex2* maskTex3* maskTex4* 4.0f);
			Out.vColor = Out.vColor * maskTex * g_vColor;
		}
	}
	else
		Out.vColor = Out.vColor * g_vColor;

	if (Out.vColor.a < 0.1f)
		discard;

	Out.vColor = CalcHDRColor(Out.vColor, g_fHDRValue);

	if (g_bTimer)
	{
		float fTime = min(g_Time, 1.f);

		if (0.5f < fTime)   // Time 이상 컬러값이 내려가야함
			Out.vColor.a = Out.vColor.a * (1.f - fTime);
		else // Time 이하 컬러값이 올라가야함
			Out.vColor.a = Out.vColor.a * (fTime / 1.f);
	}

	return Out;
}

float2 Distort(float2 uv , float time)
{
	float2 displacementCoords =uv * 1.f + time * 1.f;
	//float4 displacementColor = displacementMap.Sample(sampleType, displacementCoords);
	float2 displacementVector = displacementCoords * 1.f;
	return uv += displacementVector;
}

PS_OUT PS_MAIN_E_PULSECLOUD(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector maskTex = g_MTexture_0.Sample(LinearSampler, In.vTexUV);
	maskTex.a = maskTex.r;
	if (maskTex.r >= 0.035f && maskTex.g >= 0.035f && maskTex.b >= 0.035f)
		discard;

	// Sprite
	In.vTexUV.x = In.vTexUV.x + g_WidthFrame;
	In.vTexUV.y = In.vTexUV.y + g_HeightFrame;

	In.vTexUV.x = In.vTexUV.x / g_SeparateWidth;
	In.vTexUV.y = In.vTexUV.y / g_SeparateHeight;

	// DTexture
	vector albedo = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	albedo.a = albedo.r;
	
	float4 finalcolor = lerp(albedo, maskTex, maskTex.r);
	finalcolor = finalcolor * float4(3.f, 208.f, 255.f, 84.f) / 255.f * 2.f;
	if (finalcolor.a < 0.01f)
		discard;

	float	fHpRatio = max(0.f, g_HpRatio);
	float	fColorRatio;
	vector	vColor;

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

	Out.vColor = finalcolor * g_fHDRValue;
	return Out;
}

PS_OUT PS_MAIN_E_CHARGING(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector albedo = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	albedo.a = albedo.r;
	albedo.a = albedo.g * 0.55;

	albedo.rgb = g_vColor.rgb * 1.3f;

	Out.vColor = albedo;
	return Out;
}

PS_OUT PS_MAIN_E_CHARGING_LIGHT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector albedo = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	albedo.a = albedo.r;
	albedo.rgb = float3(1.f, 1.f, 1.f) * 3.f;
	albedo = albedo * g_vColor;
	//albedo.a = g_vColor.a;
	if (albedo.a < 0.1f)
		discard;

	float fTime = min(g_Time, 1.f);
	if (0.5f < fTime)   // Time 이상 컬러값이 내려가야함
		albedo = albedo * (1.f - fTime);
	else // Time 이하 컬러값이 올라가야함
		albedo = albedo * (fTime / 1.f);
	
	Out.vColor = albedo;
	return Out;
}

PS_OUT PS_MAIN_E_ENEMYZONE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 OffsetUV = TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(0.5f, 0.5f));

	vector vMask = g_DTexture_0.Sample(LinearSampler, OffsetUV); // mask
	vector vBlur = g_DTexture_1.Sample(LinearSampler, In.vTexUV);    
	vector vLine = g_DTexture_2.Sample(LinearSampler, In.vTexUV);    

	float4 finalcolor = saturate(vMask + vBlur + vLine);
	finalcolor.a = finalcolor.r;
	if (finalcolor.a < 0.1)
		discard;

	Out.vColor = finalcolor;
	return Out;
}
//PS_MAIN_E_HIT / PS_MAIN_E_DAMAGE
PS_OUT PS_MAIN_E_HIT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	/* Sprite */
	In.vTexUV.x = In.vTexUV.x + g_WidthFrame;
	In.vTexUV.y = In.vTexUV.y + g_HeightFrame;

	In.vTexUV.x = In.vTexUV.x / g_SeparateWidth;
	In.vTexUV.y = In.vTexUV.y / g_SeparateHeight;
	/* Sprite */

	/* DiffuseTexture */
	vector albedo = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	float3 vColor = float3(215.f, 174.f, 129.f) / 255.f;
	albedo.rgb = vColor * 1.7f;
	Out.vColor = albedo;

	if (Out.vColor.a < 0.1f)
		discard;
	return Out;
}

PS_OUT PS_MAIN_E_DAMAGE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	/* Sprite */
	In.vTexUV.x = In.vTexUV.x + g_WidthFrame;
	In.vTexUV.y = In.vTexUV.y + g_HeightFrame;

	In.vTexUV.x = In.vTexUV.x / g_SeparateWidth;
	In.vTexUV.y = In.vTexUV.y / g_SeparateHeight;
	/* Sprite */

	/* DiffuseTexture */
	vector albedo = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	albedo.rgb = (albedo.rgb * g_vColor.rgb) * 2.f;
	albedo.b = 10.f;

	float fTIme = min(g_Time, 1.f);
	if (In.vTexUV.y < fTIme)
		albedo.a *= (1.f - fTIme);

	Out.vColor = albedo;
	return Out;
}

//PS_MAIN_E_JUMP 7
PS_OUT PS_MAIN_E_JUMP(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	/* Sprite */
	In.vTexUV.x = In.vTexUV.x + g_WidthFrame;
	In.vTexUV.y = In.vTexUV.y + g_HeightFrame;

	In.vTexUV.x = In.vTexUV.x / g_SeparateWidth;
	In.vTexUV.y = In.vTexUV.y / g_SeparateHeight;
	/* Sprite */

	/* DiffuseTexture */
	vector albedo = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
//	float3 vColor = g_vColor.rgb;
//	albedo.rgb = vColor * 1.2f;
	Out.vColor = albedo * g_vColor * 1.5f;
	return Out;
}

//PS_MAIN_E_HEAVYATTACK
PS_OUT PS_MAIN_E_HEAVYATTACK(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	/* Sprite */
	In.vTexUV.x = In.vTexUV.x + g_WidthFrame;
	In.vTexUV.y = In.vTexUV.y + g_HeightFrame;

	In.vTexUV.x = In.vTexUV.x / g_SeparateWidth;
	In.vTexUV.y = In.vTexUV.y / g_SeparateHeight;
	/* Sprite */

	/* DiffuseTexture */
	vector albedo = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	albedo.a = albedo.r * 0.3f;

	float3 vColor = float3(15.f, 130.f, 190.f) / 255.f;
	albedo.rgb = float3(1.f, 1.f, 1.f) * 3.f;
	albedo = albedo * g_vColor;
	if (albedo.a < 0.01f)
		discard;

	// Out.vColor = albedo;
	Out.vColor = CalcHDRColor(albedo, g_fHDRValue);

	return Out;
}

//PS_MAIN_E_SPRITE
PS_OUT PS_MAIN_E_SPRITE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	/* Sprite */
	In.vTexUV.x = In.vTexUV.x + g_WidthFrame;
	In.vTexUV.y = In.vTexUV.y + g_HeightFrame;

	In.vTexUV.x = In.vTexUV.x / g_SeparateWidth;
	In.vTexUV.y = In.vTexUV.y / g_SeparateHeight;
	/* Sprite */

	/* DiffuseTexture */
	vector albedo = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	albedo.a = albedo.r;
	albedo.a = albedo.g * 0.55;

	float4 vColor = float4(99.f, 248.f, 255.f, 255.f) / 255.f;
	albedo.rgb = vColor.rgb * 1.3f;
	if (albedo.a < 0.1f)
		discard;

	Out.vColor = albedo;
	return Out;
}

//PS_MAIN_E_ONETEXTURE
PS_OUT PS_MAIN_E_ONETEXTURE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	/* DiffuseTexture */
	vector albedo = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	albedo.a = albedo.r;

	albedo.rgb = g_vColor.rgb * 2.f;
	Out.vColor = albedo;
	return Out;
}

//PS_MAIN_E_BLINK
PS_OUT PS_MAIN_E_BLINK(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	/* Sprite */
	In.vTexUV.x = In.vTexUV.x + g_WidthFrame;
	In.vTexUV.y = In.vTexUV.y + g_HeightFrame;

	In.vTexUV.x = In.vTexUV.x / g_SeparateWidth;
	In.vTexUV.y = In.vTexUV.y / g_SeparateHeight;

	/* DiffuseTexture */
	vector vDiffuse = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	vDiffuse.a = vDiffuse.r;
	//if (vDiffuse.a < 0.1f)
	//	discard;

	Out.vColor = vDiffuse;
	Out.vColor = Out.vColor * g_vColor;
	Out.vColor = saturate(Out.vColor.r * g_vColor) * 2.5f;
	Out.vColor.rgb = Out.vColor.rgb * 2.f;

	float fTime = min(g_Time, 3.f);
	if (1.5f < fTime)   // Time 이상 컬러값이 내려가야함
		Out.vColor.a = Out.vColor.a * (3.f - fTime);
	else // Time 이하 컬러값이 올라가야함
		Out.vColor.a = Out.vColor.a * (fTime / 3.f);

	return Out;
}

//PS_MAIN_E_BLINKSPRITE
PS_OUT PS_MAIN_E_BLINKSPRITE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	/* Sprite */
	In.vTexUV.x = In.vTexUV.x + g_WidthFrame;
	In.vTexUV.y = In.vTexUV.y + g_HeightFrame;

	In.vTexUV.x = In.vTexUV.x / g_SeparateWidth;
	In.vTexUV.y = In.vTexUV.y / g_SeparateHeight;

	/* DiffuseTexture */
	vector vDiffuse = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	vDiffuse.a = vDiffuse.r;

	Out.vColor = vDiffuse + g_vColor;
	Out.vColor = saturate(Out.vColor);
	Out.vColor.a = (vDiffuse * g_vColor).a;

	return Out;
}

//PS_MAIN_E_CULLOFF
PS_OUT PS_MAIN_E_CULLOFF(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	/* DiffuseTexture */
	vector vDiffuse = g_DTexture_0.Sample(LinearSampler, In.vTexUV);
	vDiffuse.a = vDiffuse.r;

	Out.vColor = vDiffuse + g_vColor;
	Out.vColor = saturate(Out.vColor);
	Out.vColor.a = (vDiffuse * g_vColor).a;
	
	Out.vColor = CalcHDRColor(Out.vColor, g_fHDRValue);
	if (Out.vColor.a < 0.1f)
		discard;

	if (g_bTimer)
	{
		float fTime = min(g_Time, 1.f);

		if (0.5f < fTime)   // Time 이상 컬러값이 내려가야함
			Out.vColor.a = Out.vColor.a * (1.f - fTime);
		else // Time 이하 컬러값이 올라가야함
			Out.vColor.a = Out.vColor.a * (fTime / 1.f);
	}

	return Out;
}

technique11 DefaultTechnique
{
	pass Effect_Dafalut // 0
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

	pass Effect_PulseCloud // 1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_E_PULSECLOUD();
	}

	pass Effect_Charging // 2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_E_CHARGING();
	}

	pass Effect_ChargingCenterLight // 3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_E_CHARGING_LIGHT();
	}

	pass Effect_EnemyZone // 4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_E_ENEMYZONE();
	}

	pass Effect_Hit // 5
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_E_HIT();
	}

	pass Effect_Damage // 6
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_E_DAMAGE();
	}

	pass Effect_Jump // 7
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_E_JUMP();
	}

	pass Effect_HeavyAttack // 8
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_E_HEAVYATTACK();
	}

	pass Effect_Sprite // 9
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_E_SPRITE();
	}

	pass Effect_OneTexture // 10
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_E_ONETEXTURE();
	}

	pass Effect_Blink // 11
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_E_BLINK();
	}

	pass Effect_BlinkSprite // 12
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_E_BLINKSPRITE();
	}

	pass Effect_CullOff // 13
	{
		SetRasterizerState(RS_CULLNONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_E_CULLOFF();
	}

}
