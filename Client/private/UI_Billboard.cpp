#include "stdafx.h"
#include "..\public\UI_Billboard.h"
#include "GameInstance.h"

CUI_Billboard::CUI_Billboard(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI(pDevice,pContext)
{
}

CUI_Billboard::CUI_Billboard(const CUI_Billboard & rhs)
	:CUI(rhs)
{
}

HRESULT CUI_Billboard::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Billboard::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg == nullptr)
		return E_FAIL;

	UIDESC* tDesc = (UIDESC*)pArg;

	return S_OK;//Load_Data(tDesc->fileName);
}

void CUI_Billboard::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


}

void CUI_Billboard::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	_float fDist = Get_CamDistance();
	CUtile::Execute_BillBoardOrtho(m_pTransformCom, m_vOriginalSettingScale, Get_CamDistance());
}

HRESULT CUI_Billboard::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
	{
		MSG_BOX("Failed To Setup ShaderResources : UI_Canvas");
		return E_FAIL;
	}

	m_pShaderCom->Begin(m_iRenderPass);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Billboard::Save_Data()
{
	return S_OK;
}

HRESULT CUI_Billboard::Load_Data(wstring fileName)
{
	return S_OK;
}

void CUI_Billboard::Free()
{
	__super::Free();
}
