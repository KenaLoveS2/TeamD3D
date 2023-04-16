
#include "Shader_Client_Defines.h"

/**********Constant Buffer*********/
float          g_fFar = 500.f;
float          g_fStonePulseIntensity = 0.f;
/**********************************/

Texture2D<float4>      g_DiffuseTexture;
Texture2D<float4>      g_NormalTexture;
Texture2D<float4>      g_EmissiveTexture;

Texture2D<float4>      g_HRAOTexture;
Texture2D<float4>      g_MRAOTexture;
Texture2D<float4>      g_ERAOTexture;

Texture2D<float4>      g_BlendDiffuseTexture;
Texture2D<float4>      g_DetailNormalTexture;
Texture2D<float4>      g_MaskTexture;
Texture2D<float4>       g_RoughnessTexture;

/* Dissolve */
Texture2D<float4>	g_DissolveTexture;
bool				g_bDissolve;
float				g_fDissolveTime;
float				_DissolveSpeed = 0.2f;
float				_FadeSpeed = 1.5f;


float                   g_TimeDelta;

float4                  g_CenterPos;
float4                  vColorData;
int                     g_iSign = 1;



float4                  g_FirstColor;
float                   g_FirstRatio =1.f;
float4                  g_SecondColor;
float                   g_SecondRatio =1.f;


float4                  g_fShineColor;
float                   g_fCycle_Interval;
float                   g_fShine_Speed;
float                   g_fShine_Width;





struct VS_IN
{
    float3      vPosition : POSITION;
    float3      vNormal : NORMAL;
    float2      vTexUV : TEXCOORD0;
    float3      vTangent : TANGENT;
};

struct VS_OUT
{
    float4      vPosition : SV_POSITION;
    float4      vNormal : NORMAL;
    float2      vTexUV : TEXCOORD0;
    float4      vProjPos : TEXCOORD1;
    float4      vTangent : TANGENT;
    float3      vBinormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT      Out = (VS_OUT)0;

    matrix      matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexUV = In.vTexUV;
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));

    return Out;
}

VS_OUT VS_MAIN_SOCKET(VS_IN In)
{
    VS_OUT      Out = (VS_OUT)0;

    matrix      matVP = mul(g_ViewMatrix, g_ProjMatrix);

    vector      vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_SocketMatrix);

    vector      vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
    vNormal = mul(vNormal, g_SocketMatrix);

    vector      vTangent = mul(float4(In.vTangent, 0.f), g_WorldMatrix);
    vTangent = mul(vTangent, g_SocketMatrix);

    Out.vPosition = mul(vPosition, matVP);
    Out.vNormal = normalize(vNormal);
    Out.vTexUV = In.vTexUV;
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(vTangent);
    Out.vBinormal = normalize(cross(vNormal.xyz, vTangent.xyz));

    return Out;
}

struct VS_IN_INSTANCE
{
    float3      vPosition : POSITION;
    float3      vNormal : NORMAL;
    float2      vTexUV : TEXCOORD0;
    float3      vTangent : TANGENT;
    float4      vRight : TEXCOORD1;
    float4      vUp : TEXCOORD2;
    float4      vLook : TEXCOORD3;
    float4      vTranslation : TEXCOORD4;
};

struct VS_OUT_INSTANCE
{
    float4      vPosition : SV_POSITION;
    float4      vNormal : NORMAL;
    float2      vTexUV : TEXCOORD0;
    float4      vProjPos : TEXCOORD1;
    float4      vTangent : TANGENT;
    float3      vBinormal : BINORMAL;
};

VS_OUT_INSTANCE VS_MAIN_INSTANCE(VS_IN_INSTANCE In)
{
    VS_OUT_INSTANCE      Out = (VS_OUT_INSTANCE)0;

    matrix      matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    float4x4   Transform = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

    vector      vPosition = mul(float4(In.vPosition, 1.f), Transform);
    vector      vNormal = mul(float4(In.vNormal, 0.f), Transform);
    vector      vTangent = mul(float4(In.vTangent.xyz, 0.f), Transform);



    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(float4(vNormal.xyz, 0.f), g_WorldMatrix));
    Out.vTexUV = In.vTexUV;
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(vTangent, g_WorldMatrix));
    Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));

    return Out;
}


