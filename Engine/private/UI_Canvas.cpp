#include "..\public\UI_Canvas.h"

CUI_Canvas::CUI_Canvas(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI(pDevice, pContext)
{
}

CUI_Canvas::CUI_Canvas(const CUI_Canvas & rhs)
	:CUI(rhs)
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

	return S_OK;
}

void CUI_Canvas::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Canvas::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Canvas::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CUI_Canvas::Add_ChildUI(CUI * pUI)
{
	if (nullptr == pUI)
		return E_FAIL;

	m_vecChildUI.push_back(pUI);
	Safe_AddRef(pUI);

	pUI->Set_Parent(this);

	return S_OK;
}

void CUI_Canvas::Free()
{
	__super::Free();

	for (auto &pChildUI : m_vecChildUI)
		Safe_Release(pChildUI);
}
