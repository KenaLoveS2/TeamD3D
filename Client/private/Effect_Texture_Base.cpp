#include "stdafx.h"
#include "..\public\Effect_Texture_Base.h"
#include "GameInstance.h"


CEffect_Texture_Base::CEffect_Texture_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base_S2(pDevice, pContext)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
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

CEffect_Texture_Base::CEffect_Texture_Base(const CEffect_Texture_Base& rhs)
	: CEffect_Base_S2(rhs)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
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

HRESULT CEffect_Texture_Base::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Texture_Base::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_TextureInfo()))
		return E_FAIL;

	if (pArg != nullptr)
	{
		m_pfileName = (_tchar*)pArg;
		if (FAILED(Load_Data(m_pfileName)))
			m_pTransformCom->Set_Scaled(_float3(5.f, 5.f, 1.f));
	}
	else
		m_pTransformCom->Set_Scaled(_float3(5.f, 5.f, 1.f));

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetupComponents : CEffect_Texture_Base");
		return E_FAIL;
	}





	/* temp */


	return S_OK;
}

HRESULT CEffect_Texture_Base::Late_Initialize(void* pArg)
{
	return S_OK;
}

void CEffect_Texture_Base::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Tick(fTimeDelta);

	if (0.0f != m_vScaleSpeed.Length())
	{
		_smatrix matLocal = m_LocalMatrix;

		_float fScaleRight	= matLocal.Right().Length() + (m_vScaleSpeed.x * fTimeDelta);
		_float fScaleUp		= matLocal.Up().Length() + (m_vScaleSpeed.y * fTimeDelta);

		_float3 vRight = XMVector3Normalize(matLocal.Right());
		_float3 vUp = XMVector3Normalize(matLocal.Up());

		matLocal.Right(fScaleRight * vRight);
		matLocal.Up(fScaleUp * vUp);
		m_LocalMatrix = matLocal;
	}
}

void CEffect_Texture_Base::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);

	_matrix worldMatrix = XMMatrixIdentity();
	_smatrix matLocal = m_LocalMatrix;

	if (m_pTarget != nullptr)
	{
		worldMatrix = m_pTarget->Get_WorldMatrix();
		worldMatrix = _smatrix::CreateBillboard(
			XMLoadFloat4(&m_pTarget->Get_TransformCom()->Get_Position()),
			CGameInstance::GetInstance()->Get_CamPosition_Float3(),
			CGameInstance::GetInstance()->Get_CamUp_Float3(),
			&CGameInstance::GetInstance()->Get_CamLook_Float3());
	}
	else
	{

		worldMatrix = _smatrix::CreateBillboard(
			//XMVectorSet(0.f, 0.f, 0.f, 1.f),
			//matLocal.Translation(),
			XMLoadFloat4(&m_ParentPosition),
			CGameInstance::GetInstance()->Get_CamPosition_Float3(),
			CGameInstance::GetInstance()->Get_CamUp_Float3(),
			&CGameInstance::GetInstance()->Get_CamLook_Float3());
		//matLocal.Translation({ 0.f, 0.f, 0.f });
	}

	worldMatrix = matLocal * worldMatrix;

	//worldMatrix = m_pTransformCom->Get_WorldMatrix() * worldMatrix;
	m_pTransformCom->Set_WorldMatrix(worldMatrix);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

}

HRESULT CEffect_Texture_Base::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	m_pShaderCom->Begin(m_iRenderPass);
	m_pVIBufferCom->Render();
	return S_OK;
}

