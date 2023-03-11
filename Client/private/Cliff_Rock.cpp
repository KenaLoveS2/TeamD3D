#include "stdafx.h"
#include "..\public\Cliff_Rock.h"
#include "GameInstance.h"
#include "ControlMove.h"
#include "Interaction_Com.h"

CCliff_Rock::CCliff_Rock(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnviromentObj(pDevice,pContext)
{
}

CCliff_Rock::CCliff_Rock(const CCliff_Rock & rhs)
	:CEnviromentObj(rhs)
{
}

HRESULT CCliff_Rock::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCliff_Rock::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_bRenderActive = true;

	return S_OK;
}

void CCliff_Rock::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CCliff_Rock::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if(  m_pRendererCom )
		 m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CCliff_Rock::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if(m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Cliff_Ledge_04" 
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Cliff_Ledge_03"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Cliff_Ledge_01")
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_HRAOTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_MASK, "g_DetailNormalTexture");
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 9);
		}
	}
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Cliff_Wall_Short02"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Cliff_Wall_Large02"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Cliff_Wall_Large01")
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_MASK, "g_DetailNormalTexture");
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 10);
		}
	}
	else if(m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Cliff_Wall_Med03")
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_BlendDiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_SSS_MASK, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_HAIR_DEPTH, "g_DetailNormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_MASK, "g_MaskTexture");
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 11);
		}
	}
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Cliff_W2_02"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Cliff_W2_03"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Cliff_W2_05")
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_H_R_AO, "g_HRAOTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_MASK, "g_DetailNormalTexture");
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 9);
		}
	}
	else
	{
		// 쉐이더 안한건 안그릴꺼야
		//for (_uint i = 0; i < iNumMeshes; ++i)
		//{
		//	/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
		//	//m_pMasterDiffuseBlendTexCom->Bind_ShaderResource(m_pShaderCom, "g_MasterBlendDiffuseTexture");
		//	m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		//	m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		//	//m_pE_R_AoTexCom->Bind_ShaderResource(m_pShaderCom, "g_ERAOTexture");
		//	m_pModelCom->Render(m_pShaderCom, i,nullptr , m_iShaderOption);
		//}
	}

	return S_OK;
}

void CCliff_Rock::ImGui_ShaderValueProperty()
{
	__super::ImGui_ShaderValueProperty();
	ImGui::Text(CUtile::WstringToString(m_EnviromentDesc.szModelTag).c_str());
	m_pModelCom->Imgui_MaterialPath();
	m_pTransformCom->Imgui_RenderProperty();
}

HRESULT CCliff_Rock::Add_AdditionalComponent(_uint iLevelIndex,const _tchar * pComTag, COMPONENTS_OPTION eComponentOption)
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

HRESULT CCliff_Rock::SetUp_Components()
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

HRESULT CCliff_Rock::SetUp_ShaderResources()
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

CCliff_Rock * CCliff_Rock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCliff_Rock*		pInstance = new CCliff_Rock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCliff_Rock");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCliff_Rock::Clone(void * pArg)
{
	CCliff_Rock*		pInstance = new CCliff_Rock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CForkLift");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCliff_Rock::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);
	//Safe_Release(m_pMasterDiffuseBlendTexCom);
}
