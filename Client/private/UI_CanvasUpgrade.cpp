#include "stdafx.h"
#include "..\public\UI_CanvasUpgrade.h"
#include "GameInstance.h"
#include "SkillInfo.h"
#include "UI_NodeSkill.h"
#include "UI_NodeEffect.h"
#include "UI_NodeVideo.h"
#include "UI_NodeSkillName.h"
#include "UI_NodeSkillDesc.h"
#include "UI_NodeSkillCond.h"
#include "UI_NodeRotLevel.h"
#include "UI_NodeRotGuage.h"

#include "Kena.h"
#include "Kena_Status.h"

CUI_CanvasUpgrade::CUI_CanvasUpgrade(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Canvas(pDevice, pContext)
	, m_pSelected(nullptr)
	, m_iPickedIndex(-1)
	, m_bPick(true)
	, m_pPlayer(nullptr)
{
	for (auto skill : m_pSkills)
		skill = nullptr;
}

CUI_CanvasUpgrade::CUI_CanvasUpgrade(const CUI_CanvasUpgrade & rhs)
	: CUI_Canvas(rhs)
	, m_pSelected(nullptr)
	, m_iPickedIndex(-1)
	, m_bPick(true)
	, m_pPlayer(nullptr)
{
	for (auto skill : m_pSkills)
		skill = nullptr;
}

HRESULT CUI_CanvasUpgrade::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CanvasUpgrade::Initialize(void * pArg)
{
	/* Get a Texture Size, and Make an Initial Matrix */
	XMStoreFloat4x4(&m_matInit, XMMatrixScaling((_float)g_iWinSizeX, 830.f, 1.f));

	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3((_float)g_iWinSizeX, 830.f, 1.f));
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components : CanvasUpgrade");
		return E_FAIL;
	}

	if (FAILED(Ready_SkillInfo()))
	{
		MSG_BOX("Failed To Ready PlayerSkill : CanvasUpgrade");
		return E_FAIL;
	}

	if (FAILED(Ready_Nodes()))
	{
		MSG_BOX("Failed To Ready Nodes : CanvasUpgrade");
		return E_FAIL;
	}
	//m_bActive = true;

	return S_OK;
}

HRESULT	CUI_CanvasUpgrade::Late_Initialize(void* pArg)
{
	// 레벨 이동 시 플레이어 정보 받게 함.
	return S_OK;
}

void CUI_CanvasUpgrade::Tick(_float fTimeDelta)
{
	if (!m_bBindFinished)
	{
		if (FAILED(Bind()))
		{
			//	MSG_BOX("Bind Failed");

			return;
		}
	}

	if (!m_bActive)
		return;

	/* Return To Play */
	if (CGameInstance::GetInstance()->Key_Down(DIK_M))
	{
		CGameInstance::GetInstance()->Get_Back();
		m_bActive = false;
		return;
	}


	/* Test */
	//if(CGameInstance::GetInstance()->Key_Down(DIK_X))
	//{
	//	static_cast<CUI_NodeVideo*>(m_vecNode[UI_SKILLVIDEO])->Play_Video(
	//		L"AirHeavy_Tutorial_PC", true, 0.05f);
	//}

	/* Picking the Skill Icons */
	Picking();

	/* Try To UnLock */
	if (CGameInstance::GetInstance()->Key_Down(DIK_SPACE))
	{
		if (m_pSelected != nullptr)
		{
			//if(CSkillInfo::CHECK_UNLOCKED_AVAILABLE == 
			//	m_pSkills[m_iPickedIndex / 5]
			//	->Check(static_cast<CUI_NodeSkill*>(m_vecNode[m_iPickedIndex])->Get_Level(), m_pPlayer))
			{
				m_bPick = false;
				CUI_ClientManager::GetInstance()->Call_ConfirmWindow(L"이 업그레이드를 잠금 해제하시겠습니까?", true, this);	
				CGameInstance::GetInstance()->Play_Sound(L"UI_ConfirmWindow.ogg", 1.f, false, SOUND_UI);
			}
		}
	}




	__super::Tick(fTimeDelta);

}

void CUI_CanvasUpgrade::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_CanvasUpgrade::Render()
{
	__super::Render();

	return S_OK;
}

