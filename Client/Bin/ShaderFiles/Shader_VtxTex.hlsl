
#include "Shader_Client_Defines.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_Texture;
texture2D		g_DepthTexture;
texture2D		g_MaskTexture;
texture2D		g_NoiseTexture;


int		g_XFrameNow = 0, g_YFrameNow = 0;	/* Current Sprite frame */
int		g_XFrames = 1, g_YFrames = 1;		/* the number of sprites devided each side */
float	g_fSpeedX = 0.f, g_fSpeedY = 0.f;	/* UV Move Speed */
float4	g_vColor = { 1.f, 1.f, 1.f, 1.f };
float4	g_vMinColor = { 0.f, 0.f, 0.f,0.f };

float	g_fAlpha = 1.f;
float	g_fAmount = 1.f; /* Guage Data (normalized) */

int		g_iCheck = 0;

float	g_Time;
unsigned int g_State = 0;

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

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_MAIN_MASKALPHATEST(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	Out.vColor.a = Out.vColor.r;

	if (Out.vColor.a < 0.1)
		discard;

	return Out;
}

PS_OUT PS_MAIN_EFFECT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	float2		vTexUV;

	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, vTexUV);

	float		fOldViewZ = vDepthDesc.y * 300.f;
	float		fViewZ = In.vProjPos.w;

	Out.vColor.a = Out.vColor.a * (saturate(fOldViewZ - fViewZ) * 2.5f);

	return Out;
}

PS_OUT PS_MAIN_AlphaBlend(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_MAIN_ALPHACHANGE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	Out.vColor.a *= g_fAlpha;

	return Out;
}

PS_OUT PS_MAIN_MASKMAP(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vDiffuse = g_Texture.Sample(LinearSampler, In.vTexUV);
	vector vMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);

	vDiffuse.a = vMask.r;

	Out.vColor = vDiffuse;

	return Out;
}

PS_OUT PS_MAIN_HPBar(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (In.vTexUV.x > 0.01 && In.vTexUV.x < 0.99)
		In.vTexUV.x = (In.vTexUV.x - 0.5) / 3 + 0.5;
	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_MAIN_SPRITE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	In.vTexUV.x = In.vTexUV.x + g_XFrameNow;
	In.vTexUV.y = In.vTexUV.y + g_YFrameNow;

	In.vTexUV.x = In.vTexUV.x / g_XFrames;
	In.vTexUV.y = In.vTexUV.y / g_YFrames;


	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_MAIN_UVMOVE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	In.vTexUV.x += g_fSpeedX;
	In.vTexUV.y += g_fSpeedY;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_MAIN_RINGGUAGE(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	float4  vDiffuse = g_Texture.Sample(LinearSampler, In.vTexUV);

	float3 center = float3(0.5f, 0.5f, 0);
	float3 top = float3(0.5f, 0, 0);
	float3 curUV = float3(In.vTexUV.xy, 0);
	float angle = 0;

	float3 centerToTop = top - center;
	float3 centerToCurUV = curUV - center;

	centerToTop = normalize(centerToTop);
	centerToCurUV = normalize(centerToCurUV);

	angle = acos(dot(centerToTop, centerToCurUV));
	angle = angle * (180.0f / 3.141592654f); // radian to degree

											 /* Check the angle is 0 ~ 180 or 180 ~ 360 */
	angle = (centerToTop.x * centerToCurUV.x - centerToTop.y * centerToCurUV.x > 0.0f) ? angle : (-angle) + 360.0f;

	float condition = 360 * g_fAmount;

	if (angle > condition)
		discard;

	Out.vColor = vDiffuse;

	return Out;

}

PS_OUT PS_MAIN_BARGUAGE(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;


	float4 vColor = g_vColor;
	vColor.r = g_vColor.r * (In.vTexUV.x + g_vMinColor.r); // 0.1f
	vColor.g = g_vColor.g * (In.vTexUV.x + g_vMinColor.g); // 0.3f
	vColor.b = g_vColor.b * (In.vTexUV.x + g_vMinColor.b); // 0.5f
	vColor.a = g_vColor.a;

	/* Discard pixel depends on original UV.x */
	if (In.vTexUV.x > g_fAmount)
		discard;

	In.vTexUV.x += g_fSpeedX;

	float4  vDiffuse = g_Texture.Sample(LinearSampler, In.vTexUV);

	Out.vColor = vDiffuse * vColor;

	return Out;

}

PS_OUT PS_MAIN_LOADING(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	//vector vDiffuse = g_Texture.Sample(LinearSampler, In.vTexUV);
	//vDiffuse.a = 1.f;

	In.vTexUV.x += g_fSpeedX;
	In.vTexUV.y += g_fSpeedX;

	vector vNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexUV * 10);

	vNoise.a = vNoise.r * vNoise.r;
	if (vNoise.a < 0.6)
		vNoise.a = 0;

	vNoise.a = vNoise.a * g_fAlpha;
	vNoise.a = saturate(vNoise.a);

	Out.vColor = vNoise;


	return Out;
}

