#include "stdafx.h"
#include "..\public\Level_Logo.h"

#include "Level_Loading.h"
#include "GameInstance.h"

CLevel_Logo::CLevel_Logo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{

}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_For_LevelName()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Logo::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CLevel_Logo::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	ChoseLevel();

	

}

HRESULT CLevel_Logo::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : LOGO"));

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);	

	if(FAILED(pGameInstance->Clone_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_BackGround"), TEXT("Clone_BackGround"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Logo::Ready_For_LevelName()
{
	//arrLevel[LEVEL_GAMEPLAY] = "LEVEL_GAMEPLAY";
	//arrLevel[LEVEL_MAPTOOL] = "LEVEL_MAPTOOL";
	//arrLevel[LEVEL_TESTPLAY] = "LEVEL_TESTPLAY";

	arrLevel.reserve(3);

	arrLevel.push_back("LEVEL_GAMEPLAY");
	arrLevel.push_back("LEVEL_MAPTOOL");
	arrLevel.push_back("LEVEL_TESTPLAY");
	arrLevel.push_back("LEVEL_TEST_EFFECT");
	return S_OK;
}

void CLevel_Logo::ChoseLevel()
{
	if (ImGui::BeginListBox("#Chose Level#"))
	{
		const bool bLevelSelect = false;
		_int iIndex = 0;
		for (auto& pstr : arrLevel)
		{
			if (ImGui::Selectable(pstr.c_str(), bLevelSelect))
			{
				iLevelIndex = iIndex;
				iLevelIndex += 2;
			}
			++iIndex;
		}
		ImGui::EndListBox();
	}

	if(iLevelIndex >=0 )
		ChangeScene((iLevelIndex ));


	/*if (GetKeyState(VK_SPACE) & 0x8000)
	{
		CGameInstance*		pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

#ifdef FOR_MAPTOOL
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_MAPTOOL))))
			return;
#else
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return;
#endif

		Safe_Release(pGameInstance);

	}*/

}

void CLevel_Logo::ChangeScene(_uint iLevel)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, (LEVEL)(iLevel)))))
		return;
	Safe_Release(pGameInstance);
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Logo*		pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();


}
