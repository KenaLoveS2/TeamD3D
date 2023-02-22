#include "stdafx.h"
#include "..\public\UI_ClientManager.h"
#include "GameInstance.h"

/* Canvas */
#include "UI_CanvasHUD.h"

/* Node */
#include "UI_NodeHUDHPBar.h"
#include "UI_NodeHUDHP.h"

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

	if (FAILED(Ready_InformationList()))
	{
		MSG_BOX("Failed To Ready_InformationList : UI_ClientManager");
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

	/* For. HUD */
	/* HUD Frame */
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_HUDFrame"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/HUDFrame.png")))))
		return E_FAIL;
	pGameInstance->Add_UITextureTag(L"Prototype_Component_Texture_HUDFrame");
	m_vecTextureProtoTag.push_back(L"Prototype_Component_Texture_HUDFrame");

	/* Bar */
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_HUDHPBar"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/HealthbarBG.png")))))
		return E_FAIL;
	pGameInstance->Add_UITextureTag(L"Prototype_Component_Texture_HUDHPBar");
	m_vecTextureProtoTag.push_back(L"Prototype_Component_Texture_HUDHPBar");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_HUDHPBarMask"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/HealthbarMask.png")))))
		return E_FAIL;
	pGameInstance->Add_UITextureTag(L"Prototype_Component_Texture_HUDHPBarMask");
	m_vecTextureProtoTag.push_back(L"Prototype_Component_Texture_HUDHPBarMask");
	
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_HUDHPBarNoise"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/HealthbarNoise.png")))))
		return E_FAIL;
	pGameInstance->Add_UITextureTag(L"Prototype_Component_Texture_HUDHPBarNoise");
	m_vecTextureProtoTag.push_back(L"Prototype_Component_Texture_HUDHPBarNoise");


	/* RingBar */
	/* PipGuage */

	/* RotActionIcon */


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_ClientManager::Ready_InformationList()
{
	/* For. Save List for UI */
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* RenderPass List */
	pGameInstance->Add_UIString(L"RenderPass", "Default");
	pGameInstance->Add_UIString(L"RenderPass", "x(Effect)");
	pGameInstance->Add_UIString(L"RenderPass", "DiffuseAlphaBlend");
	pGameInstance->Add_UIString(L"RenderPass", "MaskMap");
	pGameInstance->Add_UIString(L"RenderPass", "HPBar");


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

	/* HP Bar */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_HP"), CUI_NodeHUDHP::Create(pDevice, pContext))))
		return E_FAIL;
	m_vecNodeProtoTag.push_back(TEXT("Prototype_GameObject_UI_Node_HP"));

	/* HP Guage */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_HPBar"), CUI_NodeHUDHPBar::Create(pDevice, pContext))))
		return E_FAIL;
	m_vecNodeProtoTag.push_back(TEXT("Prototype_GameObject_UI_Node_HPBar"));

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
	m_vecNodeProtoTag.clear();
}