PS_OUT PS_MAIN_RINGGUAGE_MASK(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	float4  vDiffuse = g_Texture.Sample(LinearSampler, In.vTexUV);
	float4	vMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);

	float3 center = float3(0.5f, 0.5f, 0);
	float3 top = float3(0.5f, 0, 0);
	float3 curUV = float3(In.vTexUV.xy, 0);
	float angle = 0;

	float3 centerToTop = top - center;
	float3 centerToCurUV = curUV - center;

	centerToTop = normalize(centerToTop);
	centerToCurUV = normalize(centerToCurUV);

	angle = acos(dot(centerToTop, centerToCurUV));
	angle = angle * (180.0f / 3.141592654f); // radian to degree

											 /* Check the angle is 0 ~ 180 or 180 ~ 360 */
	angle = (centerToTop.x * centerToCurUV.x - centerToTop.y * centerToCurUV.x > 0.0f) ? angle : (-angle) + 360.0f;

	float4 vColor = g_vColor;
	vColor.r = g_vColor.r * (In.vTexUV.x);
	vColor.g = g_vColor.g * (In.vTexUV.x);
	vColor.b = g_vColor.b * (In.vTexUV.x);
	vColor.a = g_vColor.a;
	Out.vColor = vDiffuse * vColor;
	Out.vColor += g_vMinColor;
	Out.vColor.a = vMask.r;


	float condition = 360 * g_fAmount;
	if (angle > condition)
		discard;


	return Out;

}

PS_OUT PS_MAIN_AimThings(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	if (g_iCheck == 0) /* empty */
	{
		Out.vColor.rgb = 0.5;
		Out.vColor.a *= 0.3;
	}

	return Out;
}
PS_OUT PS_MAIN_PAINTDROP(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	if (g_State == 1) /* Open */
	{
		if (Out.vColor.a <= g_Time)
			discard;
	}
	else if (g_State == 2) /* Close */
	{
		float2 vNewUV;
		vNewUV = In.vTexUV;

		vNewUV += 2;
		vNewUV /= 4;
		float4 gBigSize = g_Texture.Sample(LinearSampler, vNewUV);

		if (In.vTexUV.y >= 0.6)
		{
			if (Out.vColor.a <= g_Time)
				discard;
		}
		else
		{
			if (gBigSize.a <= g_Time)
				discard;

		}
	}

	Out.vColor.a *= g_fAlpha;
	return Out;
	//PS_OUT			Out = (PS_OUT)0;

	//float4  vDiffuse = g_Texture.Sample(LinearSampler, In.vTexUV);
	//   
	//   float fTime = sin(g_Time);
	//   float2 vDir = float2(0.5, 0.5) - In.vTexUV;
	//   float fDist = length(vDir);
	//vDir = normalize(vDir);
	//   float offset = fTime * fDist * 0.1;
	//   float4 sum = float4(0.0, 0.0, 0.0, 0.0);
	//   float weight = 0.0;
	//   for (int i = 0; i < 10; i++) {
	//       float2 offsetCoords = In.vTexUV + offset * vDir;
	//       float4 tex = g_Texture.Sample(LinearSampler, offsetCoords);
	//       float len = length(tex.rgb);
	//       float w = pow(1.0 - saturate(len), 2.0);
	//       sum += tex * w;
	//       weight += w;
	//       offset *= 1.1;
	//   }
	//   float4 finalColor = sum / weight;

	// 효과 적용
	//Out.vColor = lerp(vDiffuse, finalColor, 0.2);
	//return Out;
}

PS_OUT PS_MAIN_TRIAL(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	/* Sprite Animation  */
	In.vTexUV.x = In.vTexUV.x + g_XFrameNow;
	In.vTexUV.y = In.vTexUV.y + g_YFrameNow;

	In.vTexUV.x = In.vTexUV.x / g_XFrames;
	In.vTexUV.y = In.vTexUV.y / g_YFrames;

	/* UV Move */
	In.vTexUV.x += g_fSpeedX;
	In.vTexUV.y += g_fSpeedY;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	return Out;
}


technique11 DefaultTechnique
{
	pass Rect // 0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass DiffuseAlphaBlend // 1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_AlphaBlend();
	}

	pass MaskMap // 2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MASKMAP();
	}

	pass HPBar // 3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HPBar();
	}

	pass AlphaBlendSprites // 4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SPRITE();
	}

	pass DefaultUVMove // 5
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_UVMOVE();
	}

	pass RingGuage //6
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RINGGUAGE();
	}

	pass BarGuage //7
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BARGUAGE();
	}

	pass Loading //8
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LOADING();
	}

	pass RingGuageMask // 9
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RINGGUAGE_MASK();
	}

	pass AimThings // 10
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_AimThings();
	}

	pass PAINTDROP // 11
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PAINTDROP();
	}

	pass Mask_AlphaTest // 12
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MASKALPHATEST();
	}

	pass AlphaChange // 13
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHACHANGE();
	}

	pass Trial
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TRIAL();
	}
}
