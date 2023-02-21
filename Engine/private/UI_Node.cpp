#include "stdafx.h"
#include "..\public\UI_Node.h"


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

	return S_OK;
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

		m_pTransformCom->Set_WorldMatrix(m_matLocal*matParentTrans);
		_matrix matWorld = m_pTransformCom->Get_WorldMatrix();

		_uint i = 0;

		//float fRatioX = matWorldParent._11 / m_matParentInit._11;
		//float fRatioY = matWorldParent._22 / m_matParentInit._22;

		//ImGui::SliderFloat3("position", position, -3000.f, 3000.f);
		//ImGui::SliderFloat3("scale", scale, 0.f, 100.f);

		//XMStoreFloat4x4(&m_matLocal,
		//	XMMatrixScaling(fRatioX*scale[0], fRatioY*scale[1], 1.f)*
		//	XMMatrixTranslation(position[0] + matWorldParent._41,
		//		position[1] + matWorldParent._42,
		//		position[2] + matWorldParent._43));


	}
}

HRESULT CUI_Node::Render()
{
	__super::Render();

	return S_OK;
}

void CUI_Node::Imgui_RenderProperty()
{
	//	m_pTransformCom->Imgui_RenderProperty();
	if (ImGui::CollapsingHeader("Local Transform"))
	{
		static float position[3] = { };
		static float scale[3] = { };




	}
}

void CUI_Node::Free()
{
	__super::Free();
}
