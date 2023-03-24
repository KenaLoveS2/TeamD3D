#include "stdafx.h"
#include "..\public\Effect_Particle_Base.h"
#include "GameInstance.h"
#include "Kena.h"

CEffect_Particle_Base::CEffect_Particle_Base(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice,pContext)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pTarget(nullptr)
	, m_pTextureCom(nullptr)
	, m_pVIBufferCom(nullptr)
	, m_iRenderPass(0)
	, m_iTextureIndex(0)
	, m_vColor(1.f, 1.f, 1.f, 1.f)
{
}

CEffect_Particle_Base::CEffect_Particle_Base(const CEffect_Particle_Base & rhs)
	: CGameObject(rhs)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pTarget(nullptr)
	, m_pTextureCom(nullptr)
	, m_pVIBufferCom(nullptr)
	, m_iRenderPass(0)
	, m_iTextureIndex(0)
	, m_vColor(1.f, 1.f, 1.f, 1.f)
{
}

HRESULT CEffect_Particle_Base::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Particle_Base::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		m_pfileName = (_tchar*)pArg;
		if (FAILED(Load_Data(m_pfileName)))
			SetUp_Buffer();
	}
	else
		SetUp_Buffer();

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetupComponents : CEffect_Particle_Base");
		return E_FAIL;
	}

	/* temp */
	m_pTransformCom->Set_Scaled(_float3(0.2f, 0.2f, 0.2f));

	return S_OK;
}

HRESULT CEffect_Particle_Base::Late_Initialize(void * pArg)
{
	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	//CKena* pKena = dynamic_cast<CKena*>(pGameInstance->Get_GameObjectPtr(
	//	g_LEVEL, L"Layer_Player", L"Kena"));
	//RELEASE_INSTANCE(CGameInstance);

	//m_pTransformCom->Set_Position(pKena->Get_TransformCom()->Get_Position());

	return S_OK;
}

void CEffect_Particle_Base::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pTarget != nullptr)
	{
		_float4 vTargetPos = m_pTarget->Get_TransformCom()->Get_Position();
		m_pTransformCom->Set_Position(vTargetPos);
	}

	m_pVIBufferCom->Tick(fTimeDelta);
}

void CEffect_Particle_Base::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CEffect_Particle_Base::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(m_iRenderPass);
	m_pVIBufferCom->Render();
	return S_OK;
}

void CEffect_Particle_Base::Imgui_RenderProperty()
{
	/* RenderPass */
	static _int iRenderPass;
	iRenderPass = m_iRenderPass;
	const char* renderPass[2] = { "Default", "BlackDiffuse" };
	if (ImGui::ListBox("RenderPass", &iRenderPass, renderPass, 2, 5))
		m_iRenderPass = iRenderPass;

	/* Diffuse Texture Select */
	if (ImGui::CollapsingHeader(" > DiffuseTexture"))
	{
		if (m_pTextureCom == nullptr)
			return;

		/* Texture Tiles */
		for (_uint i = 0; i < m_pTextureCom->Get_TextureIdx(); ++i)
		{
			if (i % 6)
				ImGui::SameLine();

			if (ImGui::ImageButton(m_pTextureCom->Get_Texture(i), ImVec2(50.f, 50.f)))
			{
				m_iTextureIndex = i;
			}
		}

		/* Color */
		ColorCode();
	}

	/* Buffer Update */
	if (ImGui::CollapsingHeader(" > Update Buffer"))
	{
		if (m_pVIBufferCom == nullptr)
			return;

		CVIBuffer_Point_Instancing_S2::POINTINFO tInfo = m_pVIBufferCom->Get_Info();
		using pointType = CVIBuffer_Point_Instancing_S2::POINTINFO::TYPE;

		/* Type */
		static _int iType = tInfo.eType;
		const char* list[1] = { "Haze" };
		ImGui::ListBox("Type", &iType, list, 1, 1);
		tInfo.eType = (pointType)iType;

		/* NumInstance */
		static _int		iNumInstance = tInfo.iNumInstance;
		ImGui::DragInt("NumInstance", &iNumInstance, 1.f, 1, 1000);
		tInfo.iNumInstance = iNumInstance;

		/* Size */
		static _float2	vPSize = tInfo.vPSize;
		ImGui::DragFloat2("vPSize", (_float*)&vPSize, 0.1f, 0.f, 10.f, "%.3f");
		tInfo.vPSize = vPSize;

		/* Term */
		static _float	fTerm = tInfo.fTerm;
		ImGui::DragFloat("fTerm", &fTerm, 0.1f, 0.f, 100.f);
		tInfo.fTerm = fTerm;

		/* MinPos */
		static _float3  vMinPos = tInfo.vMinPos;
		ImGui::DragFloat3("vMinPos", (_float*)&vMinPos, 0.1f, -50.f, 50.f, "%.3f");
		tInfo.vMinPos = vMinPos;

		/* MaxPos */
		static _float3  vMaxPos = tInfo.vMaxPos;
		ImGui::DragFloat3("vMaxPos", (_float*)&vMaxPos, 0.1f, -50.f, 50.f, "%.3f");
		tInfo.vMaxPos = vMaxPos;

		/* SpeedMin */
		static _float3  vSpeedMin = tInfo.vSpeedMin;
		ImGui::DragFloat3("vSpeedMin", (_float*)&vSpeedMin, 0.1f, -10.f, 10.f, "%.3f");
		tInfo.vSpeedMin = vSpeedMin;

		/* SpeedMax */
		static _float3  vSpeedMax = tInfo.vSpeedMax;
		ImGui::DragFloat3("vSpeedMax", (_float*)&vSpeedMax, 0.1f, -10.f, 10.f, "%.3f");
		tInfo.vSpeedMax = vSpeedMax;


		if (ImGui::Button("Reset"))
		{
			CVIBuffer_Point_Instancing_S2::POINTINFO tInfo = m_pVIBufferCom->Get_Info();
			using pointType = CVIBuffer_Point_Instancing_S2::POINTINFO::TYPE;

			iType = tInfo.eType;
			iNumInstance = tInfo.iNumInstance;
			vPSize = tInfo.vPSize;
			fTerm = tInfo.fTerm;
			vSpeedMin = tInfo.vSpeedMin;
			vSpeedMax = tInfo.vSpeedMax;
			vMinPos = tInfo.vMinPos;
			vMaxPos = tInfo.vMaxPos;
		} 
		ImGui::SameLine(); ImGui::PushItemWidth(5);
		
		if (ImGui::Button("Update Confirm"))
		{
			if (FAILED(m_pVIBufferCom->Update_Buffer(&tInfo)))
				MSG_BOX("The Update Process ended unexpectedly. Please Check the Code.");
		}
	}

	ImGui::Separator();
	Save_Data();

}

