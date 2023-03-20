#include "stdafx.h"
#include "..\public\UI_FocusMonster.h"
#include "GameInstance.h"
#include "UI_FocusMonsterParts.h"
#include "Monster.h"

CUI_FocusMonster::CUI_FocusMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Billboard(pDevice, pContext)
	, m_pTarget(nullptr)
	, m_bStart(false)
{
	for (_uint i = 0; i < PARTS_END; ++i)
		m_pParts[i] = nullptr;
}

CUI_FocusMonster::CUI_FocusMonster(const CUI_FocusMonster & rhs)
	:CUI_Billboard(rhs)
	, m_pTarget(nullptr)
	, m_bStart(false)
{
	for (_uint i = 0; i < PARTS_END; ++i)
		m_pParts[i] = nullptr;
}

HRESULT CUI_FocusMonster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_FocusMonster::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(80.f, 5.f, 1.f));
		m_vOriginalSettingScale = m_pTransformCom->Get_Scaled();
	}

	/* It might be faster.... */
	m_iRenderPass = 20;
	m_pTransformCom->Set_Scaled(_float3(80.f, 5.f, 1.f));
	m_vOriginalSettingScale = m_pTransformCom->Get_Scaled();

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}

	if (FAILED(SetUp_Parts()))
	{
		MSG_BOX("Failed To Setup Parts");
		return E_FAIL;
	}

	return S_OK;
}

void CUI_FocusMonster::Tick(_float fTimeDelta)
{	
	if (!m_bActive)
		return;

	__super::Tick(fTimeDelta);

	if (m_bStart)
	{
		for (_uint i = 0; i < PARTS_END; ++i)
		{
			//if (!m_pParts[i]->IsEnd())
			//{

			//}
		}
	}


	for (_uint i = 0; i < PARTS_END; ++i)
	{
		m_pParts[i]->Tick(fTimeDelta);
	}
}

void CUI_FocusMonster::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);

	for (_uint i = 0; i < PARTS_END; ++i)
		m_pParts[i]->Late_Tick(fTimeDelta);

	//if (nullptr != m_pRendererCom && m_bActive)
	//	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_FocusMonster::Render()
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

void CUI_FocusMonster::Set_Pos(CGameObject * pTarget)
{
	if (pTarget == nullptr)
	{
		m_pTarget = nullptr;
		m_bActive = false;
		return;
	}

	m_pTarget = pTarget;
	m_bActive = true;
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, 
		static_cast<CMonster*>(pTarget)->Get_FocusPosition());
}

void CUI_FocusMonster::Off_Focus(CGameObject* pTarget)
{
	if (pTarget && m_pTarget == pTarget)
	{
		m_pTarget = nullptr;
		m_bActive = false;
	}
}

void CUI_FocusMonster::Start_Animation()
{
}

HRESULT CUI_FocusMonster::SetUp_Components()
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

HRESULT CUI_FocusMonster::SetUp_ShaderResources()
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


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_FocusMonster::SetUp_Parts()
{
	CUI_FocusMonsterParts::PARTSDESC tDesc;

	for (_uint i = 0; i < PARTS_END; ++i)
	{
		tDesc.iType = i;
		m_pParts[i] = static_cast<CUI_FocusMonsterParts*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_FocusMonsterParts",
			CUtile::Create_DummyString(), &tDesc));

		m_pParts[i]->Set_Parent(this);
	}

	return S_OK;
}

CUI_FocusMonster * CUI_FocusMonster::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_FocusMonster*	pInstance = new CUI_FocusMonster(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_FocusMonster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_FocusMonster::Clone(void * pArg)
{
	CUI_FocusMonster*	pInstance = new CUI_FocusMonster(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_FocusMonster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_FocusMonster::Free()
{
	for (_uint i = 0; i < PARTS_END; ++i)
		Safe_Release(m_pParts[i]);
	__super::Free();
}
