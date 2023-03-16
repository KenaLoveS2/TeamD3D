#include "stdafx.h"
#include "..\public\UI_CanvasQuest.h"
#include "GameInstance.h"
#include "Quest.h"
#include "UI_NodeQuest.h"
#include "Kena.h"
#include "UI_NodeEffect.h"
#include "Saiya.h"

CUI_CanvasQuest::CUI_CanvasQuest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Canvas(pDevice, pContext)
	, m_fTimeAcc(0.8f)
	, m_iCurQuestIndex(0)
	, m_bOpen(false)
	, m_bClose(false)
	, m_fAlpha(0.f)
	, m_eState(STATE_NORMAL)
	, m_fTmpAcc(0.f)
{
	for (_uint i = 0; i < QUEST_END; ++i)
		m_Quests[i] = nullptr;
}

CUI_CanvasQuest::CUI_CanvasQuest(const CUI_CanvasQuest & rhs)
	: CUI_Canvas(rhs)
	, m_fTimeAcc(1.0f)
	, m_iCurQuestIndex(0)
	, m_bOpen(false)
	, m_bClose(false)
	, m_fAlpha(0.f)
	, m_eState(STATE_NORMAL)
	, m_fTmpAcc(0.f)
{
	for (_uint i = 0; i < QUEST_END; ++i)
		m_Quests[i] = nullptr;
}

HRESULT CUI_CanvasQuest::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CanvasQuest::Initialize(void * pArg)
{
	/* Get a Texture Size, and Make an Initial Matrix */
	XMStoreFloat4x4(&m_matInit, XMMatrixScaling(952.f, 1200.f, 1.f));

	if (FAILED(__super::Initialize(pArg)))
	{
		m_tDesc.vSize = { (_float)g_iWinSizeX, (_float)g_iWinSizeY };
		m_tDesc.vPos = { g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f };
		m_pTransformCom->Set_Scaled(_float3(m_tDesc.vSize.x, m_tDesc.vSize.y, 1.f));
		m_pTransformCom->Set_Scaled(_float3(952.f, 1200.f, 1.f));

		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,
			XMVectorSet(0.f, 0.f, 0.f, 1.f));
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components : CanvasQuest");
		return E_FAIL;
	}
	if (FAILED(Ready_Quests()))
	{
		MSG_BOX("Failed To Ready Quests : CanvasQuest");
		return E_FAIL;
	}
	if (FAILED(Ready_Nodes()))
	{
		MSG_BOX("Failed To Ready Nodes : CanvasQuest");
		return E_FAIL;
	}

	//m_bActive = true;

	return S_OK;
}

void CUI_CanvasQuest::Tick(_float fTimeDelta)
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

	if (m_bOpen)
	{
		m_fTimeAcc -= 0.2f * fTimeDelta;
		if (m_fTimeAcc > 0.6f)
		m_fAlpha += 0.5f * fTimeDelta;
		if (m_fAlpha >= 1.f)
			m_fAlpha = 1.f;

		if (m_fTimeAcc <= 0.f)
			m_bOpen = false;
	}
	else if (m_bClose)
	{
		if (m_fTimeAcc > 0.5f)
			m_fAlpha -= 0.5f * fTimeDelta;
		m_fTimeAcc += 0.2f * fTimeDelta;
		if (m_fTimeAcc >= 1.f)
		{
			m_bActive = false;
			m_bClose = false;
		}
	}
	else
		m_eState = STATE_NORMAL;

	/*test */
	//for (_uint i = 0; i < QUEST_END; ++i)
	//	m_Quests[i]->Tick(fTimeDelta);

	__super::Tick(fTimeDelta);

}