struct VS_OUT_INSTANCE_GEOMETRY
{
    float3      vPosition : POSITION;
    float4      vNormal : NORMAL;
    float2      vTexUV : TEXCOORD0;
    float4      vProjPos : TEXCOORD1;
    float4      vTangent : TANGENT;
    float3      vBinormal : BINORMAL;
};

//float3 CalculateInstanceCenter(VS_IN_INSTANCE In, float4x4  Transform)
//{
//    float3 center = float3(0, 0, 0);
//    int count = 0;
//
//    for (int i = 0; i < g_InstasncingCount; ++i)
//    {
//        float4x4 instanceTransform = g_InstanceTransforms[i];
//        float3 position = mul(float4(In.vPosition, 1.0f), mul(Transform, instanceTransform)).xyz;
//        center += position;
//        count++;
//    }
//
//    return center / count;
//}

VS_OUT_INSTANCE_GEOMETRY VS_MAIN_INSTANCE_GEOMETRY(VS_IN_INSTANCE In)
{
    VS_OUT_INSTANCE_GEOMETRY      Out = (VS_OUT_INSTANCE_GEOMETRY)0;


    float4x4   Transform = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

    vector      vRealPosition = mul(float4(In.vPosition, 1.f), Transform);
    vector      vNormal = mul(float4(In.vNormal, 0.f), Transform);
    vector      vTangent = mul(float4(In.vTangent.xyz, 0.f), Transform);



   float moveDistance = g_TimeDelta *5.f* 1.f ;
 
   float3 vDir;
   if(g_iSign == 1)
   {
       vDir = normalize(In.vPosition.xyz - g_CenterPos.xyz);
   }
   else
   {
       vDir = normalize(g_CenterPos.xyz - In.vPosition.xyz);
   }
   // float3 vDir = normalize(g_CenterPos -In.vPosition.xyz);
   float distance = length(In.vPosition.xyz - g_CenterPos.xyz);
	float3 CenterPos = mul(g_CenterPos.xyz, (float3x3)Transform);
    float3 vPosition = In.vPosition.xyz + (vDir * moveDistance );


   // float3 vPosition = In.vPosition.xyz + (  0.5f *  moveDistance);   //
   

    vPosition = mul(vPosition.xyz, (float3x3)Transform);
   
    //vRealPosition = mul(vRealPosition, g_WorldMatrix);
	vPosition.y = vRealPosition.y;

    Out.vPosition = vPosition.xyz;  //mul(vPosition, g_WorldMatrix);
    Out.vNormal = normalize(mul(float4(vNormal.xyz, 0.f), g_WorldMatrix));
    Out.vTexUV = In.vTexUV;
    Out.vProjPos =vector(Out.vPosition,1.f);
    Out.vTangent = normalize(mul(vTangent, g_WorldMatrix));
    Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));

    return Out;
}


VS_OUT_INSTANCE_GEOMETRY VS_GEOMETRY_NON_INST(VS_IN In)
{
    VS_OUT_INSTANCE_GEOMETRY      Out = (VS_OUT_INSTANCE_GEOMETRY)0;

    float fOriginYPos = In.vPosition.y;
    float distance = length(In.vPosition.xyz - g_CenterPos.xyz);


    float3 vDirPos = In.vPosition.xyz;
    vDirPos.y += 1;
    vDirPos.z += 1;

    float3 vDir = normalize(vDirPos - In.vPosition.xyz);
    float moveDistance = g_TimeDelta * 3.f * 1.f;
    float3 vPosition = In.vPosition.xyz + (vDir * moveDistance);

    


    Out.vPosition = vPosition;
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexUV = In.vTexUV;
    Out.vProjPos = float4(Out.vPosition, 1.f);
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));

    return Out;
}

struct GS_IN
{
    float3      vPosition : POSITION;
    float4      vNormal : NORMAL;
    float2      vTexUV : TEXCOORD0;
    float4      vProjPos : TEXCOORD1;
    float4      vTangent : TANGENT;
    float3      vBinormal : BINORMAL;
};

struct GS_OUT
{
    float4      vPosition : SV_POSITION;
    float4      vNormal : NORMAL;
    float2      vTexUV : TEXCOORD0;
    float4      vProjPos : TEXCOORD1;
    float4      vTangent : TANGENT;
    float3      vBinormal : BINORMAL;
};


