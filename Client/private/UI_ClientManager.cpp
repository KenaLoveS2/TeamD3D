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
#include "UI_NodeSkillVideo.h"

/* CanvasInventoryHeader */
#include "UI_CanvasInvHeader.h"
#include "UI_NodeKarma.h"
#include "UI_NodeCrystal.h"
#include "UI_NodeNumRots.h"

/* Commons */
// Effect 
#include "UI_NodeEffect.h"
// Confirm Window
#include "UI_CanvasConfirm.h"
#include "UI_NodeButton.h"
#include "UI_NodeConfWindow.h"
// Video
#include "UI_NodeVideo.h"

/* Bottom */
#include "UI_CanvasBottom.h"
#include "UI_NodeLetterBox.h"
#include "UI_NodeChat.h"
#include "UI_NodeKey.h"

/* Top */
#include "UI_CanvasTop.h"
#include "UI_NodeTitle.h"
#include "UI_NodeRotCnt.h"
#include "UI_NodeRotFrontGuage.h"
#include "UI_NodeRotArrow.h"
#include "UI_NodeLvUp.h"
#include "UI_NodeBossHP.h"
#include "UI_NodeMood.h"

/* HatCart */
#include "UI_CanvasHatCart.h"
#include "UI_CanvasItemBar.h"
#include "UI_NodeItemBox.h"
#include "UI_NodeHat.h"
#include "UI_NodeScrollLine.h"
#include "UI_NodeScrollBar.h"
#include "UI_NodeCurrentCrystal.h"
#include "UI_NodeHatCartBG.h"
#include "UI_NodeSideBar.h"

/* InfoWindow */
#include "UI_CanvasInfo.h"

