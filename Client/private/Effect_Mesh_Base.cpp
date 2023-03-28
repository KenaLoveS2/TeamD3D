#include "stdafx.h"
#include "..\public\Effect_Mesh_Base.h"
#include "GameInstance.h"

CEffect_Mesh_Base::CEffect_Mesh_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base_S2(pDevice, pContext)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pModelCom(nullptr)
	, m_pVIBufferCom(nullptr)
	, m_iModelIndex(-1)
	, m_iMeshIndex(0)
{
	for (_uint i = 0; i < TEXTURE_END; ++i)
	{
		m_pTextureCom[i] = nullptr;
		m_iTextureIndices[i] = -1;
		m_ShaderVarName[i] = nullptr;
		m_TextureComName[i] = nullptr;
		m_vTextureColors[i] = { 1.f, 1.f, 1.f, 1.f };
	}

	for (_uint i = 0; i < 2; ++i)
		m_fUVSpeeds[i] = 0.f;
}

CEffect_Mesh_Base::CEffect_Mesh_Base(const CEffect_Mesh_Base& rhs)
	: CEffect_Base_S2(rhs)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pModelCom(nullptr)
	, m_pVIBufferCom(nullptr)
	, m_iModelIndex(-1)
	, m_iMeshIndex(0)
{
	for (_uint i = 0; i < TEXTURE_END; ++i)
	{
		m_pTextureCom[i] = nullptr;
		m_iTextureIndices[i] = -1;
		m_ShaderVarName[i] = nullptr;
		m_TextureComName[i] = nullptr;
		m_vTextureColors[i] = { 1.f, 1.f, 1.f, 1.f };
	}

	for (_uint i = 0; i < 2; ++i)
		m_fUVSpeeds[i] = 0.f;
}

HRESULT CEffect_Mesh_Base::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Mesh_Base::Initialize(void* pArg)
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
	return S_OK;
}

HRESULT CEffect_Mesh_Base::Late_Initialize(void* pArg)
{
	return S_OK;
}

void CEffect_Mesh_Base::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Tick(fTimeDelta);

	if (3 == m_iRenderPass) /* dissolve Pass */
	{
		m_fDissolveAlpha += m_fDissolveSpeed * fTimeDelta;

		if (m_fDissolveAlpha > 1.f)
		{
			m_fDissolveAlpha = 1.f;
			m_bActive = false;
		}
	}

	if (4 == m_iRenderPass) /* Diffuse dissolve Pass */
	{
		m_fDissolveAlpha += m_fDissolveSpeed * fTimeDelta;


		if (m_fDissolveAlpha > 0.3)
		{
			m_fDissolveAlpha = 0.3f;
			m_fDissolveSpeed *= -1;
		}

		else if (m_fDissolveAlpha < 0.f)
		{
			m_fDissolveAlpha = 0.f;
			m_fDissolveSpeed *= -1;
		}

	}
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
		_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

		if (m_iMeshIndex < (_int)iNumMeshes)
		{
			m_pModelCom->Render(m_pShaderCom, m_iMeshIndex, nullptr, m_iRenderPass);
			//m_pModelCom->Bind_Material(m_pShaderCom, m_iMeshIndex, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			//m_pModelCom->Bind_Material(m_pShaderCom, m_iMeshIndex, WJTextureType_MASK, "g_MaskTexture");
		}
		//
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
					m_iTextureIndices[texID] = i;

			}

			/* Color */
			ColorCode(texID);

		}
	}

	/* Mesh Index */
	static _int iMeshIndex;
	iMeshIndex = m_iMeshIndex;
	if (ImGui::DragInt("Mesh Index", &iMeshIndex, 1, 0, 50))
		m_iMeshIndex = iMeshIndex;

	/* RenderPass */
	static _int iRenderPass;
	iRenderPass = m_iRenderPass;
	const char* renderPass[5] = { "Default", "OnlyColor", "DefaultMask", "Dissolve", "HunterString"};
	if (ImGui::ListBox("RenderPass", &iRenderPass, renderPass, 5, 5))
		m_iRenderPass = iRenderPass;

	/* Dissolve Play */
	static _float fDissolveSpeed = 0.f;
	fDissolveSpeed = m_fDissolveSpeed;
	if (ImGui::DragFloat("DissolveSpeed", &fDissolveSpeed, 0.001f, -10.0f, 10.f))
		m_fDissolveSpeed = fDissolveSpeed;
	if (ImGui::Button("Dissolve Play"))
	{
		m_bActive = true;
		Set_DissolveState();
	} ImGui::SameLine();
	if (ImGui::Button("Reset normal"))
	{
		m_bActive = true;
		m_fDissolveAlpha = 0.f;
	}


	/* HDR Intensity (Diffuse) */
	static _float fAlpha = 1.f;
	fAlpha = m_fHDRIntensity;
	if (ImGui::DragFloat("HDR Intensity", &fAlpha, 0.1f, 0.f, 50.f))
		m_fHDRIntensity = fAlpha;

	/* Texture Local Y Discard */
	static _float fTest = 0.f;
	fTest = m_fCutY;
	if (ImGui::DragFloat("test", &fTest, 0.01f, -10.f, 10.f))
		m_fCutY = fTest;
	/* ~ Render Info */

	ImGui::Separator();
	Save_Data();

}