[maxvertexcount(11)]
void GS_MAIN(point GS_IN In[1], inout PointStream<GS_OUT> Stream)
{
    GS_OUT Out = (GS_OUT)0;

    matrix      matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    //matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

    // 1
    Out.vPosition = mul(float4(In[0].vPosition, 1.0f), matWVP);
    Out.vNormal = In[0].vNormal;
    Out.vTexUV = In[0].vTexUV;
    Out.vProjPos = Out.vPosition;
    Out.vTangent = In[0].vTangent;
    Out.vBinormal = In[0].vBinormal;
    Stream.Append(Out);

    // 2
    Out.vPosition =    mul(float4(In[0].vPosition + float3(0.2f, 0.0f, 0.0f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
	Stream.Append(Out);

    // 3
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.0f, 0.2f, 0.0f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
	Stream.Append(Out);

    // 4
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.0f, 0.0f, 0.2f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    // 5
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.2f, 0.0f, 0.2f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    // 6
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.2f, 0.2f, 0.f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    // 7
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.0f, 0.2f, 0.2f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    // 8
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.2f, 0.2f, 0.2f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    // 9
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.3f, 0.f, 0.f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    // 10
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.f, 0.3f, 0.f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;

    // 11
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.f, 0.f, 0.3f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

 
    Stream.RestartStrip();
}


[maxvertexcount(20)]
void GS_BossAttack(point GS_IN In[1], inout PointStream<GS_OUT> Stream)
{
    GS_OUT Out = (GS_OUT)0;

    matrix      matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    //matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

    // 1
    Out.vPosition = mul(float4(In[0].vPosition, 1.0f), matWVP);
    Out.vNormal = In[0].vNormal;
    Out.vTexUV = In[0].vTexUV;
    Out.vProjPos = Out.vPosition;
    Out.vTangent = In[0].vTangent;
    Out.vBinormal = In[0].vBinormal;
    Stream.Append(Out);

    // 2
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.2f, 0.0f, 0.0f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    // 3
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.0f, 0.2f, 0.0f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    // 4
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.0f, 0.0f, 0.2f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    // 5
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.2f, 0.0f, 0.2f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    // 6
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.2f, 0.2f, 0.f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    // 7
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.0f, 0.2f, 0.2f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    // 8
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.2f, 0.2f, 0.2f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    // 9
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.3f, 0.f, 0.f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    // 10
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.f, 0.3f, 0.f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;

    // 11
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.f, 0.f, 0.3f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    // 12
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.3f, 0.3f, 0.f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    // 13
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.3f, 0.f, 0.3f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    // 14
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.3f, 0.3f, 0.3f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    // 15
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.4f, 0.0f, 0.0f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    // 16
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.0f, 0.4f, 0.0f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    // 17
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.0f, 0.0f, 0.4f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    // 18
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.4f, 0.4f, 0.0f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    // 19
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.4f, 0.0f, 0.4f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    // 20
    Out.vPosition = mul(float4(In[0].vPosition + float3(0.0f, 0.4f, 0.4f), 1.0f), matWVP);
    Out.vProjPos = Out.vPosition;
    Stream.Append(Out);

    Stream.RestartStrip();
}

struct PS_IN
{
    float4      vPosition : SV_POSITION;
    float4      vNormal : NORMAL;
    float2      vTexUV : TEXCOORD0;
    float4      vProjPos : TEXCOORD1;
    float4      vTangent : TANGENT;
    float3      vBinormal : BINORMAL;
};

struct PS_OUT
{
    float4      vDiffuse : SV_TARGET0;
    float4      vNormal : SV_TARGET1;
    float4      vDepth : SV_TARGET2;
    float4      vAmbient : SV_TARGET3;
};

PS_OUT PS_MAIN_TESS(PS_IN In)
{
    PS_OUT      Out = (PS_OUT)0;

    vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

    if (0.1f > vDiffuse.a)
        discard;

    vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

    float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
    float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, WorldMatrix));

    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
    Out.vAmbient = (vector)1.f;
    return Out;
}//1

PS_OUT PS_MAIN_H_R_AO(PS_IN In)
{
    PS_OUT      Out = (PS_OUT)0;

    vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    vector      vHRAODesc = g_HRAOTexture.Sample(LinearSampler, In.vTexUV);

    if (0.1f > vDiffuse.a)
        discard;

    float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
    float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, WorldMatrix));
    float4 vAORM = float4(vHRAODesc.b, vHRAODesc.g, vHRAODesc.r, vHRAODesc.a);

    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
    Out.vAmbient = vAORM;
    return Out;
}//2

