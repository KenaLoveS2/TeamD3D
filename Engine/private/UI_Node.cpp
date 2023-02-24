#include "stdafx.h"
#include "..\public\UI_Node.h"
#include "GameInstance.h"
#include "Json/json.hpp"
#include <fstream>

CUI_Node::CUI_Node(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI(pDevice, pContext)
{
}

CUI_Node::CUI_Node(const CUI_Node & rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Node::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Node::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg == nullptr)
		return E_FAIL;

	UIDESC* tDesc = (UIDESC*)pArg;

	return Load_Data(tDesc->fileName);
}

void CUI_Node::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Node::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	/* Calculate with Parent(Canvas) WorldMatrix (Scale, Translation) */
	if (m_pParent != nullptr)
	{
		_float4x4 matWorldParent;
		XMStoreFloat4x4(&matWorldParent, m_pParent->Get_WorldMatrix());

		_matrix matParentTrans = XMMatrixTranslation(matWorldParent._41, matWorldParent._42, matWorldParent._43);

		float fRatioX = matWorldParent._11 / m_matParentInit._11;
		float fRatioY = matWorldParent._22 / m_matParentInit._22;
		_matrix matParentScale = XMMatrixScaling(fRatioX, fRatioY, 1.f);

		_matrix matWorld = matParentScale*m_matLocal*matParentTrans;
		m_pTransformCom->Set_WorldMatrix(matWorld);
	}
}

HRESULT CUI_Node::Render()
{
	__super::Render();

	return S_OK;
}

bool	Getter_forNode(void* data, int index, const char** output)
{
	vector<string>*	 pVec = (vector<string>*)data;
	*output = (*pVec)[index].c_str();
	return true;
}
void CUI_Node::Imgui_RenderProperty()
{
	ImGui::Separator();
	
	//if (ImGui::Button("Save Node"))
	//	Save_Data();

	/* Translation Setting */
	ImGui::Text("Local Matrix");
	static float position[3]; // = { m_matLocal._41, m_matLocal._42, m_matLocal._43 };
	static float scale[3]; // = { m_matLocal._11, m_matLocal._22, m_matLocal._33 };

	position[0] = m_matLocal._41;
	position[1] = m_matLocal._42;
	position[2] = m_matLocal._43;

	scale[0] = m_matLocal._11;
	scale[1] = m_matLocal._22;
	scale[2] = m_matLocal._33;

	ImGui::SliderFloat3(" position", position, -3000.f, 3000.f);
	ImGui::SliderFloat3(" scale", scale, 0.f, 3000.f);


	m_matLocal._41 = position[0];
	m_matLocal._42 = position[1];
	m_matLocal._43 = position[2];

	m_matLocal._11 = scale[0];
	m_matLocal._22 = scale[1];
	m_matLocal._33 = scale[2];
	ImGui::Separator();

	/* Rendering Setting */
	Imgui_RenderingSetting();


}

HRESULT CUI_Node::Save_Data()
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

	json["renderPass"] = m_iRenderPass;

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

HRESULT CUI_Node::Load_Data(wstring fileName)
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

	jLoad["renderPass"].get_to<_uint>(m_iRenderPass);

	int i = 0;
	_float4x4	matLocal;
	for (float fElement : jLoad["localMatrix"])
		memcpy(((float*)&matLocal) + (i++), &fElement, sizeof(float));

	this->Set_LocalMatrix(matLocal);

	return S_OK;
}

void CUI_Node::Free()
{
	__super::Free();
}