HRESULT CEffect_Mesh_Base::Save_Data()
{
	static	char szSaveFileName[MAX_PATH] = "";
	ImGui::SetNextItemWidth(200);
	ImGui::InputTextWithHint("##SaveData", "File Name", szSaveFileName, MAX_PATH); ImGui::SameLine();
	ImGui::SameLine();
	if (ImGui::Button("Reset FileName"))
		strcpy_s(szSaveFileName, MAX_PATH, m_strEffectTag.c_str());

	ImGui::SameLine();
	if (ImGui::Button("Save"))
		ImGuiFileDialog::Instance()->OpenDialog("Save File", "Save", ".json", "../Bin/Data/Effect_UI/", szSaveFileName, 0, nullptr, ImGuiFileDialogFlags_Modal);

	if (ImGuiFileDialog::Instance()->Display("Save File"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string		strSaveDirectory = ImGuiFileDialog::Instance()->GetCurrentPath();
			strSaveDirectory += "\\";
			strSaveDirectory += szSaveFileName;
			strSaveDirectory += ".json";
			Json	json;

			json["00. RenderPass"] = m_iRenderPass;
			for (_uint i = 0; i < 4; ++i)
			{
				_float fElement = *((float*)&m_vTextureColors[TEXTURE_DIFFUSE] + i);
				json["01. Color"].push_back(fElement);
			}

			for (_uint i = 0; i < 4; ++i)
			{
				_float fElement = *((float*)&m_vTextureColors[TEXTURE_MASK] + i);
				json["02. MaskColor"].push_back(fElement);

			}
			json["03. HDR Intensity"] = m_fHDRIntensity;
			json["04. Model Index"] = m_iModelIndex;
			json["05. CutY"] = m_fCutY;

			json["10. DiffuseTextureIndex"] = m_iTextureIndices[TEXTURE_DIFFUSE];
			json["11. MaskTextureIndex"] = m_iTextureIndices[TEXTURE_MASK];
			json["12. DissolveTextureIndex"] = m_iTextureIndices[TEXTURE_DISSOLVE];


			ofstream file(strSaveDirectory.c_str());
			file << json;
			file.close();
			ImGuiFileDialog::Instance()->Close();
		}
		else
			ImGuiFileDialog::Instance()->Close();
	}

	return S_OK;
}

HRESULT CEffect_Mesh_Base::Load_Data(_tchar* fileName)
{
	if (fileName == nullptr)
		return E_FAIL;

	Json	jLoad;

	wstring filePath = L"../Bin/Data/Effect_UI/";
	filePath += fileName;
	filePath += L".json";

	ifstream file(filePath);
	if (file.fail())
		return E_FAIL;
	file >> jLoad;
	file.close();

	jLoad["00. RenderPass"].get_to<_int>(m_iRenderPass);

	int i = 0;
	for (auto fElement : jLoad["01. Color"])
		fElement.get_to<_float>(*((_float*)&m_vTextureColors[TEXTURE_DIFFUSE] + i++));

	i = 0;
	for (auto fElement : jLoad["02. MaskColor"])
		fElement.get_to<_float>(*((_float*)&m_vTextureColors[TEXTURE_MASK] + i++));

	jLoad["03. HDR Intensity"].get_to<_float>(m_fHDRIntensity);
	jLoad["04. Model Index"].get_to<_int>(m_iModelIndex);

	jLoad["05. CutY"].get_to<_float>(m_fCutY);

	jLoad["10. DiffuseTextureIndex"].get_to<_int>(m_iTextureIndices[TEXTURE_DIFFUSE]);
	jLoad["11. MaskTextureIndex"].get_to<_int>(m_iTextureIndices[TEXTURE_MASK]);
	if (jLoad.contains("12. DissolveTextureIndex"))
		jLoad["12. DissolveTextureIndex"].get_to<_int>(m_iTextureIndices[TEXTURE_DISSOLVE]);
	return S_OK;
}

void CEffect_Mesh_Base::Set_DissolveState()
{
	m_iRenderPass = 3;			/* Default Dissolve RenderPass */
	m_fDissolveAlpha = 0.f;
}

void CEffect_Mesh_Base::BackToNormal()
{

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

	/* For Com_Model */
	SetUp_Model(m_iModelIndex);

	return S_OK;
}

