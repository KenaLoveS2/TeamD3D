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

/* CanvasAmmo */
#include "UI_CanvasAmmo.h"
#include "UI_NodeAmmoBombFrame.h"
#include "UI_NodeAmmoBombGuage.h"
#include "UI_NodeAmmoArrowGuage.h"

/* CanvasAim */
#include "UI_CanvasAim.h"
#include "UI_NodeAimLine.h"
#include "UI_NodeAimArrow.h"
#include "UI_NodeAimBomb.h"

/* CanvasQuest */
#include "UI_CanvasQuest.h"
//#include "UI_NodeQuestMain.h"
//#include "UI_NodeQuestSub.h"
#include "UI_NodeQuest.h"
#include "Quest.h"

/* CanvasUpgrade */
#include "UI_CanvasUpgrade.h"
#include "UI_NodeSkill.h"
#include "UI_NodeSkillName.h"
#include "UI_NodeSkillDesc.h"
#include "UI_NodeSkillCond.h"
#include "UI_NodeRotLevel.h"
#include "UI_NodeRotGuage.h"

/* CanvasInventoryHeader */
#include "UI_CanvasInvHeader.h"
#include "UI_NodeKarma.h"
#include "UI_NodeCrystal.h"
#include "UI_NodeNumRots.h"

/* Effect (Common) */
#include "UI_NodeEffect.h"
/* Confirm Window */
#include "UI_CanvasConfirm.h"
#include "UI_NodeButton.h"
#include "UI_NodeConfWindow.h"

/* World UI */
#include "UI_MonsterHP.h"
#include "UI_RotIcon.h"

IMPLEMENT_SINGLETON(CUI_ClientManager)

