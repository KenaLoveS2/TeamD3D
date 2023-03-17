#include "stdafx.h"
#include "..\public\Level_Logo.h"

#include "Level_Loading.h"
#include "GameInstance.h"
#include "UI_NodeButton.h"

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

	POINT pt = CUtile::GetClientCursorPos(g_hWnd);
	_bool bClicked = false;
	_uint i = 0;
	_int iClicked = -1;
	for (auto button : m_vecButtons)
	{
		_float4 vPos = button->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
		_float2 vPosConvert = { vPos.x + 0.5f*g_iWinSizeX, -vPos.y + 0.5f*g_iWinSizeY };
		// left, top, right, bottom 
		RECT rc = { LONG(vPosConvert.x - 100.f), LONG(vPosConvert.y - 20.f),
			LONG(vPosConvert.x + 100.f), LONG(vPosConvert.y + 20.f) };

		if (PtInRect(&rc, pt))
		{
			if (bClicked = static_cast<CUI_NodeButton*>(button)->MouseOverEvent())
			{
				iClicked = i;
			}
		}
		else
			static_cast<CUI_NodeButton*>(button)->BackToNormal();

		i++;
	}

	if (bClicked)
	{
		if (iClicked == 0)
			ChangeScene((4));
	}

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

	
	{ /* Start Button */
		CUI_NodeButton* pButton = nullptr;
		CUI::UIDESC tDesc;
		tDesc.fileName = CUtile::Create_StringAuto(L"Node_ButtonLogoStart");
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_LOGO,
			TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Node_Button"), L"Node_ButtonLogoStart", &tDesc, (CGameObject**)&pButton)))
			return E_FAIL;
		static_cast<CUI_NodeButton*>(pButton)->Setting(L"새게임", CUI_NodeButton::TYPE_LOGO);
		pButton->Set_Active(true);
		m_vecButtons.push_back(pButton);
	}

	{ /* Option Button */
		CUI_NodeButton* pButton = nullptr;
		CUI::UIDESC tDesc;
		tDesc.fileName = CUtile::Create_StringAuto(L"Node_ButtonLogoOption");
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_LOGO,
			TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Node_Button"), L"Node_ButtonLogoOption", &tDesc, (CGameObject**)&pButton)))
			return E_FAIL;
		static_cast<CUI_NodeButton*>(pButton)->Setting(L"옵션", CUI_NodeButton::TYPE_LOGO);
		pButton->Set_Active(true);
		m_vecButtons.push_back(pButton);
	}


	{ /* Quit Button */
		CUI_NodeButton* pButton = nullptr;
		CUI::UIDESC tDesc;
		tDesc.fileName = CUtile::Create_StringAuto(L"Node_ButtonLogoQuit");
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_LOGO,
			TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Node_Button"), L"Node_ButtonLogoQuit", &tDesc, (CGameObject**)&pButton)))
			return E_FAIL;
		static_cast<CUI_NodeButton*>(pButton)->Setting(L"종료", CUI_NodeButton::TYPE_LOGO);
		pButton->Set_Active(true);
		m_vecButtons.push_back(pButton);
	}

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

	m_vecButtons.clear();
}
