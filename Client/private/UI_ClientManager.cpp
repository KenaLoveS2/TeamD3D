#include "stdafx.h"
#include "..\public\UI_ClientManager.h"
#include "GameInstance.h"

/* Canvas */
#include "UI_CanvasHUD.h"

/* Node */

IMPLEMENT_SINGLETON(CUI_ClientManager)

CUI_ClientManager::CUI_ClientManager()
{
}

HRESULT CUI_ClientManager::Ready_UIs(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	if (FAILED(Ready_Proto_TextureComponent(pDevice, pContext)))
	{
		MSG_BOX("Failed To Ready_Proto_TextureComponent : UI_ClientManager");
		return E_FAIL;
	}

	if (FAILED(Ready_Proto_GameObject(pDevice, pContext)))
	{
		MSG_BOX("Failed To Ready_Proto_GameObject : UI_ClientManager");
		return E_FAIL;
	}

	if (FAILED(Ready_Clone_GameObject(pDevice, pContext)))
	{
		MSG_BOX("Failed To Ready_Clone_GameObject : UI_ClientManager");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_ClientManager::Ready_Proto_TextureComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* RotActionIcon */
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_RotActionCarry"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/T_ArrowAmmo_ProgressMask.png")))))
		return E_FAIL;
	m_vecTextureProtoTag.push_back(L"Prototype_Component_Texture_RotActionCarry");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_RotActionCloud"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/T_RotAction_Cloud.png")))))
		return E_FAIL; 
	m_vecTextureProtoTag.push_back(L"Prototype_Component_Texture_RotActionCloud");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_RotActionNormal"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/T_RotAction_Icon.png")))))
		return E_FAIL;
	m_vecTextureProtoTag.push_back(L"Prototype_Component_Texture_RotActionNormal");

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_ClientManager::Ready_Proto_GameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* HUD */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Canvas_HUD"), CUI_CanvasHUD::Create(pDevice, pContext))))
		return E_FAIL;
	m_vecCanvasProtoTag.push_back(TEXT("Prototype_GameObject_UI_Canvas_HUD"));


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_ClientManager::Ready_Clone_GameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* Todo : File Load */

	/* Test */


	/* For. LifeBar */
	//FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(
	//	CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_UI_LifeFrame"),
	//	TEXT("Proto_GameObject_LifeBarFrame")), E_FAIL,
	//	"Failed To Clone LifeBarFrame");


	/* Ready Tool */
	//CUI_Tool* pTool = ;
	//pGameInstance->Add_ImguiObject(pTool);


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CUI_ClientManager::Free()
{
	m_vecTextureProtoTag.clear();
	m_vecCanvasProtoTag.clear();
}
