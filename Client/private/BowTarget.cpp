#include "stdafx.h"
#include "..\public\BowTarget.h"
#include "GameInstance.h"
#include "Utile.h"
#include "ControlMove.h"
#include "Interaction_Com.h"
#include "BowTarget.h"

CBowTarget::CBowTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnviromentObj(pDevice, pContext)
{
}

CBowTarget::CBowTarget(const CBowTarget& rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CBowTarget::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CBowTarget::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	m_bRenderActive = true;

	CTransform::TRANSFORMDESC		Desc = m_pTransformCom->Get_TransformDesc();
	Desc.fSpeedPerSec = 5.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.f);
	m_pTransformCom->Set_TransformDesc(Desc);

	return S_OK;
}

HRESULT CBowTarget::Late_Initialize(void* pArg)
{
	return S_OK;
}

void CBowTarget::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_eCurState = Check_State();
	Update_State(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CBowTarget::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_ePreState != m_eCurState)
		m_ePreState = m_eCurState;

	if (m_pRendererCom != nullptr && m_bRenderActive == true)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CBowTarget::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		//m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_E_R_AO, "g_ERAOTexture");
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}

	return S_OK;
}

void CBowTarget::Imgui_RenderProperty()
{
}

void CBowTarget::ImGui_AnimationProperty()
{
	m_pTransformCom->Imgui_RenderProperty_ForJH();
	m_pModelCom->Imgui_RenderProperty();
}

void CBowTarget::ImGui_PhysXValueProperty()
{
}

HRESULT CBowTarget::Add_AdditionalComponent(_uint iLevelIndex, const _tchar* pComTag, COMPONENTS_OPTION eComponentOption)
{
	return S_OK;
}

_int CBowTarget::Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	return 0;
}

_int CBowTarget::Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	return 0;
}

_int CBowTarget::Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	return 0;
}

CBowTarget::ANIMATION CBowTarget::Check_State()
{
	ANIMATION	eState = m_ePreState;

	return eState;
}

void CBowTarget::Update_State(_float fTimeDelta)
{
}

HRESULT CBowTarget::SetUp_Components()
{
	/* For.Com_Renderer */
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);

	/* For.Com_Shader */
	if (m_EnviromentDesc.iCurLevel == 0)
		m_EnviromentDesc.iCurLevel = g_LEVEL;

	/* For.Com_Model */
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_BowTarget", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	/* For.Com_Shader */
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	return S_OK;
}

HRESULT CBowTarget::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CBowTarget* CBowTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBowTarget* pInstance = new CBowTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBowTarget");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBowTarget::Clone(void* pArg)
{
	CBowTarget* pInstance = new CBowTarget(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBowTarget");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBowTarget::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);
}
