#include "stdafx.h"
#include "..\public\ControlMove.h"
#include "GameInstance.h"

CControlMove::CControlMove(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CControlMove::CControlMove(const CControlMove & rhs)
	: CComponent(rhs)
{
}

HRESULT CControlMove::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CControlMove::Initialize(void * pArg, CGameObject * pOwner)
{
	ZeroMemory(&m_CtrlMove_Desc, sizeof(m_CtrlMove_Desc));
	if (pArg != nullptr)
		memcpy(&m_CtrlMove_Desc, pArg, sizeof(m_CtrlMove_Desc));

	FAILED_CHECK_RETURN(__super::Initialize(pArg, pOwner), E_FAIL);

	return S_OK;
}

void CControlMove::Imgui_RenderProperty()
{
	if (ImGui::CollapsingHeader("ControlMoveCom"))
	{
		ImGui::InputInt("Option", &m_iMgui_Option); 
	
		if (m_iMgui_Option < 0)
			m_iMgui_Option = 0;

		string strCurOp = "";
		if (m_iMgui_Option == MOVE_TYPE_WALL)
			strCurOp = "TYPE_WALL";
		else 	if (m_iMgui_Option == MOVE_TYPE_LAND)
			strCurOp = "TYPE_LAND";
		else 	if (m_iMgui_Option == MOVE_TYPE_PARKOUR)
			strCurOp = "TYPE_PARKOUR";
		else
			strCurOp = "MOVE_TYPE_NONE";
	
		ImGui::Text(strCurOp.c_str());

		if (m_iMgui_Option >= _int(MOVE_TYPE_END))
			m_iMgui_Option = _int(MOVE_TYPE_END);		// exp) MaxNum!!

		m_CtrlMove_Desc.eType = static_cast<CONTROL_MOVE_TYPE>(m_iMgui_Option);
	}
}

CControlMove * CControlMove::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CControlMove*		pInstance = new CControlMove(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CControlMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CControlMove::Clone(void * pArg, CGameObject * pOwner)
{
	CControlMove*		pInstance = new CControlMove(*this);

	if (FAILED(pInstance->Initialize(pArg, pOwner)))
	{
		MSG_BOX("Failed to Cloned : CControlMove");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CControlMove::Free()
{
	__super::Free();
}