void CUI_CanvasUpgrade::Common_Function(_bool bResult)
{
	m_bPick = true;

	if (bResult) /* Confirm > Yes (Unlock the skill) */
	{
	/* 1. Count Karma */
	_int iKarmaNeed =	m_pSkills[m_iPickedIndex / 5]->Get_SkillDesc(m_pSelected->Get_Level()).conditions[CSkillInfo::CONDITION_KARMA];
	if (nullptr != m_pPlayer)
	{
		_int iKarmaNow = m_pPlayer->Get_Status()->Get_Karma();
		m_pPlayer->Get_Status()->Set_Karma(iKarmaNow - iKarmaNeed);
	}

	/* 2. Change the States */
	_uint iLevel = static_cast<CUI_NodeSkill*>(m_vecNode[m_iPickedIndex])->Get_Level();
	 m_pSkills[m_iPickedIndex / 5]->UnLock(iLevel);
	 static_cast<CUI_NodeSkill*>(m_vecNode[m_iPickedIndex])->State_Change(2);

	 /* 3. Update the Player's Info */
	 CKena_Status::SKILLTAB eCategory = (CKena_Status::SKILLTAB)(m_iPickedIndex / 5);
	 m_pPlayer->Get_Status()->Unlock_Skill(eCategory, iLevel);

	 /* 4. Update the info */
	 Spread();
	}
}

void CUI_CanvasUpgrade::Set_Caller(CGameObject * pObj)
{
	m_pPlayer = static_cast<CKena*>(pObj);

	/* Excute when Inventory is Opened -> Setting UnChangable info*/
	CKena_Status* pStatus = m_pPlayer->Get_Status();

	static_cast<CUI_NodeRotLevel*>(m_vecNode[UI_ROTLEVEL])
		->Set_Info(pStatus->Get_RotLevel(),	pStatus->Get_RotMax(),	pStatus->Get_RotCount());

	static_cast<CUI_NodeRotGuage*>(m_vecNode[UI_ROTGUAGE])->Set_Guage((_float)pStatus->Get_RotCount() / (_float)pStatus->Get_RotMax());

}

HRESULT CUI_CanvasUpgrade::Bind()
{

	m_bBindFinished = true;
	return S_OK;
}