void CUI_CanvasQuest::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (m_eState == STATE_NORMAL)
	{
		m_fTmpAcc += fTimeDelta;
		if (m_fTmpAcc > 3.f)
		{
			m_eState = STATE_CLOSE;
			m_fTimeAcc = 0.f;
			m_bClose = true;
			m_fAlpha = 1.f;
		}
	}

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_CanvasQuest::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CUI_CanvasQuest::Bind()
{
	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	//CKena* pKena = dynamic_cast<CKena*>(pGameInstance->Get_GameObjectPtr(pGameInstance->Get_CurLevelIndex(),L"Layer_Player", L"Kena"));
	//RELEASE_INSTANCE(CGameInstance);

	//if (pKena == nullptr)
	//	return E_FAIL;
	//pKena->m_PlayerDelegator.bind(this, &CUI_CanvasQuest::BindFunction);

	//m_Quests[0]->m_QuestDelegator.bind(this, &CUI_CanvasQuest::BindFunction);

	CSaiya* pSaiya = dynamic_cast<CSaiya*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_NPC", L"Saiya"));
	if (pSaiya == nullptr)
		return E_FAIL;
	pSaiya->m_SaiyaDelegator.bind(this, &CUI_CanvasQuest::BindFunction);

	m_bBindFinished = true;
	return S_OK;
}

HRESULT CUI_CanvasQuest::Ready_Nodes()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/*	Note : the reason using unstable temp address variable "fileName" is that
	fileName(CloneTag) needed while it cloned when loading the data.
	(The cloneTag is stored after the clone process.)

	230303 : save in stringManager. But won't be use again.(cause already stored in cloneTag)
	*/
	for (_uint i = 0; i < QUEST_END; ++i)
	{
		CUI* pMain = nullptr;
		CUI::UIDESC tMainDesc;
		string strMain;
		strMain = "Node_QuestMain" + to_string(i);
		_tchar* mainCloneTag = CUtile::StringToWideChar(strMain);
		tMainDesc.fileName = mainCloneTag;
		pMain = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Quest", mainCloneTag, &tMainDesc));
		if (FAILED(Add_Node(pMain)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strMain);
		pGameInstance->Add_String(mainCloneTag);
		static_cast<CUI_NodeQuest*>(pMain)->Set_QuestString(m_Quests[i]->Get_MainQuest());
		/* test */
		//pMain->Set_Active(true);
		_int iNumSubs = m_Quests[i]->Get_NumSubs();
		for (_int j = 0; j < iNumSubs; ++j)
		{
			CUI* pSub = nullptr;
			string strSub;
			CUI::UIDESC tDescSub;
			strSub = "Node_QuestSub" + to_string(i) + "_" + to_string(j);
			_tchar* subCloneTag = CUtile::StringToWideChar(strSub);
			tDescSub.fileName = subCloneTag;
			pSub = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Quest", subCloneTag, &tDescSub));
			if (FAILED(Add_Node(pSub)))
				return E_FAIL;
			m_vecNodeCloneTag.push_back(strSub);
			pGameInstance->Add_String(subCloneTag);
			static_cast<CUI_NodeQuest*>(pSub)->Set_QuestString(m_Quests[i]->Get_SubQuest(j));

			//pSub->Set_Active(true);

		}
	}


	/* Effect */
	//for (_uint i = 0; i < QUEST_END; ++i)
	_uint i = 0;
	{
		string strMEffect = "Node_EffectClear_" + to_string(i);
		CUI::UIDESC tDescEffectM;
		_tchar* tagMTag = CUtile::StringToWideChar(strMEffect);
		tDescEffectM.fileName = tagMTag;
		CUI_NodeEffect* pEffectUI 
			= static_cast<CUI_NodeEffect*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Effect", tagMTag, &tDescEffectM));
		if (FAILED(Add_Node(pEffectUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strMEffect);
		pGameInstance->Add_String(tagMTag);
		m_vecEffects.push_back(pEffectUI);

		_int iNumSubs = m_Quests[i]->Get_NumSubs();
		for (_int j = 0; j < iNumSubs; ++j)
		{
			string strEffectS = "Node_EffectClear" + to_string(i) + "_" + to_string(j);
			CUI::UIDESC tDescEffectS;
			_tchar* tagEffectS = CUtile::StringToWideChar(strEffectS);
			tDescEffectS.fileName = tagEffectS;
			CUI_NodeEffect* pUISub 
				= static_cast<CUI_NodeEffect*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Effect", tagEffectS, &tDescEffectS));
			if (FAILED(Add_Node(pUISub)))
				return E_FAIL;
			m_vecNodeCloneTag.push_back(strEffectS);
			pGameInstance->Add_String(tagEffectS);
			m_vecEffects.push_back(pUISub);

		}
	}


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_CanvasQuest::SetUp_Components()
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

HRESULT CUI_CanvasQuest::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Time", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_State", &m_eState, sizeof(_uint))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CanvasQuest::Ready_Quests()
{
	/* test */
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CQuest::QUESTDESC tDesc;
	tDesc.filePath = L"Quest0.json";
	m_Quests[0] = static_cast<CQuest*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Quest",
		L"Quest0", &tDesc));

	tDesc.filePath = L"Quest1.json";
	m_Quests[1] = static_cast<CQuest*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Quest",
		L"Quest1", &tDesc));

	tDesc.filePath = L"Quest2.json";
	m_Quests[2] = static_cast<CQuest*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Quest",
		L"Quest2", &tDesc));

	RELEASE_INSTANCE(CGameInstance);
	/* ~test */
	return S_OK;
}

