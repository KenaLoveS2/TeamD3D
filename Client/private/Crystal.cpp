#include "stdafx.h"
#include "..\public\Crystal.h"
#include "GameInstance.h"
#include "ControlMove.h"
#include "Interaction_Com.h"
#include "Effect_Base.h"

CCrystal::CCrystal(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CCrystal::CCrystal(const CCrystal & rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CCrystal::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCrystal::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_bRenderActive = true;

	return S_OK;
}

HRESULT CCrystal::Late_Initialize(void * pArg)
{
	
	CGameInstance*	pGameInstance = CGameInstance::GetInstance();
	/* Pulse */
	m_pPulseEffect = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_KenaPulse", L"KenaPulse"));
	NULL_CHECK_RETURN(m_pPulseEffect, E_FAIL);

	m_pPulseEffect->Set_Parent(this);


	return S_OK;
}

void CCrystal::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CCrystal::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CCrystal::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
		//m_pMasterDiffuseBlendTexCom->Bind_ShaderResource(m_pShaderCom, "g_MasterBlendDiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		//m_pE_R_AoTexCom->Bind_ShaderResource(m_pShaderCom, "g_ERAOTexture");
		m_pModelCom->Render(m_pShaderCom, i, nullptr, m_iShaderOption);
	}
	return S_OK;
}

void CCrystal::Create_Pulse()
{
	if (m_pPulseEffect != nullptr)
	{
		m_pPulseEffect->Set_Active(true);
	}
}

HRESULT CCrystal::Add_AdditionalComponent(_uint iLevelIndex, const _tchar * pComTag, COMPONENTS_OPTION eComponentOption)
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

HRESULT CCrystal::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (m_EnviromentDesc.iCurLevel == 0)
		m_EnviromentDesc.iCurLevel = LEVEL_MAPTOOL;
	/* For.Com_Shader */
	/*나중에  레벨 인덱스 수정해야됌*/
	/* For.Com_Model */ 	/*나중에  레벨 인덱스 수정해야됌*/
	if (FAILED(__super::Add_Component(g_LEVEL, m_EnviromentDesc.szModelTag.c_str(), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (m_pModelCom->Get_IStancingModel())
	{
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModelInstance"), TEXT("Com_Shader"),
			(CComponent**)&m_pShaderCom)))
			return E_FAIL;
		m_iShaderOption = 1;
	}
	else
	{
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModelTess"), TEXT("Com_Shader"),
			(CComponent**)&m_pShaderCom)))
			return E_FAIL;
		m_iShaderOption = 4;
	}


	return S_OK;
}

HRESULT CCrystal::SetUp_ShaderResources()
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

CCrystal * CCrystal::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCrystal*		pInstance = new CCrystal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCrystal");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCrystal::Clone(void * pArg)
{
		CCrystal*		pInstance = new CCrystal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCrystal");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCrystal::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);

}
