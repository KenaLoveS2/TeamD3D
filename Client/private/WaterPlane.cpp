#include "stdafx.h"
#include "..\public\WaterPlane.h"
#include "GameInstance.h"

CWaterPlane::CWaterPlane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CWaterPlane::CWaterPlane(const CWaterPlane& rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CWaterPlane::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CWaterPlane::Initialize(void* pArg)
{

	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);

	m_EnviromentDesc.ObjectDesc.TransformDesc.fSpeedPerSec = 4.f;
	m_EnviromentDesc.ObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	m_pTransformCom->Set_Position(_float4(15.f, 0.f, 15.f, 1.f));
	return S_OK;
}

HRESULT CWaterPlane::Late_Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Late_Initialize(pArg), E_FAIL);
	return S_OK;
}

void CWaterPlane::Tick(_float fTimeDelta)
{
	CGameObject::Tick(fTimeDelta);
	m_fTimeDelta += fTimeDelta;
}

void CWaterPlane::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	if (m_pRendererCom && m_bRenderActive && CGameInstance::GetInstance()->isInFrustum_WorldSpace(vPos, 20.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EFFECT, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}
}

HRESULT CWaterPlane::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		// diffuse ¾øÀÝ¾î~
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_ReflectTexture", pGameInstance->Get_ReflectSRV())))
			return E_FAIL;
		RELEASE_INSTANCE(CGameInstance);
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Render(m_pShaderCom, i, nullptr);
	}
	return S_OK;
}

void CWaterPlane::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();
}

void CWaterPlane::ImGui_ShaderValueProperty()
{
	CGameObject::ImGui_ShaderValueProperty();
}

void CWaterPlane::ImGui_PhysXValueProperty()
{
	CGameObject::ImGui_PhysXValueProperty();
}

HRESULT CWaterPlane::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_WaterPlane", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_Water", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);
	return S_OK;
}

HRESULT CWaterPlane::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ReflectedViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_REFLECTVIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fTime", &m_fTimeDelta, sizeof(_float)), E_FAIL);
	return S_OK;
}

CWaterPlane* CWaterPlane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWaterPlane*	pInstance = new CWaterPlane(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CWaterPlane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWaterPlane::Clone(void* pArg)
{
	CWaterPlane*	pInstance = new CWaterPlane(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CWaterPlane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWaterPlane::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
