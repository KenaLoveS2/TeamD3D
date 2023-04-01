#include "stdafx.h"
#include "RotEater_Small.h"
#include "GameInstance.h"
#include "ControlMove.h"
#include "Interaction_Com.h"
#include "RotEater.h"

CRotEater_Small::CRotEater_Small(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CRotEater_Small::CRotEater_Small(const CRotEater_Small& rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CRotEater_Small::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRotEater_Small::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CGameInstance::GetInstance()->Add_AnimObject(g_LEVEL, this);

	m_bRenderActive = true;
	
	m_pModelCom->Set_AnimIndex(CRotEater::ANIMATION::IDLE);

	
	return S_OK;
}

HRESULT CRotEater_Small::Late_Initialize(void* pArg)
{
	m_pTransformCom->Set_Scaled(_float3(0.5f, 0.5f, 0.5f));
	

	return S_OK;
}

void CRotEater_Small::Tick(_float fTimeDelta)
{
	if( false==m_bOnceTest)
	{
		Late_Initialize();
		m_bOnceTest = true;
	}

	__super::Tick(fTimeDelta);

	State_RotEaterSmall(fTimeDelta);


	m_pModelCom->Play_Animation(fTimeDelta);
}

void CRotEater_Small::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CRotEater_Small::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		if (i == 0)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", AO_R_M_E);
		}

		if (i == 1)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", AO_R_M);
		}
	}
	return S_OK;
}

void CRotEater_Small::State_RotEaterSmall(_float fTimedelta)
{
	if(ImGui::Button("Water_Jump"))
	{
		m_pModelCom->Set_AnimIndex(CRotEater::JUMPBACK);
	}

	if (ImGui::Button("Run"))
	{
		m_pModelCom->Set_AnimIndex(CRotEater::RUN);
	}

	if (ImGui::Button("Turn"))
	{
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f)); 
		m_pModelCom->Set_AnimIndex(CRotEater::TURN180);

	}

	if (ImGui::Button("Idle"))
	{
		m_pModelCom->Set_AnimIndex(CRotEater::IDLE);


	}

}

void CRotEater_Small::ImGui_AnimationProperty()
{
	m_pTransformCom->Imgui_RenderProperty_ForJH();
	m_pModelCom->Imgui_RenderProperty();
}


HRESULT CRotEater_Small::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	/*나중에  레벨 인덱스 수정해야됌*/
	if (m_EnviromentDesc.iCurLevel == 0)
		m_EnviromentDesc.iCurLevel = LEVEL_MAPTOOL;

	/* For.Com_Model */ 	/*나중에  레벨 인덱스 수정해야됌*/
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_RotEater", L"Com_Model",
		(CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	/* For.Com_Shader */
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(),
		L"Prototype_Component_Shader_VtxAnimMonsterModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	return S_OK;
}

HRESULT CRotEater_Small::SetUp_ShaderResources()
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

CRotEater_Small* CRotEater_Small::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRotEater_Small* pInstance = new CRotEater_Small(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRotEater_Small");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CRotEater_Small::Clone(void* pArg)
{
	CRotEater_Small* pInstance = new CRotEater_Small(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRotEater_Small");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRotEater_Small::Free()
{
	Safe_Release(m_pModelCom);
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);
}