/* World UI */
#include "UI_MonsterHP.h"
#include "UI_RotIcon.h"
#include "UI_FocusMonster.h"
#include "UI_FocusNPC.h"
#include "UI_FocusMonsterParts.h"
#include "UI_MousePointer.h"
#include "UI_Weakpoint.h"

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

	if (FAILED(pGameInstance->Add_Font(pDevice, pContext, TEXT("Font_Jangmi0"), TEXT("../Bin/Resources/fonts/NanumJangmi00.SpriteFont"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Font(pDevice, pContext, TEXT("Font_JungE0"), TEXT("../Bin/Resources/fonts/NanumJungHagSaeng00.SpriteFont"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Font(pDevice, pContext, TEXT("Font_SR0"), TEXT("../Bin/Resources/fonts/SquareRound00.SpriteFont"))))
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
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/07. Common/T_UI_BGGlow_%d.png"), 2))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_BGGlow");


	/********************************************/
	/*				For. Bottom					*/
	/********************************************/
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_KeyboardIcon"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/02. KeyboardIcon/SS_XboxKBMIconAtlas.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_KeyboardIcon");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_LetterBox"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/08. Title/LetterBox.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_LetterBox");


	/********************************************/
	/*				For. Top					*/
	/********************************************/
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_MapTitle"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/08. Title/MapTitle/TitleCard_%d.png"), 4))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_MapTitle");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_IconArrowUp"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/RotLevel/T_ArrowRight.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_IconArrowUp");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_LvUpCard"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/RotLevel/T_TitleCard_RotLevel.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_LvUpCard");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_FlareBack"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/99. Effect/E_Effect_93.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_FlareBack");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_FlareBack1"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/99. Effect/E_Effect_42.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_FlareBack1");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_FlareRound"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/99. Effect/T_FlareRound_Noise.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_FlareRound");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_FlareShape"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/99. Effect/FlareBomb.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_FlareShape");


	/********************************************/
	/*				For. HatCart				*/
	/********************************************/
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_ItemBG1"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/09. Shop/ItemBG_1_%d.png"), 2))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_ItemBG1");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_ItemBG2"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/09. Shop/ItemBG_2_%d.png"), 2))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_ItemBG2");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_ItemBG3"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/09. Shop/ItemBG_3_%d.png"), 2))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_ItemBG3");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_ItemBG4"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/09. Shop/ItemBG_4_%d.png"), 2))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_ItemBG4");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_HatIconBox"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/09. Shop/HatIconBG_%d.png"), 2))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_HatIconBox");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_HatIcon"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/09. Shop/Hats/Hat_%d.png"), 16))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_HatIcon");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_ScrollLine"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/09. Shop/T_SeparatorAlpha_01.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_ScrollLine");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_ScrollBar"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/09. Shop/AbilityLine.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_ScrollBar");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_CartBottom"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/09. Shop/BarBottm.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_CartBottom");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_CartTop"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/09. Shop/BarTop.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_CartTop");

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_CartBG"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/09. Shop/CartBG.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_CartBG");



	/********************************************/
	/*				For. Keys					*/
	/********************************************/
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_KeyIcon"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/02. KeyboardIcon/key_%d.png"), 5))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_KeyIcon"); // 66

	/********************************************/
	/*				For. SimpleRot				*/
	/********************************************/
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_SimpleRotIcon"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/06. Inventory/RotIconSimple.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_SimpleRotIcon"); // 67

	/********************************************/
	/*				For. InfoWindow				*/
	/********************************************/
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_InfoWindow"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/04. Menu/Information_0.png")))))
		return E_FAIL;
	Save_TextureComStrings(pGameInstance, L"Prototype_Component_Texture_InfoWindow"); // 67





	/* Not Include in the Texture List */
	/********************************************/
	/*				For. World_UI				*/
	/********************************************/
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_RotFocus"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/03. PlayerUI/Focusing/T_RotAction_Icon.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_LockOnCenter"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/10. WorldUI/T_LockOn.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_LockOnSide"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/10. WorldUI/T_LockOn_Small.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_LockOnSide2"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/10. WorldUI/T_LockOn_Small2.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_FocusNPC"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/10. WorldUI/NPCCarrot2.png")))))
		return E_FAIL;

	// WeakPoint
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_WeakPoint"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/10. WorldUI/cursor.png")))))
		return E_FAIL;


	/********************************************/
	/*				For. SkillIcons				*/
	/********************************************/
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_SkillIcon_Bomb"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/06. Inventory/upgrade/Output/Bomb/Bomb%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_SkillIcon_Bow"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/06. Inventory/upgrade/Output/Bow/Bow%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_SkillIcon_Shield"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/06. Inventory/upgrade/Output/Shield/Shield%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_SkillIcon_Stick"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/06. Inventory/upgrade/Output/Stick/Stick%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_SkillIcon_RotSkill"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/UI/06. Inventory/upgrade/Output/Rot/Rot%d.png"), 5))))
		return E_FAIL;


	/********************************************/
	/*				For. Videos					*/
	/********************************************/
	// MeleeCore
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_MeleeCore"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Video/MeleeCore/MeleeCore_Tutorial_PC%03d.png"), 215))))
		return E_FAIL;
	// AirHeavy
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_AirHeavy"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Video/AirHeavy/AirHeavy_Tutorial_PC%03d.png"), 185))))
		return E_FAIL;
	// SprintHeavy
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_SprintHeavy"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Video/SprintHeavy/SprintHeavy_Tutorial_PC%03d.png"), 127))))
		return E_FAIL;
	// SprintLight
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_SprintLight"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Video/SprintLight/SprintLight_Tutorial_PC%03d.png"), 113))))
		return E_FAIL;
	// PerfectHeavy
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_PerfectHeavy"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Video/PerfectHeavy/PerfectHeavy_Tutorial_PC%03d.png"), 130))))
		return E_FAIL;

	// Shield
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_Shield"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Video/Shield/Shield_Tutorial_PC%03d.png"), 185))))
		return E_FAIL;
	// ParryPip
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_ParryPip"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Video/ParryPip/ParryPip_Tutorial_PC%03d.png"), 118))))
		return E_FAIL;
	// ParryCounter
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_ParryCounter"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Video/ParryCounter/ParryCounter_Tutorial_PC%03d.png"), 154))))
		return E_FAIL;

	// MultiBomb
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_MultiBomb"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Video/MultiBomb/MultiBomb_Tutorial_PC%03d.png"), 163))))
		return E_FAIL;
	// BombAbility
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_BombAbility"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Video/BombAbility/BombAbility_Tutorial_PC%03d.png"), 330))))
		return E_FAIL;
	// QuickDrawBomb
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_QuickDrawBomb"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Video/QuickDrawBomb/QuickDrawBomb_Tutorial_PC%03d.png"), 168))))
		return E_FAIL;
	// BombSplatter2
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_BombSplatter2"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Video/BombSplatter2/BombSplatter2_PC%03d.png"), 192))))
		return E_FAIL;

	// BowAbility
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_BowAbility"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Video/BowAbility/BowAbility_Tutorial_PC%03d.png"), 163))))
		return E_FAIL;
	// Quiver
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_Quiver"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Video/Quiver/Quiver_Tutorial_PC%03d.png"), 183))))
		return E_FAIL;
	// Focus
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_Focus"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Video/Focus/Focus_Tutorial_PC%03d.png"), 148))))
		return E_FAIL;

	// IntoTheFray
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_IntoTheFray"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Video/IntoTheFray/IntoTheFray_Tutorial_PC%03d.png"), 221))))
		return E_FAIL;

	// RotActionSelector
	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_RotActionSelector"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Video/RotActionSelector/RotActionSelector_Tutorial_PC%03d.png"), 226))))
		return E_FAIL;

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
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "MonsterBar");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "NoDiffuseColorGuage");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "OnlyAlphaWithColorTexture");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "HDR");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "SwapMask");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_RENDERPASS, "ForInfoWindow");


	

	/* Event List */
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_EVENT, "Event_Guage");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_EVENT, "Event_ChangeImg");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_EVENT, "Event_Animation");
	//pGameInstance->Add_UIString(CUI_Manager::STRKEY_EVENT, "Event_Transform");
	pGameInstance->Add_UIString(CUI_Manager::STRKEY_EVENT, "Event_Fade");




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

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_SkillVideo"), CUI_NodeSkillVideo::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_SkillVideo");

	/********************************************/
	/*				For. Bottom					*/
	/********************************************/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Canvas_Bottom"), CUI_CanvasBottom::Create(pDevice, pContext))))
		return E_FAIL;
	Save_CanvasStrings(pGameInstance, L"Prototype_GameObject_UI_Canvas_Bottom");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_LetterBox"), CUI_NodeLetterBox::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_LetterBox");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_Chat"), CUI_NodeChat::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_Chat");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_Key"), CUI_NodeKey::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_Key");


	/********************************************/
	/*				For. Top					*/
	/********************************************/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Canvas_Top"), CUI_CanvasTop::Create(pDevice, pContext))))
		return E_FAIL;
	Save_CanvasStrings(pGameInstance, L"Prototype_GameObject_UI_Canvas_Top");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_MapTitle"), CUI_NodeTitle::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_MapTitle");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_RotMaxCnt"), CUI_NodeRotCnt::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_RotMaxCnt");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_RotFrontGuage"), CUI_NodeRotFrontGuage::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_RotFrontGuage");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_RotArrow"), CUI_NodeRotArrow::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_RotArrow");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_RotLvUpCard"), CUI_NodeLvUp::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_RotLvUpCard");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_Mood"), CUI_NodeMood::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_Mood");

	/********************************************/
	/*				For. HatCart				*/
	/********************************************/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Canvas_HatCart"), CUI_CanvasHatCart::Create(pDevice, pContext))))
		return E_FAIL;
	Save_CanvasStrings(pGameInstance, L"Prototype_GameObject_UI_Canvas_HatCart");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Canvas_ItemBar"), CUI_CanvasItemBar::Create(pDevice, pContext))))
		return E_FAIL;
	//Save_CanvasStrings(pGameInstance, L"Prototype_GameObject_UI_Canvas_ItemBar");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_ItemBox"), CUI_NodeItemBox::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_ItemBox");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_ItemHat"), CUI_NodeHat::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_ItemHat");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_ScrollLine"), CUI_NodeScrollLine::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_ScrollLine");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_ScrollBar"), CUI_NodeScrollBar::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_ScrollBar");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_CurrentCrystal"), CUI_NodeCurrentCrystal::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_CurrentCrystal");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_HatCartBG"), CUI_NodeHatCartBG::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_HatCartBG");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_SideBar"), CUI_NodeSideBar::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_SideBar");

	/********************************************/
	/*				For. InfoWindow				*/
	/********************************************/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Canvas_InfoWindow"), CUI_CanvasInfo::Create(pDevice, pContext))))
		return E_FAIL;
	Save_CanvasStrings(pGameInstance, L"Prototype_GameObject_UI_Canvas_InfoWindow");


	/* Commons */
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

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_BossHP"), CUI_NodeBossHP::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_BossHP");

	/********************************************/
	/*				For. Video					*/
	/********************************************/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Node_Video"), CUI_NodeVideo::Create(pDevice, pContext))))
		return E_FAIL;
	Save_NodeStrings(pGameInstance, L"Prototype_GameObject_UI_Node_Video");



	/********************************************/
	/*				For. WorldUI				*/
	/********************************************/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MonsterHP"), CUI_MonsterHP::Create(pDevice, pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_RotFocuss"), CUI_RotIcon::Create(pDevice, pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_FocusMonster"), CUI_FocusMonster::Create(pDevice, pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_FocusMonsterParts"), CUI_FocusMonsterParts::Create(pDevice, pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_FocusNPC"), CUI_FocusNPC::Create(pDevice, pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Weakpoint"), CUI_Weakpoint::Create(pDevice, pContext))))
		return E_FAIL;
	// MousePointer
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MousePointer"), CUI_MousePointer::Create(pDevice, pContext))))
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
	wstring		strTag = pTag;
	pGameInstance->Add_UIWString(CUI_Manager::WSTRKEY_TEXTURE_PROTOTAG, strTag);

	// 2) Add TextureName(To make Texture List)
	wstring head = L"Prototype_Component_Texture_";
	size_t headLength = head.length();

	wstring tag = pTag;
	size_t length = tag.length();
	length = tag.length() - headLength;

	wstring name = tag.substr(headLength, length);

	string str = CUtile::wstring_to_utf8(name);
	//string str = string(tag.begin() + headLength, tag.begin() + headLength + length);

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
	str = CUtile::wstring_to_utf8(tag);
	//str = str.assign(tag.begin(), tag.end());
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
	//str = str.assign(tag.begin(), tag.end());
	str = CUtile::wstring_to_utf8(tag);
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

void CUI_ClientManager::Switch_FrontUI(_bool bActive)
{
	m_vecCanvas[CANVAS_HUD]->Set_Active(bActive);
	m_vecCanvas[CANVAS_AMMO]->Set_Active(bActive);
}

void CUI_ClientManager::Free()
{
	m_vecEffects.clear();
	m_vecCanvas.clear();
}
