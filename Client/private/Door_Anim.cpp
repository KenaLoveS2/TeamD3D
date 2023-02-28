#include "stdafx.h"
#include "Door_Anim.h"
#include "GameInstance.h"
#include "ControlMove.h"
#include "Interaction_Com.h"

CDoor_Anim::CDoor_Anim(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CDoor_Anim::CDoor_Anim(const CDoor_Anim & rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CDoor_Anim::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDoor_Anim::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_bRenderActive = true;

	m_pModelCom->Set_AnimIndex(0);
	return S_OK;
}

void CDoor_Anim::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CDoor_Anim::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CDoor_Anim::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}
	
	return S_OK;
}

HRESULT CDoor_Anim::Add_AdditionalComponent(_uint iLevelIndex, const _tchar * pComTag, COMPONENTS_OPTION eComponentOption)
{
	__super::Add_AdditionalComponent(iLevelIndex, pComTag, eComponentOption);

	/* For.Com_CtrlMove */
	if (eComponentOption == COMPONENTS_CONTROL_MOVE)
	{
		if (FAILED(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_ControlMove"), pComTag,
			(CComponent**)&m_pControlMoveCom)))
			return E_FAIL;
	}
	/* For.Com_Interaction */
	else if (eComponentOption == COMPONENTS_INTERACTION)
	{
		if (FAILED(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_Interaction_Com"), pComTag,
			(CComponent**)&m_pInteractionCom)))
			return E_FAIL;
	}
	else
		return S_OK;

	return S_OK;
}

HRESULT CDoor_Anim::SetUp_Components()
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
	if (FAILED(__super::Add_Component(m_EnviromentDesc.iCurLevel, m_EnviromentDesc.szModelTag.c_str(), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;
	/* For.Com_Shader */
	FAILED_CHECK_RETURN(__super::Add_Component(m_EnviromentDesc.iCurLevel,
		L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	return S_OK;
}

HRESULT CDoor_Anim::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;



	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CDoor_Anim * CDoor_Anim::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDoor_Anim*		pInstance = new CDoor_Anim(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDoor_Anim");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDoor_Anim::Clone(void * pArg)
{
	CDoor_Anim*		pInstance = new CDoor_Anim(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDoor_Anim");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDoor_Anim::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);
}
