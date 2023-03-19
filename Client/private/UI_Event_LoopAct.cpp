#include "stdafx.h"
#include "..\public\UI_Event_LoopAct.h"
#include "GameInstance.h"
#include "UI.h"

CUI_Event_LoopAct::CUI_Event_LoopAct()
	: m_eType(TYPE_END)
	, m_fDistance(0.f)
	, m_fSpeed(0.f)
{
	m_szEventName = "Fade";
	m_iRenderPass = 1;
}

void CUI_Event_LoopAct::Call_Event(_bool bStart)
{
	m_fTimeAcc = 0.0f;
	m_fTime = 0.5f;
}

HRESULT CUI_Event_LoopAct::Tick(_float fTimeDelta)
{
	//switch (m_eType)
	//{

	//}



 	return S_OK;
}

HRESULT CUI_Event_LoopAct::Late_Tick(_float fTimeDelta)
{
	switch (m_eType)
	{
	case TYPE_LOOP_VRT:
		Act_Loop_Verticle(fTimeDelta);
		break;
	}


	return S_OK;
}

HRESULT CUI_Event_LoopAct::SetUp_ShaderResources(CShader * pShader)
{
	return S_OK;
}

void CUI_Event_LoopAct::Imgui_RenderProperty()
{
}

HRESULT CUI_Event_LoopAct::Save_Data(Json * json)
{
	return S_OK;
}

HRESULT CUI_Event_LoopAct::Load_Data(wstring fileName)
{
	return S_OK;
}

void CUI_Event_LoopAct::Initialize(CUI* pParent, TYPE eType, _float fDistance, _float fSpeed)
{
	m_pParent = pParent;
	m_eType = eType;
	m_fDistance = fDistance;
	m_fSpeed = fSpeed;
}

void CUI_Event_LoopAct::Act_Loop_Verticle(_float fTimeDelta)
{
	if (m_pParent == nullptr)
		return;

	//m_fSpeed = 4.f;
	m_fTimeAcc += fTimeDelta;
	_float4x4 matParent = m_pParent->Get_TransformCom()->Get_WorldMatrixFloat4x4();
	//matParent._42 += m_fTimeAcc;
	//if (fabsf(m_fTimeAcc) > m_fDistance)
	//	m_fSpeed = -m_fSpeed;
	matParent._42 += m_fDistance * sinf(m_fSpeed * m_fTimeAcc);
	m_pParent->Set_WorldMatrix(matParent);

}

CUI_Event_LoopAct * CUI_Event_LoopAct::Create()
{
	CUI_Event_LoopAct* pInstance = new CUI_Event_LoopAct();
	return pInstance;
}

CUI_Event_LoopAct * CUI_Event_LoopAct::Create(CUI* pParent, TYPE eType, _float fDistance, _float fSpeed)
{
	CUI_Event_LoopAct* pInstance = new CUI_Event_LoopAct();
	if (pInstance != nullptr)
		pInstance->Initialize(pParent, eType, fDistance, fSpeed);

	return pInstance;
}

void CUI_Event_LoopAct::Free()
{
}

