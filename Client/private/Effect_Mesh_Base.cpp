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
		m_fUVScale[i] = 1.f;
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
		m_fUVScale[i] = 1.f;
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
	//m_pTransformCom->Set_Scaled({ 5.f, 5.f ,5.f });
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

	if (m_bActiveSlowly)
	{
		m_vTextureColors[0].w += fTimeDelta;
		if (m_vTextureColors[0].w > 1.0f)
		{
			m_vTextureColors[0].w = 1.0f;
			m_bActiveSlowly = false;
		}
	}

	if (m_bDeActiveSlowly)
	{
		m_vTextureColors[0].w -= fTimeDelta;
		if (m_vTextureColors[0].w < 0.0f)
		{
			m_vTextureColors[0].w = 1.0f;
			m_bDeActiveSlowly = false;
			DeActivate();
		}
	}

	if (0.0f != m_vScaleSpeed.Length())
	{
		_float3 vScale = m_pTransformCom->Get_Scaled();
		vScale.x += m_vScaleSpeed.x;
		vScale.z += m_vScaleSpeed.x;

		vScale.y += m_vScaleSpeed.y;

		m_pTransformCom->Set_Scaled(vScale);
	}


	if (3 == m_iRenderPass) /* dissolve Pass */
	{
		m_fDissolveSpeed = 1.f;
		m_fDissolveAlpha += m_fDissolveSpeed * fTimeDelta;

		if (m_fDissolveAlpha > 1.f)
		{
			m_fDissolveAlpha = 1.0f;

			DeActivate();
			//m_bActive = false;
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

	if (m_pTarget != nullptr)
	{
		if (m_pBoneName != nullptr)
		{
			_float4 vPos = m_pTarget->Get_ComputeBonePosition(m_pBoneName);
			m_pTransformCom->Set_Position(
				m_pTarget->Get_ComputeBonePosition(m_pBoneName));
		}
		else
		{
			_float4 vTargetPos = m_pTarget->Get_TransformCom()->Get_Position() + m_ParentPosition;/* correct */
			m_pTransformCom->Set_Position(vTargetPos);
		}
	}

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CEffect_Mesh_Base::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	if (m_pModelCom != nullptr && m_pShaderCom != nullptr)
	{
		_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

		if (m_iMeshIndex < (_int)iNumMeshes)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, m_iMeshIndex, nullptr, m_iRenderPass),E_FAIL);
		}
	}

	return S_OK;
}

