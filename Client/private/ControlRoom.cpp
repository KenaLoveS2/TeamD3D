#include "stdafx.h"
#include "..\public\ControlRoom.h"
#include "GameInstance.h"

CControlRoom::CControlRoom(ID3D11Device* pDevice, ID3D11DeviceContext* p_context)
	:CGameObject(pDevice, p_context)
{
}

CControlRoom::CControlRoom(const CControlRoom& rhs)
	:CGameObject(rhs)
{
}

HRESULT CControlRoom::Initialize_Prototype()
{
	return CGameObject::Initialize_Prototype();
}

HRESULT CControlRoom::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	return S_OK;
}

HRESULT CControlRoom::Late_Initialize(void* pArg)
{
	// 피직스 박스 생성
	__super::Late_Initialize();
	return S_OK;
}

void CControlRoom::Tick(_float fTimeDelta)
{
	CGameObject::Tick(fTimeDelta);
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

HRESULT CControlRoom::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);
	return S_OK;
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
