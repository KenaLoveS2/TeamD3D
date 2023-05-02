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
#include "WorldTrigger_S2.h"


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
	, m_iClearQuestIndex(-1)
{
	for (_uint i = 0; i < (_uint)QUEST_END; ++i)
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
	, m_iClearQuestIndex(-1)
{
	for (_uint i = 0; i < (_uint)QUEST_END; ++i)
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
	
	if (!m_bActive)
		return;

	if (-1 != m_iClearQuestIndex)
	{
		if (static_cast<CUI_NodeQuest*>(m_vecOpenQuest[(_int)m_iClearQuestIndex])->Is_Finished())
		{
			auto& iter = m_vecOpenQuest.begin();
			for (iter ; iter != m_vecOpenQuest.end(); ++iter)
			{
				if (*iter == m_vecOpenQuest[m_iClearQuestIndex])
				{
					iter = m_vecOpenQuest.erase(iter);
					break;
				}
			}

			if (iter != m_vecOpenQuest.end())
			{
				for (iter; iter != m_vecOpenQuest.end(); ++iter)
					(*iter)->Move_Up();
			}

			m_iClearQuestIndex = -1;
		}
	}

	__super::Tick(fTimeDelta);

}

void CUI_CanvasQuest::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;
	//__super::Late_Tick(fTimeDelta);
	CUI::Late_Tick(fTimeDelta);

	//if (m_eState == STATE_NORMAL)
	//{
	//	m_fTmpAcc += fTimeDelta;
	//	if (m_fTmpAcc > 2.f)
	//	{
	//		m_eState = STATE_CLOSE;
	//		m_fTimeAcc = 0.f;
	//		m_bClose = true;
	//		m_fAlpha = 1.f;
	//	}
	//}

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

	return S_OK;
}

HRESULT CUI_CanvasQuest::Bind()
{
	//CKena* pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL,L"Layer_Player", L"Kena"));
	//if (pKena == nullptr)
	//	return E_FAIL;
	//
	//CSaiya* pSaiya = dynamic_cast<CSaiya*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_NPC", L"Saiya"));
	//if (pSaiya == nullptr)
	//	return E_FAIL;

	map<const _tchar*, CGameObject*>* pMap =  CGameInstance::GetInstance()->Get_GameObjects(g_LEVEL, L"Layer_Trigger");
	if (pMap == nullptr)
		return E_FAIL;
	if (!pMap->empty())
	{
		for (auto pair : *pMap)
		{
			CWorldTrigger_S2* pTrigger = dynamic_cast<CWorldTrigger_S2*>(pair.second);

			if (pTrigger != nullptr)
				pTrigger->m_TriggerDelegatorB.bind(this, &CUI_CanvasQuest::BindFunction);
		}
	}

	
	//pKena->m_PlayerQuestDelegator.bind(this, &CUI_CanvasQuest::BindFunction);
	//pSaiya->m_SaiyaDelegator.bind(this, &CUI_CanvasQuest::BindFunction);

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
	for (_uint i = 0; i < (_uint)QUEST_END; ++i)
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

	{ /* Quest Reward */
		string strTag = "Node_QuestReward";
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;
		_tchar* cloneTag = CUtile::StringToWideChar(strTag);
		tDesc.fileName = cloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_QuestReward", cloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strTag);
		pGameInstance->Add_String(cloneTag);
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
		m_bActive = true;
		static_cast<CUI_NodeQuest*>(m_vecNode[(_int)fValue])
			->Set_HeightByIndex(300.f - 140.f * (m_vecOpenQuest.size() % 4));
		m_vecOpenQuest.push_back(static_cast<CUI_NodeQuest*>(m_vecNode[(_int)fValue]));
		static_cast<CUI_NodeQuest*>(m_vecNode[(_int)fValue])->Appear(0.1f);
		break;
	case CUI_ClientManager::QUEST_LINE: /* Old */
		break;
	case CUI_ClientManager::QUEST_CLEAR:
	{
		static_cast<CUI_NodeQuest*>(m_vecNode[(_int)fValue])->Disappear(1.f);
		_smatrix matLocal = m_vecNode[(_int)fValue]->Get_LocalMatrix();
		_float4 vPos = { matLocal._41,matLocal._42,matLocal._43, 1.f };
		vPos.x += 400.f;
		static_cast<CUI_NodeQuestReward*>(m_vecNode[UI_REWARD])->RewardOn(vPos);

		m_iClearQuestIndex = -1;
		_int i = 0;
		for (auto& quest : m_vecOpenQuest)
		{
			if (m_vecNode[(_int)fValue] == quest)
			{
				m_iClearQuestIndex = i;
				break;
			}
			else
				i++;
		}
	}
		break;
	case CUI_ClientManager::QUEST_CLEAR_ALL:
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

	for (_uint i = 0; i < (_uint)QUEST_END; ++i)
		Safe_Release(m_Quests[i]);

	m_vecEffects.clear();

	m_vecOpenQuest.clear();
}
