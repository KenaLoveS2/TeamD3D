#include "stdafx.h"
#include "..\public\E_EnrageInto.h"
#include "GameInstance.h"

CE_EnrageInto::CE_EnrageInto(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_EnrageInto::CE_EnrageInto(const CE_EnrageInto & rhs)
	:CEffect_Mesh(rhs)
{
	
}

HRESULT CE_EnrageInto::Initialize_Prototype(const _tchar* pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_EnrageInto::Initialize(void * pArg)
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

HRESULT CE_EnrageInto::Late_Initialize(void * pArg)
{	
	return S_OK;
}

void CE_EnrageInto::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Tick(fTimeDelta);

	TurnOffSystem(m_fTimeDelta, 1.f, fTimeDelta);
}

void CE_EnrageInto::Late_Tick(_float fTimeDelta)
{
   	if (m_eEFfectDesc.bActive == false)
   		return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EFFECT, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}
}

HRESULT CE_EnrageInto::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_pModelCom != nullptr)
		m_pModelCom->Render(m_pShaderCom, 0, nullptr, m_eEFfectDesc.iPassCnt);

	return S_OK;
}

void CE_EnrageInto::Reset()
{
}

HRESULT CE_EnrageInto::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (g_bDayOrNight)	m_fHDRValue = 1.2f;

	return S_OK;
}

HRESULT CE_EnrageInto::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxEffectModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom), E_FAIL);

	Set_ModelCom(m_eEFfectDesc.eMeshType);

	return S_OK;
}

void CE_EnrageInto::Imgui_RenderProperty()
{
}

CE_EnrageInto * CE_EnrageInto::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_EnrageInto * pInstance = new CE_EnrageInto(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_Distortion_Plane Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_EnrageInto::Clone(void * pArg)
{
	CE_EnrageInto * pInstance = new CE_EnrageInto(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_Distortion_Plane Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_EnrageInto::Free()
{
	__super::Free();
}
