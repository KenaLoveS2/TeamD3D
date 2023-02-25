#include "stdafx.h"
#include "..\public\UI_Canvas.h"
#include "GameInstance.h"
#include "Json/json.hpp"
#include <fstream>
#include "Utile.h"

CUI_Canvas::CUI_Canvas(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI(pDevice, pContext)
	, m_bBindFinished(false)
{
}

CUI_Canvas::CUI_Canvas(const CUI_Canvas & rhs)
	:CUI(rhs)
	, m_bBindFinished(false)
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
}

void CUI_Canvas::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	for (auto e : m_vecEvents)
		e->Late_Tick(fTimeDelta);
}

HRESULT CUI_Canvas::Render()
{
	__super::Render();

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

	/* Node Setting */
	if (ImGui::CollapsingHeader("Nodes"))
	{
		static int selected_Node = 0;
		_uint iNumNodes = (_uint)m_vecNodeCloneTag.size();
		ImGui::ListBox(" : Node", &selected_Node, getter_ForCanvas, &m_vecNodeCloneTag, iNumNodes, 5);

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
