#include "stdafx.h"
#include "..\public\UI_NodePlayerSkill.h"
#include "GameInstance.h"
#include "Json/json.hpp"
#include <fstream>

CUI_NodePlayerSkill::CUI_NodePlayerSkill(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
	, m_iLevel{ 0 }
	, m_eState{ STATE_BLOCKED }
	, m_fDiffuseAlpha(0.f)
	, m_fMaskAlpha(0.f)
{
}

CUI_NodePlayerSkill::CUI_NodePlayerSkill(const CUI_NodePlayerSkill & rhs)
	: CUI_Node(rhs)
	, m_iLevel{ 0 }
	, m_eState{ STATE_BLOCKED }
	, m_fDiffuseAlpha(0.f)
	, m_fMaskAlpha(0.f)
{
}

HRESULT CUI_NodePlayerSkill::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodePlayerSkill::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(100, 100.f, 1.f));
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components : nodeplayerskill");
		return E_FAIL;
	}

	m_bActive = true;
	return S_OK;
}

HRESULT CUI_NodePlayerSkill::Late_Initialize(void * pArg)
{
	return S_OK;
}

void CUI_NodePlayerSkill::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CUI_NodePlayerSkill::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodePlayerSkill::Render()
{
	if (nullptr == m_pTextureCom[TEXTURE_DIFFUSE])
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
	{
		MSG_BOX("Failed To Setup ShaderResources : UI_HUDHPBar");
		return E_FAIL;
	}

	m_pShaderCom->Begin(m_iRenderPass);
	m_pVIBufferCom->Render();
	return S_OK;
}

void CUI_NodePlayerSkill::BackToOriginal()
{
	m_matLocal = m_matLocalOriginal;
}

void CUI_NodePlayerSkill::Picked(_float fRatio)
{
	m_matLocal._11 *= fRatio;
	m_matLocal._22 *= fRatio;
}

void CUI_NodePlayerSkill::State_Change(_uint eState)
{
	switch (eState)
	{
	case STATE_BLOCKED:
		if (m_iLevel == 0)
		{
			m_fDiffuseAlpha = 0.f;
			m_fMaskAlpha = 1.f;
		}
		else
		{
			m_fDiffuseAlpha = 1.f;
			m_fMaskAlpha = 1.f;
		}
		break;
	case STATE_LOCKED:

		break;
	case STATE_UNLOCKED:
		m_fDiffuseAlpha = 1.f;
		m_fMaskAlpha = 0.f;
		break;
	}
}

HRESULT CUI_NodePlayerSkill::Save_Data()
{
	Json	json;

	_smatrix matWorld = m_matLocalOriginal;
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

HRESULT CUI_NodePlayerSkill::Load_Data(wstring fileName)
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

	jLoad["MaskTextureIndex"].get_to<_int>(m_TextureListIndices[TEXTURE_MASK]);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	vector<wstring>* pTags = pGameInstance->Get_UIWString(CUI_Manager::WSTRKEY_TEXTURE_PROTOTAG);
	RELEASE_INSTANCE(CGameInstance);

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
	m_matLocalOriginal = matLocal;

	m_vOriginalSettingScale = m_pTransformCom->Get_Scaled();

	return S_OK;
}

HRESULT CUI_NodePlayerSkill::Setting(_tchar * textureProtoTag, _uint iLevel)
{
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), textureProtoTag, m_TextureComTag[TEXTURE_DIFFUSE].c_str(), (CComponent**)&m_pTextureCom[TEXTURE_DIFFUSE]))
		return E_FAIL;

	m_iLevel = iLevel;

	return S_OK;
}

HRESULT CUI_NodePlayerSkill::SetUp_Components()
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

HRESULT CUI_NodePlayerSkill::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_tDesc.ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_tDesc.ProjMatrix)))
		return E_FAIL;

	if (m_pTextureCom[TEXTURE_DIFFUSE] != nullptr)
	{
		if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iLevel)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fDiffuseAlpha", &m_fDiffuseAlpha, sizeof(_float))))
			return E_FAIL;
	}

 	if (m_pTextureCom[TEXTURE_MASK] != nullptr)
	{
		if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fMaskAlpha", &m_fMaskAlpha, sizeof(_float))))
			return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CUI_NodePlayerSkill::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	ImGui::Separator();



}

CUI_NodePlayerSkill * CUI_NodePlayerSkill::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodePlayerSkill*	pInstance = new CUI_NodePlayerSkill(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodePlayerSkill");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodePlayerSkill::Clone(void * pArg)
{
	CUI_NodePlayerSkill*	pInstance = new CUI_NodePlayerSkill(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodePlayerSkill");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodePlayerSkill::Free()
{
	__super::Free();
}
