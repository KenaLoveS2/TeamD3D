#include "stdafx.h"
#include "WorldTrigger_S2.h"
#include "GameInstance.h"
#include "Kena.h"
#include "PostFX.h"

CWorldTrigger_S2::CWorldTrigger_S2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
	, m_pRendererCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_bTriggerOn(true)
	, m_bActive(true)
	, m_pPlayer(nullptr)
{
	//ZeroMemory(&m_tDesc, sizeof(m_tDesc));
	m_tDesc.fData = -1.f;
	strcpy_s(m_tDesc.szName, MAX_PATH, "");
	strcpy_s(m_tDesc.szData, MAX_PATH, "");
	wcscpy_s(m_tDesc.wstrData, MAX_PATH, L"");
	m_tDesc.eType = TYPE_END;
	m_tDesc.vPosition = { 0.f, 0.f, 0.f };
	m_tDesc.vScale = { 1.f, 1.f, 1.f };
}

CWorldTrigger_S2::CWorldTrigger_S2(const CWorldTrigger_S2& rhs)
	:CGameObject(rhs)
	, m_pRendererCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_bTriggerOn(true)
	, m_bActive(true)
	, m_pPlayer(nullptr)
{
	//ZeroMemory(&m_tDesc, sizeof(m_tDesc));
	m_tDesc.fData = -1.f;
	strcpy_s(m_tDesc.szName, MAX_PATH, "");
	strcpy_s(m_tDesc.szData, MAX_PATH, "");
	wcscpy_s(m_tDesc.wstrData, MAX_PATH, L"");
	m_tDesc.eType = TYPE_END;
	m_tDesc.vPosition = { 0.f, 0.f, 0.f };
	m_tDesc.vScale = { 1.f, 1.f, 1.f };

}