PS_OUT PS_MAIN_MRAO_E(PS_IN In)
{
    PS_OUT      Out = (PS_OUT)0;

    vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    vector      vMRAODesc = g_MRAOTexture.Sample(LinearSampler, In.vTexUV);
    vector      vEmissiveDesc = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);

    if (0.1f > vDiffuse.a)
        discard;

    float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
    float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, WorldMatrix));
    float4 vAORM = float4(vMRAODesc.b, vMRAODesc.g, vMRAODesc.r, vMRAODesc.a);

    float4   FinalColor = vDiffuse + (vEmissiveDesc * g_fStonePulseIntensity);
    FinalColor.a = vDiffuse.a;
    Out.vDiffuse = FinalColor;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.3f, 0.f);
    Out.vAmbient = vAORM;
    return Out;
}//3

PS_OUT PS_MAIN_HRAO_E(PS_IN In)
{
    PS_OUT      Out = (PS_OUT)0;

    vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    vector      vHRAODesc = g_HRAOTexture.Sample(LinearSampler, In.vTexUV);
    vector      vEmissiveDesc = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);

    if (0.1f > vDiffuse.a)
        discard;

    float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
    float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, WorldMatrix));
    float4 vAORM = float4(vHRAODesc.b, vHRAODesc.g, vHRAODesc.r, vHRAODesc.a);

    float4   FinalColor = vDiffuse + (vEmissiveDesc * g_fStonePulseIntensity);
    FinalColor.a = vDiffuse.a;
    Out.vDiffuse = FinalColor;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.3f, 0.f);
    Out.vAmbient = vAORM;
    return Out;
}//4

// EMPTY
PS_OUT PS_MAIN_ERAO(PS_IN In)
{
    PS_OUT      Out = (PS_OUT)0;

    vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    vector      vERAODesc = g_ERAOTexture.Sample(LinearSampler, In.vTexUV);

    if (0.1f > vDiffuse.a)
        discard;

    float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
    float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, WorldMatrix));
    float4 vAORM = float4(vERAODesc.b, vERAODesc.g, vERAODesc.r, vERAODesc.a);

    float4   FinalColor = vDiffuse;
    FinalColor.a = vDiffuse.a;
    Out.vDiffuse = FinalColor;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
    Out.vAmbient = vAORM;
    return Out;
}//5

PS_OUT PS_MAIN_MRAO(PS_IN In)
{
    PS_OUT      Out = (PS_OUT)0;

    vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    vector      vMRAODesc = g_MRAOTexture.Sample(LinearSampler, In.vTexUV);

    if (0.1f > vDiffuse.a)
        discard;

    float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
    float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, WorldMatrix));
    float4 vAORM = float4(vMRAODesc.b, vMRAODesc.g, vMRAODesc.r, vMRAODesc.a);

    float4   FinalColor = vDiffuse;
    FinalColor.a = vDiffuse.a;
    Out.vDiffuse = FinalColor;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
    Out.vAmbient = vAORM;
    return Out;
}//6

PS_OUT PS_MAIN_LEAF_MRAO(PS_IN In)
{
    PS_OUT      Out = (PS_OUT)0;

    vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    vector      vMRAODesc = g_MRAOTexture.Sample(LinearSampler, In.vTexUV);

    if (0.1f > vDiffuse.a)
        discard;

    float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
    float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, WorldMatrix));
    float4 vAORM = float4(vMRAODesc.b, vMRAODesc.g, vMRAODesc.r, vMRAODesc.a);

    float4   FinalColor = vDiffuse;
    FinalColor.a = vDiffuse.a;
    Out.vDiffuse = FinalColor;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
    Out.vAmbient = vAORM;
    return Out;
}//7