CUI_ClientManager::CUI_ClientManager()
{
	for (_uint i = 0; i < EFFECT_END; ++i)
		m_vecEffects.push_back(nullptr);

	for (_uint i = 0; i < CANVAS_END; ++i)
		m_vecCanvas.push_back(nullptr);

	m_pConfirmWindow = nullptr;
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

	/* Fonts */
	if (FAILED(pGameInstance->Add_Font(pDevice, pContext, TEXT("Font_Gulim"), TEXT("../Bin/Resources/fonts/Gulim.spritefont"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Font(pDevice, pContext, TEXT("Font_Comic"), TEXT("../Bin/Resources/fonts/131.SpriteFont"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Font(pDevice, pContext, TEXT("Font_Basic0"), TEXT("../Bin/Resources/fonts/Font00.SpriteFont"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Font(pDevice, pContext, TEXT("Font_Basic1"), TEXT("../Bin/Resources/fonts/Font04.SpriteFont"))))
		return E_FAIL;
	/********************************************/
	/*				For. Canvas_HUD				*/
	/********************************************/
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
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_HUDRotIcons"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/Rot_%d.png"), 4))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_HUDRotIcons");

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
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/T_PipRing_%d.png"), 2))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_HUDPipGauge");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_HUDPipBar"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/T_PipRing_Frame.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_HUDPipBar");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_HUDPipFull"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/HUDPipFull.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_HUDPipFull");


	/********************************************/
	/*				For. Canvas_Ammo			*/
	/********************************************/
	/* Ammo Frame */
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_AmmoFrame"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/Ammo.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_AmmoFrame");

	/* Bomb Frame */
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_AmmoBombFrame"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/Ammo_BombFrame.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_AmmoBombFrame");

	/* Bomb Guage */
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_AmmoBombGuage"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/Ammo_BombGuage_%d.png"), 2))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_AmmoBombGuage");

	/* Arrow Guage Diffuse */
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_AmmoArrowGuage"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/T_ArrowAmmo_ProgressGradient.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_AmmoArrowGuage");

	/* Arrow Guage Mask */
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_AmmoArrowGuageMask"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/T_ArrowAmmo_ProgressMask.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_AmmoArrowGuageMask");


	/********************************************/
	/*				For. Canvas_Aim				*/
	/********************************************/
	/* Aim Frame(Default Aim Circle) */
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_DefaultAim"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/Aim/Aim_Default.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_DefaultAim");

	/* Aim Line */
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_AimLine"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/Aim/Aim_Line.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_AimLine");

	/* Aim Arrow */
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_AimArrow"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/Aim/Aim_Arrow.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_AimArrow");

	/* Aim Bomb */
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_AimBomb"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/Aim/Aim_Bomb.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_AimBomb");

	/********************************************/
	/*				For. Canvas_Quest			*/
	/********************************************/
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_QuestBG"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/05. Quest/T_CurrentObjectiveBG.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_QuestBG");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_QuestMain"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/05. Quest/primaryobjective.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_QuestMain");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_QuestSub"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/05. Quest/subobjective.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_QuestSub");


	/********************************************/
	/*				For. Effects				*/
	/********************************************/

	/* Pip Full */
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_PipFill"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/T_PipFill_Anim.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_PipFill");

	/* Bomb Full */
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_BombFill"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/T_BombFill_Anim.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_BombFill");

	/* Arrow Full */
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_ArrowFill"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/HUD/T_ArrowRefill_Anim.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_ArrowFill");

	/* Seperator */
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_Seperator"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/05. Quest/T_SeparatorAlpha_01.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_Seperator");


	/********************************************/
	/*				For. Upgrade				*/
	/********************************************/
	//if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_Inventory"),
	//	CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/06. Inventory/Inventory%d.png"),2))))
	//	return E_FAIL;
	//Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_Inventory");
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_UpgradeWindow"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/06. Inventory/UpgradeWindow.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_UpgradeWindow");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_PlayerSkillLevel0Locked"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/06. Inventory/upgrade/Output/Level0Blocked.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_PlayerSkillLevel0Locked");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_LockIcon"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/06. Inventory/T_Padlock_Icon.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_LockIcon");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_SelectedRing"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/06. Inventory/AbilityCore_HighlightRing.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_SelectedRing");
	
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_SelectedCircle"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/06. Inventory/upgrade/Kena_Ability_BGCircle_01.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_SelectedCircle");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_TitleBrush"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/06. Inventory/T_BrushStroke_AlphaOnly.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_TitleBrush");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_CountDown"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/06. Inventory/upgrade/Countdown.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_CountDown");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_SimpleBar"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/06. Inventory/T_ProgressBarBGRounded.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_SimpleBar");



	/********************************************/
	/*				For. InvHeader				*/
	/********************************************/
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_InvHeader"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/06. Inventory/InventoryHeader.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_InvHeader");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_CrystalIcon"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/06. Inventory/currency/T_Gem_Glow_128.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_CrystalIcon");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_KarmaIcon"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/06. Inventory/currency/T_Karma_Glow_128.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_KarmaIcon");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_NumRotsIcon"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/06. Inventory/currency/T_Rot_Glow_128.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_NumRotsIcon");


	/********************************************/
	/*				For. Common_Confirm			*/
	/********************************************/
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_ConfirmWindow"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/07. Common/T_UIBGBox_Alpha.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_ConfirmWindow");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_ButtonHighLight"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/07. Common/T_ButtonHighlight_BrushStroke.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_ButtonHighLight");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_ButtonHighLightCenter"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/07. Common/T_ButtonHighlight_BrushStroke_Centered.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_ButtonHighLightCenter");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_ButtonHighLightGlow"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/07. Common/T_ButtonHighlight_BrushStroke_Glow.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_ButtonHighLightGlow");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_ButtonHighLightGlowCenter"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/07. Common/T_ButtonHighlight_BrushStroke_Centered_Glow.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_ButtonHighLightGlowCenter");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_BGGlow"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/07. Common/T_UI_BGGlow.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_BGGlow");


	/********************************************/
	/*				For. World_UI				*/
	/********************************************/
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_RotFocus"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/Focusing/T_RotAction_Icon.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_RotFocus");




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
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "RotationZ(Loading)");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "RingGuage_Mask");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "For.AimThings");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "PaintDrop");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "AlphaTest");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "AlphaChange");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "MaskAlphaTestGuage");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "One_Effect(Terrain)"); /* temp */
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "MaskAlpha"); 
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "OnlyAlphaTexture");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "AlphaTestColor");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "SpriteColor");



																					 /* Event List */
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_EVENT, "Event_Guage");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_EVENT, "Event_ChangeImg");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_EVENT, "Event_Animation");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_EVENT, "Event_Transform");



	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CUI_ClientManager::Ready_Proto_GameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/********************************************/
	/*				For. Canvas_HUD				*/
	/********************************************/

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


	/********************************************/
	/*				For. Canvas_Ammo			*/
	/********************************************/

	/* Canvas Ammo(Arrow Frame) */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Canvas_Ammo"), CUI_CanvasAmmo::Create(pDevice, pContext))))
		return E_FAIL;
	Save_CanvasStrings(pGameInstance, L"Prototype_GameObject_UI_Canvas_Ammo");

	/* Bomb Frame */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_BombFrame"), CUI_NodeAmmoBombFrame::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_BombFrame");

	/* Bomb Guage */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_BombGuage"), CUI_NodeAmmoBombGuage::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_BombGuage");

	/* Arrow Guage */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_ArrowGuage"), CUI_NodeAmmoArrowGuage::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_ArrowGuage");


	/********************************************/
	/*				For. Canvas_Aim				*/
	/********************************************/

	/* Canvas Aim */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Canvas_Aim"), CUI_CanvasAim::Create(pDevice, pContext))))
		return E_FAIL;
	Save_CanvasStrings(pGameInstance, L"Prototype_GameObject_UI_Canvas_Aim");

	/* Aim Line */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_AimLine"), CUI_NodeAimLine::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_AimLine");

	/* Aim Arrow */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_AimArrow"), CUI_NodeAimArrow::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_AimArrow");

	/* Aim Bomb */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_AimBomb"), CUI_NodeAimBomb::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_AimBomb");


	/********************************************/
	/*				For. Canvas_Quest			*/
	/********************************************/

	/* Canvas Quest */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Canvas_Quest"), CUI_CanvasQuest::Create(pDevice, pContext))))
		return E_FAIL;
	Save_CanvasStrings(pGameInstance, L"Prototype_GameObject_UI_Canvas_Quest");

	///* Main Quest */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_QuestMain"), CUI_NodeQuestMain::Create(pDevice, pContext))))
	//	return E_FAIL;
	//Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_QuestMain");

	///* Sub Quest */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_QuestSub"), CUI_NodeQuestSub::Create(pDevice, pContext))))
	//	return E_FAIL;
	//Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_QuestSub");

	/* Node Quest */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_Quest"), CUI_NodeQuest::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_Quest");

	/* Real Quest Object (test)*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Quest"), CQuest::Create(pDevice, pContext))))
		return E_FAIL;


	/********************************************/
	/*				For. InvHeader				*/
	/********************************************/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Canvas_InvHeader"), CUI_CanvasInvHeader::Create(pDevice, pContext))))
		return E_FAIL;
	Save_CanvasStrings(pGameInstance, L"Prototype_GameObject_UI_Canvas_InvHeader");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_Karma"), CUI_NodeKarma::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_Karma");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_NumRots"), CUI_NodeNumRots::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_NumRots");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_Crystal"), CUI_NodeCrystal::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_Crystal");


	/********************************************/
	/*				For. Upgrades				*/
	/********************************************/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Canvas_Upgrade"), CUI_CanvasUpgrade::Create(pDevice, pContext))))
		return E_FAIL;
	Save_CanvasStrings(pGameInstance, L"Prototype_GameObject_UI_Canvas_Upgrade");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_Skill"), CUI_NodeSkill::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_Skill");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_SkillName"), CUI_NodeSkillName::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_SkillName");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_SkillDesc"), CUI_NodeSkillDesc::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_SkillDesc");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_SkillCondition"), CUI_NodeSkillCond::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_SkillCondition");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_RotLevel"), CUI_NodeRotLevel::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_RotLevel");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_RotGuage"), CUI_NodeRotGuage::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_RotGuage");

	/********************************************/
	/*				For. Effects				*/
	/********************************************/
	/* Effect (Sprite Animation) */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_Effect"), CUI_NodeEffect::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_Effect");

	/********************************************/
	/*				For. Confirm				*/
	/********************************************/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Canvas_Confirm"), CUI_CanvasConfirm::Create(pDevice, pContext))))
		return E_FAIL;
	Save_CanvasStrings(pGameInstance, L"Prototype_GameObject_UI_Canvas_Confirm");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_Button"), CUI_NodeButton::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_Button");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_Window"), CUI_NodeConfWindow::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_Window");



	/********************************************/
	/*				For. WorldUI				*/
	/********************************************/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MonsterHP"), CUI_MonsterHP::Create(pDevice, pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_RotFocuss"), CUI_RotIcon::Create(pDevice, pContext))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_ClientManager::Ready_Clone_GameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{


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

void CUI_ClientManager::Set_Canvas(UI_CANVAS eID, CUI_Canvas * pCanvas)
{
	if (eID < 0 || eID >= m_vecCanvas.size() || pCanvas == nullptr)
		return;
	m_vecCanvas[eID] = pCanvas;

	if (eID == CANVAS_CONFIRM)
		m_pConfirmWindow = static_cast<CUI_CanvasConfirm*>(pCanvas);
}

CUI_Canvas * CUI_ClientManager::Get_Canvas(UI_CANVAS eID)
{
	if (eID < 0 || eID >= m_vecCanvas.size())
		return nullptr;
	return m_vecCanvas[eID];
}

void CUI_ClientManager::Set_Effect(UI_EFFECT eID, CUI_NodeEffect * pEffect)
{
	if (eID < 0 || eID >= m_vecEffects.size() || pEffect == nullptr)
		return;
	m_vecEffects[eID] = pEffect;
}

CUI_NodeEffect * CUI_ClientManager::Get_Effect(UI_EFFECT eID)
{
	if (eID < 0 || eID >= m_vecEffects.size())
		return nullptr;
	return m_vecEffects[eID];
}

void CUI_ClientManager::Call_ConfirmWindow(wstring msg, _bool bActive, CUI_Canvas* pCaller)
{
	if (nullptr == m_pConfirmWindow)
		return;

	if (nullptr == pCaller)
	{
		m_pConfirmWindow->Set_Active(false);
		return;
	}


	//m_pConfirmWindow->Set_Active(true);
	m_pConfirmWindow->Set_Message(msg, pCaller);

}

void CUI_ClientManager::Free()
{
	m_vecEffects.clear();
	m_vecCanvas.clear();
}
