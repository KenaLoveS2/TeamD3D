#include "stdafx.h"
#include "..\public\Stone.h"
#include "GameInstance.h"
#include "ControlMove.h"
#include "Interaction_Com.h"

CStone::CStone(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CStone::CStone(const CStone & rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CStone::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStone::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_bRenderActive = true;
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STATIC_SHADOW, this);
	return S_OK;
}

HRESULT CStone::Late_Initialize(void * pArg)
{
	if (m_pModelCom->Get_IStancingModel() == false)
		m_pModelCom->Create_PxBox(m_szCloneObjectTag, m_pTransformCom, COL_ENVIROMENT);
	else
		m_pModelCom->Create_InstModelPxBox(m_szCloneObjectTag, m_pTransformCom, COL_ENVIROMENT
		, _float3(0.5f, 0.5f, 0.5f)); //(0~1)

	m_pRendererCom->Set_PhysXRender(true);
	return S_OK;
}

void CStone::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CStone::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CStone::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if(m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_ShrineOfLife_Rock_03"	)
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_H_R_AO, "g_HRAOTexture");
			m_pShaderCom->Set_RawValue("g_fStonePulseIntensity", &m_fEmissivePulse, sizeof(float));
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 4);
		}
	}
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_ShrineOfDeath_MainRock")
	{
		// 매터리얼 잘못들어가서 수정되야하는 친구임 일단 이렇게 수정
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, 0, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, 0, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, 0, WJTextureType_EMISSIVE, "g_EmissiveTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, 0, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture");
			m_pShaderCom->Set_RawValue("g_fStonePulseIntensity", &m_fEmissivePulse, sizeof(float));
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 3);
		}
	}
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_ShrineOfDeath_Rock_02"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_ShrineOfDeath_Rock_03")
	{
		// 매터리얼 잘못들어가서 수정되야하는 친구임 일단 이렇게 수정
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, 1, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, 1, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, 1, WJTextureType_EMISSIVE, "g_EmissiveTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, 1, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture");
			m_pShaderCom->Set_RawValue("g_fStonePulseIntensity", &m_fEmissivePulse, sizeof(float));
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 3);
		}
	}
	else if(m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_ShrineOfLife_Rock_01"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_ShrineOfLife_Rock_02")
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_H_R_AO, "g_HRAOTexture");
			m_pShaderCom->Set_RawValue("g_fStonePulseIntensity", &m_fEmissivePulse, sizeof(float));
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 4);
		}
	}
	else if(m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Rock_Small_03"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Rock_Small_04"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Rock_Small_05"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Rock_Small_06")
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_E_R_AO, "g_ERAOTexture");
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 5);
		}
	}
	else if(m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_ShrineOfLife")
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if(i == 0)
			{
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVEMASK, "g_EmissiveTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture");
				m_pShaderCom->Set_RawValue("g_fStonePulseIntensity", &m_fEmissivePulse, sizeof(float));
				m_pModelCom->Render(m_pShaderCom, i, nullptr, 3);
			}
			else if(i == 1)
			{
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture");
				m_pModelCom->Render(m_pShaderCom, i, nullptr, 6);
			}
			else if(i == 2)
			{
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture");
				m_pModelCom->Render(m_pShaderCom, i, nullptr, 7);
			}
		}
	}
	else if(m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_RockRubble_02")
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture");
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 6);
		}
	}
	else if(m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Rock_Medium_06")
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_E_R_AO, "g_ERAOTexture");
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 5);
		}
	}
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_GodRock_01" 
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_GodRock_02")
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_E_R_AO, "g_ERAOTexture");
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 5);
		}
	}
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_RockRubble_01"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_RockRubble_02"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_RockRubble_03")
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture");
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 6);
		}
	}
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_StoneTorch_01"
		|| m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_StoneTorch_02")
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture");
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 6);
		}
	}
	else
	{
		// 쉐이더 안힙인건 안그릴꺼임
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Render(m_pShaderCom, i, nullptr, m_iShaderOption);
		}
	}

	return S_OK;
}

HRESULT CStone::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, i, nullptr, 0);
	}
		
	return S_OK;
}

void CStone::ImGui_ShaderValueProperty()
{
	if(ImGui::Button("Emissive"))
		m_bPulseTest = true;

	if (m_bPulseTest)
		m_fEmissivePulse += 0.05f;
	else
	{
		m_fEmissivePulse -= 0.05f;
		if (m_fEmissivePulse <= 0.f)
			m_fEmissivePulse = 0.f;
	}
		
	if (m_fEmissivePulse >= 2.f)
		m_bPulseTest = false;
	__super::ImGui_ShaderValueProperty();
	//ImGui::Text(CUtile::WstringToString(m_EnviromentDesc.szModelTag).c_str());
	//m_pModelCom->Imgui_MaterialPath();
	//m_pTransformCom->Imgui_RenderProperty();
}

void CStone::ImGui_PhysXValueProperty()
{
	__super::ImGui_PhysXValueProperty();

	_float3 vPxPivotScale = m_pTransformCom->Get_vPxPivotScale();

	float fScale[3] = { vPxPivotScale.x, vPxPivotScale.y, vPxPivotScale.z };
	ImGui::DragFloat3("PxScale", fScale, 0.01f, 0.1f, 100.0f);
	vPxPivotScale.x = fScale[0]; vPxPivotScale.y = fScale[1]; vPxPivotScale.z = fScale[2];
	CPhysX_Manager::GetInstance()->Set_ActorScaling(m_szCloneObjectTag, vPxPivotScale);
	m_pTransformCom->Set_PxPivotScale(vPxPivotScale);

	_float3 vPxPivot = m_pTransformCom->Get_vPxPivot();

	float fPos[3] = { vPxPivot.x, vPxPivot.y, vPxPivot.z };
	ImGui::DragFloat3("PxPivotPos", fPos, 0.01f, -100.f, 100.0f);
	vPxPivot.x = fPos[0]; vPxPivot.y = fPos[1]; vPxPivot.z = fPos[2];
	m_pTransformCom->Set_PxPivot(vPxPivot);
}

HRESULT CStone::Add_AdditionalComponent(_uint iLevelIndex, const _tchar * pComTag, COMPONENTS_OPTION eComponentOption)
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

_int CStone::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	if (iColliderIndex == (_int)COL_PULSE)
		_bool b = false;

	return 0;
}

_int CStone::Execute_TriggerTouchFound(CGameObject * pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (iColliderIndex == (_int)COL_PULSE)
		_bool b = false;
	return 0;
}

_int CStone::Execute_TriggerTouchLost(CGameObject * pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (iColliderIndex == (_int)COL_PULSE)
		_bool b = false;
	return 0;
}

HRESULT CStone::SetUp_Components()
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

HRESULT CStone::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;

}

HRESULT CStone::SetUp_ShadowShaderResources()
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

CStone * CStone::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStone*		pInstance = new CStone(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CStone::Clone(void * pArg)
{
	CStone*		pInstance = new CStone(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStone::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);
}
