#include "stdafx.h"
#include "..\public\Tree.h"
#include "GameInstance.h"
#include "ControlMove.h"
#include "Interaction_Com.h"

CTree::CTree(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CTree::CTree(const CTree & rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CTree::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTree::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_bRenderActive = true;
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STATIC_SHADOW, this);
	return S_OK;
}

void CTree::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CTree::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CTree::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if(m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_CadarTree_02"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_CadarTree_00"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_CadarTree_03")
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if(i ==0)
			{
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_E_R_AO, "g_ERAOTexture");
				m_pModelCom->Render(m_pShaderCom, i, nullptr, 5);
			}
			else	if(i == 1)
			{
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
				m_pModelCom->Render(m_pShaderCom, i, nullptr, 1);
			}
		}
	}
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_RuinsKit_Brick04"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_ForestTree_Canopy_02"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Giant_GodTreeSmall_01"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Giant_GodTreeSmall_02"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Giant_GodTreeSmall_03"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Giant_GodTreeSmall_04"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_RuinsKit_Rubble03")
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_H_R_AO, "g_HRAOTexture");
				m_pModelCom->Render(m_pShaderCom, i, nullptr, 2);
		}
	}
	else if(m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_ForestTree_Canopy_03")
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if(i == 0)
			{
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture");
				m_pModelCom->Render(m_pShaderCom, i, nullptr, 6);
			}
			else if (i == 1)
			{
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_H_R_AO, "g_HRAOTexture");
				m_pModelCom->Render(m_pShaderCom, i, nullptr, 2);
			}
		}
	}
	else	if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Giant_GodTreeStump_02")
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (i == 0)
			{
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_E_R_AO, "g_ERAOTexture");
				m_pModelCom->Render(m_pShaderCom, i, nullptr, 5);
			}
			else	if (i == 1)
			{
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_H_R_AO, "g_HRAOTexture");
				m_pModelCom->Render(m_pShaderCom, i, nullptr, 2);
			}
		}
	}
	else	if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_CedarTree_Bare01"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_BigTreeLog")
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_H_R_AO, "g_HRAOTexture");
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 2);
		}
	}
	else	if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_TreeLog")
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_E_R_AO, "g_ERAOTexture");
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 5);
		}
	}
	else	if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Giant_GodTree"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Giant_GodTree02")
	{
		// 모르겠음
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Render(m_pShaderCom, i, nullptr, m_iShaderOption);
		}
	}
	else	if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_FirstTear_fallenTree")
	{
		// 모르겠음
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Render(m_pShaderCom, i, nullptr, m_iShaderOption);
		}
	}
	else	if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_RockRubble_02")
	{
		// 모르겠음
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture");
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 6);
		}
	}
	else if(m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_ForestTree_Canopy_05"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_LeafPile")
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture");
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 6);
		}
	}
	else
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Render(m_pShaderCom, i, nullptr, m_iShaderOption);
		}
	}

	return S_OK;
}

HRESULT CTree::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_FirstTear_fallenTree")
		return S_OK;

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, i, nullptr, 0);
	}

	return S_OK;
}

void CTree::ImGui_ShaderValueProperty()
{
	__super::ImGui_ShaderValueProperty();
	ImGui::Text(CUtile::WstringToString(m_EnviromentDesc.szModelTag).c_str());
	m_pModelCom->Imgui_MaterialPath();
	m_pTransformCom->Imgui_RenderProperty();
}

HRESULT CTree::Add_AdditionalComponent(_uint iLevelIndex, const _tchar * pComTag, COMPONENTS_OPTION eComponentOption)
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

HRESULT CTree::SetUp_Components()
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

HRESULT CTree::SetUp_ShaderResources()
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

HRESULT CTree::SetUp_ShadowShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CTree * CTree::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTree*		pInstance = new CTree(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTree");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTree::Clone(void * pArg)
{
	CTree*		pInstance = new CTree(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTree");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTree::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);
}
