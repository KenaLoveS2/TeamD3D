#include "stdafx.h"
#include "..\public\UI_NodeEffect.h"
#include "GameInstance.h"
#include "UI_Event_Animation.h"

/* m_pParent가 UI인 UI */

CUI_NodeEffect::CUI_NodeEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
	, m_pTarget(nullptr)
{
}

CUI_NodeEffect::CUI_NodeEffect(const CUI_NodeEffect & rhs)
	: CUI_Node(rhs)
	, m_pTarget(nullptr)
{
}

void CUI_NodeEffect::Start_Effect(CUI * pTarget, _float fX, _float fY)
{
	if (nullptr == pTarget)
		return;

	m_pTarget = pTarget;
	m_bActive = true;

	_float4 vPos = pTarget->Get_LocalMatrix().r[3];
	vPos.x += fX;
	vPos.y += fY;

	this->Set_LocalTranslation(vPos);

	//_float4 vPos = pTarget->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
	//vPos.x += fX;
	//vPos.y += fY;
	//
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,vPos);

	m_vecEvents[0]->Call_Event((_uint)0);
}

HRESULT CUI_NodeEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeEffect::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(100.f, 100.f, 1.f));
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}

	//m_bActive = true;

	/* Events */
	/* 이미지가 변경되도록 하는 이벤트 */
	UIDESC* tDesc = (UIDESC*)pArg;
	m_vecEvents.push_back(CUI_Event_Animation::Create(tDesc->fileName, this));

	return S_OK;
}

void CUI_NodeEffect::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (static_cast<CUI_Event_Animation*>(m_vecEvents[0])->Is_Finished())
		m_bActive = false;

	__super::Tick(fTimeDelta);
}

void CUI_NodeEffect::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (m_pParent != nullptr)
	{
		_float4x4 matWorldParent;
		XMStoreFloat4x4(&matWorldParent, m_pParent->Get_WorldMatrix());

		_matrix matParentTrans = XMMatrixTranslation(matWorldParent._41, matWorldParent._42, matWorldParent._43);

		float fRatioX = matWorldParent._11 / m_matParentInit._11;
		float fRatioY = matWorldParent._22 / m_matParentInit._22;
		_matrix matParentScale = XMMatrixScaling(fRatioX, fRatioY, 1.f);

		_matrix matWorld = m_matLocal*matParentScale*matParentTrans;
		m_pTransformCom->Set_WorldMatrix(matWorld);
	}

	for (auto e : m_vecEvents)
		e->Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && m_bActive)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

}

HRESULT CUI_NodeEffect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeEffect::SetUp_Components()
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

HRESULT CUI_NodeEffect::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI::SetUp_ShaderResources(); /* Events Resourece Setting */

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_tDesc.ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_tDesc.ProjMatrix)))
		return E_FAIL;

	if (m_pTextureCom[TEXTURE_DIFFUSE] != nullptr)
	{
		if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIdx)))
			return E_FAIL;
	}

	if (m_pTextureCom[TEXTURE_MASK] != nullptr)
	{
		if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CUI_NodeEffect * CUI_NodeEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeEffect*	pInstance = new CUI_NodeEffect(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeEffect::Clone(void * pArg)
{
	CUI_NodeEffect*	pInstance = new CUI_NodeEffect(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeEffect::Free()
{
	__super::Free();
}
