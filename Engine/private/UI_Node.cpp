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
}

HRESULT CUI_Node::Render()
{
	__super::Render();

	return S_OK;
}

void CUI_Node::Free()
{
	__super::Free();
}
