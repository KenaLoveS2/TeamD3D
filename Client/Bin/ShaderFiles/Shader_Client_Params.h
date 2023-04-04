#ifndef __SHADER_CLIENT_PARAMS__
#define __SHADER_CLIENT_PARAMS__
//
//// matrix
//float4x4		g_WorldMatrix;
//float4x4		g_ViewMatrix;
//float4x4		g_ProjMatrix;
//float4x4		g_SocketMatrix;
//float4x4		g_WorldMatrixInv;

//float
//float      g_fFar = 500.f;
float		 g_fHDRIntensity = 0.f;
float		 g_fDissolveTime;
float		 _DissolveSpeed = 0.2f;
float		 _FadeSpeed = 1.5f;
float		 g_fUVSpeedX = 0.f;
float		 g_fUVSpeedY = 0.f;
float		 g_fStringDissolve;
float		 g_fStringHDR;
float		 g_fAlpha;
float		 g_fHDRItensity;
float      g_fStonePulseIntensity = 0.f;
float		 g_fHairLength = 1.f;
float		 g_fHairThickness = 1.f;
float		 g_fLashDensity = 0.5f;
float		 g_fLashWidth = 0.5f;
float		 g_fLashIntensity = 0.5f;
float		 g_fSSSAmount = 1.f;
float		 g_HitRimIntensity = 0.f;
float		 g_ParryRimIntensity = 0.f;
float		 g_Time;
float		 g_BowDurationTime;
float		 g_fHDRValue;
float      g_WidthFrame;
float		 g_HeightFrame;
float		 g_InfoSize;
float		 g_fWidth = 1.f;
float		 g_fLife;
float		 g_fTime;
float		 g_fBrushRange = 5.f;
float		 g_DissolveSpeed = 0.2f;
float		 g_FadeSpeed = 1.5f;
float		 g_fDissolveAlpha = 0.f;
float		 g_fColorIntensity;
float		 g_fHairTransparency = 2.f;
float		 g_fCutY;			 /* For. discard Y Range option */
float		 g_UVScaleX = 1.f;
float		 g_UVScaleY = 1.f;
float		 g_HpRatio;
float		 g_DamageDurationTime;
float		 g_WaveHeight;
float		 g_Speed;
float		 g_WaveFrequency;
float	 	 g_UVSpeed;
float		 g_fSpeedX = 0.f, g_fSpeedY = 0.f;	/* UV Move Speed */
float		g_fAmount = 1.f; /* Guage Data (normalized) */
float		g_fEnd = 0.f;
float		g_fDiffuseAlpha = 1.f;
float		g_fMaskAlpha = 0.f;

//float2
float2     g_fUV;
float2	  g_UV;

//int
int		g_XFrames = 1;
int		g_YFrames = 1;
int		g_XFrameNow = 0;
int		g_YFrameNow = 0;
int		g_TextureRenderType;
int		g_BlendType;
int		g_SeparateWidth;
int		g_SeparateHeight;
int		g_iCheck = 0;

//uint
uint	    g_iTotalDTextureComCnt;
uint		g_iTotalMTextureComCnt;
uint		g_PulseState;
uint	    g_State = 0;

//float4
//float4	g_vCamPosition;
float4	g_EmissiveColor = float4(1.f,1.f,1.f,1.f);
float4   g_vColor = float4(1.f,1.f,1.f,1.f);
float4	g_vAmbientEyeColor = float4(1.f, 1.f, 1.f, 1.f);
float4	g_vAmbientColor = float4(1.f, 1.f, 1.f, 1.f);
float4	g_vSSSColor = float4(1.f, 0.f, 0.f, 1.f);
float4   g_WorldCamPosition;
float4	g_vMaskColor = float4(1.f ,1.f, 1.f, 1.f );
float4	g_vDissolveColor = float4(1.f, 1.f, 1.f, 1.f );
float4	g_vBrushPos;
float4	g_vMinColor = float4(0.f, 0.f, 0.f,0.f);

//bool
bool		g_bDissolve;
bool		g_IsSpriteAnim = false;
bool		g_IsUVAnim = false;
bool		g_Hit = false;
bool		g_Parry = false;
bool		g_IsTrail;
bool     g_bDistanceAlpha;
bool	    g_bTimer;
bool     g_bTurn;
bool		g_IsUseMask;
bool		g_IsUseNormal;
bool		g_bPulseRecive = false;
bool g_IsGray = false;


