#include "Shader_Client_Defines.h"

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

    /* ÅºÁ¨Æ®½ºÆäÀÌ½º */
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
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 1.f, 0.f);
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
          SetRasterizerState(RS_Default); //RS_Default , RS_Wireframe
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
}