HRESULT CEffect_Particle_Base::Save_Data()
{
	static	char szSaveFileName[MAX_PATH] = "";
	ImGui::SetNextItemWidth(200);
	ImGui::InputTextWithHint("##SaveData", "File Name", szSaveFileName, MAX_PATH);
	ImGui::SameLine();
	if (ImGui::Button("Save"))
		ImGuiFileDialog::Instance()->OpenDialog("Select File", "Select Json", ".json", "../Bin/Data/Effect_UI/", szSaveFileName, 0, nullptr, ImGuiFileDialogFlags_Modal);

	if (ImGuiFileDialog::Instance()->Display("Select File"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string		strSaveDirectory = ImGuiFileDialog::Instance()->GetCurrentPath();
			strSaveDirectory += "\\";
			strSaveDirectory += szSaveFileName;
			strSaveDirectory += ".json";

			Json	json;

			for (_uint i = 0; i < 4; ++i)
			{
				_float fElement = *((float*)&m_pTransformCom->Get_Position() + i++);
				json["00. Position"].push_back(fElement);
			}

			json["01. TextureIndex"] = m_iTextureIndex;
			json["02. RenderPass"] = m_iRenderPass;
			for (_uint i = 0; i < 4; ++i)
			{
				_float fElement = *((float*)&m_vColor + i++);
				json["03. Color"].push_back(fElement);
			}

			CVIBuffer_Point_Instancing_S2::POINTINFO tInfo = m_pVIBufferCom->Get_Info();
			json["04. Type"] = (_int)tInfo.eType;
			json["05. NumInstance"] = tInfo.iNumInstance;
			json["06. Term"] = tInfo.fTerm;

			for (_uint i = 0; i < 4; ++i)
			{
				_float fElement = *((float*)&tInfo.vPSize + i++);
				json["07. PSize"].push_back(fElement);
			}

			for (_uint i = 0; i < 4; ++i)
			{
				_float fElement = *((float*)&tInfo.vSpeedMin + i++);
				json["08. SpeedMin"].push_back(fElement);
			}

			for (_uint i = 0; i < 4; ++i)
			{
				_float fElement = *((float*)&tInfo.vSpeedMax + i++);
				json["09. SpeedMax"].push_back(fElement);
			}

			for (_uint i = 0; i < 4; ++i)
			{
				_float fElement = *((float*)&tInfo.vMinPos + i++);
				json["10. minPos"].push_back(fElement);
			}

			for (_uint i = 0; i < 4; ++i)
			{
				_float fElement = *((float*)&tInfo.vMaxPos + i++);
				json["11. maxPos"].push_back(fElement);
			}

			ofstream file(strSaveDirectory.c_str());
			file << json;
			file.close();
			ImGuiFileDialog::Instance()->Close();
		}
	}

	return S_OK;
}

