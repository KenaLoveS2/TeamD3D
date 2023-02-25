#include "stdafx.h"
#include "..\public\UI_ClientManager.h"
#include "GameInstance.h"

/* CanvasHUD */
#include "UI_CanvasHUD.h"
#include "UI_NodeHUDHPBar.h"
#include "UI_NodeHUDHP.h"
#include "UI_NodeHUDRot.h"
#include "UI_NodeHUDPip.h"
#include "UI_NodeHUDPipBar.h"
#include "UI_NodeHUDShield.h"
#include "UI_NodeHUDShieldBar.h"


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
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_HUDFrame");

	/* Bar */
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_HUDHPBar"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/HealthbarBG.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_HUDHPBar");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_HUDHPBarMask"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/HealthbarMask.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_HUDHPBarMask");
	
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_HUDHPBarNoise"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/HealthbarNoise.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_HUDHPBarNoise");

	/* RotActionIcon */
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_HUDRotCarry"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/T_RotCarry_Anim.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_HUDRotCarry");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_HUDRotCloud"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/T_RotCloud_Anim.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_HUDRotCloud");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_HUDRotHide"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/T_RotHide_Anim.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_HUDRotHide");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_HUDRotNeutral"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/T_RotNeutral_Anim.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_HUDRotNeutral");


	/* PipEnergy */
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_HUDPipGauge"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/T_PipRing.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_HUDPipGauge");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_HUDPipBar"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/T_PipRing_Frame.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_HUDPipBar");

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_ClientManager::Ready_InformationList()
{
	/* For. Save List for UI */
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* RenderPass List */
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "Default");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "DiffuseAlphaBlend");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "MaskMap");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "HPBar");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "Sprite_AlphaBlend");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "DefaultUVMove");	
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "RingGuage");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "BarGuage");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "Trial_AlphaBlend");


	/* Event List */
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_EVENT, "BarGuage");

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CUI_ClientManager::Ready_Proto_GameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/************** Canvas_HUD ****************/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Canvas_HUD"), CUI_CanvasHUD::Create(pDevice, pContext))))
		return E_FAIL;
	Save_CanvasStrings(pGameInstance, L"Prototype_GameObject_UI_Canvas_HUD");

	/* HP Bar */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_HPBar"), CUI_NodeHUDHPBar::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_HPBar");

	/* HP Guage */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_HP"), CUI_NodeHUDHP::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_HP");

	/* HP Shield */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_ShieldBar"), CUI_NodeHUDShieldBar::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_ShieldBar");

	/* Shield Guage */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_Shield"), CUI_NodeHUDShield::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_Shield");

	/* RotIcon */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_RotIcon"), CUI_NodeHUDRot::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_RotIcon");

	/* Pip Guage */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_PipGuage"), CUI_NodeHUDPip::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_PipGuage");

	/* Pip Bar */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_PipBar"), CUI_NodeHUDPipBar::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_PipBar");
	/************** ~Canvas_HUD ****************/


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_ClientManager::Ready_Clone_GameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* Todo : File Load */



	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CUI_ClientManager::Save_TextureComStrings(CGameInstance* pGameInstance, const _tchar*  pTag)
{
	// Ex. Prototype_Component_Texture_HUDFrame

	// 1) Add TextureProtoTag(wstr)
	pGameInstance->Add_UIWString(CUI_Manager::WSTRKEY_TEXTURE_PROTOTAG, pTag);

	// 2) Add TextureName(To make Texture List)
	wstring head = L"Prototype_Component_Texture_";
	size_t headLength = head.length();

	wstring tag = pTag;
	size_t length = tag.length();
	length = tag.length() - headLength;

	string str;
	str = str.assign(tag.begin(), tag.end()).substr(headLength, length);
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_TEXTURE_NAME, str);
}

void CUI_ClientManager::Save_CanvasStrings(CGameInstance* pGameInstance, const _tchar * pTag)
{
	// Ex. Prototype_GameObject_UI_Canvas_HUD

	// 1) Add Canvas ProtoTag
	pGameInstance->Add_UIWString(CUI_Manager::WSTRKEY_CANVAS_PROTOTAG, pTag);

	// 2) Add Canvas CloneTag
	wstring head = L"Prototype_GameObject_UI_";
	size_t headLength = head.length();
	wstring tag = pTag;
	size_t length = tag.length();
	length = tag.length() - headLength;
	tag = tag.substr(headLength, length);
	pGameInstance->Add_UIWString(CUI_Manager::WSTRKEY_CANVAS_CLONETAG, tag);

	// 3) Add Canvas Name(CloneTag(wstr) to str)
	string str;
	str = str.assign(tag.begin(), tag.end());
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_CANVAS_NAME, str);

}

void CUI_ClientManager::Save_NodeStrings(CGameInstance* pGameInstance, const _tchar * pTag)
{
	// Ex. Prototype_GameObject_UI_Node_HPBar

	// 1) Add Canvas ProtoTag
	pGameInstance->Add_UIWString(CUI_Manager::WSTRKEY_NODE_PROTOTAG, pTag);

	// 2) Add Canvas CloneTag
	wstring head = L"Prototype_GameObject_UI_";
	size_t headLength = head.length();
	wstring tag = pTag;
	size_t length = tag.length();
	length = tag.length() - headLength;
	tag = tag.substr(headLength, length);
	pGameInstance->Add_UIWString(CUI_Manager::WSTRKEY_NODE_CLONETAG, tag);

	// 3) Add Canvas Name(CloneTag(wstr) to str)
	string str;
	str = str.assign(tag.begin(), tag.end());
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_NODE_NAME, str);

}

void CUI_ClientManager::Free()
{
}
