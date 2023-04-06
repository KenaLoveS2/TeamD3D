#include "stdafx.h"
#include "UI_CanvasInfo.h"

CUI_CanvasInfo::CUI_CanvasInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Canvas(pDevice, pContext)
{
}

CUI_CanvasInfo::CUI_CanvasInfo(const CUI_CanvasInfo& rhs)
	:CUI_Canvas(rhs)
{
}

HRESULT CUI_CanvasInfo::Initialize_Prototype()
{
	return CUI_Canvas::Initialize_Prototype();
}

HRESULT CUI_CanvasInfo::Initialize(void* pArg)
{
	return CUI_Canvas::Initialize(pArg);
}

void CUI_CanvasInfo::Tick(_float fTimeDelta)
{
	CUI_Canvas::Tick(fTimeDelta);
}

void CUI_CanvasInfo::Late_Tick(_float fTimeDelta)
{
	CUI_Canvas::Late_Tick(fTimeDelta);
}

HRESULT CUI_CanvasInfo::Render()
{
	return CUI_Canvas::Render();
}

HRESULT CUI_CanvasInfo::Bind()
{
	return CUI_Canvas::Bind();
}

HRESULT CUI_CanvasInfo::Ready_Nodes()
{
	return CUI_Canvas::Ready_Nodes();
}

HRESULT CUI_CanvasInfo::SetUp_Components()
{
	return S_OK;
}

HRESULT CUI_CanvasInfo::SetUp_ShaderResources()
{
	return CUI_Canvas::SetUp_ShaderResources();
}

void CUI_CanvasInfo::BindFunction(CUI_ClientManager::UI_PRESENT eType, _float fValue)
{
}

CUI_CanvasInfo* CUI_CanvasInfo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_CanvasInfo* pInstance = new CUI_CanvasInfo(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_CanvasInfo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_CanvasInfo::Clone(void* pArg)
{
	CUI_CanvasInfo* pInstance = new CUI_CanvasInfo(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_CanvasInfo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CanvasInfo::Free()
{
	__super::Free();
}