HRESULT CEffect_Particle_Base::Load_Data(_tchar* fileName)
{
	if (fileName == nullptr)
		return E_FAIL;

	//ImGuiFileDialog::Instance()->OpenDialog("Load File", "Select Json", ".json", "../Bin/Data", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);
	//if (ImGuiFileDialog::Instance()->Display("Load File"))
	//{
	//	if (ImGuiFileDialog::Instance()->IsOk())
	//	{
	//		ImGuiFileDialog::Instance()->Close();
	//	}
	//}


	Json	jLoad;

	wstring filePath = L"../Bin/Data/Effect_UI/";
	filePath += fileName;
	filePath += L".json";

	ifstream file(filePath);
	if (file.fail())
		return E_FAIL;
	file >> jLoad;
	file.close();

	_float4 vPos;
	for (auto fElement : jLoad["00. Position"])
	{
		static int i = 0;
		fElement.get_to<_float>(*((float*)&vPos + i++));
	}
	m_pTransformCom->Set_Position(vPos);

	jLoad["01. TextureIndex"].get_to<_int>(m_iTextureIndex);
	jLoad["02. RenderPass"].get_to<_int>(m_iRenderPass);
	
	for (auto fElement : jLoad["03. Color"])
	{
		static int i = 0;
		fElement.get_to<_float>(*((float*)&m_vColor + i++));
	}

	CVIBuffer_Point_Instancing_S2::POINTINFO tInfo;
	using pointType = CVIBuffer_Point_Instancing_S2::POINTINFO::TYPE;

	_int iType;
	jLoad["04. Type"].get_to<_int>(iType);
	tInfo.eType = (pointType)iType;
	jLoad["05. NumInstance"].get_to<_int>(tInfo.iNumInstance);
	jLoad["06. Term"].get_to<_float>(tInfo.fTerm);
	tInfo.fTermAcc = 0.f;

	for (auto fElement : jLoad["07. PSize"])
	{
		static int i = 0;
		fElement.get_to<_float>(*((float*)&tInfo.vPSize + i++));
	}

	for (auto fElement : jLoad["08. SpeedMin"])
	{
		static int i = 0;
		fElement.get_to<_float>(*((float*)&tInfo.vSpeedMin + i++));
	}

	for (auto fElement : jLoad["09. SpeedMax"])
	{
		static int i = 0;
		fElement.get_to<_float>(*((float*)&tInfo.vSpeedMax + i++));
	}

	for (auto fElement : jLoad["10. minPos"])
	{
		static int i = 0;
		fElement.get_to<_float>(*((float*)&tInfo.vMinPos + i++));
	}

	for (auto fElement : jLoad["11. maxPos"])
	{
		static int i = 0;
		fElement.get_to<_float>(*((float*)&tInfo.vMaxPos + i++));
	}

	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_VIBuffer_PtInstancing_S2"), TEXT("Com_VIBuffer")
		, (CComponent**)&m_pVIBufferCom, &tInfo)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Particle_Base::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_EffectS2"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_Effect"), TEXT("Com_DiffuseTexture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Particle_Base::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DepthTexture", pGameInstance->Get_DepthTargetSRV())))
	//	return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iTextureIndex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Particle_Base::SetUp_Buffer()
{
	/* For. Com_VIBuffer */
	CVIBuffer_Point_Instancing_S2::POINTINFO	tInfo;
	tInfo.eType = tInfo.TYPE_HAZE;
	tInfo.fTerm = 10.f;
	tInfo.fTermAcc = 0.f;
	tInfo.iNumInstance = 50;
	tInfo.vPSize = {0.1f, 0.1f};
	tInfo.vSpeedMin = { -0.1f, 0.1f, -0.1f };
	tInfo.vSpeedMax = { 0.1f, 0.5f, 0.1f };
	tInfo.vMinPos = { -3.0f, 0.0f, -3.0f };
	tInfo.vMaxPos = { 3.0f, 0.0f, 3.0f };

	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_VIBuffer_PtInstancing_S2"), TEXT("Com_VIBuffer")
		,(CComponent**)&m_pVIBufferCom, &tInfo)))
		return E_FAIL;

	return S_OK;
}

_float4 CEffect_Particle_Base::ColorCode()
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
	vSelectColor = m_vColor;

	if (ImGui::ColorPicker4("CurColor##4", (float*)&vSelectColor, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color ? &ref_color_v.x : NULL))
		m_vColor = vSelectColor;
	if (ImGui::ColorEdit4("Diffuse##2f", (float*)&vSelectColor, ImGuiColorEditFlags_DisplayRGB | misc_flags))
		m_vColor = vSelectColor;

	static _float fAlpha = 1.f; 
	fAlpha = m_vColor.w;
	if(ImGui::DragFloat("HDR Alpha", &fAlpha, 0.1f, 0.f, 10.f))
		m_vColor.w = (_float)fAlpha;

	return vSelectColor;
}


CEffect_Particle_Base * CEffect_Particle_Base::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffect_Particle_Base * pInstance = new CEffect_Particle_Base(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create: CEffect_Particle_Base");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Particle_Base::Clone(void * pArg)
{
	CEffect_Particle_Base * pInstance = new CEffect_Particle_Base(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CEffect_Particle_Base");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Particle_Base::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
