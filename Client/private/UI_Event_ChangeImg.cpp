#include "stdafx.h"
#include "..\public\UI_Event_ChangeImg.h"
#include "GameInstance.h"
#include "UI.h"
#include "Json/json.hpp"


CUI_Event_ChangeImg::CUI_Event_ChangeImg()
{
	m_szEventName = "ChangeTexture";
	//m_vecTextures.push_back(nullptr); /* Prepare for original */

	m_iLastSelected = 0;

	m_bFirstCall = false;

	/* Test */
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//CTexture* pTex = static_cast<CTexture*>(pGameInstance->Clone_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Texture_HUDPipFull"));
	//if (pTex == nullptr)
	//	MSG_BOX("Failed To Create Tex : CHAGEImg");
	//else
	//{
	//	m_vecTextures.push_back(pTex);
	//}

	RELEASE_INSTANCE(CGameInstance);
}


HRESULT CUI_Event_ChangeImg::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Event_ChangeImg::Late_Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Event_ChangeImg::SetUp_ShaderResources(CShader * pShader)
{
	return S_OK;
}

void CUI_Event_ChangeImg::Imgui_RenderProperty()
{
	ImGui::Separator();
	ImGui::Text("Event: "); ImGui::SameLine();
	ImGui::Text(m_szEventName);

	/* RenderPass */
	_int iPass = (_int)m_iRenderPass;
	if (ImGui::InputInt("RenderPass", &iPass))
		m_iRenderPass = iPass;

}

void CUI_Event_ChangeImg::Call_Event(CUI* pUI, _uint iImgIndex)
{
	//if (iImgIndex >= (_uint)m_vecTextures.size())
	//	return;

	if (!m_bFirstCall)
	{
		/* Store Original Image in m_pOriginal; */
		//pUI->Change_DiffuseTexture(m_vecTextures[m_iLastSelected], &m_pOriginal);
		m_iOriginalRenderPass = pUI->Get_RenderPass();
		//pUI->Set_RenderPass(m_iRenderPass);
		m_bFirstCall = true;
	}

	m_iLastSelected = iImgIndex;
	pUI->Set_TextureIndex(m_iLastSelected);

	if (iImgIndex == 0)
		pUI->Set_RenderPass(m_iOriginalRenderPass);
	else if (iImgIndex != 0)
		pUI->Set_RenderPass(m_iRenderPass);

#pragma region Old
	/* modify date : 230226 */
	/* desc: i thought i need to change texture component, so it's complicated to manage reference. */
	/* but... only make the texture have several index at first will solve this problem. */
	/* ex) TEXT("Bin/Resource/Textures/UI/HUD/HUD_Pip_%d.png"), 2  */

	//if (!m_bFirstCall)
	//{
	//	/* Store Original Image in m_pOriginal; */
	//	pUI->Change_DiffuseTexture(m_vecTextures[m_iLastSelected], &m_pOriginal);
	//	m_iOriginalRenderPass = pUI->Get_RenderPass();
	//	pUI->Set_RenderPass(m_iRenderPass);
	//	m_bFirstCall = true;
	//}
	//else if (iImgIndex == 0)
	//{
	//	pUI->Set_RenderPass(m_iOriginalRenderPass);
	//	pUI->Change_DiffuseTexture(m_pOriginal);
	//}
	//else if (iImgIndex != 0)
	//{
	//	pUI->Change_DiffuseTexture(m_vecTextures[m_iLastSelected]);
	//	pUI->Set_RenderPass(m_iRenderPass);
	//}
#pragma endregion Old
}

HRESULT CUI_Event_ChangeImg::Save_Data(Json * json)
{
	/* 텍스처 몇개 저장했고, 각 텍스처의 prototag가 무엇인지 저장 */
	if (json == nullptr)
		return E_FAIL;

	(*json)["EventRenderPass"] = m_iRenderPass;

	return S_OK;
}

HRESULT CUI_Event_ChangeImg::Load_Data(wstring fileName)
{
	Json	jLoad;

	wstring name = L"../Bin/Data/UI/";
	name += fileName;
	name += L"_Property.json";
	string filePath;
	filePath.assign(name.begin(), name.end());

	ifstream file(filePath);
	if (file.fail())
		return E_FAIL;
	file >> jLoad;
	file.close();

	jLoad["EventRenderPass"].get_to<_uint>(m_iRenderPass);


	return S_OK;
}

CUI_Event_ChangeImg * CUI_Event_ChangeImg::Create()
{
	CUI_Event_ChangeImg* pInstance = new CUI_Event_ChangeImg();
	return pInstance;
}

CUI_Event_ChangeImg * CUI_Event_ChangeImg::Create(wstring fileName)
{
	CUI_Event_ChangeImg* pInstance = new CUI_Event_ChangeImg();
	if (pInstance != nullptr)
		pInstance->Load_Data(fileName);
	return pInstance;
}

void CUI_Event_ChangeImg::Free()
{

#pragma region old
	//size_t iSize = m_vecTextures.size();

	/* original texture go back */
	//if (m_iLastSelected == 0)
	//{
	//	for (size_t i = 1; i < iSize; ++i)
	//	{
	//		Safe_Release(m_vecTextures[i]);
	//	}
	//}
	///* if UI's m_pTextureCom is not the original texture */
	//else
	//{
	//	Safe_Release(m_pOriginal);

	//	for (size_t i = 1; i < iSize; ++i)
	//	{
	//		if (m_iLastSelected != i)
	//		{
	//			Safe_Release(m_vecTextures[i]);
	//		}
	//	}
	//}
	//m_vecTextures.clear();
#pragma endregion old
}