HRESULT CEffect_Mesh_Base::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_fCutY", &m_fCutY, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveAlpha", &m_fDissolveAlpha, sizeof(_float))))
		return E_FAIL;


	m_fUVSpeeds[0] += 0.01f;
	m_fUVSpeeds[1] = 0.f;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fUVSpeedX", &m_fUVSpeeds[0], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fUVSpeedY", &m_fUVSpeeds[1], sizeof(_float))))
		return E_FAIL;



	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Mesh_Base::SetUp_TextureInfo()
{
	/* Ready For Textures */
	m_ShaderVarName[TEXTURE_DIFFUSE] = "g_DiffuseTexture";
	m_ShaderVarName[TEXTURE_MASK] = "g_MaskTexture";
	m_ShaderVarName[TEXTURE_DISSOLVE] = "g_DissolveTexture";

	m_TextureComName[TEXTURE_DIFFUSE] = L"Com_DiffuseTexture";
	m_TextureComName[TEXTURE_MASK] = L"Com_MaskTexture";
	m_TextureComName[TEXTURE_DISSOLVE] = L"Com_DissolveTexture";


	m_ShaderColorName[TEXTURE_DIFFUSE] = "g_vColor";
	m_ShaderColorName[TEXTURE_MASK] = "g_vMaskColor";
	m_ShaderColorName[TEXTURE_DISSOLVE] = "g_vDissolveColor";

	return S_OK;
}

HRESULT CEffect_Mesh_Base::SetUp_Model(_int iModelIndex)
{
	/* For.Com_Model */
	switch (iModelIndex)
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
	case 7: 
		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Boss_Hunter"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	default:
		break;
	}

	return S_OK;
}

HRESULT CEffect_Mesh_Base::Set_ModelCom()
{
	static _int iSelected;
	ImGui::RadioButton("Plane", &iSelected, 0); ImGui::SameLine();
	ImGui::RadioButton("Cube", &iSelected, 1); ImGui::SameLine();
	ImGui::RadioButton("Cone", &iSelected, 2); ImGui::SameLine();
	ImGui::RadioButton("Sphere", &iSelected, 3); ImGui::SameLine();
	ImGui::RadioButton("ShockBall", &iSelected, 4);
	ImGui::RadioButton("Cylinder", &iSelected, 5); ImGui::SameLine();
	ImGui::RadioButton("HunterArrow", &iSelected, 6); ImGui::SameLine();
	ImGui::RadioButton("Hunter", &iSelected, 7); ImGui::SameLine();


	if (ImGui::Button("Model Confirm"))
	{
		if (m_pModelCom != nullptr)
		{
			Delete_Component(L"Com_Model");
		}

		m_iModelIndex = iSelected;

		/* For.Com_Model */
		SetUp_Model(iSelected);
		//	switch (iSelected)
		//	{
		//	case 0:
		//		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Plane"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		//			return E_FAIL;
		//		break;
		//	case 1:
		//		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Cube"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		//			return E_FAIL;
		//		break;
		//	case 2:
		//		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Cone"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		//			return E_FAIL;
		//		break;
		//	case 3:
		//		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Sphere"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		//			return E_FAIL;
		//		break;
		//	case 4:
		//		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_shockball"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		//			return E_FAIL;
		//		break;
		//	case 5:
		//		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Cylinder"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		//			return E_FAIL;
		//		break;
		//	case 6:
		//		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Boss_Hunter_Arrow"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		//			return E_FAIL;
		//		break;
		//	}
	}

	return S_OK;
}

void CEffect_Mesh_Base::ColorCode(_int iTexID)
{
	static bool alpha_preview = true;
	static bool alpha_half_preview = false;
	static bool drag_and_drop = true;
	static bool options_menu = true;
	static bool hdr = false;

	ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

	static bool   ref_color = false;
	static ImVec4 ref_color_v(1.0f, 1.0f, 1.0f, 1.0f);

	static _float4 vSelectColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	vSelectColor = m_vTextureColors[iTexID];

	if (ImGui::ColorPicker4("CurColor##4", (float*)&vSelectColor, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color ? &ref_color_v.x : NULL))
		m_vTextureColors[iTexID] = vSelectColor;
	if (ImGui::ColorEdit4("Diffuse##2f", (float*)&vSelectColor, ImGuiColorEditFlags_DisplayRGB | misc_flags))
		m_vTextureColors[iTexID] = vSelectColor;

}

CEffect_Mesh_Base* CEffect_Mesh_Base::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Mesh_Base* pInstance = new CEffect_Mesh_Base(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create: CEffect_Mesh_Base");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect_Mesh_Base::Clone(void* pArg)
{
	CEffect_Mesh_Base* pInstance = new CEffect_Mesh_Base(*this);

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