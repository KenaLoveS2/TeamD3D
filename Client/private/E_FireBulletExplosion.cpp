#include "stdafx.h"
#include "..\public\E_FireBulletExplosion.h"
#include "GameInstance.h"

CE_FireBulletExplosion::CE_FireBulletExplosion(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_FireBulletExplosion::CE_FireBulletExplosion(const CE_FireBulletExplosion & rhs)
	: CEffect(rhs)
{
}

HRESULT CE_FireBulletExplosion::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_FireBulletExplosion::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;
	m_fInitSpriteCnt = _float2(0.0f, 0.0f);
	m_fDurationTime = 0.3f;

	return S_OK;
}

void CE_FireBulletExplosion::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_fTimeDelta += fTimeDelta;

	if (m_fTimeDelta > m_fDurationTime)
	{
		ResetSprite();
		m_eEFfectDesc.bActive = false;
		m_fTimeDelta = 0.0;
	}
}

void CE_FireBulletExplosion::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

	if (m_pParent != nullptr)
		Set_Matrix();
}

HRESULT CE_FireBulletExplosion::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CE_FireBulletExplosion::Imgui_RenderProperty()
{

}

HRESULT CE_FireBulletExplosion::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	return S_OK;
}

CE_FireBulletExplosion * CE_FireBulletExplosion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_FireBulletExplosion * pInstance = new CE_FireBulletExplosion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_FireBulletExplosion Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_FireBulletExplosion::Clone(void * pArg)
{
	CE_FireBulletExplosion * pInstance = new CE_FireBulletExplosion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_FireBulletExplosion Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_FireBulletExplosion::Free()
{
	__super::Free();
}

