#include "stdafx.h"
#include "..\public\Effect_Mesh_Base.h"
#include "GameInstance.h"

CEffect_Mesh_Base::CEffect_Mesh_Base(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Base_S2(pDevice, pContext)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pModelCom(nullptr)
	, m_pVIBufferCom(nullptr)
{
	for (_uint i = 0; i < TEXTURE_END; ++i)
	{
		m_pTextureCom[i] = nullptr;
		m_iTextureIndices[i] = -1;
		m_ShaderVarName[i] = nullptr;
		m_TextureComName[i] = nullptr;
		m_vTextureColors[i] = { 1.f, 1.f, 1.f, 1.f };
	}
}

CEffect_Mesh_Base::CEffect_Mesh_Base(const CEffect_Mesh_Base & rhs)
	: CEffect_Base_S2(rhs)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pModelCom(nullptr)
	, m_pVIBufferCom(nullptr)
{
	for (_uint i = 0; i < TEXTURE_END; ++i)
	{
		m_pTextureCom[i] = nullptr;
		m_iTextureIndices[i] = -1;
		m_ShaderVarName[i] = nullptr;
		m_TextureComName[i] = nullptr;
		m_vTextureColors[i] = { 1.f, 1.f, 1.f, 1.f };
	}
}

HRESULT CEffect_Mesh_Base::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Mesh_Base::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_TextureInfo()))
		return E_FAIL;

	if (pArg != nullptr)
	{
		m_pfileName = (_tchar*)pArg;
		if (FAILED(Load_Data(m_pfileName)))
		{
			m_pTransformCom->Set_Scaled({ 5.f, 5.f, 5.f });
		}
	}
	else
	{
		m_pTransformCom->Set_Scaled({ 5.f, 5.f, 5.f });
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetupComponents : CEffect_Particle_Base");
		return E_FAIL;
	}

	/* Temp */
	m_pTransformCom->Set_Scaled({ 5.f, 5.f ,5.f });
	m_fTest = 0.f;
	return S_OK;
}

HRESULT CEffect_Mesh_Base::Late_Initialize(void * pArg)
{
	return S_OK;
}

void CEffect_Mesh_Base::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Tick(fTimeDelta);
}

void CEffect_Mesh_Base::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CEffect_Mesh_Base::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_pModelCom != nullptr && m_pShaderCom != nullptr)
	{
	//	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

		//m_pModelCom->Bind_Material(m_pShaderCom, 0, m_eTextureType, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 0, nullptr, m_iRenderPass);
	}

	return S_OK;
}

void CEffect_Mesh_Base::Imgui_RenderProperty()
{
	ImGui::Separator();

	ImGui::Text("<Set Model>");
	Set_ModelCom();

	m_pTransformCom->Imgui_RenderProperty();



	/* Render Info */
	for (_uint texID = 0; texID < TEXTURE_END; ++texID)
	{
		if (ImGui::CollapsingHeader(m_ShaderVarName[texID]))
		{
			if (m_pTextureCom[texID] == nullptr)
				return;

			/* Texture Tiles */
			for (_uint i = 0; i < m_pTextureCom[texID]->Get_TextureIdx(); ++i)
			{
				if (i % 6)
					ImGui::SameLine();

				if (ImGui::ImageButton(m_pTextureCom[texID]->Get_Texture(i), ImVec2(50.f, 50.f)))
				{
					m_iTextureIndices[texID] = i;
				}
			}

			/* RenderPass */
			static _int iRenderPass;
			iRenderPass = m_iRenderPass;
			const char* renderPass[2] = { "Default", "OnlyColor" };
			if (ImGui::ListBox("RenderPass", &iRenderPass, renderPass, 2, 5))
				m_iRenderPass = iRenderPass;

			/* Color */
			Color(texID);

			static _float fTest = 0.f;
			if (ImGui::DragFloat("test", &fTest, 0.01f, -10.f, 10.f))
				m_fTest = fTest;
		}
	}

}

HRESULT CEffect_Mesh_Base::Save_Data()
{
	/* Model Index */
	/* Texture Index */
	/* UV Animation */
	/* m_vColor */
	/* m_fHDRIntensity*/
	return S_OK;
}

HRESULT CEffect_Mesh_Base::Load_Data(_tchar * fileName)
{
	return S_OK;
}

HRESULT CEffect_Mesh_Base::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */ 
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_Effect_Mesh_S2"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For Com_Texture */
	for (_uint i = 0; i < TEXTURE_END; ++i)
	{
		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_Effect"), m_TextureComName[i],
			(CComponent**)&m_pTextureCom[i])))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Mesh_Base::SetUp_ShaderResources()
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
	//if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DepthTexture", pGameInstance->Get_DepthTargetSRV())))
	//	return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DepthTexture", pGameInstance->Get_DepthTargetSRV())))
	//	return E_FAIL;

	for (_uint i = 0; i < TEXTURE_END; ++i)
	{
		if (m_iTextureIndices[i] != -1)
		{
			if (FAILED(m_pTextureCom[i]->Bind_ShaderResource(m_pShaderCom,
				m_ShaderVarName[i], m_iTextureIndices[i])))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Set_RawValue(m_ShaderColorName[i],
				&m_vTextureColors[i], sizeof(_float4))))
				return E_FAIL;
		}

	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_fHDRItensity", &m_fHDRIntensity, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fTest", &m_fTest, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Mesh_Base::SetUp_TextureInfo()
{
	return E_NOTIMPL;
}

HRESULT CEffect_Mesh_Base::Set_ModelCom()
{
	static _int iSelected;
	ImGui::RadioButton("Plane", &iSelected, 0); ImGui::SameLine();
	ImGui::RadioButton("Cube", &iSelected, 1); ImGui::SameLine();
	ImGui::RadioButton("Cone", &iSelected, 2); ImGui::SameLine();
	ImGui::RadioButton("Sphere", &iSelected, 3); ImGui::SameLine();
	ImGui::RadioButton("ShockBall", &iSelected, 4); ImGui::SameLine();
	ImGui::RadioButton("Cylinder", &iSelected, 5); ImGui::SameLine();
	ImGui::RadioButton("HunterArrow", &iSelected, 6); ImGui::SameLine();

	if (ImGui::Button("Model Confirm"))
	{
		if (m_pModelCom != nullptr)
		{
			Delete_Component(L"Com_Model");
		}

		/* For.Com_Model */
		switch (iSelected)
		{
		case 0:
			if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Plane"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
			break;
		case 1:
			if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Cube"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
			break;
		case 2:
			if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Cone"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
			break;
		case 3:
			if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Sphere"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
			break;
		case 4:
			if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_shockball"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
			break;
		case 5:
			if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Cylinder"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
			break;
		case 6:
			if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Boss_Hunter_Arrow"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
			break;
		}
	}

	return S_OK;
}

CEffect_Mesh_Base * CEffect_Mesh_Base::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffect_Mesh_Base * pInstance = new CEffect_Mesh_Base(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create: CEffect_Mesh_Base");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Mesh_Base::Clone(void * pArg)
{
	CEffect_Mesh_Base * pInstance = new CEffect_Mesh_Base(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CEffect_Mesh_Base");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Mesh_Base::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pVIBufferCom);

	for (_uint i = 0; i < TEXTURE_END; ++i)
		Safe_Release(m_pTextureCom[i]);
}