void CEffect_Mesh_Base::Imgui_RenderProperty()
{
	ImGui::Separator();

	ImGui::Text("<Set Model>");
	Set_ModelCom();

	m_pTransformCom->Imgui_RenderProperty_ForJH();

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

	if (ImGui::CollapsingHeader("Render Info"))
	{
		/* Mesh Index */
		static _int iMeshIndex;
		iMeshIndex = m_iMeshIndex;
		if (ImGui::DragInt("Mesh Index", &iMeshIndex, 1, 0, 50))
			m_iMeshIndex = iMeshIndex;

		/* RenderPass */
		static _int iRenderPass;
		iRenderPass = m_iRenderPass;
		const char* renderPass[6] = { "Default", "OnlyColor", "DefaultMask", "Dissolve", "Mask_V2", "Mask_DiffuseMove"};
		if (ImGui::ListBox("RenderPass", &iRenderPass, renderPass, 6, 5))
			m_iRenderPass = iRenderPass;

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

		static _float UVScale[2];
		UVScale[0] = m_fUVScale[0];
		UVScale[1] = m_fUVScale[1];
		if (ImGui::DragFloat2("UVScale", UVScale, 0.10f, 0.0f, 100.0f))
		{
			m_fUVScale[0] = UVScale[0];
			m_fUVScale[1] = UVScale[1];
		}

		if (ImGui::Button("ReCompile"))
		{
			m_pShaderCom->ReCompile();
			m_pRendererCom->ReCompile();
		}
	}


	/* Options(UV Animation, Sprite Animation, etc...) */
	Options();

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

			for (_uint i = 0; i < 2; ++i)
				json["06. UVScale"].push_back(m_fUVScale[i]);

			_float3 vScale = m_pTransformCom->Get_Scaled();
			for (_uint i = 0; i < 3; ++i)
			{
				_float fElement = *((float*)&vScale + i);
				json["07. vScale"].push_back(fElement); 
			}  

			json["10. DiffuseTextureIndex"] = m_iTextureIndices[TEXTURE_DIFFUSE];
			json["11. MaskTextureIndex"] = m_iTextureIndices[TEXTURE_MASK];
			json["12. DissolveTextureIndex"] = m_iTextureIndices[TEXTURE_DISSOLVE];

			for (_uint i = 0; i < OPTION_END; ++i)
				json["20. Options"].push_back(m_bOptions[i]);

			for (_uint i = 0; i < 2; ++i)
			{
				json["21. UVSpeed"].push_back(m_fUVSpeeds[i]);
				json["22. SpriteFrames"].push_back(m_iFrames[i]);
			}
			json["23. SpriteSpeed"] = m_fFrameSpeed;
			json["24. SelfStop"] = m_bSelfStop;
			json["25. SelfStopTime"] = m_fSelfStopTime;

			json["26. DissolvePass"] = m_iDissolvePass;
			json["27. DissolveSpeed"] = m_fDissolveSpeed;

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
	m_iRenderPassOriginal = m_iRenderPass;

	int i = 0;
	for (auto fElement : jLoad["01. Color"])
		fElement.get_to<_float>(*((_float*)&m_vTextureColors[TEXTURE_DIFFUSE] + i++));

	i = 0;
	for (auto fElement : jLoad["02. MaskColor"])
		fElement.get_to<_float>(*((_float*)&m_vTextureColors[TEXTURE_MASK] + i++));

	jLoad["03. HDR Intensity"].get_to<_float>(m_fHDRIntensity);
	jLoad["04. Model Index"].get_to<_int>(m_iModelIndex);

	jLoad["05. CutY"].get_to<_float>(m_fCutY);

	if (jLoad.contains("06. UVScale"))
	{
		i = 0;
		for (auto bOption : jLoad["06. UVScale"])
			bOption.get_to<_float>(m_fUVScale[i++]);
	}

	if (jLoad.contains("07. vScale"))
	{
		_float3 vScale = { 0.f,0.f,0.f };
		i = 0;
		for (auto fElement : jLoad["07. vScale"])
			fElement.get_to<_float>(*((_float*)&vScale + i++));
		m_pTransformCom->Set_Scaled(vScale);
		m_vScaleOrignial = vScale;
	}

	jLoad["10. DiffuseTextureIndex"].get_to<_int>(m_iTextureIndices[TEXTURE_DIFFUSE]);
	jLoad["11. MaskTextureIndex"].get_to<_int>(m_iTextureIndices[TEXTURE_MASK]);
	if (jLoad.contains("12. DissolveTextureIndex"))
		jLoad["12. DissolveTextureIndex"].get_to<_int>(m_iTextureIndices[TEXTURE_DISSOLVE]);


	if (jLoad.contains("20. Options"))
	{
		i = 0;
		for (auto bOption : jLoad["20. Options"])
			bOption.get_to<_bool>(m_bOptions[i++]);
	}

	if (jLoad.contains("21. UVSpeed"))
	{
		i = 0;
		for (auto fElement : jLoad["21. UVSpeed"])
			fElement.get_to<_float>(m_fUVSpeeds[i++]);
	}

	i = 0;
	if (jLoad.contains("22. SpriteFrames"))
	{
		for (auto fElement : jLoad["22. SpriteFrames"])
			fElement.get_to<_int>(m_iFrames[i++]);
	}

	if (jLoad.contains("23. SpriteSpeed"))
		jLoad["23. SpriteSpeed"].get_to<_float>(m_fFrameSpeed);

	if (jLoad.contains("24. SelfStop"))
		jLoad["24. SelfStop"].get_to<_bool>(m_bSelfStop);

	if (jLoad.contains("25. SelfStopTime"))
		jLoad["25. SelfStopTime"].get_to<_float>(m_fSelfStopTime);

	if (jLoad.contains("26. DissolvePass"))
		jLoad["26. DissolvePass"].get_to<_int>(m_iDissolvePass);

	if (jLoad.contains("27. DissolveSpeed"))
		jLoad["27. DissolveSpeed"].get_to<_float>(m_fDissolveSpeed);

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

void CEffect_Mesh_Base::Activate(_float4 vPos)
{
	m_bActive = true;
	m_pTransformCom->Set_Position(vPos);
}

void CEffect_Mesh_Base::Activate(CGameObject* pTarget)
{
	m_bActive = true;
	m_pTarget = pTarget;
}

void CEffect_Mesh_Base::Activate_Scaling(_float4 vDir, _float4 vPos, _float2 vScaleSpeed)
{
	m_bActive = true;
	m_pTransformCom->Set_Position(vPos);

	m_fDissolveAlpha = 0.0f;
	m_vScaleSpeed = vScaleSpeed;
}

void CEffect_Mesh_Base::Activate_Slowly(_float4 vPos)
{
	m_bActive = true;
	m_pTransformCom->Set_Position(vPos);
	m_vTextureColors[0].w = 0.0f;
	m_bActiveSlowly = true;
	m_bDeActiveSlowly = false;
}

void CEffect_Mesh_Base::DeActivate()
{
	__super::DeActivate();

	m_pTransformCom->Set_Scaled(m_vScaleOrignial);

	m_vScaleSpeed = { 0.0f, 0.0f };
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
		if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Texture_Effect"), m_TextureComName[i],
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
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

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

	if (FAILED(m_pShaderCom->Set_RawValue("g_float_0", &m_fHDRIntensity, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_float_1", &m_fCutY, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_float_2", &m_fDissolveAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_bool_0", &m_bOptions[OPTION_SPRITE], sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_bool_1", &m_bOptions[OPTION_UV], sizeof(_bool))))
		return E_FAIL;

	if (m_bOptions[OPTION_UV])
	{
		_float2 vUVMove = { m_fUVMove[0], m_fUVMove[1] };
		if (FAILED(m_pShaderCom->Set_RawValue("g_float2_0", &vUVMove, sizeof(_float2))))
			return E_FAIL;

		_float2 vUVScale = { m_fUVScale[0], m_fUVScale[1] };
		if (FAILED(m_pShaderCom->Set_RawValue("g_float2_1", &vUVScale, sizeof(_float2))))
			return E_FAIL;
	}

	if (m_bOptions[OPTION_SPRITE])
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_int_0", &m_iFrames[0], sizeof(_int))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_int_1", &m_iFrames[1], sizeof(_int))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_int_2", &m_iFrameNow[0], sizeof(_int))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_int_3", &m_iFrameNow[1], sizeof(_int))))
			return E_FAIL;
	} 


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Mesh_Base::SetUp_TextureInfo()
{
	/* Ready For Textures */

	m_ShaderVarName[TEXTURE_DIFFUSE]	= "g_tex_0";
	m_ShaderVarName[TEXTURE_MASK]		= "g_tex_1";
	m_ShaderVarName[TEXTURE_DISSOLVE]	= "g_tex_2";

	m_TextureComName[TEXTURE_DIFFUSE]	= L"Com_DiffuseTexture";
	m_TextureComName[TEXTURE_MASK]		= L"Com_MaskTexture";
	m_TextureComName[TEXTURE_DISSOLVE]	= L"Com_DissolveTexture";

	m_ShaderColorName[TEXTURE_DIFFUSE]	= "g_float4_0";
	m_ShaderColorName[TEXTURE_MASK]		= "g_float4_1";
	m_ShaderColorName[TEXTURE_DISSOLVE] = "g_float4_2";

	return S_OK;
}

HRESULT CEffect_Mesh_Base::SetUp_Model(_int iModelIndex)
{
	/* For.Com_Model */
	switch (iModelIndex)
	{
	case 0:
		if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Model_Plane"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 1:
		if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Model_Cube"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 2:
		if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Model_Cone"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 3:
		if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Model_Sphere"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 4:
		if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Model_shockball"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 5:
		if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Model_Cylinder"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 6:
		if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Model_Boss_Hunter_Arrow"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 7:
		if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Model_Boss_Hunter"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	case 8:
		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_ShockRing"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
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
	ImGui::RadioButton("Ring", &iSelected, 8); ImGui::SameLine();


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