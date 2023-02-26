#include "stdafx.h"
#include "..\public\UI_Canvas.h"
#include "GameInstance.h"
#include "Json/json.hpp"
#include <fstream>
#include "Utile.h"

CUI_Canvas::CUI_Canvas(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI(pDevice, pContext)
	, m_bBindFinished(false)
	, m_iSelectedNode(-1)
{
}

CUI_Canvas::CUI_Canvas(const CUI_Canvas & rhs)
	:CUI(rhs)
	, m_bBindFinished(false)
	, m_iSelectedNode(-1)
{
}

HRESULT CUI_Canvas::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Canvas::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg == nullptr)
		return E_FAIL;

	UIDESC* tDesc = (UIDESC*)pArg;

	return Load_Data(tDesc->fileName);
}

void CUI_Canvas::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	for (auto e : m_vecEvents)
		e->Tick(fTimeDelta);

	for (auto node : m_vecNode)
		node->Tick(fTimeDelta);
}

void CUI_Canvas::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	for (auto e : m_vecEvents)
		e->Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && m_bActive)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

		/* Nodes added to RenderList After the canvas. */
		/* So It can be guaranteed that Canvas Draw first */
		for (auto node : m_vecNode)
			node->Late_Tick(fTimeDelta);
	}
}

HRESULT CUI_Canvas::Render()
{
	if (nullptr == m_pTextureCom[TEXTURE_DIFFUSE])
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
	{
		MSG_BOX("Failed To Setup ShaderResources : UI_Canvas");
		return E_FAIL;
	}

	m_pShaderCom->Begin(m_iRenderPass);
	m_pVIBufferCom->Render();

	return S_OK;
}

bool	getter_ForCanvas(void* data, int index, const char** output)
{
	vector<string>*	 pVec = (vector<string>*)data;
	*output = (*pVec)[index].c_str();
	return true;
}
void CUI_Canvas::Imgui_RenderProperty()
{
	if (ImGui::Button("Save"))
		Save_Data();

	/* Translation Setting */
	m_pTransformCom->Imgui_RenderProperty();

	/* Texture Setting */
	if (ImGui::CollapsingHeader("Texture"))
		Imgui_RenderingSetting();

	if (m_vecNode.empty())
		return;

	/* Node Setting */
	if (ImGui::CollapsingHeader("Nodes"))
	{
		static int selected_Node = 0;
		_uint iNumNodes = (_uint)m_vecNodeCloneTag.size();
		if (ImGui::ListBox(" : Node", &selected_Node, getter_ForCanvas, &m_vecNodeCloneTag, iNumNodes, 5))
			m_iSelectedNode = selected_Node;

		m_vecNode[selected_Node]->Imgui_RenderProperty();	
	}

}

HRESULT CUI_Canvas::Save_Data()
{
	Json	json;

	_smatrix matWorld = m_pTransformCom->Get_WorldMatrix();
	_float fValue = 0.f;
	for (int i = 0; i < 16; ++i)
	{
		fValue = 0.f;
		memcpy(&fValue, (float*)&matWorld + i, sizeof(float));
		json["worldMatrix"].push_back(fValue);
	}

	json["renderPass"] = m_iRenderPass;
	
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



	wstring filePath = L"../Bin/Data/UI/";
	filePath += this->Get_ObjectCloneName();
	filePath += L"_Property.json";

	string fileName;
	fileName = fileName.assign(filePath.begin(), filePath.end());

	ofstream	file(fileName);
	file << json;
	file.close();

	for (auto node : m_vecNode)
		node->Save_Data();

	MSG_BOX("Save Complete");

	return S_OK;
}

HRESULT CUI_Canvas::Load_Data(wstring fileName)
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


	jLoad["DiffuseTextureIndex"].get_to<_int>(m_TextureListIndices[TEXTURE_DIFFUSE]);
	jLoad["MaskTextureIndex"].get_to<_int>(m_TextureListIndices[TEXTURE_MASK]);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	vector<wstring>* pTags = pGameInstance->Get_UIWString(CUI_Manager::WSTRKEY_TEXTURE_PROTOTAG);
	RELEASE_INSTANCE(CGameInstance);


	if (-1 != m_TextureListIndices[TEXTURE_DIFFUSE])
	{
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(),
			(*pTags)[m_TextureListIndices[TEXTURE_DIFFUSE]].c_str(), m_TextureComTag[TEXTURE_DIFFUSE].c_str(),
			(CComponent**)&m_pTextureCom[0])))
			return S_OK;
	}
	if (-1 != m_TextureListIndices[TEXTURE_MASK])
	{
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(),
			(*pTags)[m_TextureListIndices[TEXTURE_MASK]].c_str(), m_TextureComTag[TEXTURE_MASK].c_str(),
			(CComponent**)&m_pTextureCom[1])))
			return S_OK;
	}


	int i = 0;
	_float4x4	matWorld;
	for (float fElement : jLoad["worldMatrix"])
		memcpy(((float*)&matWorld) + (i++), &fElement, sizeof(float));
	m_pTransformCom->Set_WorldMatrix_float4x4(matWorld);

	return S_OK;
}

HRESULT CUI_Canvas::Add_Node(CUI * pUI)
{
	if (nullptr == pUI)
		return E_FAIL;

	m_vecNode.push_back(pUI);
	// Safe_AddRef(pUI); 

	pUI->Set_Parent(this);

	return S_OK;
}

void CUI_Canvas::Free()
{
	__super::Free();

	//for (auto &pChildUI : m_vecChildUI)
	//	Safe_Release(pChildUI);
	m_vecNodeCloneTag.clear();

	for (auto node : m_vecNode)
		Safe_Release(node);
	m_vecNode.clear();
}
