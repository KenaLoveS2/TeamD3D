#include "stdafx.h"
#include "..\public\Effect_Particle_Base.h"
#include "GameInstance.h"


CEffect_Particle_Base::CEffect_Particle_Base(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Base_S2(pDevice,pContext)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pVIBufferCom(nullptr)
{
}

CEffect_Particle_Base::CEffect_Particle_Base(const CEffect_Particle_Base & rhs)
	: CEffect_Base_S2(rhs)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pVIBufferCom(nullptr)
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

		/* RenderPass */
		static _int iRenderPass;
		iRenderPass = m_iRenderPass;
		const char* renderPass[3] = { "DefaultHaze", "BlackHaze", "BlackGather" };
		if (ImGui::ListBox("RenderPass", &iRenderPass, renderPass, 3, 5))
			m_iRenderPass = iRenderPass;

		/* Color */
		ColorCode();
	}

	/* Buffer Update */
	if (ImGui::CollapsingHeader(" > Update Buffer"))
	{
		if (m_pVIBufferCom == nullptr)
			return;

		CVIBuffer_Point_Instancing_S2::POINTINFO tInfo;
		using pointType = CVIBuffer_Point_Instancing_S2::POINTINFO::TYPE;
		ZeroMemory(&tInfo, sizeof(tInfo)); 
		tInfo = m_pVIBufferCom->Get_Info();

		/* Type */
		static _int iType = tInfo.eType;
		const char* list[3] = { "Haze", "Gather", "Parabola" };
		ImGui::ListBox("Type", &iType, list, 3, 5);
		tInfo.eType = (pointType)iType;

		/* NumInstance */
		static _int		iNumInstance = tInfo.iNumInstance;
		ImGui::DragInt("NumInstance", &iNumInstance, 1.f, 1, 1000);
		tInfo.iNumInstance = iNumInstance;

		/* Size */
		static _float2	vPSize = tInfo.vPSize;
		ImGui::DragFloat2("vPSize", (_float*)&vPSize, 0.10f, 0.0f, 10.0f, "%.3f");
		tInfo.vPSize = vPSize;

		/* Term */
		static _float	fTerm = tInfo.fTerm;
		ImGui::DragFloat("fTerm", &fTerm, 0.10f, 0.0f, 100.0f);
		tInfo.fTerm = fTerm;

		/* MinPos */
		static _float3  vMinPos = tInfo.vMinPos;
		ImGui::DragFloat3("vMinPos", (_float*)&vMinPos, 0.10f, -50.0f, 50.0f, "%.3f");
		tInfo.vMinPos = vMinPos;

		/* MaxPos */
		static _float3  vMaxPos = tInfo.vMaxPos;
		ImGui::DragFloat3("vMaxPos", (_float*)&vMaxPos, 0.10f, -50.0f, 50.0f, "%.3f");
		tInfo.vMaxPos = vMaxPos;

		/* SpeedMin */
		static _float3  vSpeedMin = tInfo.vSpeedMin;
		ImGui::DragFloat3("vSpeedMin", (_float*)&vSpeedMin, 0.10f, -10.0f, 10.0f, "%.3f");
		tInfo.vSpeedMin = vSpeedMin;

		/* SpeedMax */
		static _float3  vSpeedMax = tInfo.vSpeedMax;
		ImGui::DragFloat3("vSpeedMax", (_float*)&vSpeedMax, 0.10f, -10.0f, 10.0f, "%.3f");
		tInfo.vSpeedMax = vSpeedMax;

		static _float	fPlaySpeed = tInfo.fPlaySpeed;
		ImGui::DragFloat("fPlaySpeed", &fPlaySpeed, 0.10f, 0.0f, 50.0f, "%.3f");
		tInfo.fPlaySpeed = fPlaySpeed;


		if (ImGui::Button("Reset"))
		{
			CVIBuffer_Point_Instancing_S2::POINTINFO tInfo;
			using pointType = CVIBuffer_Point_Instancing_S2::POINTINFO::TYPE;
			ZeroMemory(&tInfo, sizeof(tInfo));
			tInfo = m_pVIBufferCom->Get_Info();

			iType = tInfo.eType;
			iNumInstance = tInfo.iNumInstance;
			vPSize = tInfo.vPSize;
			fTerm = tInfo.fTerm;
			vSpeedMin = tInfo.vSpeedMin;
			vSpeedMax = tInfo.vSpeedMax;
			vMinPos = tInfo.vMinPos;
			vMaxPos = tInfo.vMaxPos;
			fPlaySpeed = tInfo.fPlaySpeed;
		} 
		ImGui::SameLine(); ImGui::PushItemWidth(5);
		
		if (ImGui::Button("Update Confirm"))
		{
			if (FAILED(m_pVIBufferCom->Update_Buffer(&tInfo)))
				MSG_BOX("The Update Process ended unexpectedly. Please Check the Code.");
		}
	}

	ImGui::Separator();
	/* Position */
	{
		static _float4  vPosition = { 0.f, 0.f,0.f,1.f };
		vPosition = m_pTransformCom->Get_Position();
		if (ImGui::DragFloat3("vWorldPosition", (_float*)&vPosition, 0.10f, -1000.0f, 1000.0f, "%.3f"))
			m_pTransformCom->Set_Position(vPosition);
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

			_float4 vPos = m_pTransformCom->Get_Position();
			for (_uint i = 0; i < 4; ++i)
			{
				_float fElement = *((_float*)&vPos + i);
				json["00. Position"].push_back(fElement);
			}

			json["01. TextureIndex"] = m_iTextureIndex;
			json["02. RenderPass"] = m_iRenderPass;
			for (_uint i = 0; i < 4; ++i)
			{
				_float fElement = *((float*)&m_vColor + i);
				json["03. Color"].push_back(fElement);
			}
			json["04. HDR Intensity"] = m_fHDRIntensity;

			CVIBuffer_Point_Instancing_S2::POINTINFO tInfo;
			ZeroMemory(&tInfo, sizeof(tInfo));
			tInfo = m_pVIBufferCom->Get_Info();
			json["10. Type"] = (_int)tInfo.eType;
			json["11. NumInstance"] = tInfo.iNumInstance;
			json["12. Term"] = tInfo.fTerm;

			for (_uint i = 0; i < 2; ++i)
			{
				_float fElement = *((float*)&tInfo.vPSize + i);
				json["13. PSize"].push_back(fElement);
			}

			for (_uint i = 0; i < 3; ++i)
			{
				_float fElement = *((float*)&tInfo.vSpeedMin + i);
				json["14. SpeedMin"].push_back(fElement);
			}

			for (_uint i = 0; i < 3; ++i)
			{
				_float fElement = *((float*)&tInfo.vSpeedMax + i);
				json["15. SpeedMax"].push_back(fElement);
			}

			for (_uint i = 0; i < 3; ++i)
			{
				_float fElement = *((float*)&tInfo.vMinPos + i);
				json["16. minPos"].push_back(fElement);
			}

			for (_uint i = 0; i < 3; ++i)
			{
				_float fElement = *((float*)&tInfo.vMaxPos + i);
				json["17. maxPos"].push_back(fElement);
			}

			json["18. fPlaySpeed"] = tInfo.fPlaySpeed;

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

HRESULT CEffect_Particle_Base::Load_Data(_tchar* fileName)
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

	_float4 vPos = { 0.f,0.f,0.f,1.f };
	int i = 0;
	for (auto fElement : jLoad["00. Position"])
		fElement.get_to<_float>(*((_float*)&vPos + i++));
	m_pTransformCom->Set_Position(vPos);

	jLoad["01. TextureIndex"].get_to<_int>(m_iTextureIndex);
	jLoad["02. RenderPass"].get_to<_int>(m_iRenderPass);
	
	i = 0;
	for (auto fElement : jLoad["03. Color"])
		fElement.get_to<_float>(*((_float*)&m_vColor + i++));

	jLoad["04. HDR Intensity"].get_to<_float>(m_fHDRIntensity);

	CVIBuffer_Point_Instancing_S2::POINTINFO tInfo;
	using pointType = CVIBuffer_Point_Instancing_S2::POINTINFO::TYPE;
	ZeroMemory(&tInfo, sizeof(tInfo));

	_int iType = 0;
	jLoad["10. Type"].get_to<_int>(iType);
	tInfo.eType = (pointType)iType;
	jLoad["11. NumInstance"].get_to<_int>(tInfo.iNumInstance);
	jLoad["12. Term"].get_to<_float>(tInfo.fTerm);
	tInfo.fTermAcc = 0.f;

	i = 0;
	for (auto fElement : jLoad["13. PSize"])
		fElement.get_to<float>(*((float*)&tInfo.vPSize + i++));

	i = 0;
	for (auto fElement : jLoad["14. SpeedMin"])
		fElement.get_to<float>(*((float*)&tInfo.vSpeedMin + i++));

	i = 0;
	for (auto fElement : jLoad["15. SpeedMax"])
		fElement.get_to<float>(*((float*)&tInfo.vSpeedMax + i++));

	i = 0;
	for (auto fElement : jLoad["16. minPos"])
		fElement.get_to<float>(*((float*)&tInfo.vMinPos + i++));

	i = 0;
	for (auto fElement : jLoad["17. maxPos"])
		fElement.get_to<float>(*((float*)&tInfo.vMaxPos + i++));

	float fValue;
	jLoad["18. fPlaySpeed"].get_to<float>(fValue);

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
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_Effect_Particle_S2"), TEXT("Com_Shader"),
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_fHDRItensity", &m_fHDRIntensity, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Particle_Base::SetUp_Buffer()
{
	/* For. Com_VIBuffer */
	CVIBuffer_Point_Instancing_S2::POINTINFO	tInfo;
	ZeroMemory(&tInfo, sizeof(tInfo));
	tInfo.eType = tInfo.TYPE_HAZE;
	tInfo.fTerm = 10.0f;
	tInfo.fTermAcc = 0.0f;
	tInfo.iNumInstance = 50;
	tInfo.vPSize = {0.10f, 0.10f};
	tInfo.vSpeedMin = { -0.10f, 0.10f, -0.10f };
	tInfo.vSpeedMax = { 0.10f, 0.50f, 0.10f };
	tInfo.vMinPos = { -3.0f, 0.0f, -3.0f };
	tInfo.vMaxPos = { 3.0f, 0.0f, 3.0f };

	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_VIBuffer_PtInstancing_S2"), TEXT("Com_VIBuffer")
		,(CComponent**)&m_pVIBufferCom, &tInfo)))
		return E_FAIL;

	return S_OK;
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
