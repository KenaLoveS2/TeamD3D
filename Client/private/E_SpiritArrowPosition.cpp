#include "stdafx.h"
#include "..\public\E_SpiritArrowPosition.h"
#include "GameInstance.h"
#include "Effect_Point_Instancing.h"

CE_SpiritArrowPosition::CE_SpiritArrowPosition(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_SpiritArrowPosition::CE_SpiritArrowPosition(const CE_SpiritArrowPosition & rhs)
	: CEffect(rhs)
{
	
}

HRESULT CE_SpiritArrowPosition::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_SpiritArrowPosition::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);

	Set_Child();

	m_eEFfectDesc.bActive = false;
	return S_OK;
}

HRESULT CE_SpiritArrowPosition::Late_Initialize(void * pArg)
{
	for (auto& pChild : m_vecChild)
	{
		pChild->Set_Parent(this);

		wstring strCloneTag = this->m_szCloneObjectTag;
		strCloneTag += L"_";
		strCloneTag += to_wstring(m_iHaveChildCnt);
		_tchar* pChildCloneTag = CUtile::Create_StringAuto(strCloneTag.c_str());
		pChild->Set_CloneTag(pChildCloneTag);
		m_iHaveChildCnt++;
	}

	return S_OK;
}

void CE_SpiritArrowPosition::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_eEFfectDesc.bActive == false)
	{
		m_bChildActive = false;
		m_vecChild[EFFECT_PARTICLE]->Set_Active(false);
		m_fTimeDelta = 0.f;
	}
	else
	{
		/* 자식 지우기 위한 용도 */
		m_fTimeDelta += fTimeDelta;
		if (m_bChildActive == false)
		{
			for (auto& pChild : m_vecChild)
				pChild->Set_Active(true);
			
			dynamic_cast<CEffect_Point_Instancing*>(m_vecChild[EFFECT_PARTICLE])->Set_ShapePosition();
			m_bChildActive = true;
		}
		if (m_fTimeDelta > 1.5f)
		{
			m_eEFfectDesc.fWidthFrame = 0.0;
			m_eEFfectDesc.fHeightFrame = 0.0;
			m_vecChild[EFFECT_SPRITEPOSITION]->ResetSprite();
			m_vecChild[EFFECT_SPRITEPOSITION]->Set_Active(false);
			m_fTimeDelta = 0.0f;
		}
	}
}

void CE_SpiritArrowPosition::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_SpiritArrowPosition::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CE_SpiritArrowPosition::Set_Child()
{
	CEffect_Base* pEffectBase = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// E_P_SpiritArrow
	wstring strCloneTag = this->m_szCloneObjectTag;
	strCloneTag += L"_";
	strCloneTag += to_wstring(m_iHaveChildCnt);
	_tchar* pChildCloneTag = CUtile::Create_StringAuto(strCloneTag.c_str());

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_SpiritArrow_P", pChildCloneTag));
	NULL_CHECK_RETURN(pEffectBase, );
	m_vecChild.push_back(pEffectBase);

	RELEASE_INSTANCE(CGameInstance);
}

CE_SpiritArrowPosition * CE_SpiritArrowPosition::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_SpiritArrowPosition * pInstance = new CE_SpiritArrowPosition(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_SpiritArrowPosition Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_SpiritArrowPosition::Clone(void * pArg)
{
	CE_SpiritArrowPosition * pInstance = new CE_SpiritArrowPosition(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_SpiritArrowPosition Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_SpiritArrowPosition::Free()
{
	__super::Free();
}
