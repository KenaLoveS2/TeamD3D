#include "stdafx.h"
#include "..\public\Tool_Animation.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Utile.h"

CTool_Animation::CTool_Animation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CImguiObject(pDevice, pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CTool_Animation::Initialize(void * pArg)
{
	m_szWindowName = "Animation";

	m_iCurrentLevel = m_pGameInstance->Get_CurLevelIndex();
	m_mapAnimObject = m_pGameInstance->Get_AnimObjects(m_iCurrentLevel);

	return S_OK;
}

void CTool_Animation::Imgui_RenderWindow()
{
	Update_Level();

	if (m_iCurrentLevel == LEVEL_LOADING)
	{
		ImGui::Text("Wait for Loading...");
		return;
	}

	if (m_mapAnimObject->empty())
	{
		ImGui::Text("There is no Object has Animation.");
		return;
	}

	_uint	iObjectCount = (_uint)m_mapAnimObject->size();

	ImGui::BulletText("Animation Objects");

	static _int	iSelectObject = -1;
	char**			ppObjectTag = new char*[iObjectCount];

	_uint	iTagLength = 0;
	_uint	i = 0;
	for (auto& Pair : *m_mapAnimObject)
		ppObjectTag[i++] = CUtile::WideCharToChar(const_cast<_tchar*>(Pair.first));

	ImGui::ListBox("AnimObject List", &iSelectObject, ppObjectTag, iObjectCount);

	if (iSelectObject != -1)
	{
		CGameObject*	pGameObject = Find_GameObject(iSelectObject);
		
		if (pGameObject != nullptr)
		{
			ImGui::BulletText("Current Object : ");
			ImGui::SameLine();
			ImGui::Text(ppObjectTag[iSelectObject]);
			pGameObject->ImGui_AnimationProperty();
		}
	}

	for (_uint i = 0; i < iObjectCount; ++i)
		Safe_Delete_Array(ppObjectTag[i]);
	Safe_Delete_Array(ppObjectTag);
}

void CTool_Animation::Update_Level()
{
	m_iCurrentLevel = m_pGameInstance->Get_CurLevelIndex();
	m_mapAnimObject = m_pGameInstance->Get_AnimObjects(m_iCurrentLevel);
}

CGameObject * CTool_Animation::Find_GameObject(_int iIndex)
{
	NULL_CHECK_RETURN(m_mapAnimObject, nullptr);

	auto	iter = m_mapAnimObject->begin();

	for (_int i = 0; i < iIndex; ++i)
		++iter;

	return iter->second;
}

CTool_Animation * CTool_Animation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, void * pArg)
{
	CTool_Animation*	pInstance = new CTool_Animation(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : CTool_Animation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTool_Animation::Free()
{
	__super::Free();
}
