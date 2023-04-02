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
	m_eEFfectDesc.vScale = XMVectorSet(2.f, 2.f, 2.f, 1.f);

	for (auto& pChild : m_vecChild)
		pChild->Set_Parent(this);

	return S_OK;
}

void CE_FireBulletExplosion::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	for (auto& pChild : m_vecChild)
		pChild->Set_Active(m_eEFfectDesc.bActive);

	if (m_eEFfectDesc.bActive == true)
	{
		m_fDurationTime += fTimeDelta;
		for (auto& pChild : m_vecChild)
			pChild->Set_AddScale(fTimeDelta + 0.3f);

		Set_AddScale(fTimeDelta + 0.3f);

		if (m_fDurationTime > 0.4f)
		{
			ResetSprite();
			for (auto& pChild : m_vecChild)
				pChild->Set_Scale(_float3(0.1f, 0.1f, 0.1f));

			m_eEFfectDesc.vScale = _float3(0.1f, 0.1f, 0.1f);
			m_bFinishSprite = false;
			m_eEFfectDesc.bActive = false;
			m_fDurationTime = 0.0f;
		}
	}
}

void CE_FireBulletExplosion::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	//if (m_pParent != nullptr)
	//	Set_Matrix();

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
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

