#include "stdafx.h"
#include "..\public\UI_CanvasQuest.h"
#include "GameInstance.h"
#include "Quest.h"
#include "UI_NodeQuest.h"
#include "Kena.h"
#include "UI_NodeEffect.h"
#include "UI_NodeQuestReward.h"
#include "Saiya.h"
#include "Kena_Status.h"
#include "WorldTrigger.h"

CUI_CanvasQuest::CUI_CanvasQuest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Canvas(pDevice, pContext)
	, m_fTimeAcc(0.8f)
	, m_iCurQuestIndex(0)
	, m_bOpen(false)
	, m_bClose(false)
	, m_fAlpha(0.f)
	, m_eState(STATE_NORMAL)
	, m_fTmpAcc(0.f)
	, m_bFollowAlpha(false)
	, m_iCurLineIndex(0)
	, m_fTimeOn(0.f)
	, m_fTimeOnAcc(0.f)
{
	for (_uint i = 0; i < QUEST_END; ++i)
	{
		m_Quests[i] = nullptr;
		m_iNumsQuestLine[i] = 0;
	}
}

CUI_CanvasQuest::CUI_CanvasQuest(const CUI_CanvasQuest& rhs)
	: CUI_Canvas(rhs)
	, m_fTimeAcc(1.0f)
	, m_iCurQuestIndex(0)
	, m_bOpen(false)
	, m_bClose(false)
	, m_fAlpha(0.f)
	, m_eState(STATE_NORMAL)
	, m_fTmpAcc(0.f)
	, m_bFollowAlpha(false)
	, m_iCurLineIndex(0)
	, m_fTimeOn(0.f)
	, m_fTimeOnAcc(0.f)
{
	for (_uint i = 0; i < QUEST_END; ++i)
	{
		m_Quests[i] = nullptr;
		m_iNumsQuestLine[i] = 0;
	}
}