void CEffect_Texture_Base::Imgui_RenderProperty()
{
	if (ImGui::Button("Reset LocalMatrix"))
		m_LocalMatrix = m_LocalMatrixOriginal;

	//m_pTransformCom->Imgui_RenderProperty();
	/* Transform */
	{
		/* Local Position */
		static _float3  vPosition = { 0.f, 0.f,0.f };
		vPosition = { m_LocalMatrix._41, m_LocalMatrix._42, m_LocalMatrix._43 };
		if (ImGui::DragFloat3("LocalPosition", (_float*)&vPosition, 0.10f, -1000.0f, 1000.0f, "%.3f"))
		{
			m_LocalMatrix._41 = vPosition.x; // right 
			m_LocalMatrix._42 = vPosition.y; // up 
			m_LocalMatrix._43 = vPosition.z; // look
		}

		/* Local Scale */
		static _float2 vScale = { 1.f, 1.f };
		_smatrix matLocal = m_LocalMatrix;
		vScale.x = matLocal.Right().Length();
		vScale.y = matLocal.Up().Length();
		if (ImGui::DragFloat2("LocalScale", (_float*)&vScale, 0.10f, 0.001f, 100.0f, "%.3f"))
		{
			_float3 vRight = vScale.x * XMVector3Normalize(matLocal.Right());
			_float3 vUp = vScale.y * XMVector3Normalize(matLocal.Up());
			m_LocalMatrix._11 = vRight.x;
			m_LocalMatrix._12 = vRight.y;
			m_LocalMatrix._13 = vRight.z;

			m_LocalMatrix._21 = vUp.x;
			m_LocalMatrix._22 = vUp.y;
			m_LocalMatrix._23 = vUp.z;

			_int i = 0;
		}

		/* Local Rotation */
		static _float fDegree = 0.f;
		if (ImGui::DragFloat("Rotation Degree", &fDegree, 0.1f, -180.f, 180.f))
		{
			_smatrix matLocal = m_LocalMatrix;
			matLocal.Right({ matLocal.Right().Length(), 0.f, 0.f });
			matLocal.Up({ 0.f, matLocal.Up().Length(), 0.f });
			matLocal.Forward({ 0.f,0.f, 1.f });
			_float3 translation = matLocal.Translation(); /* save */
			matLocal.Translation({ 0,0,0 });
			_matrix matRot = XMMatrixRotationZ(XMConvertToRadians(fDegree));
			matLocal = matLocal * matRot;
			matLocal.Translation(translation);
			XMStoreFloat4x4(&m_LocalMatrix, matLocal);
		}
	}


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

			/* Color */
			ColorCode(texID);

		}
	}

	/* RenderPass */
	static _int iRenderPass;
	iRenderPass = m_iRenderPass;
	const char* renderPass[2] = { "Default", "MaskDefault" };
	if (ImGui::ListBox("RenderPass", &iRenderPass, renderPass, 2, 5))
		m_iRenderPass = iRenderPass;

	/* HDR Intensity (Diffuse) */
	static _float fAlpha = 1.f;
	fAlpha = m_fHDRIntensity;
	if (ImGui::DragFloat("HDR Intensity", &fAlpha, 0.1f, 0.f, 50.f))
		m_fHDRIntensity = fAlpha;

	/* Options(UV Animation, Sprite Animation, etc...) */
	Options();

	ImGui::Separator();
	Save_Data();

}