HRESULT CWorldTrigger_S2::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CWorldTrigger_S2::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (pArg != nullptr)
	{
		if (FAILED(Load_Data(pArg)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CWorldTrigger_S2::Late_Initialize(void* pArg)
{
	m_pPlayer = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena"));
	//if (m_pPlayer == nullptr)
	//	return E_FAIL;

	return S_OK;
}

void CWorldTrigger_S2::Tick(_float fTimeDelta)
{
	//m_bActive = true;
	if (!m_bActive)
		return;

	__super::Tick(fTimeDelta);

	if (CPostFX::GetInstance()->Get_Capture())
	{
		CGameInstance::GetInstance()->Set_SingleLayer(g_LEVEL, L"Layer_Canvas");
		m_pRendererCom->Set_CaptureMode(true);
	}

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CWorldTrigger_S2::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);

	if (Check_Collision())
	{
		_bool bStart = true;
		wstring wstr = L"";
		CUI_ClientManager::UI_PRESENT eQuest = CUI_ClientManager::QUEST_;

		// Delegator To Something
		switch (m_tDesc.eType)
		{
		case TYPE_QUEST: /* New Quest Occured */
			m_TriggerDelegatorB.broadcast(eQuest, bStart, m_tDesc.fData, wstr);
			m_bActive = false;
			break;
		case TYPE_ALARM:
			//CPostFX::GetInstance()->BlurCapture();
			//CUI_ClientManager::UI_PRESENT present = CUI_ClientManager::INFO_;
			//m_TriggerDelegator.broadcast(present, m_tDesc.fData);
			break;
		case TYPE_LINE:
		{
			CUI_ClientManager::UI_PRESENT eLine = CUI_ClientManager::BOT_LINE;
			_float fY = 345.f;
			wstring wstr = m_tDesc.wstrData;
			m_TriggerDelegatorB.broadcast(eLine, bStart, fY, wstr);
			m_bActive = false;
			break;
		}
		case TYPE_QUESTCLEAR:
		{
			CUI_ClientManager::UI_PRESENT eClear = CUI_ClientManager::QUEST_CLEAR;
			m_TriggerDelegatorB.broadcast(eClear, bStart, m_tDesc.fData, wstr);
			m_bActive = false;
		}
			break;

		}

	}

#ifdef _DEBUG
	if (m_pColliderCom != nullptr && m_pRendererCom != nullptr)
		m_pRendererCom->Add_DebugRenderGroup(m_pColliderCom);
#endif

}

void CWorldTrigger_S2::Imgui_RenderProperty()
{
	/* Name */
	static char szName[MAX_PATH] = "";
	ImGui::SetNextItemWidth(200);
	ImGui::InputTextWithHint("##stringName", "Name", szName, MAX_PATH);
	ImGui::SameLine();
	if (ImGui::Button("Set Name"))
	{
		ZeroMemory(m_tDesc.szName, sizeof(m_tDesc.szName));
		strcpy_s(m_tDesc.szName, MAX_PATH, szName);
	} ImGui::SameLine();
	if (ImGui::Button("Reset Name"))
	{
		ZeroMemory(szName, sizeof(szName));
		strcpy_s(szName, MAX_PATH, m_tDesc.szName);
	}

	/* Position */
	m_pTransformCom->Imgui_RenderProperty();

	/* Type */
	ImGui::DragInt("Type(Quest,Alarm,Line,QuestClear)", (_int*)&m_tDesc.eType, 1.f, 0, TYPE_END);

	/* Data */
	switch (m_tDesc.eType)
	{
	case TYPE_QUEST:
		ImGui::InputFloat("Float Data", &m_tDesc.fData, 0.f, 1.f);
		break;
	case TYPE_ALARM:
		ImGui::InputFloat("Float Data", &m_tDesc.fData, 0.f, 1.f);
		break;
	case TYPE_LINE:
	{
		static char szData[MAX_PATH] = "";
		ImGui::SetNextItemWidth(200);
		ImGui::InputTextWithHint("##stringData", "String Data", szData, MAX_PATH);
		ImGui::SameLine();
		if (ImGui::Button("Set Data"))
		{
			ZeroMemory(m_tDesc.szData, sizeof(m_tDesc.szData));
			strcpy_s(m_tDesc.szData, MAX_PATH, szData);
		} ImGui::SameLine();
		if (ImGui::Button("Reset Data"))
		{
			ZeroMemory(szData, sizeof(szData));
			strcpy_s(szData, MAX_PATH, m_tDesc.szData);
		}
	}
		break;
	case TYPE_QUESTCLEAR:
		ImGui::InputFloat("Float Data", &m_tDesc.fData, 0.f, 1.f);
		break;
	}

}

HRESULT CWorldTrigger_S2::Save_Data(Json* json, _uint iIndex)
{
	string tag = to_string(iIndex);

	(*json)[tag].push_back(m_tDesc.szName);
	(*json)[tag].push_back(m_tDesc.eType);
	(*json)[tag].push_back(m_tDesc.fData);
	(*json)[tag].push_back(m_tDesc.szData);

	{
		_float3 vPosition = m_pTransformCom->Get_Position();
		(*json)[tag].push_back(vPosition.x);
		(*json)[tag].push_back(vPosition.y);
		(*json)[tag].push_back(vPosition.z);
	}

	{
		_float3 vScale = m_pTransformCom->Get_Scaled();
		(*json)[tag].push_back(vScale.x);
		(*json)[tag].push_back(vScale.y);
		(*json)[tag].push_back(vScale.z);
	}

	return S_OK;
}

HRESULT CWorldTrigger_S2::Load_Data(void* pArg)
{
	DESC tDesc = *(DESC*)pArg;

	m_tDesc = tDesc;
	strcpy_s(m_tDesc.szName, MAX_PATH, tDesc.szName);
	strcpy_s(m_tDesc.szData, MAX_PATH, tDesc.szData);
	wcscpy_s(m_tDesc.wstrData, MAX_PATH, tDesc.wstrData);

	_float4 vPos = m_tDesc.vPosition;
	vPos.w = 1.f;
	m_pTransformCom->Set_Position(vPos);
	m_pTransformCom->Set_Scaled(m_tDesc.vScale);

	return S_OK;
}

HRESULT CWorldTrigger_S2::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Collider_AABB_ForTrigger"), TEXT("Com_AABB"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

_bool CWorldTrigger_S2::Check_Collision()
{
	if (m_bTriggerOn)
	{
		_float3 vPlayerPos	= m_pPlayer->Get_Position();
		_float3 vPos		= m_pTransformCom->Get_Position();
		_float3 vScale		= m_pTransformCom->Get_Scaled();
		
		if ((abs(vPlayerPos.x - vPos.x) < vScale.x * 0.5f) && (abs(vPlayerPos.z - vPos.z) < vScale.z * 0.5f))
		{
			return true;
		}
		//if (CGameInstance::GetInstance()->Key_Down(DIK_0))
		//	return true;
		//if (CGameInstance::GetInstance()->Key_Down(DIK_9) && m_tDesc.eType == TYPE_QUEST)
		//{
		//	_bool bStart = true;
		//	wstring wstr = L"";
		//	CUI_ClientManager::UI_PRESENT eQuest = CUI_ClientManager::QUEST_CLEAR;
		//	m_TriggerDelegatorB.broadcast(eQuest, bStart, m_tDesc.fData, wstr);
		//}
	}

	return false;
}

CWorldTrigger_S2* CWorldTrigger_S2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWorldTrigger_S2* pInstance = new CWorldTrigger_S2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWorldTrigger_S2");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWorldTrigger_S2::Clone(void* pArg)
{
	CWorldTrigger_S2* pInstance = new CWorldTrigger_S2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWorldTrigger_S2");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWorldTrigger_S2::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
}