HRESULT CUI_CanvasUpgrade::Ready_Nodes()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* PlayerSkill */
	for (_uint i = 0; i < TYPE_END-1; ++i)
	{
		string strHeader = "Node_PlayerSkill" + m_pSkills[i]->Get_TypeName();

		for (_uint j = 0; j < CSkillInfo::LEVEL_END; ++j)
		{
			CUI* pUI = nullptr;
			CUI::UIDESC tDesc;
			
			string strCloneTag = strHeader + to_string(j);
			_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
			tDesc.fileName = wstrCloneTag;
			pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Skill", wstrCloneTag, &tDesc));
			if (FAILED(Add_Node(pUI)))
				return E_FAIL;
			m_vecNodeCloneTag.push_back(strCloneTag);
			pGameInstance->Add_String(wstrCloneTag);
			if (FAILED(static_cast<CUI_NodeSkill*>(pUI)->Setting(m_pSkills[i]->Get_TextureProtoTag(), j)))
				MSG_BOX("Failed To Setting : CanvasUpgrade");
			static_cast<CUI_NodeSkill*>(pUI)->State_Change(0);
		}
	}

	/* RotSkill */
	for (_uint i = 1; i < CSkillInfo::LEVEL_END; ++i)
	{
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;

		string strCloneTag = "Node_" + m_pSkills[TYPE_ROT]->Get_TypeName() + to_string(i);
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Skill", wstrCloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCloneTag);
		pGameInstance->Add_String(wstrCloneTag);
		if (FAILED(static_cast<CUI_NodeSkill*>(pUI)->Setting(m_pSkills[TYPE_ROT]->Get_TextureProtoTag(), i)))
			MSG_BOX("Failed To Setting : CanvasUpgrade");
		static_cast<CUI_NodeSkill*>(pUI)->State_Change(0);
	}


	/* SelectedRing */
	{
		string strRing = "Node_SelectRing";
		CUI::UIDESC tDescRing;
		_tchar* tagRing = CUtile::StringToWideChar(strRing);
		tDescRing.fileName = tagRing;
		CUI_NodeEffect* pEffectUI
			= static_cast<CUI_NodeEffect*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Effect", tagRing, &tDescRing));
		if (FAILED(Add_Node(pEffectUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strRing);
		pGameInstance->Add_String(tagRing);
		m_vecEffects.push_back(pEffectUI);
	}


	/* SelectedCircle */
	{
		string strCircle = "Node_SelectCircle";
		CUI::UIDESC tDescCircle;
		_tchar* tagCircle = CUtile::StringToWideChar(strCircle);
		tDescCircle.fileName = tagCircle;
		CUI_NodeEffect* pCircle
			= static_cast<CUI_NodeEffect*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Effect", tagCircle, &tDescCircle));
		if (FAILED(Add_Node(pCircle)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCircle);
		pGameInstance->Add_String(tagCircle);
		m_vecEffects.push_back(pCircle);
	}


	/* Lock */
	for (_uint i = 0; i < 20; ++i)
	{
		string strLock = "Node_EffectLock_" + to_string(i);
		CUI::UIDESC tDescLock;
		_tchar* tagLock = CUtile::StringToWideChar(strLock);
		tDescLock.fileName = tagLock;
		CUI_NodeEffect* pLock
			= static_cast<CUI_NodeEffect*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Effect", tagLock, &tDescLock));
		if (FAILED(Add_Node(pLock)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strLock);
		pGameInstance->Add_String(tagLock);
		m_vecEffects.push_back(pLock);

		if (i % 5 != 0)
		{
			//pLock->Start_Effect(m_vecNode[i], 0.f, 0.f);
			static_cast<CUI_NodeSkill*>(m_vecNode[i])->Set_LockEffect(pLock);
		}

	}

	/* SkillVideo */
	{
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;

		string strCloneTag = "Node_SkillVideo";
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Video", wstrCloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCloneTag);
		pGameInstance->Add_String(wstrCloneTag);
	}


	/* SkillName */
	{
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;

		string strCloneTag = "Node_SkillName";
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_SkillName", wstrCloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCloneTag);
		pGameInstance->Add_String(wstrCloneTag);
	}

	/* SkillCond */
	{
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;

		string strCloneTag = "Node_SkillCondition";
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_SkillCondition", wstrCloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCloneTag);
		pGameInstance->Add_String(wstrCloneTag);
	}

	/* SkillDesc */
	{
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;

		string strCloneTag = "Node_SkillDesc";
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_SkillDesc", wstrCloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCloneTag);
		pGameInstance->Add_String(wstrCloneTag);
	}

	/* RotLevel */
	{
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;

		string strCloneTag = "Node_RotLevel";
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_RotLevel", wstrCloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCloneTag);
		pGameInstance->Add_String(wstrCloneTag);
	}

	/* RotGuage */
	{
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;

		string strCloneTag = "Node_RotGuage";
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_RotGuage", wstrCloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCloneTag);
		pGameInstance->Add_String(wstrCloneTag);
	}
	
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CUI_CanvasUpgrade::SetUp_Components()
{
	/* Renderer */
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom))
		return E_FAIL;

	/* Shader */
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom))
		return E_FAIL;

	/* VIBuffer_Rect */
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CanvasUpgrade::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CUI::SetUp_ShaderResources();

	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_tDesc.ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_tDesc.ProjMatrix)))
		return E_FAIL;

	if (m_pTextureCom[TEXTURE_DIFFUSE] != nullptr)
	{
		if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
			return E_FAIL;
	}

	if (m_pTextureCom[TEXTURE_MASK] != nullptr)
	{
		if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_CanvasUpgrade::Ready_SkillInfo()
{
	/* Load Data of Player Skill */

	m_pSkills[TYPE_STICK] =
		CSkillInfo::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Skill/PlayerSkill_Stick.json"));

	m_pSkills[TYPE_SHIELD] =
		CSkillInfo::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Skill/PlayerSkill_Shield.json"));

	m_pSkills[TYPE_BOW] =
		CSkillInfo::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Skill/PlayerSkill_Bow.json"));

	m_pSkills[TYPE_BOMB] =
		CSkillInfo::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Skill/PlayerSkill_Bomb.json"));

	m_pSkills[TYPE_ROT] =
		CSkillInfo::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Skill/RotSkill.json"));

	return S_OK;
}

void CUI_CanvasUpgrade::Picking()
{
	/* Temp Methd*/
	if (!m_bPick)
		return;

	/* Picking */
	POINT pt = CUtile::GetClientCursorPos(g_hWnd);

	_bool isPicked = false; /* Blue Effect */
	for (_uint i = 0; i <= UI_ROTSKILL_END; ++i)
	{
		_float4 vPos = m_vecNode[i]->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
		_float2 vPosConvert = { vPos.x + 0.5f*g_iWinSizeX, -vPos.y + 0.5f*g_iWinSizeY };
		// left, top, right, bottom 
		RECT rc = { LONG(vPosConvert.x - 30.f), LONG(vPosConvert.y - 30.f),
			LONG(vPosConvert.x + 30.f), LONG(vPosConvert.y + 30.f) };

		if (PtInRect(&rc, pt))
		{
			if (i >= UI_ROTSKILLS_START && i <= UI_ROTSKILL_END)
			{
				isPicked = true;
				if (m_pSelected != m_vecNode[i])
					m_vecEffects[EFFECT_BLUE]->Change_Scale(1.3f);
				else
					m_vecEffects[EFFECT_BLUE]->Change_Scale(1.6f);
				m_vecEffects[EFFECT_BLUE]->Start_Effect(m_vecNode[i], 0.f, 0.f);
			}
			else if (i % 5 != 0)	 /* Level0 doesn't need this effect */
			{
				isPicked = true;
				if (m_pSelected != m_vecNode[i])
					m_vecEffects[EFFECT_BLUE]->BackToOriginalScale();
				else
					m_vecEffects[EFFECT_BLUE]->Change_Scale(1.2f);

				m_vecEffects[EFFECT_BLUE]->Start_Effect(m_vecNode[i], 0.f, 0.f);
			}


			if (CGameInstance::GetInstance()->Mouse_Down(DIM_LB))
			{
				if (nullptr != m_pSelected) /* Previous One */
					m_pSelected->BackToOriginal();

				CGameInstance::GetInstance()->Play_Sound(L"UI_UpgradeMouseOn.ogg", 1.f, false, SOUND_UI);

				m_pSelected = static_cast<CUI_NodeSkill*>(m_vecNode[i]);
				m_iPickedIndex = i;
				m_pSelected->Picked(1.2f);
				/* Spread Selected Skill's Information To Nodes */
				Spread();

				if (i >= UI_ROTSKILLS_START && i <= UI_ROTSKILL_END)
				{
					m_vecEffects[EFFECT_RING]->Change_Scale(1.5f);
					m_vecEffects[EFFECT_RING]->Start_Effect(m_pSelected, 0.f, 0.f);
					m_vecEffects[EFFECT_BLUE]->Change_Scale(1.6f);
				}
				else if (i % 5 == 0) /* Player's Level0 SKill */
				{
					m_vecEffects[EFFECT_RING]->Change_Scale(1.4f);
					m_vecEffects[EFFECT_RING]->Start_Effect(m_pSelected, 0.f, 0.f);
				}
				else
				{
					m_vecEffects[EFFECT_RING]->BackToOriginalScale();
					m_vecEffects[EFFECT_RING]->Start_Effect(m_pSelected, 0.f, 0.f);
					m_vecEffects[EFFECT_BLUE]->Change_Scale(1.2f);

				}
			}
		}
	}

	if (!isPicked)
		m_vecEffects[EFFECT_BLUE]->Set_Active(false);

}

void CUI_CanvasUpgrade::Spread()
{
	CSkillInfo::SKILLDESC tDesc;
	tDesc = m_pSkills[m_iPickedIndex / 5]->Get_SkillDesc(m_pSelected->Get_Level());
	static_cast<CUI_NodeSkillName*>(m_vecNode[UI_SKILLNAME])->Set_String(tDesc.wstrName);
	static_cast<CUI_NodeSkillDesc*>(m_vecNode[UI_SKILLDESC])->Set_String(tDesc.wstrDesc);
	static_cast<CUI_NodeVideo*>(m_vecNode[UI_SKILLVIDEO])->Play_Video(tDesc.wstrVideo, true, 0.05f);

	CSkillInfo::CHECK eCheck = m_pSkills[m_iPickedIndex / 5]->Check(m_pSelected->Get_Level(), m_pPlayer);
	static_cast<CUI_NodeSkillCond*>(m_vecNode[UI_SKILLCOND])->Set_Condition(tDesc, eCheck);



}

CUI_CanvasUpgrade * CUI_CanvasUpgrade::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_CanvasUpgrade*	pInstance = new CUI_CanvasUpgrade(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_CanvasUpgrade");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_CanvasUpgrade::Clone(void * pArg)
{
	CUI_CanvasUpgrade*	pInstance = new CUI_CanvasUpgrade(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_CanvasUpgrade");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CanvasUpgrade::Free()
{
	if (m_isCloned)
	{
		for (_uint i = 0; i < TYPE_END; ++i)
			Safe_Release(m_pSkills[i]);
	}
	
	m_vecEffects.clear();


	__super::Free();
}