HRESULT CEffect_Texture_Base::Save_Data()
{
	static	char szSaveFileName[MAX_PATH] = "";
	ImGui::SetNextItemWidth(200);
	ImGui::InputTextWithHint("##SaveData", "File Name", szSaveFileName, MAX_PATH);
	ImGui::SameLine();
	if (ImGui::Button("Reset FileName"))
		strcpy_s(szSaveFileName, MAX_PATH, m_strEffectTag.c_str());
	ImGui::SameLine();

	if (ImGui::Button("Save"))
		ImGuiFileDialog::Instance()->OpenDialog("Select File", "Select", ".json", "../Bin/Data/Effect_UI/", szSaveFileName, 0, nullptr, ImGuiFileDialogFlags_Modal);

	if (ImGuiFileDialog::Instance()->Display("Select File"))
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
			json["02. HDR Intensity"] = m_fHDRIntensity;

			for (_uint i = 0; i < 16; ++i)
			{
				_float fElement = *((float*)&m_LocalMatrix + i);
				json["03. LocalMatrix"].push_back(fElement);
			}
			for (_uint i = 0; i < 4; ++i)
			{
				_float fElement = *((float*)&m_vTextureColors[TEXTURE_MASK] + i);
				json["04. MaskColor"].push_back(fElement);
			}

			json["10. DiffuseTextureIndex"] = m_iTextureIndices[TEXTURE_DIFFUSE];
			json["11. MaskTextureIndex"] = m_iTextureIndices[TEXTURE_MASK];

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

HRESULT CEffect_Texture_Base::Load_Data(_tchar* fileName)
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

	jLoad["02. HDR Intensity"].get_to<_float>(m_fHDRIntensity);

	i = 0;
	_float4x4 matLocal;
	for (auto fElement : jLoad["03. LocalMatrix"])
		fElement.get_to<_float>(*((_float*)&matLocal + i++));
	m_LocalMatrix = matLocal;
	m_LocalMatrixOriginal = matLocal;


	i = 0;
	if (jLoad.contains("04. MaskColor"))
	{
		for (auto fElement : jLoad["04. MaskColor"])
			fElement.get_to<_float>(*((_float*)&m_vTextureColors[TEXTURE_MASK] + i++));
	}

	jLoad["10. DiffuseTextureIndex"].get_to<_int>(m_iTextureIndices[TEXTURE_DIFFUSE]);
	jLoad["11. MaskTextureIndex"].get_to<_int>(m_iTextureIndices[TEXTURE_MASK]);

	i = 0;
	if (jLoad.contains("20. Options"))
	{
		for (auto bOption : jLoad["20. Options"])
			bOption.get_to<_bool>(m_bOptions[i++]);
	}

	i = 0;
	if (jLoad.contains("21. UVSpeed"))
	{
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

void CEffect_Texture_Base::BackToNormal()
{
	m_LocalMatrix = m_LocalMatrixOriginal;
}

void CEffect_Texture_Base::Activate(_float4 vPos)
{
	m_bActive = true;
	m_ParentPosition = vPos;

}

void CEffect_Texture_Base::Activate(CGameObject* pTarget)
{
	m_bActive = true;
	m_pTarget = pTarget;
}

void CEffect_Texture_Base::Activate_Scaling(CGameObject* pTarget, _float2 vScaleSpeed)
{
	m_bActive = true;
	m_pTarget = pTarget;
	m_vScaleSpeed = vScaleSpeed;
}

void CEffect_Texture_Base::Activate_Scaling(_float4 vPos, _float2 vScaleSpeed)
{
	m_bActive = true;
	m_ParentPosition = vPos;
	m_vScaleSpeed = vScaleSpeed;
}

void CEffect_Texture_Base::Activate_Spread(_float4 vPos, _float2 vScaleSpeed)
{
	m_bActive = true;
	m_ParentPosition = vPos;
	m_vScaleSpeed = vScaleSpeed;

	/* Shrink At First */
	_smatrix matLocal = m_LocalMatrix;

	_float3 vRight = matLocal.Right();
	vRight.Normalize();
	matLocal.Right(0.01f * vRight);

	_float3 vUp = matLocal.Up();
	vUp.Normalize();
	matLocal.Up(0.1f * vUp);

	m_LocalMatrix = matLocal;
}

void CEffect_Texture_Base::DeActivate()
{
	__super::DeActivate();

	m_LocalMatrix = m_LocalMatrixOriginal;
	m_vScaleSpeed = { 0.0f, 0.0f };
}

HRESULT CEffect_Texture_Base::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_Effect_Texture_S2"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))

		return E_FAIL;
	/* VIBuffer_Rect */
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom))
		return E_FAIL;

	/* For Com_Texture */
	for (_uint i = 0; i < TEXTURE_END; ++i)
	{
		if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Texture_Effect"), m_TextureComName[i],
			(CComponent**)&m_pTextureCom[i])))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CEffect_Texture_Base::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_bool_0", &m_bOptions[OPTION_SPRITE], sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_bool_1", &m_bOptions[OPTION_UV], sizeof(_bool))))
		return E_FAIL;

	if (m_bOptions[OPTION_UV])
	{
		_float2 vUVMove = { m_fUVMove[0], m_fUVMove[1] };

		if (FAILED(m_pShaderCom->Set_RawValue("g_float2_0", &vUVMove, sizeof(_float2))))
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

HRESULT CEffect_Texture_Base::SetUp_TextureInfo()
{
	/* Ready For Textures */
	m_ShaderVarName[TEXTURE_DIFFUSE]	= "g_tex_0";
	m_ShaderVarName[TEXTURE_MASK]		= "g_tex_1";

	m_TextureComName[TEXTURE_DIFFUSE]	= L"Com_DiffuseTexture";
	m_TextureComName[TEXTURE_MASK]		= L"Com_MaskTexture";

	m_ShaderColorName[TEXTURE_DIFFUSE]	= "g_float4_0";
	m_ShaderColorName[TEXTURE_MASK]		 = "g_float4_1";

	return S_OK;
}

void CEffect_Texture_Base::ColorCode(_int iTexID)
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

CEffect_Texture_Base* CEffect_Texture_Base::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Texture_Base* pInstance = new CEffect_Texture_Base(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create: CEffect_Texture_Base");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect_Texture_Base::Clone(void* pArg)
{
	CEffect_Texture_Base* pInstance = new CEffect_Texture_Base(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CEffect_Texture_Base");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Texture_Base::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	for (_uint i = 0; i < TEXTURE_END; ++i)
		Safe_Release(m_pTextureCom[i]);
}