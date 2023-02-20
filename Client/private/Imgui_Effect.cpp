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
	wstring		strDefault = L"Prototype_GameObject_";
	static char szEffectTag[64] = "";
// 	_tchar*   	szEffectTotalTag = L"";

	static _int iSelectEffectType = 0;
	static _int iCreateCnt = 1;
	static _bool bIsCreate = false;

	// 1. Texture Type Select
	const char* szEffectType[] = { " ~SELECT_EFFECT TYPE~ ",
		" EFFECT_PLANE ", " EFFECT_PARTICLE", " EFFECT_MESH" };
	ImGui::BulletText("EffectType : "); ImGui::Combo("##EffectType", &iSelectEffectType, szEffectType, IM_ARRAYSIZE(szEffectType));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_int iCurLevel = pGameInstance->Get_CurLevelIndex();

	switch (iSelectEffectType)
	{
	case 0:
		bIsCreate = false;
		break;

	case EFFECT_PLANE : 
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
				pGameInstance->Clone_GameObject(iCurLevel, L"Layer_Effect", strDefault.c_str());

			bIsCreate = true;
		}

		ImGui::Separator();
		if (bIsCreate == true)
			CreateEffect_Plane();

		break;

	case EFFECT_PARTICLE:
		if (ImGui::Button("Confirm"))
		{
		}
		ImGui::Separator();
		CreateEffect_Particle();
		break;

	case EFFECT_MESH:
		if (ImGui::Button("Confirm"))
		{
		}
		ImGui::Separator();
		CreateEffect_Mesh();
		break;
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CImgui_Effect::CreateEffect_Plane()
{
	ImGui::BulletText("CreateEffect_Plane");
	// 2. Texture Create
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
