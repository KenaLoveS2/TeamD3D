#include "stdafx.h"
#include "..\public\Imgui_Effect.h"

#include "GameInstance.h"
#include "Effect.h"

CImgui_Effect::CImgui_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CImguiObject(pDevice, pContext)
{
	ZeroMemory(&m_eEffectDesc, sizeof(CEffect_Base::EFFECTDESC));
}

HRESULT CImgui_Effect::Initialize(void * pArg)
{
	m_szWindowName = "Effect_Tool";
	return S_OK;
}

void CImgui_Effect::Imgui_RenderWindow()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//map<const _tchar*, class CGameObject*>& pGameObject = pGameInstance->Get_ProtoTypeObjects();
	//map<const _tchar*, class CLayer*>* pLayer = pGameInstance->get_Lat();

	wstring		strDefault = L"Prototype_GameObject_";
	static char szEffectTag[64] = "";
	static _int iCreateRectCnt = 0;

	static _int  iSelectEffectType = 0;
	static _int  iCreateCnt = 1;
	static _bool bIsCreate = false;

	_tchar*		 szEffectCloneTag = L"";

	// 1. Texture Type Select
	const char* szEffectType[] = { " ~SELECT_EFFECT TYPE~ ", " EFFECT_PLANE ", " EFFECT_PARTICLE", " EFFECT_MESH" };
	ImGui::BulletText("EffectType : "); ImGui::Combo("##EffectType", &iSelectEffectType, szEffectType, IM_ARRAYSIZE(szEffectType));

	_int iCurLevel = pGameInstance->Get_CurLevelIndex();

	switch (iSelectEffectType)
	{
	case 0:
		bIsCreate = false;
		break;

	case EFFECT_PLANE:
		ImGui::NewLine();
		ImGui::BulletText("CreateCnt : ");  ImGui::InputInt("##CreateCnt", &iCreateCnt);
		ImGui::InputTextWithHint("##EffectTag", "Only Input Tagname.", szEffectTag, 64); ImGui::SameLine();

		if (ImGui::Button("Confirm"))
		{
			wstring	strEffectTag(szEffectTag, &szEffectTag[64]);
			strDefault = strDefault + strEffectTag;

			if (FAILED(pGameInstance->Add_Prototype(strDefault.c_str(), CEffect::Create(m_pDevice, m_pContext))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			for (_int i = 0; i < iCreateCnt; ++i)
			{
				_tchar   szDefault[64] = L"";
				wsprintf(szDefault, L"Effect_Plane_%d", iCreateRectCnt);

				_tchar* szEffectCloneTag = CUtile::Create_String(szDefault);
				pGameInstance->Add_String(szEffectCloneTag);

				if (FAILED(pGameInstance->Clone_GameObject(iCurLevel, L"Layer_Effect", strDefault.c_str(), szEffectCloneTag)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
				iCreateRectCnt++;
			}

			bIsCreate = true;
			m_bIsRectLayer = true;
		}

		ImGui::Separator();
		LayerEffects_ListBox();

		if (bIsCreate == true)
			CreateEffect_Plane();
		break;

	case EFFECT_PARTICLE:
		ImGui::NewLine();
		ImGui::BulletText("CreateCnt : ");  ImGui::InputInt("##CreateCnt", &iCreateCnt);
		ImGui::InputTextWithHint("##EffectTag", "Only Input Tagname.", szEffectTag, 64); ImGui::SameLine();

		if (ImGui::Button("Confirm"))
		{
		}
		ImGui::Separator();
		LayerEffects_ListBox();

		if (bIsCreate == true)
			CreateEffect_Particle();
		break;

	case EFFECT_MESH:
		ImGui::NewLine();
		ImGui::BulletText("CreateCnt : ");  ImGui::InputInt("##CreateCnt", &iCreateCnt);
		ImGui::InputTextWithHint("##EffectTag", "Only Input Tagname.", szEffectTag, 64); ImGui::SameLine();

		if (ImGui::Button("Confirm"))
		{
		}
		ImGui::Separator();
		LayerEffects_ListBox();

		if (bIsCreate == true)
			CreateEffect_Mesh();
		break;
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CImgui_Effect::LayerEffects_ListBox()
{
}

void CImgui_Effect::Set_ColorValue(OUT _float4& vColor)
{
	static bool alpha_preview = true;
	static bool alpha_half_preview = false;
	static bool drag_and_drop = true;
	static bool options_menu = true;
	static bool hdr = false;

	ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

	static bool   ref_color = false;
	static ImVec4 ref_color_v(1.0f, 1.0f, 1.0f, 1.0f);

	static _float4 color = _float4(vColor.x / 255.0f, vColor.y / 255.0f, vColor.z / 255.0f, vColor.w / 255.0f);

	ImGui::ColorPicker4("CurColor##4", (float*)&color, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color ? &ref_color_v.x : NULL);
	ImGui::ColorEdit4("Diffuse##2f", (float*)&color, ImGuiColorEditFlags_Float | misc_flags);

	vColor = _float4(color.x, color.y, color.z, color.w);
}

void CImgui_Effect::CreateEffect_Plane()
{
	if (m_bIsRectLayer == false)
		return;

	static _int iTextureRender = 0;
	static _int iBillboard = 0;
	static _int iBlendType = 0;
	static _float  fTimeDelta = 0.0f;
	static _float2 fCnt, fSeparateCnt;
	static _float4 fVector = _float4(1.0f, 1.0f, 1.0f, 1.0f);
	static _float4 fColor = _float4(1.0f, 1.0f, 1.0f, 1.0f);

	// 2. Texture Create
	// Layer 가져와서 Layer_Effect 출력

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None; // Tabbar Flag
	ImGui::BulletText("TextureRender_Type : ");
	if (ImGui::RadioButton("TEX_ONE", &iTextureRender, 0))
		m_eEffectDesc.eTextureRenderType = CEffect_Base::EFFECTDESC::TEXTURERENDERTYPE::TEX_ONE;
	if (ImGui::RadioButton("TEX_SPRITE", &iTextureRender, 1))
		m_eEffectDesc.eTextureRenderType = CEffect_Base::EFFECTDESC::TEXTURERENDERTYPE::TEX_SPRITE;

	if (iTextureRender == 1)
	{
		ImGui::BulletText("CntWidth, Height : "); ImGui::InputFloat2("##CntWidth, Height", (_float*)&fCnt);
		m_eEffectDesc.iWidthCnt = (_int)fCnt.x;
		m_eEffectDesc.iHeightCnt = (_int)fCnt.y;

		ImGui::BulletText("fSeparateWidthCnt, HeightCnt : "); ImGui::InputFloat2("##fSeparateWidthCnt, HeightCnt", (_float*)&fSeparateCnt);
		m_eEffectDesc.iSeparateWidth = (_int)fSeparateCnt.x;
		m_eEffectDesc.iSeparateHeight = (_int)fSeparateCnt.y;

		ImGui::BulletText("fTimeDelta : "); ImGui::InputFloat("##fTimeDelta", &fTimeDelta);
		m_eEffectDesc.fTimeDelta = fTimeDelta;
		ImGui::Separator();
	}

	ImGui::Separator();
	ImGui::BulletText("Billboard Type : ");
	if (ImGui::RadioButton("Use Billboard", &iBillboard, 0))
		m_eEffectDesc.IsBillboard = true;
	if (ImGui::RadioButton("No Use Billboard", &iBillboard, 1))
		m_eEffectDesc.IsBillboard = false;
	ImGui::Separator();

	ImGui::BulletText("BlendType : ");
	if (ImGui::RadioButton("BlendType_Default", &iBlendType, 0))
		m_eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_DEFAULT;
	if (ImGui::RadioButton("BlendType_Alpha", &iBlendType, 1))
		m_eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_ALPHA;
	if (ImGui::RadioButton("BlendType_OneEffect", &iBlendType, 2))
		m_eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_ONEEFFECT;
	ImGui::Separator();

	ImGui::BulletText("fVector : ");	 ImGui::InputFloat4("##fVector", (_float*)&fVector);
	m_eEffectDesc.vScale = XMVectorSet(fVector.x, fVector.y, fVector.z, fVector.w);

	ImGui::Separator();
	ImGui::BulletText("vColor : "); Set_ColorValue(fColor);
	m_eEffectDesc.vColor = XMVectorSet(fColor.x, fColor.y, fColor.z, fColor.w);

	//////////////////////////////////////////////////////////////////////////

// 	ImGui::BulletText("Texture : ");
// 	if (ImGui::BeginTabBar("##TextureImage", tab_bar_flags))
// 	{
// 		if (ImGui::BeginTabItem("Diffuse_Texture"))
// 		{
// 			CTexture*		pDiffuseTexture = (CTexture*)m_pEffect->Find_Component(L"Com_Texture");
// 			if (pDiffuseTexture != nullptr)
// 			{
// 				ImGui::BulletText("Diffuse _ %d / %d", pDiffuseTexture->Get_CurTexIdx(), pDiffuseTexture->Get_NumTexIdx());
// 				ImGui::Separator();
// 
// 				for (_uint i = 0; i < pDiffuseTexture->Get_NumTexIdx(); ++i)
// 				{
// 					if (i % 5)
// 						ImGui::SameLine();
// 
// 					if (ImGui::ImageButton(pDiffuseTexture->Get_Texture(i), ImVec2(50.f, 50.f)))
// 					{
// 						pDiffuseTexture->Set_CurTexIdx(i);
// 						m_eEffect.fFrame = i*1.0f;
// 					}
// 				}
// 			}
// 			if (pDiffuseTexture == nullptr)
// 				return;
// 
// 			ImGui::EndTabItem();
// 		}
// 	}
}

void CImgui_Effect::CreateEffect_Particle()
{
	ImGui::BulletText("CreateEffect_Particle");

}

void CImgui_Effect::CreateEffect_Mesh()
{
	ImGui::BulletText("CreateEffect_Mesh");
}

CImgui_Effect* CImgui_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, void * pArg)
{
	CImgui_Effect * pInstance = new CImgui_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CImgui_Effect Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_Effect::Free()
{
	__super::Free();
}
