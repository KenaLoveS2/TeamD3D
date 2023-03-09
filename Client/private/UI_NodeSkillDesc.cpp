#include "stdafx.h"
#include "..\public\UI_NodeSkillDesc.h"
#include "GameInstance.h"

CUI_NodeSkillDesc::CUI_NodeSkillDesc(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
	, m_szDesc(nullptr)
{
}

CUI_NodeSkillDesc::CUI_NodeSkillDesc(const CUI_NodeSkillDesc & rhs)
	:CUI_Node(rhs)
	,m_szDesc(nullptr)
{
}

void CUI_NodeSkillDesc::Set_String(wstring str)
{
	Safe_Delete_Array(m_szDesc);

	m_szDesc = CUtile::Create_String(str.c_str());
}

HRESULT CUI_NodeSkillDesc::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeSkillDesc::Initialize(void * pArg)
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

	m_bActive = true;
	return S_OK;
}

void CUI_NodeSkillDesc::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;


	__super::Tick(fTimeDelta);
}

void CUI_NodeSkillDesc::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeSkillDesc::Render()
{
	//if (nullptr == m_pTextureCom[TEXTURE_DIFFUSE])
	//	return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
	{
		MSG_BOX("Failed To Setup ShaderResources : CUI_NodeSkillName");
		return E_FAIL;
	}

	m_pShaderCom->Begin(m_iRenderPass);
	m_pVIBufferCom->Render();

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	_float2 vNewPos = { vPos.x + g_iWinSizeX*0.5f + 25.f, g_iWinSizeY*0.5f - vPos.y - 10.f };

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr != m_szDesc)
	{
		pGameInstance->Render_Font(TEXT("Font_Basic0"), m_szDesc,
			vNewPos /* position */,
			0.f, _float2(1.f, 1.f)/* size */,
			XMVectorSet(1.f, 1.f, 1.f, 1.f)/* color */);
	}

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

HRESULT CUI_NodeSkillDesc::SetUp_Components()
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

HRESULT CUI_NodeSkillDesc::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_tDesc.ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_tDesc.ProjMatrix)))
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

CUI_NodeSkillDesc * CUI_NodeSkillDesc::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeSkillDesc*	pInstance = new CUI_NodeSkillDesc(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeSkillDesc");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeSkillDesc::Clone(void * pArg)
{
	CUI_NodeSkillDesc*	pInstance = new CUI_NodeSkillDesc(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeSkillDesc");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeSkillDesc::Free()
{
	__super::Free();

	Safe_Delete_Array(m_szDesc);
}
