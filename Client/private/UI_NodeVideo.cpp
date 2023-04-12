#include "stdafx.h"
#include "UI_NodeVideo.h"
#include "GameInstance.h"

CUI_NodeVideo::CUI_NodeVideo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Node(pDevice, pContext)
	, m_bEnd(false)
	, m_fTimeAcc(0.0f)
	, m_fTime(0.0f)
	, m_bLoop(false)
{
}

CUI_NodeVideo::CUI_NodeVideo(const CUI_NodeVideo& rhs)
	: CUI_Node(rhs)
	, m_bEnd(false)
	, m_fTimeAcc(0.0f)
	, m_fTime(0.0f)
	, m_bLoop(false)
{
}

void CUI_NodeVideo::Play_Video(wstring wstrVideo, _bool bLoop, _float fTime)
{
	m_bActive = true;
	m_bEnd = false;
	m_iTextureIdx = 0;
	m_fTimeAcc = 0.0f;

	/* Todo */
	/* Change The Video */
	if (m_pTextureCom[TEXTURE_DIFFUSE] != nullptr)
	{
		Delete_Component(m_wstrCurVideo.c_str());
		Safe_Release(m_pTextureCom[TEXTURE_DIFFUSE]);
	}

	m_wstrCurVideo = L"Prototype_Component_Texture_" + wstrVideo;
	
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), m_wstrCurVideo.c_str(), CUtile::Create_DummyString(),
		(CComponent**)&m_pTextureCom[TEXTURE_DIFFUSE])))
	{
		MSG_BOX("Video Failed");
		return;
	}

	m_bLoop = bLoop;
	m_fTime = fTime;
}

HRESULT CUI_NodeVideo::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeVideo::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(500.f, 300.f, 1.f));
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}


	/* Temp */
	m_bActive = true;
	//m_pTransformCom->Set_Position(XMVectorSet(1600.f, 1600.f, 1.f, 1.f));

	return S_OK;
}

void CUI_NodeVideo::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;
	//m_iRenderPass = 0;
	if (nullptr == m_pTextureCom[TEXTURE_DIFFUSE])
		return;

	if (m_bEnd)
		return;

	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc > m_fTime)
	{
		m_iTextureIdx++;
		m_fTimeAcc = 0.0f;

		if (m_iTextureIdx >= m_pTextureCom[TEXTURE_DIFFUSE]->Get_TextureIdx() && m_bLoop)
		{
			if (m_bLoop)
				m_iTextureIdx = 0;
			else
				m_bEnd = true;
		}
	}

	__super::Tick(fTimeDelta);
}

void CUI_NodeVideo::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeVideo::Render()
{
	if (m_pTextureCom[TEXTURE_DIFFUSE] == nullptr)
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CUI_NodeVideo::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	ImGui::Separator();

	ImGui::DragFloat("Time Interval", &m_fTime, 0.1f, 0.0f, 30.f);
	ImGui::Checkbox("IsLoop?", &m_bLoop);

}

//HRESULT CUI_NodeVideo::Save_Data()
//{
//	Json	json;
//
//	_smatrix matWorld = m_matLocal;
//	_float fValue = 0.f;
//	for (int i = 0; i < 16; ++i)
//	{
//		fValue = 0.f;
//		memcpy(&fValue, (float*)&matWorld + i, sizeof(float));
//		json["localMatrix"].push_back(fValue);
//	}
//
//	json["renderPass"] = m_iOriginalRenderPass;
//
//	_int iIndex;
//	if (m_pTextureCom[TEXTURE_DIFFUSE] != nullptr)
//		iIndex = m_TextureListIndices[TEXTURE_DIFFUSE];
//	else
//		iIndex = -1;
//	json["DiffuseTextureIndex"] = iIndex;
//
//	if (m_pTextureCom[TEXTURE_MASK] != nullptr)
//		iIndex = m_TextureListIndices[TEXTURE_MASK];
//	else
//		iIndex = -1;
//	json["MaskTextureIndex"] = iIndex;
//
//
//	json["Frame Interval"] = m_fTime;
//	json["Loop"] = m_bLoop;
//
//	for (auto e : m_vecEvents)
//		e->Save_Data(&json);
//
//	//vector<wstring>* list = CGameInstance::GetInstance()->Get_UIWString(CUI_Manager::WSTRKEY_ELSE);
//	wstring filePath = L"../Bin/Data/UI/";
//	filePath += this->Get_ObjectCloneName();
//	//filePath += this->Get_Name();
//	filePath += L"_Property.json";
//
//	string fileName;
//	fileName = CUtile::wstring_to_utf8(filePath);// fileName.assign(filePath.begin(), filePath.end());
//
//	ofstream	file(fileName);
//	file << json;
//	file.close();
//
//
//	return S_OK;
//}
//
//HRESULT CUI_NodeVideo::Load_Data(wstring fileName)
//{
//	Json	jLoad;
//
//	wstring name = L"../Bin/Data/UI/";
//	name += fileName;
//	name += L"_Property.json";
//	string filePath = CUtile::wstring_to_utf8(name);
//	//filePath.assign(name.begin(), name.end());
//
//	ifstream file(filePath);
//	if (file.fail())
//		return E_FAIL;
//	file >> jLoad;
//	file.close();
//
//	jLoad["renderPass"].get_to<_uint>(m_iRenderPass);
//	m_iOriginalRenderPass = m_iRenderPass;
//
//	int i = 0;
//	_float4x4	matLocal;
//	for (float fElement : jLoad["localMatrix"])
//		memcpy(((float*)&matLocal) + (i++), &fElement, sizeof(float));
//
//	this->Set_LocalMatrix(matLocal);
//
//	m_vOriginalSettingScale = m_pTransformCom->Get_Scaled();
//
//	m_matLocalOriginal = m_matLocal;
//
//	if (jLoad.contains("Frame Interval"))
//		jLoad["Frame Interval"].get_to<_float>(m_fTime);
//
//	if (jLoad.contains("Loop"))
//		jLoad["Loop"].get_to<_bool>(m_bLoop);
//
//	return S_OK;
//}

HRESULT CUI_NodeVideo::SetUp_Components()
{
	/* Renderer */
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom))
		return E_FAIL;

	/* Shader */
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom))
		return E_FAIL;

	/* VIBuffer_Rect */
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeVideo::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CUI::SetUp_ShaderResources(); /* Events Resourece Setting */

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_tDesc.ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_tDesc.ProjMatrix)))
		return E_FAIL;

	if (m_pTextureCom[TEXTURE_DIFFUSE] != nullptr)
	{
		if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIdx)))
			return E_FAIL;
	}

	if (m_pTextureCom[TEXTURE_MASK] != nullptr)
	{
		if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			return E_FAIL;
	}

	return S_OK;
}

CUI_NodeVideo* CUI_NodeVideo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_NodeVideo* pInstance = new CUI_NodeVideo(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeVideo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_NodeVideo::Clone(void* pArg)
{
	CUI_NodeVideo* pInstance = new CUI_NodeVideo(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeVideo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeVideo::Free()
{
	__super::Free();
}
