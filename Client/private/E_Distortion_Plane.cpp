#include "stdafx.h"
#include "..\public\E_Distortion_Plane.h"
#include "GameInstance.h"

CE_Distortion_Plane::CE_Distortion_Plane(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_Distortion_Plane::CE_Distortion_Plane(const CE_Distortion_Plane & rhs)
	:CEffect_Mesh(rhs)
{
	
}

HRESULT CE_Distortion_Plane::Initialize_Prototype(const _tchar* pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_Distortion_Plane::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 4.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	m_eEFfectDesc.bActive = false;
	return S_OK;
}

HRESULT CE_Distortion_Plane::Late_Initialize(void * pArg)
{	
	return S_OK;
}

void CE_Distortion_Plane::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
	{
		m_fTimeDelta = 0.0f;
		return;
	}

	__super::Tick(fTimeDelta);
	m_fTimeDelta += fTimeDelta;

	if (m_fTimeDelta > 2.f)
	{
		m_eEFfectDesc.bActive = false;
		m_fTimeDelta = 0.0f;
	}
}

void CE_Distortion_Plane::Late_Tick(_float fTimeDelta)
{
   	if (m_eEFfectDesc.bActive == false)
   		return;

	__super::Late_Tick(fTimeDelta);

	if (m_pParent != nullptr)
	{

	}

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EFFECT, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}
}

HRESULT CE_Distortion_Plane::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_pModelCom != nullptr)
		m_pModelCom->Render(m_pShaderCom, 0, nullptr, m_eEFfectDesc.iPassCnt);

	return S_OK;
}

void CE_Distortion_Plane::Reset()
{
}

HRESULT CE_Distortion_Plane::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Distortion_Plane::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxEffectModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom), E_FAIL);

	Set_ModelCom(m_eEFfectDesc.eMeshType);

	return S_OK;
}

void CE_Distortion_Plane::Imgui_RenderProperty()
{
}

CE_Distortion_Plane * CE_Distortion_Plane::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_Distortion_Plane * pInstance = new CE_Distortion_Plane(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_Distortion_Plane Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_Distortion_Plane::Clone(void * pArg)
{
	CE_Distortion_Plane * pInstance = new CE_Distortion_Plane(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_Distortion_Plane Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_Distortion_Plane::Free()
{  
	__super::Free();
}