PS_OUT PS_MAIN_RUBBLE(PS_IN In)
{
    PS_OUT      Out = (PS_OUT)0;

    vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    vector      vHRAODesc = g_HRAOTexture.Sample(LinearSampler, In.vTexUV);
    vector      vBlendDiffuse = g_BlendDiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vector      vMaskDesc = g_MaskTexture.Sample(LinearSampler, In.vTexUV);
    vector      vDetailNormalDesc = g_DetailNormalTexture.Sample(LinearSampler, In.vTexUV);

    if (0.1f > vDiffuse.a)
        discard;

    float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
    float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, WorldMatrix));

    float3      vDetailNormal = vDetailNormalDesc.xyz * 2.f - 1.f;
    vDetailNormal = normalize(mul(vDetailNormal, WorldMatrix));

    vNormal += vDetailNormal;
    vNormal = normalize(vNormal);

    float4 vAORM = float4(vHRAODesc.b, vHRAODesc.g, vHRAODesc.r, vHRAODesc.a);

    float4   FinalColor = vDiffuse * (1.f - vMaskDesc.r) + vBlendDiffuse * vMaskDesc.r;
    FinalColor.a = vDiffuse.a;

    Out.vDiffuse = FinalColor;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
    Out.vAmbient = vAORM;
    return Out;
}//8

PS_OUT PS_MAIN_HRAO_DETAILNORMAL(PS_IN In)
{
    PS_OUT      Out = (PS_OUT)0;

    vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    vector      vHRAODesc = g_HRAOTexture.Sample(LinearSampler, In.vTexUV);
    vector      vDetailNormalDesc = g_DetailNormalTexture.Sample(LinearSampler, In.vTexUV);

    if (0.1f > vDiffuse.a)
        discard;

    float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
    float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, WorldMatrix));

    float3      vDetailNormal = vDetailNormalDesc.xyz * 2.f - 1.f;
    vDetailNormal = normalize(mul(vDetailNormal, WorldMatrix));

    vNormal += vDetailNormal;
    vNormal = normalize(vNormal);

    float4 vAORM = float4(vHRAODesc.b, vHRAODesc.g, vHRAODesc.r, vHRAODesc.a);

    float4   FinalColor = vDiffuse;
    FinalColor.a = vDiffuse.a;

    Out.vDiffuse = FinalColor;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
    Out.vAmbient = vAORM;
    return Out;
}//9

PS_OUT PS_MAIN_NOAO_DETAILNORMAL(PS_IN In)
{
    PS_OUT      Out = (PS_OUT)0;

    vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    vector      vDetailNormalDesc = g_DetailNormalTexture.Sample(LinearSampler, In.vTexUV);

    if (0.1f > vDiffuse.a)
        discard;

    float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
    float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, WorldMatrix));

    float3      vDetailNormal = vDetailNormalDesc.xyz * 2.f - 1.f;
    vDetailNormal = normalize(mul(vDetailNormal, WorldMatrix));

    vNormal += vDetailNormal;
    vNormal = normalize(vNormal);

    float4 vAORM = (float4)1.f;

    float4   FinalColor = vDiffuse;
    FinalColor.a = vDiffuse.a;

    Out.vDiffuse = FinalColor;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
    Out.vAmbient = vAORM;
    return Out;
}//10

PS_OUT PS_MAIN_NOAO_DN_BM(PS_IN In)
{
    PS_OUT      Out = (PS_OUT)0;

    vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    vector      vDetailNormalDesc = g_DetailNormalTexture.Sample(LinearSampler, In.vTexUV);
    vector      vBlendDiffuse = g_BlendDiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vector      vMaskDesc = g_MaskTexture.Sample(LinearSampler, In.vTexUV);

    if (0.1f > vDiffuse.a)
        discard;

    float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
    float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, WorldMatrix));

    float3      vDetailNormal = vDetailNormalDesc.xyz * 2.f - 1.f;
    vDetailNormal = normalize(mul(vDetailNormal, WorldMatrix));

    vNormal += vDetailNormal;
    vNormal = normalize(vNormal);

    float4 vAORM = (float4)1.f;

    float4   FinalColor = vDiffuse * (1.f - vMaskDesc.r) + vBlendDiffuse * vMaskDesc.r;
    FinalColor.a = vDiffuse.a;

    Out.vDiffuse = FinalColor;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
    Out.vAmbient = vAORM;
    return Out;
}//11

