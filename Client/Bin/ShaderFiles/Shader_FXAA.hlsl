#include "Shader_Engine_Defines.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D<float4>	g_FinalTexture;

float g_TexelX = 1600.f;
float g_TexelY = 900.f;

#define FXAA_QUALITY_SUBPIX  0.75f
#define FXAA_QUALITY_EDGE_THRESHOLD 0.125f
#define FXAA_QUALITY_EDGE_THRESHOLD_MIN 0.0312f
#define FXAA_QUALITY_EDGE_THRESHOLD_MAX 0.25f
#define FXAA_SUBPIX_FLOOR  0.125f
#define FXAA_QUALITY_CORNER_SHARPNESS 1.f

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 texel_size = 1.0f / float2(g_TexelX, g_TexelY);
	float3 rgb_nw = g_FinalTexture.Sample(LinearSampler, In.vTexUV + float2(-1.0f, -1.0f) * texel_size).rgb;
	float3 rgb_n = g_FinalTexture.Sample(LinearSampler, In.vTexUV + float2(0.0f, -1.0f) * texel_size).rgb;
	float3 rgb_ne = g_FinalTexture.Sample(LinearSampler, In.vTexUV + float2(1.0f, -1.0f) * texel_size).rgb;
	float3 rgb_w = g_FinalTexture.Sample(LinearSampler, In.vTexUV + float2(-1.0f, 0.0f) * texel_size).rgb;
	float3 rgb_c = g_FinalTexture.Sample(LinearSampler, In.vTexUV).rgb;
	float3 rgb_e = g_FinalTexture.Sample(LinearSampler, In.vTexUV + float2(1.0f, 0.0f) * texel_size).rgb;
	float3 rgb_sw = g_FinalTexture.Sample(LinearSampler, In.vTexUV + float2(-1.0f, 1.0f) * texel_size).rgb;
	float3 rgb_s = g_FinalTexture.Sample(LinearSampler, In.vTexUV + float2(0.0f, 1.0f) * texel_size).rgb;
	float3 rgb_se = g_FinalTexture.Sample(LinearSampler, In.vTexUV + float2(1.0f, 1.0f) * texel_size).rgb;

	float3 luma = float3(0.299f, 0.587f, 0.114f);
	float luma_nw = dot(rgb_nw, luma);
	float luma_n = dot(rgb_n, luma);
	float luma_ne = dot(rgb_ne, luma);
	float luma_w = dot(rgb_w, luma);
	float luma_c = dot(rgb_c, luma);
	float luma_e = dot(rgb_e, luma);
	float luma_sw = dot(rgb_sw, luma);
	float luma_s = dot(rgb_s, luma);
	float luma_se = dot(rgb_se, luma);

	float4 weights = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 weights_luma = float4(luma_nw, luma_n, luma_w, luma_c);
	float4 weights_luma2 = float4(luma_e, luma_se, luma_sw, luma_s);

	float luma_min = min(luma_c, min(min(luma_n, luma_s), min(luma_w, luma_e)));
	float luma_max = max(luma_c, max(max(luma_n, luma_s), max(luma_w, luma_e)));

	float luma_range = luma_max - luma_min;

	float luma_max_nz = max(luma_max, 1e-5f);

	float edge_horz1 = (-2.0f * weights_luma.y) + weights_luma.z + (2.0f * weights_luma.w);
	float edge_horz2 = (-2.0f * weights_luma2.x) + weights_luma2.z + (2.0f * weights_luma2.y);
	float edge_horz = abs(edge_horz1) + abs(edge_horz2);

	float edge_vert1 = (-2.0f * weights_luma.z) + weights_luma.x + (2.0f * weights_luma.w);
	float edge_vert2 = (-2.0f * weights_luma2.y) + weights_luma2.w + (2.0f * weights_luma2.z);
	float edge_vert = abs(edge_vert1) + abs(edge_vert2);

	bool horz_span = edge_horz >= edge_vert;
	float length_span = horz_span ? edge_horz : edge_vert;
	float inv_span_length = 1.0f / max(length_span, 1e-5f);

	bool antialias_this_pixel = length_span >= luma_range * FXAA_QUALITY_EDGE_THRESHOLD + FXAA_QUALITY_EDGE_THRESHOLD_MIN;

	float2 dir = horz_span ? float2(texel_size.x, 0.0f) : float2(0.0f, texel_size.y);

	float4 weights_dir = float4(-dir, dir);
	float4 weights_value = float4(luma_n, luma_e, luma_w, luma_s);

	float4 weights_sign = float4(1.0f, -1.0f, -1.0f, 1.0f);

	float4 subpix = float4(FXAA_QUALITY_SUBPIX, FXAA_QUALITY_SUBPIX, FXAA_QUALITY_SUBPIX, FXAA_QUALITY_SUBPIX);
	subpix *= float4(FXAA_SUBPIX_FLOOR, FXAA_SUBPIX_FLOOR, FXAA_SUBPIX_FLOOR, FXAA_SUBPIX_FLOOR);

	float4 final_weights = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 weights_scaled = weights_luma * float4(1.0f / luma_max_nz, 1.0f / luma_max_nz, 1.0f / luma_max_nz, 1.0f / luma_max_nz);
	weights_scaled *= weights_scaled;

	final_weights = weights_scaled + FXAA_QUALITY_EDGE_THRESHOLD;
	weights_scaled = weights_luma2 * float4(1.0f / luma_max_nz, 1.0f / luma_max_nz, 1.0f / luma_max_nz, 1.0f / luma_max_nz);
	weights_scaled *= weights_scaled;
	final_weights += weights_scaled + FXAA_QUALITY_EDGE_THRESHOLD;

	float4 weights_corner = float4(0.0f, 0.0f, 0.0f, 0.0f);
	weights_corner.x = weights_luma.x + weights_luma.y;
	weights_corner.y = weights_luma.z + weights_luma.w;
	weights_corner.z = weights_luma.z + weights_luma.y;
	weights_corner.w = weights_luma.x + weights_luma.w;
	weights_corner *= float4(1.0f / (luma_max_nz * luma_max_nz), 1.0f / (luma_max_nz * luma_max_nz), 1.0f / (luma_max_nz * luma_max_nz), 1.0f / (luma_max_nz * luma_max_nz));

	final_weights += weights_corner * float4(FXAA_QUALITY_CORNER_SHARPNESS, FXAA_QUALITY_CORNER_SHARPNESS, FXAA_QUALITY_CORNER_SHARPNESS, FXAA_QUALITY_CORNER_SHARPNESS);

	float4 weights_scaled_dir = weights_scaled * weights_dir;
	float4 weights_value_dir = weights_value * weights_dir;

	float3 color_accum = 0.0f;
	float weight_accum = 0.0f;

	for (int i = -2; i <= 2; i++) {
		float2 offset = dir * float(i);

		float4 scaled_dir = weights_scaled_dir + float4(offset, offset);
		float4 color_dir = g_FinalTexture.Sample(LinearSampler, In.vTexUV + scaled_dir.xy);

		float3 color_dir3 = color_dir.xyz;
		float4 color_dir4 = float4(color_dir3, 1.0f);
		color_dir4 *= weights_value_dir + float4(offset.x, offset.x, offset.y, offset.y);

		float luma_dir = dot(color_dir3, luma);

		float weight_dir = max(0.0f, 1.0f - (abs(luma_dir - luma_n) * inv_span_length));
		weight_dir *= weight_dir;

		bool luma_condition = (luma_dir < luma_n) != horz_span;

		bool weight_condition = (weight_dir * (FXAA_QUALITY_EDGE_THRESHOLD * FXAA_QUALITY_EDGE_THRESHOLD)) > final_weights.x;

		if (luma_condition && weight_condition) {
			color_accum += color_dir3 * weight_dir;
			weight_accum += weight_dir;
		}
	}
	float3 final_color = color_accum / max(weight_accum, 1e-5f);

	Out.vColor = float4(final_color, 1.0f);
	return Out;
}

technique11 DefaultTechnique
{
	pass FXAA
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
