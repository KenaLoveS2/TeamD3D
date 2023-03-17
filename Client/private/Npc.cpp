#include "stdafx.h"
#include "..\public\Npc.h"
#include "GameInstance.h"
#include "Kena.h"
#include "CameraForNpc.h"

CNpc::CNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CNpc::CNpc(const CNpc& rhs)
	:CGameObject(rhs)
{
}

HRESULT CNpc::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CNpc::Initialize(void* pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)

		CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	if (pArg == nullptr)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 7.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	}
	else
		memcpy(&GameObjectDesc, pArg, sizeof(CGameObject::GAMEOBJECTDESC));

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State(), E_FAIL);
	m_pKena = (CKena*)pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Player"), TEXT("Kena"));

	RELEASE_INSTANCE(CGameInstance)
	return S_OK;
}

HRESULT CNpc::Late_Initialize(void* pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
	m_pMyCam = static_cast<CCameraForNpc*>(pGameInstance->Find_Camera(L"NPC_CAM"));
	RELEASE_INSTANCE(CGameInstance)
	return S_OK;
}

void CNpc::Tick(_float fTimeDelta)
{
	CGameObject::Tick(fTimeDelta);
}

void CNpc::Late_Tick(_float fTimeDelta)
{
	CGameObject::Late_Tick(fTimeDelta);
}

HRESULT CNpc::Render()
{
	return S_OK;
}

HRESULT CNpc::RenderShadow()
{
	return S_OK;
}

void CNpc::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();
	if(ImGui::Button("Add_ShaderValue"))
	{
		CGameInstance* p_game_instance = GET_INSTANCE(CGameInstance);
		p_game_instance->Add_ShaderValueObject(g_LEVEL, this);
		RELEASE_INSTANCE(CGameInstance)
	}
}

void CNpc::ImGui_AnimationProperty()
{
	CGameObject::ImGui_AnimationProperty();
}

void CNpc::ImGui_ShaderValueProperty()
{
	CGameObject::ImGui_ShaderValueProperty();
}

void CNpc::ImGui_PhysXValueProperty()
{
	CGameObject::ImGui_PhysXValueProperty();
}

void CNpc::Calc_RootBoneDisplacement(_fvector vDisplacement)
{
	_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vPos = vPos + vDisplacement;
	m_pTransformCom->Set_Translation(vPos, vDisplacement);
}

_bool CNpc::AnimFinishChecker(_uint eAnim, _double FinishRate)
{
	return m_pModelCom->Find_Animation(eAnim)->Get_PlayRate() >= FinishRate;
}

_bool CNpc::AnimIntervalChecker(_uint eAnim, _double StartRate, _double FinishRate)
{
	if (m_pModelCom->Find_Animation(eAnim)->Get_PlayRate() > StartRate &&
		m_pModelCom->Find_Animation(eAnim)->Get_PlayRate() <= FinishRate)
	{
		return true;
	}

	return false;
}

_bool CNpc::DistanceTrigger(_float distance)
{
	_float3 vPlayerPos = m_pKena->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_float fDistance = _float3::Distance(vPos, vPlayerPos);

	if (distance >= fDistance)
		return true;
	else
		return false;
}

void CNpc::AdditiveAnim(_float fTimeDelta)
{
}

HRESULT CNpc::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxAnimNPCModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);
	return S_OK;
}

HRESULT CNpc::SetUp_UI()
{
	return S_OK;
}

void CNpc::Free()
{
	CGameObject::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pFSM);
}
