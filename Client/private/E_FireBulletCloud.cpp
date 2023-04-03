#include "stdafx.h"
#include "..\public\E_FireBulletCloud.h"
#include "GameInstance.h"
#include "Bone.h"
#include "Effect_Trail.h"
#include "Kena.h"

CE_FireBulletCloud::CE_FireBulletCloud(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_FireBulletCloud::CE_FireBulletCloud(const CE_FireBulletCloud & rhs)
	: CEffect(rhs)
{
}

HRESULT CE_FireBulletCloud::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_FireBulletCloud::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	Set_ShaderOption(m_eEFfectDesc.iPassCnt, 1.0f, _float2(0.0f, 0.0f), false);
	m_fInitSpriteCnt = _float2(0.0f, 0.0f);
	return S_OK;
}

void CE_FireBulletCloud::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CE_FireBulletCloud::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	if (m_pParent != nullptr)
		Set_Matrix();

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_FireBulletCloud::Render()
{	
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CE_FireBulletCloud::Imgui_RenderProperty()
{

}

HRESULT CE_FireBulletCloud::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	return S_OK;
}

CE_FireBulletCloud * CE_FireBulletCloud::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext,  const _tchar* pFilePath )
{
	CE_FireBulletCloud * pInstance = new CE_FireBulletCloud(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_FireBulletCloud Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_FireBulletCloud::Clone(void * pArg)
{
	CE_FireBulletCloud * pInstance = new CE_FireBulletCloud(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_FireBulletCloud Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_FireBulletCloud::Free()
{
	__super::Free();
}

