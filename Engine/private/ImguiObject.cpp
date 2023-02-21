#include "stdafx.h"
#include "..\public\ImguiObject.h"

CImguiObject::CImguiObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CImguiObject::Initialize(void* pArg)
{
	return S_OK;
}

void CImguiObject::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
