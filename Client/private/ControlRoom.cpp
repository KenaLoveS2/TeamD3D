#include "stdafx.h"
#include "..\public\ControlRoom.h"
#include "GameInstance.h"
#include "Gimmick_EnviObj.h"
#include "Crystal.h"
#include "DZ_FallenTree_Anim.h"

CControlRoom::CControlRoom(ID3D11Device* pDevice, ID3D11DeviceContext* p_context)
	:CGameObject(pDevice, p_context)
{
}

CControlRoom::CControlRoom(const CControlRoom& rhs)
	:CGameObject(rhs)
{
}

void CControlRoom::Add_Gimmick_TrggerObj(const _tchar * pCloneTag, CGameObject * pTriggerObj)
{
	CGameObject* pObj = Get_Find_TriggerObj(pCloneTag);
	if (pObj != nullptr)
		return;

	m_Gimmcik_Trigger_map.emplace(pCloneTag, pTriggerObj);
}

void CControlRoom::Add_GimmickObj(_int iRoomNumber, CGameObject* pGimmickObj, CEnviromentObj::CHAPTER GimmickType)
{
	CGameObject* pGimmick = Get_GimmickObj(iRoomNumber,pGimmickObj);
	if (pGimmick != nullptr)
		return;

	m_GimmcikObj_List[iRoomNumber].push_back({ GimmickType, pGimmickObj });

}

void CControlRoom::Clear_Static_ShadowList()
{
	if (CGameInstance::GetInstance()->Get_CurLevelIndex() == LEVEL_TESTPLAY
		|| CGameInstance::GetInstance()->Get_CurLevelIndex() == LEVEL_GIMMICK)
		m_pRendererCom->EraseAllStaticShadowObject();
}

HRESULT CControlRoom::Initialize_Prototype()
{
	return CGameObject::Initialize_Prototype();
}

HRESULT CControlRoom::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	m_MapShadowArray.fill(false);

	CGameInstance::GetInstance()->Work_LightCamera(TEXT("LIGHT_CAM_0"));

	return S_OK;
}

HRESULT CControlRoom::Late_Initialize(void* pArg)
{
	CGameObject* pPlayerPtr = CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena");
	if (pPlayerPtr == nullptr)
		return S_OK;

	m_pPlayerTransformPtr = pPlayerPtr->Get_TransformCom();


	return S_OK;
}

void CControlRoom::Tick(_float fTimeDelta)
{
	CGameObject::Tick(fTimeDelta);

	if (m_pPlayerTransformPtr ==nullptr)
		return;

	_float4 vPlayerPos;
	XMStoreFloat4(&vPlayerPos, m_pPlayerTransformPtr->Get_State(CTransform::STATE_TRANSLATION));

	if (false == m_MapShadowArray[0] && vPlayerPos.z > 240.f)
	{
		//To.do 
		// 2번째 터레인 쉐도우 온
		m_MapShadowArray[0] = true;
	}
	else if (false == m_MapShadowArray[1] && vPlayerPos.z > 480.f)
	{
		//To.do 
		// 3번째 터레인 쉐도우 온
		m_MapShadowArray[1] = true;
		CGameInstance::GetInstance()->Work_LightCamera(TEXT("LIGHT_CAM_2"));
		m_pRendererCom->ShootStaticShadow();
	}
	else if (false == m_MapShadowArray[2] && vPlayerPos.z > 710.f)
	{
		//To.do 
		// 4번째 터레인 쉐도우 온
		m_MapShadowArray[2] = true;
		CGameInstance::GetInstance()->Work_LightCamera(TEXT("LIGHT_CAM_3"));
		m_pRendererCom->ShootStaticShadow();
	}
	else if (false == m_MapShadowArray[3] && vPlayerPos.z > 917.f)
	{
		//To.do 
		// 5번째 터레인 쉐도우 온
		m_MapShadowArray[3] = true;
		CGameInstance::GetInstance()->Work_LightCamera(TEXT("LIGHT_CAM_4"));
		m_pRendererCom->ShootStaticShadow();
	}
	else
		return;
}

void CControlRoom::Late_Tick(_float fTimeDelta)
{
	CGameObject::Late_Tick(fTimeDelta);

	if(!m_bInitRender)
	{
		m_pRendererCom->ShootStaticShadow();
		m_bInitRender = true;
	}
}

void CControlRoom::Imgui_RenderProperty()
{
	if(ImGui::Button("StaticShadow"))
		m_pRendererCom->ShootStaticShadow();
}

void CControlRoom::ImGui_PhysXValueProperty()
{

}

void CControlRoom::PulsePlate_Down_Active(_int iRoomIndex,_bool bTriggerActive)
{
	if (iRoomIndex == 1)
	{
		Trigger_Active( iRoomIndex, CEnviromentObj::Gimmick_TYPE_GO_UP, bTriggerActive);
	}
	else if (iRoomIndex == 2)
	{
		CCrystal* pLastryCstal = static_cast<CCrystal*>(Get_Find_TriggerObj(L"2_Water_GimmickCrystal01"));
		assert(nullptr != pLastryCstal && "CControlRoom::PulsePlate_Down_Active");
		pLastryCstal->Set_GimmickActive(bTriggerActive);
	}
}

void CControlRoom::Trigger_Active(_int iRoomIndex,CEnviromentObj::CHAPTER eChpater, _bool IsTrigger)
{
	for (auto& pGimmickObj : m_GimmcikObj_List[iRoomIndex])
	{
		static_cast<CGimmick_EnviObj*>(pGimmickObj.second)->Set_Gimmick_Active(iRoomIndex,IsTrigger);
	}
}

void CControlRoom::DeadZoneObject_Change(_bool bChnage)
{
	/*Test .*/

	if (bChnage != true)
		return;

	//_int iDissovleTimer = 0;
	//m_DeadZoneChangeDelegator.broadcast(iDissovleTimer);

	CGameObject* pObj = Get_Find_TriggerObj(L"3_DeadzoneAnim_Tree");
	assert(pObj != nullptr && "CControlRoom::DeadZoneObject_Change(_bool bChnage) ");
	
	static_cast<CDZ_FallenTree_Anim*>(pObj)->Set_BossClear(true);
}



HRESULT CControlRoom::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);
	return S_OK;
}

CGameObject * CControlRoom::Get_Find_TriggerObj(const _tchar * pCloneTag)
{
	auto iter = find_if(m_Gimmcik_Trigger_map.begin(), m_Gimmcik_Trigger_map.end(), CTag_Finder(pCloneTag));

	if (iter != m_Gimmcik_Trigger_map.end())
		return iter->second;

	return nullptr;
}

CGameObject * CControlRoom::Get_GimmickObj(_int iRoomIndex ,CGameObject * pGameObj)
{
	auto Pair = find_if(m_GimmcikObj_List[iRoomIndex].begin(), m_GimmcikObj_List[iRoomIndex].end(), [&](auto pMyList)->bool
	{
		if (!lstrcmp(pGameObj->Get_ObjectCloneName(), pMyList.second->Get_ObjectCloneName()))
			return true;
		return false;
	});

	if (Pair != m_GimmcikObj_List[iRoomIndex].end())
		return Pair->second;


	return nullptr;
}

CControlRoom* CControlRoom::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CControlRoom*	pInstance = new CControlRoom(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CControlRoom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CControlRoom::Clone(void* pArg)
{
	CControlRoom*	pInstance = new CControlRoom(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CControlRoom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CControlRoom::Free()
{


	CGameObject::Free();
	Safe_Release(m_pRendererCom);
}
