#include "stdafx.h"
#include "..\public\E_EnrageAttack.h"
#include "GameInstance.h"

CE_EnrageAttack::CE_EnrageAttack(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_EnrageAttack::CE_EnrageAttack(const CE_EnrageAttack & rhs)
	:CEffect_Mesh(rhs)
{
	
}

HRESULT CE_EnrageAttack::Initialize_Prototype(const _tchar* pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_EnrageAttack::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 4.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Child(), E_FAIL);

	m_eEFfectDesc.bActive = false;
	return S_OK;
}

HRESULT CE_EnrageAttack::Late_Initialize(void * pArg)
{	
	return S_OK;
}

void CE_EnrageAttack::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
	{
		m_fTimeDelta = 0.0f;
		return;
	}
	__super::Tick(fTimeDelta);

	m_fRazerOnTime = m_fTimeDelta += fTimeDelta;
	if (m_fTimeDelta > 4.f)
	{
		m_eEFfectDesc.bActive = false;
		m_fRazerOnTime = 0.0f;
		m_fTimeDelta = 0.0f;
	}
}

void CE_EnrageAttack::Late_Tick(_float fTimeDelta)
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

HRESULT CE_EnrageAttack::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_pModelCom != nullptr)
		m_pModelCom->Render(m_pShaderCom, 0, nullptr, m_eEFfectDesc.iPassCnt);
	
	return S_OK;
}

void CE_EnrageAttack::Reset()
{
}

HRESULT CE_EnrageAttack::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_RazerValue", &m_fRazerOnTime, sizeof(_float)), E_FAIL);

	return S_OK;
}

HRESULT CE_EnrageAttack::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxEffectModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom), E_FAIL);

	Set_ModelCom(m_eEFfectDesc.eMeshType);

	return S_OK;
}

HRESULT CE_EnrageAttack::SetUp_Child()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEffect_Base* pEffectBase = nullptr;

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_EnrageAttack_P", L"W_P_EnrageAttack"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	m_vecChild.push_back(pEffectBase);

	return S_OK;
}

void CE_EnrageAttack::Imgui_RenderProperty()
{
	if (ImGui::Button("Active"))
		m_eEFfectDesc.bActive = true;

	if (ImGui::Button("Recompile"))
		m_pShaderCom->ReCompile();

}

CE_EnrageAttack * CE_EnrageAttack::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_EnrageAttack * pInstance = new CE_EnrageAttack(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_Distortion_Plane Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_EnrageAttack::Clone(void * pArg)
{
	CE_EnrageAttack * pInstance = new CE_EnrageAttack(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_Distortion_Plane Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_EnrageAttack::Free()
{
	__super::Free();
}
