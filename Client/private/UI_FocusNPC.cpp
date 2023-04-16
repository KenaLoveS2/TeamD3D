#include "stdafx.h"
#include "..\public\UI_FocusNPC.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Saiya.h"
#include "UI_Event_LoopAct.h"

CUI_FocusNPC::CUI_FocusNPC(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Billboard(pDevice, pContext)
{
}

CUI_FocusNPC::CUI_FocusNPC(const CUI_FocusNPC & rhs)
	: CUI_Billboard(rhs)
{
}

HRESULT CUI_FocusNPC::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_FocusNPC::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX("Failed To Initialize : FocusNPC");
		return E_FAIL;
	}

	/* It might be faster.... */
	m_iRenderPass = 23;
	m_pTransformCom->Set_Scaled(_float3(25.f, 25.f, 1.f));
	m_vOriginalSettingScale = m_pTransformCom->Get_Scaled();

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}

	//m_bActive = true;
	/* Event */
	m_vecEvents.push_back(CUI_Event_LoopAct::Create(
		this, CUI_Event_LoopAct::TYPE_LOOP_VRT, 0.08f, 2.6f));

	m_vecEvents[0]->Call_Event(true);

	return S_OK;
}

void CUI_FocusNPC::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Tick(fTimeDelta);
}

void CUI_FocusNPC::Late_Tick(_float fTimeDelta)
{
	/* calculate camera */
	_float4 vCamLook = CGameInstance::GetInstance()->Get_WorkCameraPtr()->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	_float4 vCamPos = CGameInstance::GetInstance()->Get_WorkCameraPtr()->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
	_float4 vPos = m_tBBDesc.pOwner->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
	_float fDistance = _float4::Distance(vCamPos, vPos);
	_float4 vDir = XMVector3Normalize(vPos - vCamPos);

	if (10.f > fDistance) /*	&& (XMVectorGetX(XMVector3Dot(vDir, vCamLook)) > cosf(XMConvertToRadians(20.f))*/
		m_bActive = true;
	else
		m_bActive = false;

	if (dynamic_cast<CSaiya*>(m_tBBDesc.pOwner)->Get_Disappear() || dynamic_cast<CSaiya*>(m_tBBDesc.pOwner)->UIOFF())
		m_bActive = false;

	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && m_bActive)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CUI_FocusNPC::Render()
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

HRESULT CUI_FocusNPC::SetUp_Components()
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

	/* Diffuse Texture */
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_FocusNPC"), m_TextureComTag[TEXTURE_DIFFUSE].c_str(), (CComponent**)&m_pTextureCom[TEXTURE_DIFFUSE]))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_FocusNPC::SetUp_ShaderResources()
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

	_float4 vColor = { 1.f, 2.f, 2.f, 1.f };
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CUI_FocusNPC * CUI_FocusNPC::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_FocusNPC*	pInstance = new CUI_FocusNPC(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_FocusNPC");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_FocusNPC::Clone(void * pArg)
{
	CUI_FocusNPC*	pInstance = new CUI_FocusNPC(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_FocusNPC");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_FocusNPC::Free()
{
	__super::Free();
}