//Texture2D
Texture2D<float4>	  g_Texture;
Texture2D<float4>      g_DiffuseTexture;
Texture2D<float4>      g_NormalTexture;
Texture2D<float4>      g_GlowTexture;
Texture2D<float4>      g_AO_R_MTexture;
Texture2D<float4>      g_EmissiveTexture;
Texture2D<float4>      g_EmissiveMaskTexture;
Texture2D<float4>      g_OpacityTexture;
Texture2D<float4>      g_AOTexture;
Texture2D<float4>      g_RoughnessTexture;
Texture2D<float4>      g_MaskTexture;
Texture2D<float4>      g_NoiseTexture;
Texture2D<float4>      g_ReamTexture;
Texture2D<float4>      g_LineTexture;
Texture2D<float4>      g_SmoothTexture;
Texture2D<float4>      g_ShapeMaskTexture;
Texture2D<float4>      g_DissolveTexture;
Texture2D<float4>      g_HRAOTexture;
Texture2D<float4>      g_MRAOTexture;
Texture2D<float4>      g_ERAOTexture;
Texture2D<float4>	  g_SSSMaskTexture;
Texture2D<float4>	  g_HairDepthTexture;
Texture2D<float4>	  g_HairAlphaTexture;
Texture2D<float4>	  g_HairRootTexture;
Texture2D<float4>	  g_DetailNormal;
Texture2D<float4>	  g_SwipeTexture;
Texture2D	<float4>	  g_GradientTexture;
Texture2D	<float4>	  g_DepthTexture;
Texture2D	<float4>	  g_DTexture_0, g_DTexture_1, g_DTexture_2, g_DTexture_3, g_DTexture_4;
Texture2D	<float4>	  g_MTexture_0, g_MTexture_1, g_MTexture_2, g_MTexture_3, g_MTexture_4;
Texture2D	<float4>	  g_WispflowTexture;
Texture2D	<float4>	  g_WispOutTexture;
Texture2D	<float4>	  g_FlowTexture;
Texture2D	<float4>	  g_TypeTexture;
Texture2D	<float4>	  g_CloudTexture;
Texture2D	<float4>	  g_ShapeTexture;
Texture2D<float4>	  g_BaseTexture;
Texture2D<float4>	  g_DiffuseTexture_0;
Texture2D<float4>	  g_DiffuseTexture_1;
Texture2D<float4>	  g_DiffuseTexture_2;
Texture2D<float4>	  g_BrushTexture;
Texture2D<float4>	  g_FilterTexture[3];
Texture2D<float4>      g_BlendDiffuseTexture;
Texture2D<float4>      g_DetailNormalTexture;
Texture2D<float4>	  g_MasterBlendDiffuseTexture;
Texture2D<float4>	  g_WPOTexture;
Texture2D<float4>	  g_DissolveAlphaTexture;
Texture2D<float4>	  g_AlphaTexture;

//TextureCUBE
TextureCube<float4>			   g_SkyTexture;
//#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#ifndef __SHADER_CLIENT_PARAMS__
//#define __SHADER_CLIENT_PARAMS__

// matrix
float4x4		g_WorldMatrix;
float4x4		g_ViewMatrix;
float4x4		g_ProjMatrix;
float4x4		g_SocketMatrix;
float4x4		g_WorldMatrixInv;

float4			g_vCamPosition;
float			g_fFar;

//int
int				g_int_0;
int				g_int_1;
int				g_int_2;
int				g_int_3;
int				g_int_4;

//uint
uint			g_uint_0;
uint			g_uint_1;
uint			g_uint_2;
uint			g_uint_3;
uint			g_uint_4;

//float
float			g_float_0;
float			g_float_1;
float			g_float_2;
float			g_float_3;
float			g_float_4;

//float2
float2			g_float2_0;
float2			g_float2_1;
float2			g_float2_2;
float2			g_float2_3;
float2			g_float2_4;

//float3
float3			g_float3_0;
float3			g_float3_1;
float3			g_float3_2;
float3			g_float3_3;
float3			g_float3_4;

//float4
float4			g_float4_0;
float4			g_float4_1;
float4			g_float4_2;
float4			g_float4_3;
float4			g_float4_4;

//bool
bool			g_bool_0;
bool			g_bool_1;
bool			g_bool_2;
bool			g_bool_3;
bool			g_bool_4;

//Texture2D
Texture2D<float4>		g_tex_0;
Texture2D<float4>		g_tex_1;
Texture2D<float4>		g_tex_2;
Texture2D<float4>		g_tex_3;
Texture2D<float4>		g_tex_4;

//TextureCUBE
//TextureCube<float4>			   g_SkyTexture;

#endif