#include "stdafx.h"
#include "UI_Weakpoint.h"
#include "GameInstance.h"
#include "Monster.h"
#include "Kena.h"

CUI_Weakpoint::CUI_Weakpoint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Billboard(pDevice, pContext)
	, m_pTargetMonster(nullptr)
	, m_vColor(1.f, 1.f, 1.f, 1.f)
{
}

CUI_Weakpoint::CUI_Weakpoint(const CUI_Weakpoint& rhs)
	: CUI_Billboard(rhs)
	, m_pTargetMonster(nullptr)
	, m_vColor(1.f, 1.f, 1.f, 1.f)
{
}

HRESULT CUI_Weakpoint::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Weakpoint::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(80.f, 5.f, 1.f));
		m_vOriginalSettingScale = m_pTransformCom->Get_Scaled();
	}

	/* It might be faster.... */
	m_iRenderPass = 23;
	m_pTransformCom->Set_Scaled(_float3(80.f, 5.f, 1.f));
	m_vOriginalSettingScale = m_pTransformCom->Get_Scaled();

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Weakpoint::Late_Initialize(void* pArg)
{
	m_pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena"));
	if (m_pKena == nullptr)
	{
		MSG_BOX("Failed To Connect Kena : WeakPoint");
		return E_FAIL;
	}

	return S_OK;
}

void CUI_Weakpoint::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Tick(fTimeDelta);

	if (m_pKena == nullptr || m_pTargetMonster == nullptr)
		return;

	CTransform* pKenaTransform = m_pKena->Get_TransformCom();
	_float4 vLook	= XMVector3Normalize(pKenaTransform->Get_State(CTransform::STATE_LOOK));
	_float4 vDir	= XMVector3Normalize(m_pTargetMonster->Get_TransformCom()->Get_Position() - pKenaTransform->Get_Position());

	_float4 vCross = XMVector3Cross(vDir, vLook);
	if (vCross.y > 0)
		m_pTransformCom->RotationFromNow(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(0.1f));
	else
		m_pTransformCom->RotationFromNow(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(-0.1f));

}

void CUI_Weakpoint::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && m_bActive)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CUI_Weakpoint::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
	{
		MSG_BOX("Failed To Setup ShaderResources : CUI_MonsterHP");
		return E_FAIL;
	}

	m_pShaderCom->Begin(m_iRenderPass);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Weakpoint::SetUp_Components()
{
	/* Renderer */
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom))
		return E_FAIL;

	/* Shader */
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom))
		return E_FAIL;

	/* VIBuffer_Rect */
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Weakpoint::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI::SetUp_ShaderResources(); /* Events Resourece Setting */

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (m_pTextureCom[TEXTURE_DIFFUSE] != nullptr)
	{
		if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
			return E_FAIL;
	}

	if (m_pTextureCom[TEXTURE_MASK] != nullptr)
	{
		if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			return E_FAIL;
	}

	m_vColor = { 1.f, 1.f, 1.f, 1.f };
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CUI_Weakpoint* CUI_Weakpoint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Weakpoint* pInstance = new CUI_Weakpoint(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_FocusMonsterParts");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Weakpoint::Clone(void* pArg)
{
	CUI_Weakpoint* pInstance = new CUI_Weakpoint(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Weakpoint");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Weakpoint::Free()
{
	__super::Free();
}
