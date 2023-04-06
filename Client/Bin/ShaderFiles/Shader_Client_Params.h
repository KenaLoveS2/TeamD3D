#ifndef __SHADER_CLIENT_PARAMS__
#define __SHADER_CLIENT_PARAMS__

// matrix
float4x4		g_WorldMatrix;
float4x4		g_ViewMatrix;
float4x4		g_ProjMatrix;
float4x4		g_SocketMatrix;
float4x4		g_WorldMatrixInv;

//int
int		g_int_0;
int		g_int_1;
int		g_int_2;
int		g_int_3;
int		g_int_4;

//uint
uint		g_uint_0;
uint		g_uint_1;
uint		g_uint_2;
uint		g_uint_3;
uint		g_uint_4;

//float
float		g_float_0;
float		g_float_1;
float		g_float_2;
float		g_float_3;
float		g_float_4;

//float2
float2     g_float2_0;
float2	  g_float2_1;
float2     g_float2_2;
float2     g_float2_3;
float2     g_float2_4;

//float3
float3     g_float3_0;
float3	  g_float3_1;
float3     g_float3_2;
float3     g_float3_3;
float3     g_float3_4;

//float4
float4	g_float4_0;
float4	g_float4_1;
float4	g_float4_2;
float4	g_float4_3;
float4	g_float4_4;

//bool
bool		g_bool_0;
bool		g_bool_1;
bool		g_bool_2;
bool		g_bool_3;
bool		g_bool_4;

//Texture2D
Texture2D<float4>		g_tex_0;
Texture2D<float4>      g_tex_1;
Texture2D<float4>      g_tex_2;
Texture2D<float4>      g_tex_3;
Texture2D<float4>      g_tex_4;

//TextureCUBE
TextureCube<float4>			   g_SkyTexture;
#endif