HRESULT CUI_CanvasQuest::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CanvasQuest::Initialize(void* pArg)
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

	//m_bActive = true;
	//m_bOpen = false;
	//m_bClose = false;

	//static _bool bOpen = false;
	//if (CGameInstance::GetInstance()->Key_Down(DIK_W))
	//{
	//	if (m_bActive == false)
	//		bOpen = true;
	//	else
	//		bOpen = false;
	//	BindFunction(CUI_ClientManager::QUEST_, bOpen, 1.f, L"");
	//	//bOpen = !bOpen;

	//	//m_pRendererCom->ReCompile();
	//	//m_pShaderCom->ReCompile();
	//}

	//if (CGameInstance::GetInstance()->Key_Down(DIK_Q))
	//{
	//	m_iCurQuestIndex = 0;
	//	m_iCurLineIndex = 0;
	//	m_bFollowAlpha = false;
	//	for (_uint i = 0; i < (_uint)m_vecNode.size() - 1; ++i)
	//		m_vecNode[i]->Set_Active(false);
	//}
	//if (CGameInstance::GetInstance()->Key_Down(DIK_I))
	//{
	//	static _float fLine = 0;
	//	BindFunction(CUI_ClientManager::QUEST_LINE, false, fLine, L"");
	//	fLine = fmod(fLine + 1.f, (_float)m_iNumsQuestLine[m_iCurQuestIndex]);
	//}
	//if (CGameInstance::GetInstance()->Key_Down(DIK_U))
	//{
	//	static _float fLine = 0;
	//	BindFunction(CUI_ClientManager::QUEST_CLEAR, false, fLine, L"");
	//	fLine = fmod(fLine + 1.f, (_float)m_iNumsQuestLine[m_iCurQuestIndex]);
	//}


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

		if (m_fAlpha >= 0.8f)
		{
			_int iCount = 0;
			for (_int i = 0; i < QUEST_END; ++i)
			{
				if (i < m_iCurQuestIndex)
					iCount += (m_iNumsQuestLine[i]);
				else if (i == m_iCurQuestIndex)
				{
					for (_int line = 0; line <= m_iCurLineIndex; ++line)
					{
						static_cast<CUI_NodeQuest*>(m_vecNode[iCount + line])->QuestOn();
						_float fAlpha = static_cast<CUI_NodeQuest*>(m_vecNode[iCount + line])->Get_Alpha();
						if (line <= m_iLastClearLine && line != 0)
							static_cast<CUI_NodeEffect*>(m_vecEffects[line])->Set_Alpha(fAlpha);
					}
					break;
				}

			}
		}

	}
	else if (m_bClose)
	{
		static_cast<CUI_NodeQuest*>(m_vecNode[0])->Set_Alpha(m_fAlpha);
		if (m_fTimeAcc > 0.5f)
		{
			m_fAlpha -= 0.5f * fTimeDelta;
		}
		m_fTimeAcc += 0.5f * fTimeDelta;
		if (m_fTimeAcc >= 1.f)
		{
			m_bActive = false;
			m_bClose = false;
		}

		_int iCount = 0;
		for (_int i = 0; i < QUEST_END; ++i)
		{
			if (i < m_iCurQuestIndex)
				iCount += m_iNumsQuestLine[i];
			else if (i == m_iCurQuestIndex)
			{
				for (_int line = 0; line <= m_iCurLineIndex; ++line)
				{
					static_cast<CUI_NodeQuest*>(m_vecNode[iCount + line])->QuestOff();
					_float fAlpha = static_cast<CUI_NodeQuest*>(m_vecNode[iCount + line])->Get_Alpha();
					if (line <= m_iLastClearLine && line != 0)
						static_cast<CUI_NodeEffect*>(m_vecEffects[line])->Set_Alpha(fAlpha);
				}
				break;
			}
		}

		//for (_int i = 0; i <= m_iCurLineIndex; ++i)
		//{
		//	static_cast<CUI_NodeQuest*>(m_vecNode[m_iCurQuestIndex * m_iNumsQuestLine[m_iCurQuestIndex] + i])->QuestOff();

		//	if (i <= m_iLastClearLine)
		//		static_cast<CUI_NodeEffect*>(m_vecEffects[i])->Set_Alpha(m_fAlpha);
		//}



	}
	else
		m_eState = STATE_NORMAL;

	if (static_cast<CUI_NodeQuestReward*>(m_vecNode[(_int)m_vecNode.size() - 1])->Get_Alpha() > 0.8f)
	{
		for (_uint i = 0; i < (_uint)m_vecNode.size() - 1; ++i)
			m_vecNode[i]->Set_Active(false);

		if (m_iCurQuestIndex == QUEST_1)
		{
			BindFunction(CUI_ClientManager::QUEST_LINE, true, 0, L"");
			BindFunction(CUI_ClientManager::QUEST_LINE, true, 1, L"");
			BindFunction(CUI_ClientManager::QUEST_LINE, true, 2, L"");
			BindFunction(CUI_ClientManager::QUEST_LINE, true, 3, L"");
		}

		if (m_iCurQuestIndex == QUEST_2)
		{
			BindFunction(CUI_ClientManager::QUEST_LINE, true, 0, L"");
			BindFunction(CUI_ClientManager::QUEST_LINE, true, 1, L"");
			BindFunction(CUI_ClientManager::QUEST_LINE, true, 2, L"");
			BindFunction(CUI_ClientManager::QUEST_LINE, true, 3, L"");
		}

		if (m_iCurQuestIndex == QUEST_END)
			m_bFollowAlpha = true;
	}

	if (m_bFollowAlpha == true)
	{
		m_fAlpha = static_cast<CUI_NodeQuestReward*>(m_vecNode[(_int)m_vecNode.size() - 1])->Get_Alpha();
		if (m_fAlpha <= 0.f)
			m_bActive = false;
	}

	/*test */
	//for (_uint i = 0; i < QUEST_END; ++i)
	//	m_Quests[i]->Tick(fTimeDelta);

	__super::Tick(fTimeDelta);

}

void CUI_CanvasQuest::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;
	//__super::Late_Tick(fTimeDelta);
	CUI::Late_Tick(fTimeDelta);

	if (m_eState == STATE_NORMAL)
{
	m_fTmpAcc += fTimeDelta;
	if (m_fTmpAcc > 5.f)
	{
		m_eState = STATE_CLOSE;
		m_fTimeAcc = 0.f;
		m_bClose = true;
		m_fAlpha = 1.f;
	}
}



	for (auto e : m_vecEvents)
		e->Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && m_bActive)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UIHDR, this);

		/* Nodes added to RenderList After the canvas. */
		/* So It can be guaranteed that Canvas Draw first */
		for (auto node : m_vecNode)
			node->Late_Tick(fTimeDelta);
	}


}

HRESULT CUI_CanvasQuest::Render()
{
	__super::Render();

	/* Button Click */

	return S_OK;
}

