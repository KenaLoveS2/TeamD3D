#include "stdafx.h"
#include "..\public\Imgui_Effect.h"

#include "GameInstance.h"
#include "Effect.h"
#include "Layer.h"

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

	wstring		strDefault = L"Prototype_GameObject_";
	static char szEffectTag[64] = "";
	static _int iCreateRectCnt = 0;

	static _int  iSelectEffectType = 0;
	static _int  iCreateCnt = 1;
	static _bool bIsCreate = false;

	_tchar*		 szEffectCloneTag = L"";

	_int iCurLevel = pGameInstance->Get_CurLevelIndex();
	static _int iSelectObj = -1;

	// 1. Texture Type Select
	const char* szEffectType[] = { " ~SELECT_EFFECT TYPE~ ", " EFFECT_PLANE ", " EFFECT_PARTICLE", " EFFECT_MESH" };
	ImGui::BulletText("EffectType : "); ImGui::Combo("##EffectType", &iSelectEffectType, szEffectType, IM_ARRAYSIZE(szEffectType));

	if (m_bIsRectLayer == true)
	{
		ImGui::Separator();
		ImGui::BulletText("EffectLayer Tag : ");
		LayerEffects_ListBox(bIsCreate, iSelectObj, iSelectEffectType);
	}

	switch (iSelectEffectType)
	{
	case 0:
		bIsCreate = false;
		strcpy_s(szEffectTag, "");
		break;

	case EFFECT_PLANE:
		ImGui::Separator();
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
		if (iSelectObj != -1 &&
			bIsCreate == true && m_bIsRectLayer == true)
			CreateEffect_Plane(iSelectObj);
		break;

	case EFFECT_PARTICLE:
		ImGui::Separator();
		ImGui::BulletText("CreateCnt : ");  ImGui::InputInt("##CreateCnt", &iCreateCnt);
		ImGui::InputTextWithHint("##EffectTag", "Only Input Tagname.", szEffectTag, 64); ImGui::SameLine();

		if (ImGui::Button("Confirm"))
		{
		}
		ImGui::Separator();

		if (bIsCreate == true && bIsCreate == true)
			CreateEffect_Particle(iSelectObj);
		break;

	case EFFECT_MESH:
		ImGui::Separator();
		ImGui::BulletText("CreateCnt : ");  ImGui::InputInt("##CreateCnt", &iCreateCnt);
		ImGui::InputTextWithHint("##EffectTag", "Only Input Tagname.", szEffectTag, 64); ImGui::SameLine();

		if (ImGui::Button("Confirm"))
		{
		}
		ImGui::Separator();

		if (bIsCreate == true && bIsCreate == true)
			CreateEffect_Mesh(iSelectObj);
		break;
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CImgui_Effect::LayerEffects_ListBox(_bool& bIsCreate, _int& iSelectObject, _int& iSelectEffectType)
{
	// Layer 가져와서 Layer_Effect 출력
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_int    iCurLevel = pGameInstance->Get_CurLevelIndex();
	CLayer* pLayer = pGameInstance->Find_Layer(iCurLevel, L"Layer_Effect");
	RELEASE_INSTANCE(CGameInstance);

	// ListBox
	map<const _tchar*, class CGameObject*>& pGameObject = pLayer->GetGameObjects();
	size_t iLayerObjSize = pGameObject.size();

	char** szObjectTag = new char*[iLayerObjSize];

	auto Pair = pGameObject.begin();
	for (size_t i = 0; i < iLayerObjSize; ++i, ++Pair)
	{
		szObjectTag[i] = new char[lstrlen(Pair->first) + 1];
		CUtile::WideCharToChar(Pair->first, szObjectTag[i]);
	}

	ImGui::ListBox("##CurLayerObejct", &iSelectObject, szObjectTag, (_int)pGameObject.size());
	ImGui::Button("Save_Data"); ImGui::SameLine();
	ImGui::Button("Load_Data"); ImGui::SameLine();
	if (ImGui::Button("Reset_Data"))
		iSelectEffectType = 0;
	// ~ListBox

	for (size_t i = 0; i < iLayerObjSize; ++i)
		Safe_Delete_Array(szObjectTag[i]);
	Safe_Delete_Array(szObjectTag);
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

void CImgui_Effect::CreateEffect_Plane(_int& iSelectObject)
{
	if (m_bIsRectLayer == false)
		return;

	static _bool bSelect = false;
	static _int  iBeforeSelect = iSelectObject;

	// 2. Texture Create
	if (iSelectObject != -1)
	{
		//////////////////////////////////////////////////////////////////////////
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_int    iCurLevel = pGameInstance->Get_CurLevelIndex();
		CLayer* pLayer = pGameInstance->Find_Layer(iCurLevel, L"Layer_Effect");
		map<const _tchar*, class CGameObject*>& pGameObject = pLayer->GetGameObjects();
		size_t iLayerObjSize = pGameObject.size();

		auto iter = pGameObject.begin();
		for (size_t i = 0; i < iSelectObject; ++i)
			iter++;

		iBeforeSelect = iSelectObject;
		if (iBeforeSelect != iSelectObject)
			bSelect = true;

		CEffect* pEffect = dynamic_cast<CEffect*>(iter->second);
		if (pEffect == nullptr)
			return;
		//////////////////////////////////////////////////////////////////////////
		if (bSelect == true)
		{
			m_eEffectDesc = pEffect->Get_EffectDesc();
			bSelect = false;
		}

		static _bool IsBillboard = false;

		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None; // Tabbar Flag
		if (ImGui::BeginTabBar("##Value Setting", tab_bar_flags))
		{
			if (ImGui::BeginTabItem("Set Item Value"))
			{
				static _int iTextureRender = 0;
				static _int iBillboard = 0;
				static _int iBlendType = 0;
				static _float  fTimeDelta = 0.0f;
				static _float2 fCnt, fSeparateCnt;
				static _float4 fVector = _float4(1.0f, 1.0f, 1.0f, 1.0f);
				static _float4 fColor = _float4(1.0f, 1.0f, 1.0f, 1.0f);

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
					IsBillboard = true;
				if (ImGui::RadioButton("No Use Billboard", &iBillboard, 1))
					IsBillboard = false;
				ImGui::Separator();

				ImGui::BulletText("BlendType : ");
				if (ImGui::RadioButton("BlendType_Default", &iBlendType, 0))
					m_eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_DEFAULT;
				if (ImGui::RadioButton("BlendType_Alpha", &iBlendType, 1))
					m_eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_ALPHA;
				if (ImGui::RadioButton("BlendType_OneEffect", &iBlendType, 2))
					m_eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_ONEEFFECT;
				if (ImGui::RadioButton("BlendType_Mix", &iBlendType, 3))
					m_eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_MIX;
				ImGui::Separator();

				ImGui::BulletText("fVector : ");	 ImGui::InputFloat4("##fVector", (_float*)&fVector);
				m_eEffectDesc.vScale = XMVectorSet(fVector.x, fVector.y, fVector.z, fVector.w);

				ImGui::Separator();
				ImGui::BulletText("vColor : "); Set_ColorValue(fColor);
				m_eEffectDesc.vColor = XMVectorSet(fColor.x, fColor.y, fColor.z, fColor.w);

				ImGui::EndTabItem();
			}
			static _int iCreateCnt = 1;
			static _int iSelectDTexture = 0;
			static _int iSelectMTexture = 0;
			const _int  iTotalDTextureCnt = pEffect->Get_TotalDTextureCnt();
			const _int  iTotalMTextureCnt = pEffect->Get_TotalMTextureCnt();

			static _int iSelectTextureType = 0;
			const char* szTextureType[] = { " ~SELECT_EFFECT TYPE~ ", " EFFECT_PLANE ", " EFFECT_PARTICLE", " EFFECT_MESH" };

			if (ImGui::BeginTabItem("Set Item Texture"))
			{
				ImGui::BulletText("Texture Type : ");
				ImGui::Combo("##EffectType", &iSelectTextureType, szTextureType, IM_ARRAYSIZE(szTextureType));

				if (iSelectTextureType == 0)
				{
					if (ImGui::BeginTabItem("Diffuse_Texture"))
					{
						ImGui::BulletText("Add Texture : "); ImGui::SameLine();

						ImGui::PushItemWidth(100);
						ImGui::InputInt("##AddMTexture", (_int*)&iCreateCnt, 1, 4); ImGui::SameLine();

						if (iCreateCnt <= 1)
							iCreateCnt = 1;
						else if (iCreateCnt >= 4)
							iCreateCnt = 4;

						if (ImGui::Button("Add Texture Confirm"))
							pEffect->Add_TextureComponent(iCreateCnt, 0);

						char** szDTextureType = new char*[iTotalDTextureCnt];
						for (_int i = 0; i < iTotalDTextureCnt; ++i)
						{
							szDTextureType[i] = new char[64];

							_tchar   szDefault[64] = L"";
							wsprintf(szDefault, L"Com_DTexture_%d", i);

							CUtile::WideCharToChar(szDefault, szDTextureType[i]);
						}
						ImGui::BulletText("DTexture : "); ImGui::SameLine();
						ImGui::PushItemWidth(-FLT_MIN);
						ImGui::Combo("##DTexture", &iSelectDTexture, szDTextureType, iTotalDTextureCnt);

						_tchar szDTexture[64] = L"";
						CUtile::CharToWideChar(szDTextureType[iSelectDTexture], szDTexture);
						CTexture*		pDiffuseTexture = (CTexture*)pEffect->Find_Component(szDTexture);

						if (pDiffuseTexture != nullptr)
						{
							_float fFrame = pEffect->Get_EffectDesc().fFrame[iSelectDTexture];
							ImGui::BulletText("DTexture _ %d / %d", (_uint)fFrame, pDiffuseTexture->Get_TextureIdx());
							ImGui::Separator();

							for (_uint i = 0; i < pDiffuseTexture->Get_TextureIdx(); ++i)
							{
								if (i % 6)
									ImGui::SameLine();

								if (ImGui::ImageButton(pDiffuseTexture->Get_Texture(i), ImVec2(50.f, 50.f)))
								{
									m_eEffectDesc.fFrame[iSelectDTexture] = i * 1.0f;
									pEffect->Set_EffectDescDTexture(iSelectDTexture, i * 1.0f);
								}
							}
						}
						else
						{
							for (size_t i = 0; i < iTotalDTextureCnt; ++i)
								Safe_Delete_Array(szDTextureType[i]);
							Safe_Delete_Array(szDTextureType);

							ImGui::EndTabItem();
							return;
						}
						for (size_t i = 0; i < iTotalDTextureCnt; ++i)
							Safe_Delete_Array(szDTextureType[i]);
						Safe_Delete_Array(szDTextureType);

						ImGui::EndTabItem();
					}
				}
				else
				{
					if (ImGui::BeginTabItem("Mask_Texture"))
					{
						ImGui::BulletText("Add Texture : "); ImGui::SameLine();

						ImGui::PushItemWidth(100);
						ImGui::InputInt("##AddMTexture", (_int*)&iCreateCnt, 1, 4); ImGui::SameLine();

						if (iCreateCnt <= 1)
							iCreateCnt = 1;
						else if (iCreateCnt >= 4)
							iCreateCnt = 4;

						if (ImGui::Button("Add Texture Confirm"))
							pEffect->Add_TextureComponent(0, iCreateCnt);

						char** szMTextureType = new char*[iTotalMTextureCnt];
						if (0 != iTotalMTextureCnt)
						{
							for (_int i = 0; i < iTotalMTextureCnt; ++i)
							{
								szMTextureType[i] = new char[64];

								_tchar   szDefault[64] = L"";
								wsprintf(szDefault, L"Com_MTexture_%d", i);

								CUtile::WideCharToChar(szDefault, szMTextureType[i]);
							}
							ImGui::BulletText("MTexture : "); ImGui::SameLine();
							ImGui::PushItemWidth(-FLT_MIN);
							ImGui::Combo("##MTexture", &iSelectMTexture, szMTextureType, iTotalMTextureCnt);
						}

						_tchar szMTexture[64] = L"";
						CUtile::CharToWideChar(szMTextureType[iSelectMTexture], szMTexture);
						CTexture*		pDiffuseTexture = (CTexture*)pEffect->Find_Component(szMTexture);

						if (pDiffuseTexture != nullptr)
						{
							_float fMaskFrame = pEffect->Get_EffectDesc().fMaskFrame[iSelectMTexture];

							ImGui::BulletText("MTexture _ %d / %d", (_uint)fMaskFrame, pDiffuseTexture->Get_TextureIdx());
							ImGui::Separator();

							for (_uint i = 0; i < pDiffuseTexture->Get_TextureIdx(); ++i)
							{
								if (i % 6)
									ImGui::SameLine();

								if (ImGui::ImageButton(pDiffuseTexture->Get_Texture(i), ImVec2(50.f, 50.f)))
								{
									m_eEffectDesc.fMaskFrame[iSelectMTexture] = i * 1.0f;
									pEffect->Set_EffectDescMTexture(iSelectMTexture, i * 1.0f);
								}
							}
						}
						else
						{
							for (size_t i = 0; i < iTotalDTextureCnt; ++i)
								Safe_Delete_Array(szMTextureType[i]);
							Safe_Delete_Array(szMTextureType);

							ImGui::EndTabItem();
							return;
						}

						for (size_t i = 0; i < iTotalMTextureCnt; ++i)
							Safe_Delete_Array(szMTextureType[i]);
						Safe_Delete_Array(szMTextureType);
						ImGui::EndTabItem();
					}
				}
			}
			ImGui::EndTabBar();
		}

		if (ImGui::Button("Set_Data") || pGameInstance->Key_Down(DIK_RETURN))
		{
			if (IsBillboard == true)
				m_eEffectDesc.IsBillboard = true;
			else
				m_eEffectDesc.IsBillboard = false;

			pEffect->Set_EffectDesc(m_eEffectDesc);
		}
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CImgui_Effect::CreateEffect_Particle(_int iSelectObject)
{
	ImGui::BulletText("CreateEffect_Particle");

}

void CImgui_Effect::CreateEffect_Mesh(_int iSelectObject)
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

