#include "stdafx.h"
#include "..\public\E_ShamanWeaponBall.h"
#include "GameInstance.h"

CE_ShamanWeaponBall::CE_ShamanWeaponBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_ShamanWeaponBall::CE_ShamanWeaponBall(const CE_ShamanWeaponBall& rhs)
	: CEffect_Mesh(rhs)
{
}

HRESULT CE_ShamanWeaponBall::Initialize_Prototype(const _tchar* pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_ShamanWeaponBall::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 4.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	m_fHDRValue = 3.f;
	m_eEFfectDesc.bActive = false;
	return S_OK;
}

HRESULT CE_ShamanWeaponBall::Late_Initialize(void* pArg)
{
	m_pShamanModel = (CModel*)m_pParent->Find_Component(L"Com_Model");
	return S_OK;
}

void CE_ShamanWeaponBall::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;
	
	__super::Tick(fTimeDelta);
	m_fTimeDelta += fTimeDelta;

	Update_Trail();
}

void CE_ShamanWeaponBall::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CE_ShamanWeaponBall::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_pModelCom != nullptr)
		m_pModelCom->Render(m_pShaderCom, 0, nullptr, m_eEFfectDesc.iPassCnt);

	return S_OK;
}

void CE_ShamanWeaponBall::Imgui_RenderProperty()
{
}

void CE_ShamanWeaponBall::Update_Trail()
{
	if (m_pParent == nullptr)
		return;

	_matrix Socketmatrix = m_pShamanBone->Get_CombindMatrix() * m_pShamanModel->Get_PivotMatrix() * m_pParent->Get_WorldMatrix();
	m_pTransformCom->Set_WorldMatrix(Socketmatrix);

	m_pTransformCom->Set_Scaled(_float3(0.05f, 0.05f, 0.05f));
}

void CE_ShamanWeaponBall::Reset()
{
}

HRESULT CE_ShamanWeaponBall::SetUp_ShaderResources()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CE_ShamanWeaponBall::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxEffectModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom), E_FAIL);
	Set_ModelCom(m_eEFfectDesc.eMeshType);

	return S_OK;
}

CE_ShamanWeaponBall* CE_ShamanWeaponBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath)
{
	CE_ShamanWeaponBall* pInstance = new CE_ShamanWeaponBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_ShamanWeaponBall Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CE_ShamanWeaponBall::Clone(void* pArg)
{
	CE_ShamanWeaponBall* pInstance = new CE_ShamanWeaponBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_ShamanWeaponBall Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_ShamanWeaponBall::Free()
{
	__super::Free();
}
