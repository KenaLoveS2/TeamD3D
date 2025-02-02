#include "stdafx.h"
#include "..\public\UI_NodeQuestMain.h"
#include "GameInstance.h"
#include "Json/json.hpp"
#include <fstream>
#include <codecvt>
#include <locale>

CUI_NodeQuestMain::CUI_NodeQuestMain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice,pContext)
{
}

CUI_NodeQuestMain::CUI_NodeQuestMain(const CUI_NodeQuestMain & rhs)
	:CUI_Node(rhs)
{
}

HRESULT CUI_NodeQuestMain::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeQuestMain::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(400.f, 24.f, 1.f));
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}

	m_bActive = true;
	return S_OK;
}

void CUI_NodeQuestMain::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Tick(fTimeDelta);

}

void CUI_NodeQuestMain::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeQuestMain::Render()
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

HRESULT CUI_NodeQuestMain::SetUp_Components()
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

HRESULT CUI_NodeQuestMain::SetUp_ShaderResources()
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
		if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
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

HRESULT CUI_NodeQuestMain::Save_Data()
{
	Json	json;

	//std::string std = "한글테스트";
	//std::wstring_convert<codecvt_utf8_utf16<int16_t>, int16_t> convert;
	//auto p = reinterpret_cast<const int16_t*>(std.data());
	//json["test"] = convert.to_bytes(p, p + std.size());

	//Json	jString = {
	//	{"name", "John"},
	//	{"what", "fckyou"}
	//};

	/* convert wstring to string */
	//using convert_type = codecvt_utf8<wchar_t>;
	//wstring wstr = L"한글테스트";
	//wstring_convert<convert_type> utf8_conv;
	//json["test"] = utf8_conv.to_bytes(wstr);
	//json.dump();
	/* ~ convert wstring to string */


	using convert_type = codecvt_utf8<wchar_t>;

	wstring main = L"타로를 구하세요";
	wstring_convert<convert_type> utf8_conv;
	json["mainQuest"]; // = utf8_conv.to_bytes(main);
	//json.dump();

	Json jSub;
	wstring sub1 = L"신사를 정화하세요.";
	wstring sub2 = L"오염된 숲을 정화하세요.";
	json["sub1"] = utf8_conv.to_bytes(sub1);
	json.dump();
	json["sub2"] = utf8_conv.to_bytes(sub2);
	json.dump();
	json["mainQuest"].push_back(jSub);




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

	for (auto e : m_vecEvents)
		e->Save_Data(&json);

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

HRESULT CUI_NodeQuestMain::Load_Data(wstring fileName)
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



	/* convert string to wstring */
	//string test;
	//jLoad["test"].get_to<string>(test);
	//using convert_type = codecvt_utf8<wchar_t>;
	//wstring_convert<convert_type> utf8_conv;
	//wstring wstr = utf8_conv.from_bytes(test);
	/* ~ convert string to wstring */

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



	return S_OK;
}

CUI_NodeQuestMain * CUI_NodeQuestMain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeQuestMain*	pInstance = new CUI_NodeQuestMain(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeQuestMain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeQuestMain::Clone(void * pArg)
{
	CUI_NodeQuestMain*	pInstance = new CUI_NodeQuestMain(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeQuestMain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeQuestMain::Free()
{
	__super::Free();
}
