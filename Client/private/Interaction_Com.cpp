#include "stdafx.h"
#include "..\public\Interaction_Com.h"
#include "GameInstance.h"

CInteraction_Com::CInteraction_Com(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CInteraction_Com::CInteraction_Com(const CInteraction_Com & rhs)
	: CComponent(rhs)
{
}

HRESULT CInteraction_Com::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteraction_Com::Initialize(void * pArg, CGameObject * pOwner)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg, pOwner), E_FAIL);

	return S_OK;
}

void CInteraction_Com::Imgui_RenderProperty()
{
	if (ImGui::CollapsingHeader("Interaction_Com"))
	{
		ImGui::InputInt("Interaction_Option", &m_iMgui_InterOption);
		if (m_iMgui_InterOption < 0)
			m_iMgui_InterOption = 0;

		string strCurOp = "";
		if (m_iMgui_InterOption == INTERATIONCTION_TYPE_TEST1)
			strCurOp = "TYPE_TEST1";
		else 	if (m_iMgui_InterOption == INTERATIONCTION_TYPE_TEST2)
			strCurOp = "TYPE_TEST2";
		else 	if (m_iMgui_InterOption == INTERATIONCTION_TYPE_TEST3)
			strCurOp = "TYPE_TEST3";
		else
			strCurOp = "TYPE_NONE";

		ImGui::Text(strCurOp.c_str());

		if (m_iMgui_InterOption >= _int(INTERATIONCTION_TYPE_END))
			m_iMgui_InterOption = _int(INTERATIONCTION_TYPE_END);		// exp) MaxNum!!

		m_InteractionDesc.eType = static_cast<INTERATIONCTION_TYPE>(m_iMgui_InterOption);
	}
}

CInteraction_Com * CInteraction_Com::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CInteraction_Com*		pInstance = new CInteraction_Com(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInteraction_Com");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CInteraction_Com::Clone(void * pArg, CGameObject * pOwner)
{
	CInteraction_Com*		pInstance = new CInteraction_Com(*this);

	if (FAILED(pInstance->Initialize(pArg, pOwner)))
	{
		MSG_BOX("Failed to Cloned : CInteraction_Com");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInteraction_Com::Free()
{
	__super::Free();
}