HRESULT CUI_CanvasQuest::Bind()
{
	CKena* pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL,L"Layer_Player", L"Kena"));
	if (pKena == nullptr)
		return E_FAIL;

	//pKena->m_PlayerDelegator.bind(this, &CUI_CanvasQuest::BindFunction);

	//m_Quests[0]->m_QuestDelegator.bind(this, &CUI_CanvasQuest::BindFunction);

	CSaiya* pSaiya = dynamic_cast<CSaiya*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_NPC", L"Saiya"));
	if (pSaiya == nullptr)
		return E_FAIL;
	
	
	pKena->m_PlayerQuestDelegator.bind(this, &CUI_CanvasQuest::BindFunction);
	pSaiya->m_SaiyaDelegator.bind(this, &CUI_CanvasQuest::BindFunction);

	//CWorldTrigger* p_world_trigger = dynamic_cast<CWorldTrigger*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL,
	//	L"Layer_Effect", L"UIWorldTrigger"));
	//if (p_world_trigger == nullptr)
	//	return E_FAIL;
	//p_world_trigger->m_WorldTriggerDelegator2.bind(this, &CUI_CanvasQuest::BindFunction);



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
		m_iNumsQuestLine[i] = iNumSubs + 1;
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

	/* Reward */
	{
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;
		string strCloneTag = "Node_QuestReward";
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Node_QuestReward", wstrCloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCloneTag);
		CGameInstance::GetInstance()->Add_String(wstrCloneTag);
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

	_float4 vPos = m_pTransformCom->Get_Position();
	vPos.z = 0.1f;
	m_pTransformCom->Set_Position(vPos);
	//_uint renderpass = m_iRenderPass;
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
		m_iLineIndexIter = 0;
		m_iClearIndexIter = 0;
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
	{
		if (m_iCurLineIndex >= QUEST_END)
			return;
		if ((_int)fValue > m_iNumsQuestLine[m_iCurQuestIndex])
		{
			MSG_BOX("Try to Contact Wrong Line");
			return;
		}
		m_iCurLineIndex = (_int)fValue;
		m_vecNode[m_iCurQuestIndex * m_iNumsQuestLine[m_iCurQuestIndex] + (_int)fValue]->Set_Active(true);
		break;
	}
	case CUI_ClientManager::QUEST_CLEAR:
	{
		if (m_iCurLineIndex >= QUEST_END)
			return;
		if ((_int)fValue > m_iNumsQuestLine[m_iCurQuestIndex])
			return;
		m_iLastClearLine = (_int)fValue;
		//_int iLen = m_Quests[m_iCurQuestIndex]->Get_QuestStringLength((_int)fValue);
		///* 아이들을 따라가세요 : 1 */
		//if (iLen > 10)
		//	static_cast<CUI_NodeEffect*>(m_vecNode[(_int)fValue])->Change_Scale({ iLen - 10, });
		m_vecEffects[(_int)fValue]->Start_Effect(
			m_vecNode[(_int)fValue], 328.f, -2.f);

		if ((_int)fValue == m_iNumsQuestLine[m_iCurQuestIndex] - 1)
		{
			m_iCurQuestIndex++;
			if (m_iCurQuestIndex >= QUEST_END)
				m_iCurQuestIndex = QUEST_END;
			BindFunction(CUI_ClientManager::QUEST_CLEAR_ALL, false, 1.f, L"");
		}
		break;
	}
	case CUI_ClientManager::QUEST_CLEAR_ALL:

		static_cast<CUI_NodeQuestReward*>(m_vecNode[(_int)m_vecNode.size() - 1])->RewardOn();

		/* Temp code  */
		CKena* pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena"));
		if (pKena != nullptr)
			pKena->Get_Status()->Set_Karma(pKena->Get_Status()->Get_Karma() + 500);

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
		_int iIndex = (_int)fData % m_iNumsQuestLine[m_iCurQuestIndex];
		if (iIndex >= (_int)m_vecEffects.size())
			return;

		m_vecEffects[iIndex]->Start_Effect(
			m_vecNode[(_int)fData], 328.f, 5.f);
		break;
	}
}

CUI_CanvasQuest* CUI_CanvasQuest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_CanvasQuest* pInstance = new CUI_CanvasQuest(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_CanvasQuest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_CanvasQuest::Clone(void* pArg)
{
	CUI_CanvasQuest* pInstance = new CUI_CanvasQuest(*this);
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
