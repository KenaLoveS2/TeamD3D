#include "stdafx.h"
#include "..\public\UI_RotIcon.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Monster.h"
#include "RotForMonster.h"
#include "Kena.h"

CUI_RotIcon::CUI_RotIcon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Billboard(pDevice, pContext)
	,m_pTarget(nullptr)
	, m_pKena(nullptr)
{
}

CUI_RotIcon::CUI_RotIcon(const CUI_RotIcon & rhs)
	: CUI_Billboard(rhs)
	,m_pTarget(nullptr)
	, m_pKena(nullptr)
{
}

void CUI_RotIcon::Set_Pos(CGameObject* pTarget, _float4 vCorrect)
{
	/*if (m_pTarget == nullptr && pTarget != nullptr)
	{
		CUI_ClientManager::UI_PRESENT tag = CUI_ClientManager::BOT_KEY_USEROT;
		_float fNoMeaning = 1.f;
		m_pKena->m_Delegator.broadcast(tag, fNoMeaning);
	}*/

	if (pTarget == nullptr)
	{
		m_pTarget = nullptr;
		m_bActive = false;
		return;
	}

	m_pTarget = pTarget;
	m_bActive = true;

	_float4 vTargetPos = pTarget->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
	_float4 vCorrected = XMVectorSetW(vTargetPos + vCorrect, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vCorrected);
}

void CUI_RotIcon::Off_Focus(CGameObject* pTarget)
{
	if (pTarget == nullptr)
	{
		CUI_ClientManager::UI_PRESENT tag = CUI_ClientManager::BOT_KEY_OFF;
		_float fNoMeaning = 1.f;
		m_pKena->m_Delegator.broadcast(tag, fNoMeaning);
	}

	if (pTarget && m_pTarget == pTarget)
	{
		m_pTarget = nullptr;
		m_bActive = false;

		CUI_ClientManager::UI_PRESENT tag = CUI_ClientManager::BOT_KEY_OFF;
		_float fNoMeaning = 1.f;
		m_pKena->m_Delegator.broadcast(tag, fNoMeaning);

	}
}

HRESULT CUI_RotIcon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_RotIcon::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(40.f, 40.f, 1.f));
		m_vOriginalSettingScale = m_pTransformCom->Get_Scaled();
	}

	/* It might be faster.... */
	m_iRenderPass = 1;
	m_pTransformCom->Set_Scaled(_float3(40.f, 40.f, 1.f));
	m_vOriginalSettingScale = m_pTransformCom->Get_Scaled();


	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}

	//m_bActive = true;

	return S_OK;
}

void CUI_RotIcon::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Tick(fTimeDelta);

	/* Modify Date: 230402 */
	if (nullptr != m_pTarget)
	{
		CMonster* pMonster = dynamic_cast<CMonster*>(m_pTarget);
		if (pMonster != nullptr)
		{
			if (pMonster->Get_Bind())
			{
				m_pTarget = nullptr;
				m_bActive = false;
				return;
			}
		}

		/* calculate camera */
		_float4 vCamLook = CGameInstance::GetInstance()->Get_WorkCameraPtr()->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
		_float4 vCamPos = CGameInstance::GetInstance()->Get_WorkCameraPtr()->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
		_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float fDistance = _float4::Distance(vCamPos, vPos);
		_float4 vDir = XMVector3Normalize(vPos - vCamPos);

		if (10.f < fDistance || (XMVectorGetX(XMVector3Dot(vDir, vCamLook)) <= cosf(XMConvertToRadians(20.f))))
		{
			m_pTarget = nullptr;
			m_bActive = false;
		}
	}

#pragma region Old
	//if (nullptr != m_pTarget)
	//{
	//	CMonster* pMonster =  dynamic_cast<CMonster*>(m_pTarget);
	//	if (pMonster != nullptr)
	//	{
	//		/* calculate camera */
	//		_float4 vCamLook = CGameInstance::GetInstance()->Get_WorkCameraPtr()->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	//		_float4 vCamPos = CGameInstance::GetInstance()->Get_WorkCameraPtr()->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
	//		_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	//		_float fDistance = _float4::Distance(vCamPos, vPos);
	//		_float4 vDir = XMVector3Normalize(vPos - vCamPos);
	//		if (10.f < fDistance || (XMVectorGetX(XMVector3Dot(vDir, vCamLook)) <= cosf(XMConvertToRadians(20.f))) || pMonster->Get_Bind())
	//		{
	//			m_pTarget = nullptr;
	//			m_bActive = false;
	//		}
	//	}
	//}
#pragma endregion Old
}



void CUI_RotIcon::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && m_bActive)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CUI_RotIcon::Render()
{
	if (nullptr == m_pTextureCom[TEXTURE_DIFFUSE])
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
	{
		MSG_BOX("Failed To Setup ShaderResources : CUI_MonsterHP");
		return E_FAIL;
	}

	m_iRenderPass = 18;
	m_pShaderCom->Begin(m_iRenderPass);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_RotIcon::SetUp_Components()
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
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_RotFocus"), m_TextureComTag[TEXTURE_DIFFUSE].c_str(), (CComponent**)&m_pTextureCom[TEXTURE_DIFFUSE]))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_RotIcon::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI::SetUp_ShaderResources(); /* Events Resourece Setting */

	//if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;

	if (nullptr != m_pTarget)
	{
		// _float4 vPos = m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
		_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		vPos = XMVector3TransformCoord(vPos, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
		vPos = XMVector3TransformCoord(vPos, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
		_matrix matWorld = XMMatrixScaling(m_vOriginalSettingScale.x, m_vOriginalSettingScale.y, m_vOriginalSettingScale.z);
		matWorld = matWorld * XMMatrixTranslation(vPos.x * g_iWinSizeX * 0.5f, vPos.y * g_iWinSizeY * 0.5f, vPos.z);
		_float4x4 worldFloat4x4;
		XMStoreFloat4x4(&worldFloat4x4, matWorld);
		//worldFloat4x4._41 -= g_iWinSizeX * 0.5f;
		worldFloat4x4._43 = 0.f;

		if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &worldFloat4x4)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;
	}


	//if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
	//	return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_tDesc.ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_tDesc.ProjMatrix)))
		return E_FAIL;

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vColor", &_float4(1.f, 1.f, 1.f, 1.f), sizeof(_float4)), E_FAIL);
	_float fAlpha = 1.f;
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fAlpha", &fAlpha, sizeof(_float)), E_FAIL);

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


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CUI_RotIcon * CUI_RotIcon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_RotIcon*	pInstance = new CUI_RotIcon(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_RotIcon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_RotIcon::Clone(void * pArg)
{
	CUI_RotIcon*	pInstance = new CUI_RotIcon(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_RotIcon");
		Safe_Release(pInstance);
	}
	return pInstance;;
}

void CUI_RotIcon::Free()
{
	__super::Free();
}
