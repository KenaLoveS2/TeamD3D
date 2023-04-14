#include "stdafx.h"
#include "..\public\Fire_Brazier.h"
#include "GameInstance.h"
#include "E_FireBrazier.h"
#include "E_P_CommonBox.h"

CFire_Brazier::CFire_Brazier(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CFire_Brazier::CFire_Brazier(const CFire_Brazier& rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CFire_Brazier::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFire_Brazier::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	FAILED_CHECK_RETURN(Ready_Effect(), E_FAIL);

	m_bRenderActive = true;
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STATIC_SHADOW, this);
	return S_OK;
}

HRESULT CFire_Brazier::Late_Initialize(void* pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.isEnable = true;
	LightDesc.vPosition = m_pTransformCom->Get_Position();
	LightDesc.fRange = 10.0f; 
	LightDesc.vDiffuse = _float4(0.0f,0.2f,1.0f,1.0f);
	LightDesc.vAmbient = _float4(0.0f, 0.2f, 1.0f, 1.0f);
	LightDesc.vSpecular = _float4(0.0f, 0.2f, 1.0f, 1.0f);
	m_szLightName = new char[MAX_PATH];
	CUtile::WideCharToChar(m_szCloneObjectTag, m_szLightName);
	LightDesc.szLightName = m_szLightName;

	if (FAILED(CGameInstance::GetInstance()->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CFire_Brazier::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	if (m_bPulseTest)
		m_fEmissivePulse += CUtile::Get_RandomFloat(fTimeDelta * 0.5f, fTimeDelta);

	if (!m_bPulseTest)
	{
		m_fEmissivePulse -= CUtile::Get_RandomFloat(fTimeDelta * 0.5f, fTimeDelta);
		if (m_fEmissivePulse <= 0.f)
		{
			m_fEmissivePulse = 0.f;
			m_bPulseTest = !m_bPulseTest;
		}
	}

	if (m_fEmissivePulse >= 2.f)
	{
		m_bPulseTest = !m_bPulseTest;
	}

	if (m_bOncePosUpdate == false && m_bRenderActive)
	{
		m_pModelCom->InstanceModelPosInit(m_pTransformCom->Get_WorldMatrix());
		m_bOncePosUpdate = true;
	}

	if (m_pFireBrazierEffect)m_pFireBrazierEffect->Tick(fTimeDelta);
	if (m_pCommonBox)m_pCommonBox->Tick(fTimeDelta);
}

void CFire_Brazier::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	_matrix  WolrdMat = m_pTransformCom->Get_WorldMatrix();

	if (m_pRendererCom && m_bRenderActive && false == m_pModelCom->Culling_InstancingMeshs(200.f, WolrdMat))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_CINE, this);
#endif
	}

	if (m_pFireBrazierEffect)m_pFireBrazierEffect->Late_Tick(fTimeDelta);
	if (m_pCommonBox)m_pCommonBox->Late_Tick(fTimeDelta);
}

HRESULT CFire_Brazier::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
		
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 1, WJTextureType_COMP_E_R_AO, "g_MRAOTexture"), E_FAIL);
		if (i == 0)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 0, WJTextureType_EMISSIVE, "g_EmissiveTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fStonePulseIntensity", &m_fEmissivePulse, sizeof(float)), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 19), E_FAIL);
		}
		else
		{
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 16), E_FAIL);
		}
	}

	return S_OK;
}

HRESULT CFire_Brazier::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (m_pModelCom->Get_IStancingModel())
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 0), E_FAIL);
		}
	}
	else
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 21), E_FAIL);
		}
	}

	return S_OK;
}

HRESULT CFire_Brazier::RenderCine()
{
	if (FAILED(__super::RenderCine()))
		return E_FAIL;

	if (FAILED(__super::SetUp_CineShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (m_pModelCom->Get_IStancingModel())
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 12), E_FAIL);
		}
	}
	else
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 20), E_FAIL);
		}
	}

	return S_OK;
}

void CFire_Brazier::Imgui_RenderProperty()
{
	if (ImGui::Button("Emissive"))
		m_bPulseTest = true;
}

void CFire_Brazier::ImGui_ShaderValueProperty()
{
	__super::ImGui_ShaderValueProperty();
	ImGui::Text(CUtile::WstringToString(m_EnviromentDesc.szModelTag).c_str());
	m_pModelCom->Imgui_MaterialPath();
	m_pTransformCom->Imgui_RenderProperty();
}

void CFire_Brazier::ImGui_PhysXValueProperty()
{
	ImGui::Text("CFire_Brazier::ImGui_PhysXValueProperty");
}

_int CFire_Brazier::Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	if (iColliderIndex == (_int)COL_PULSE)
		_bool b = false;

	return 0;
}

_int CFire_Brazier::Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (iColliderIndex == (_int)COL_PULSE)
		_bool b = false;
	return 0;
}

_int CFire_Brazier::Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (iColliderIndex == (_int)COL_PULSE)
		_bool b = false;
	return 0;
}

HRESULT CFire_Brazier::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (m_EnviromentDesc.iCurLevel == 0)
		m_EnviromentDesc.iCurLevel = LEVEL_MAPTOOL;

	/* For.Com_Model */ 
	if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT,L"Prototype_Component_Model_Fire_Brazier", TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModelInstance"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFire_Brazier::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(float)), E_FAIL);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CFire_Brazier::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(float)), E_FAIL);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CFire_Brazier::Ready_Effect()
{

#ifdef FOR_MAP_GIMMICK
	m_pFireBrazierEffect = (CE_FireBrazier*)(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_FireBrazier", CUtile::Create_DummyString()));
	NULL_CHECK_RETURN(m_pFireBrazierEffect, E_FAIL);
	m_pFireBrazierEffect->Set_Parent(this);
	m_pFireBrazierEffect->Late_Initialize(nullptr);

	m_pCommonBox = (CE_P_CommonBox*)(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_P_CommonBox", CUtile::Create_DummyString()));
	NULL_CHECK_RETURN(m_pCommonBox, E_FAIL);
	m_pCommonBox->Set_Parent(this);
	m_pCommonBox->Late_Initialize(nullptr);
#endif
	return S_OK;
}

CFire_Brazier* CFire_Brazier::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFire_Brazier* pInstance = new CFire_Brazier(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFire_Brazier");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CFire_Brazier::Clone(void* pArg)
{
	CFire_Brazier* pInstance = new CFire_Brazier(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFire_Brazier");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFire_Brazier::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pFireBrazierEffect);
	Safe_Release(m_pCommonBox);
	Safe_Delete_Array(m_szLightName);
}