void CUI_CanvasQuest::BindFunction(CUI_ClientManager::UI_PRESENT eType, _bool bValue, _float fValue, wstring wstr)
{
	switch (eType)
	{
	case CUI_ClientManager::QUEST_:
		m_bOpen = false;
		m_bClose = false;
		if (bValue)
		{
			m_eState = STATE_OPEN;
			m_bActive = true; /* If m_bActive is false, it won't be seen at all. */
			m_fTimeAcc = 0.8f;
			m_bOpen = true;
			m_fAlpha = 0.5f;
		}
		else
		{
			m_eState = STATE_CLOSE;
			m_fTimeAcc = 0.f;
			m_bClose = true;
			m_fAlpha = 1.f;
		}
		break;

	case CUI_ClientManager::QUEST_LINE:
		m_vecNode[(_int)fValue]->Set_Active(true);
		break;
	}
}

void CUI_CanvasQuest::BindFunction(CUI_ClientManager::UI_PRESENT eType, CUI_ClientManager::UI_FUNCTION eFunc, _float fValue)
{
	switch (eFunc)
	{
	case CUI_ClientManager::FUNC_DEFAULT:
		Default(eType, fValue);
		break;
	case CUI_ClientManager::FUNC_SWITCH:
		Switch(eType, fValue);
		break;
	case CUI_ClientManager::FUNC_CHECK:
		Check(eType, fValue);
		break;
	}
}

void CUI_CanvasQuest::Default(CUI_ClientManager::UI_PRESENT eType, _float fData)
{
}

void CUI_CanvasQuest::Switch(CUI_ClientManager::UI_PRESENT eType, _float fData)
{
	switch (eType)
	{
	case CUI_ClientManager::QUEST_:/* Switch On/Off Canvas */
		m_bOpen = false;
		m_bClose = false;
		if (!m_bActive)
		{
			m_eState = STATE_OPEN;
			m_bActive = true; /* If m_bActive is false, it won't be seen at all. */
			m_fTimeAcc = 0.8f;
			m_bOpen = true;
			m_fAlpha = 0.5f;
		}
		else
		{
			m_eState = STATE_CLOSE;
			m_fTimeAcc = 0.f;
			m_bClose = true;
			m_fAlpha = 1.f;
		}
		break;
	case CUI_ClientManager::QUEST_LINE:
		m_vecNode[(_int)fData]->Set_Active(true);
		break;
	}
}

void CUI_CanvasQuest::Check(CUI_ClientManager::UI_PRESENT eType, _float fData)
{
	switch (eType)
	{
	case CUI_ClientManager::QUEST_LINE:
		_int iIndex = (_int)fData % 4;
		if (iIndex >= (_int)m_vecEffects.size())
			return;

		m_vecEffects[iIndex]->Start_Effect(
			m_vecNode[(_int)fData], 328.f, 5.f);
		break;
	}
}

CUI_CanvasQuest * CUI_CanvasQuest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_CanvasQuest*	pInstance = new CUI_CanvasQuest(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_CanvasQuest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_CanvasQuest::Clone(void * pArg)
{
	CUI_CanvasQuest*	pInstance = new CUI_CanvasQuest(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_CanvasQuest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CanvasQuest::Free()
{
	__super::Free();

	for (_uint i = 0; i < QUEST_END; ++i)
		Safe_Release(m_Quests[i]);

	m_vecEffects.clear();
}
