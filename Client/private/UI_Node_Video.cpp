#include "stdafx.h"
#include "UI_Node_Video.h"
#include "GameInstance.h"

CUI_Node_Video::CUI_Node_Video(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Node(pDevice, pContext)
	, m_bEnd(false)
	, m_fTimeAcc(0.0f)
	, m_fTime(0.0f)
	, m_bLoop(false)
{
}

CUI_Node_Video::CUI_Node_Video(const CUI_Node_Video& rhs)
	: CUI_Node(rhs)
	, m_bEnd(false)
	, m_fTimeAcc(0.0f)
	, m_fTime(0.0f)
	, m_bLoop(false)
{
}

void CUI_Node_Video::Play_Video()
{
	m_bActive = true;
	m_bEnd = false;
	m_iTextureIdx = 0;
	m_fTimeAcc = 0.0f;

	/* Todo */
	/* 어떤 콜이 들어왔냐에 따라 비디오(텍스처)가 바뀌어야 함. */
}

HRESULT CUI_Node_Video::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Node_Video::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(100.f, 100.f, 1.f));
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}

	m_bActive = true;

	/* basically it should be save and load */



	return S_OK;
}

void CUI_Node_Video::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

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
			m_iTextureIdx = 0;
		else
			m_bEnd = true;
	}

	__super::Tick(fTimeDelta);
}

void CUI_Node_Video::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Node_Video::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CUI_Node_Video::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	ImGui::Separator();

	ImGui::DragFloat("Time Interval", &m_fTime, 0.1f, 0.0f, 30.f);
	ImGui::Checkbox("IsLoop?", &m_bLoop);

}

HRESULT CUI_Node_Video::Save_Data()
{
	Json	json;

	_smatrix matWorld = m_matLocal;
	_float fValue = 0.f;
	for (int i = 0; i < 16; ++i)
	{
		fValue = 0.f;
		memcpy(&fValue, (float*)&matWorld + i, sizeof(float));
		json["localMatrix"].push_back(fValue);
	}

	json["renderPass"] = m_iOriginalRenderPass;

	_int iIndex;
	if (m_pTextureCom[TEXTURE_DIFFUSE] != nullptr)
		iIndex = m_TextureListIndices[TEXTURE_DIFFUSE];
	else
		iIndex = -1;
	json["DiffuseTextureIndex"] = iIndex;

	if (m_pTextureCom[TEXTURE_MASK] != nullptr)
		iIndex = m_TextureListIndices[TEXTURE_MASK];
	else
		iIndex = -1;
	json["MaskTextureIndex"] = iIndex;


	json["Frame Interval"] = m_fTime;
	json["Loop"] = m_bLoop;

	for (auto e : m_vecEvents)
		e->Save_Data(&json);

	//vector<wstring>* list = CGameInstance::GetInstance()->Get_UIWString(CUI_Manager::WSTRKEY_ELSE);
	wstring filePath = L"../Bin/Data/UI/";
	filePath += this->Get_ObjectCloneName();
	//filePath += this->Get_Name();
	filePath += L"_Property.json";

	string fileName;
	fileName = fileName.assign(filePath.begin(), filePath.end());

	ofstream	file(fileName);
	file << json;
	file.close();

	return S_OK;
}

HRESULT CUI_Node_Video::Load_Data(wstring fileName)
{
	Json	jLoad;

	wstring name = L"../Bin/Data/UI/";
	name += fileName;
	name += L"_Property.json";
	string filePath;
	filePath.assign(name.begin(), name.end());

	ifstream file(filePath);
	if (file.fail())
		return E_FAIL;
	file >> jLoad;
	file.close();

	jLoad["renderPass"].get_to<_uint>(m_iRenderPass);
	m_iOriginalRenderPass = m_iRenderPass;

	jLoad["DiffuseTextureIndex"].get_to<_int>(m_TextureListIndices[TEXTURE_DIFFUSE]);
	jLoad["MaskTextureIndex"].get_to<_int>(m_TextureListIndices[TEXTURE_MASK]);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	vector<wstring>* pTags = pGameInstance->Get_UIWString(CUI_Manager::WSTRKEY_TEXTURE_PROTOTAG);
	RELEASE_INSTANCE(CGameInstance);

	if (-1 != m_TextureListIndices[TEXTURE_DIFFUSE])
	{
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(),
			(*pTags)[m_TextureListIndices[TEXTURE_DIFFUSE]].c_str(), TEXT("Com_DiffuseTexture"),
			(CComponent**)&m_pTextureCom[0])))
			return S_OK;
	}
	if (-1 != m_TextureListIndices[TEXTURE_MASK])
	{
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(),
			(*pTags)[m_TextureListIndices[TEXTURE_MASK]].c_str(), TEXT("Com_MaskTexture"),
			(CComponent**)&m_pTextureCom[1])))
			return S_OK;
	}

	int i = 0;
	_float4x4	matLocal;
	for (float fElement : jLoad["localMatrix"])
		memcpy(((float*)&matLocal) + (i++), &fElement, sizeof(float));

	this->Set_LocalMatrix(matLocal);

	m_vOriginalSettingScale = m_pTransformCom->Get_Scaled();

	m_matLocalOriginal = m_matLocal;

	if (jLoad.contains("Frame Interval"))
		jLoad["Frame Interval"].get_to<_float>(m_fTime);

	if (jLoad.contains("Loop"))
		jLoad["Loop"].get_to<_bool>(m_bLoop);

	return S_OK;
}

HRESULT CUI_Node_Video::SetUp_Components()
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

HRESULT CUI_Node_Video::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

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


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CUI_Node_Video* CUI_Node_Video::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Node_Video* pInstance = new CUI_Node_Video(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Node_Video");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Node_Video::Clone(void* pArg)
{
	CUI_Node_Video* pInstance = new CUI_Node_Video(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Node_Video");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Node_Video::Free()
{
	__super::Free();
}