PS_OUT PS_MAIN_CINE(PS_IN In)
{
    PS_OUT      Out = (PS_OUT)0;

    vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

    if (0.1f > vDiffuse.a)
        discard;

    float3      vNormal = In.vNormal.xyz * 2.f - 1.f;
    float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, WorldMatrix));
    float4 vAORM = (float4)1.f;

    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
    Out.vAmbient = vAORM;
    return Out;
}//12

 //SPEARATE
PS_OUT PS_MAIN_ONLY_ROUGHNESS(PS_IN In)
{
    PS_OUT         Out = (PS_OUT)0;

    vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    vector      vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexUV);

    if (0.1f > vDiffuse.a)
        discard;

    float4 FinalColor = float4(0.f, 0.f, 0.f, 1.f);
    float  fMetalic = 1.f - vRoughness.r;
    float4 AO_R_M = float4(1.f, vRoughness.r, 1.f, 1.f);

    float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
    float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, WorldMatrix));

    FinalColor = vDiffuse;

    Out.vDiffuse = FinalColor;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 2.f, 0.f);
    Out.vAmbient = AO_R_M;

    return Out;
}//14

struct PS_IN_SHADOW
{
    float4      vPosition : SV_POSITION;
    float4      vNormal : NORMAL;
    float2      vTexUV : TEXCOORD0;
    float4      vProjPos : TEXCOORD1;
    float4      vTangent : TANGENT;
    float3      vBinormal : BINORMAL;
};

struct PS_OUT_SHADOW
{
    vector         vLightDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
    PS_OUT_SHADOW      Out = (PS_OUT_SHADOW)0;

    vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

    if (0.1f > vDiffuse.a)
        discard;

    Out.vLightDepth.r = In.vProjPos.w / g_fFar;
    Out.vLightDepth.a = 1.f;

    return Out;
}

PS_OUT PS_MAIN_PointSampler(PS_IN In)
{
    PS_OUT      Out = (PS_OUT)0;

    vector      vDiffuse = g_DiffuseTexture.Sample(PointSampler, In.vTexUV);

    if (0.1f > vDiffuse.a)
        discard;

    vDiffuse.r = vColorData.x;
    vDiffuse.g = vColorData.y;
    vDiffuse.b = vColorData.z;
    vDiffuse.a = vColorData.w;

    vector      vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexUV);

    //if (g_bDissolve)
    //{
    //    float fDissolveAmount = g_fDissolveTime * 5.f;

    //    // sample noise texture
    //    float noiseSample = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;

    //    float  _ColorThreshold1 = 1.0f;
    //    float4 _DissolveColor1 = float4(1.0f, 0.05f, 0.46f, 1.f) ;  //red

    //    //float  _ColorThreshold2 = 0.4f;
    //    //float4 _DissolveColor2 = float4(255.f, 163.f, 44.f, 1.0f) / 255.f; //orange

    //                                                                       // add edge colors0
    //    float thresh1 = fDissolveAmount * _ColorThreshold1;
    //    float useDissolve1 = noiseSample - thresh1 < 0;
    //    vDiffuse = (1 - useDissolve1) * vDiffuse + useDissolve1 * _DissolveColor1;

    //    // add edge colors1
    ///*    float thresh2 = fDissolveAmount * _ColorThreshold2;
    //    float useDissolve2 = noiseSample - thresh2 < 0;
    //    vDiffuse = (1 - useDissolve2) * vDiffuse + useDissolve2 * _DissolveColor2;*/

    //    // determine deletion threshold
    //    float threshold = fDissolveAmount * _DissolveSpeed * _FadeSpeed;
    //    clip(noiseSample - threshold);
    //}


 
    float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
    float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, WorldMatrix));

    Out.vDiffuse = vDiffuse * g_TimeDelta;//CalcHDRColor(vDiffuse,5.f) ;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
    Out.vAmbient = (vector)1.f;
    return Out;
}//1

PS_OUT PS_MAIN_Dissolve(PS_IN In)
{
    PS_OUT      Out = (PS_OUT)0;

    vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

    if (0.1f > vDiffuse.a)
        discard;

    vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

    float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
    float3x3   WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, WorldMatrix));

    float DissolveHdr = 1.f;

    if (g_bDissolve)
    {
        float fDissolveAmount = g_fDissolveTime * 5.f;

        // sample noise texture
        float noiseSample = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;

      
		float  _ColorThreshold1 = g_FirstRatio;
        float4 _DissolveColor1 = g_FirstColor;//float4(255.f, 0.f, 50.f, 1.0f) / 255.f;  //red

        float  _ColorThreshold2 = g_SecondRatio;
        float4 _DissolveColor2 = g_SecondColor;//float4(255.f, 127.f, 255.f, 1.0f) / 255.f; //orange

                                                                           // add edge colors0
        float thresh1 = fDissolveAmount * _ColorThreshold1;
        float useDissolve1 = noiseSample - thresh1 < 0;
        vDiffuse = (1 - useDissolve1) * vDiffuse + useDissolve1 * _DissolveColor1;

        // add edge colors1
        float thresh2 = fDissolveAmount * _ColorThreshold2;
        float useDissolve2 = noiseSample - thresh2 < 0;
        vDiffuse = (1 - useDissolve2) * vDiffuse + useDissolve2 * _DissolveColor2;

        // determine deletion threshold
        float threshold = fDissolveAmount * _DissolveSpeed * _FadeSpeed;
        clip(noiseSample - threshold);

        DissolveHdr = 3.f;
    }

    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, DissolveHdr, 0.f);
    Out.vAmbient = (vector)1.f;
    return Out;
}//1

technique11 DefaultTechnique
{
    pass Shadow
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_INSTANCE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }//0

    pass MESH_INStancing_Lod//
    {
        SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_INSTANCE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TESS();
    }//1

    pass Comp_H_R_AO
    {
        SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_INSTANCE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_H_R_AO();
    }//2

    pass Comp_M_R_AO_E
    {
        SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_INSTANCE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MRAO_E();
    }//3

    pass Comp_H_R_AO_E
    {
        SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_INSTANCE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_HRAO_E();
    }//4

     // EMPTY
    pass E_R_AO
    {
        SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_INSTANCE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ERAO();
    }//5

    pass M_R_AO
    {
        SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_INSTANCE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MRAO();
    }//6

    pass LEAF_M_R_AO
    {
        SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_INSTANCE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LEAF_MRAO();
    }//7

    pass RUBBLE
    {
        SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_INSTANCE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RUBBLE();
    }//8

    pass HRAO_DETAILNORMAL
    {
        SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_INSTANCE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_HRAO_DETAILNORMAL();
    }//9

    pass NOAO_DETAILNORMAL
    {
        SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_INSTANCE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NOAO_DETAILNORMAL();
    }//10

    pass NOAO_DN_BM
    {
        SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_INSTANCE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NOAO_DN_BM();
    }//11

    pass Cine
    {
        SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_INSTANCE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CINE();
    }//12

    pass MeshInst_Effect
    {
          SetRasterizerState(RS_CULLNONE); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
      VertexShader = compile vs_5_0 VS_MAIN_INSTANCE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ERAO();
    } //13

    pass OnlyRoughness
    {
        SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_INSTANCE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ONLY_ROUGHNESS();
    } //14

    pass NonIns_Default
    {
        SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TESS();
    } //15

    pass NonIns_MRAO
    {
        SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MRAO();
    } //16

    pass NonIns_HRAO
    {
        SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_H_R_AO();
    } //17

    pass NonIns_ERAO
    {
        SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ERAO();
    } //18

    pass NonIns_MRAO_E
    {
        SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MRAO_E();
    } //19

    pass NonIns_CINE
    {
        SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CINE();
    } //20

    pass NonIns_Shadow
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }//21

    pass Comp_H_R_AO_CULLNONE
    {
        SetRasterizerState(RS_CULLNONE); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_INSTANCE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_H_R_AO();
    }//22


    pass GS_Default // Instancing Model
    {
        SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_INSTANCE_GEOMETRY();
        GeometryShader =  compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_PointSampler();
    }//23

    pass GS_Non_instDefault // Instancing Model
    {
        SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_GEOMETRY_NON_INST();
        GeometryShader = compile gs_5_0 GS_BossAttack();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_PointSampler();
    }//24


    pass NonInsDissolve_Default
    {
        SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Dissolve();
    } //25

    pass InsDissolve_Default
    {
        SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_INSTANCE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Dissolve();
    } //26
}