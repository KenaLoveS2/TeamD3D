#include "stdafx.h"
#include "..\public\UI_NodeRotCnt.h"
#include "GameInstance.h"

CUI_NodeRotCnt::CUI_NodeRotCnt(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
	, m_iRotMax(5)
	, m_szInfo(nullptr)
{
}

CUI_NodeRotCnt::CUI_NodeRotCnt(const CUI_NodeRotCnt & rhs)
	:CUI_Node(rhs)
	, m_iRotMax(5)
	, m_szInfo(nullptr)
{
}

void CUI_NodeRotCnt::Set_Info(_int iRotMax)
{
	m_iRotMax = iRotMax;

	Safe_Delete_Array(m_szInfo);
	m_szInfo = CUtile::Create_String(to_wstring(m_iRotMax).c_str());

}

HRESULT CUI_NodeRotCnt::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeRotCnt::Initialize(void * pArg)
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

void CUI_NodeRotCnt::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;


	__super::Tick(fTimeDelta);
}

void CUI_NodeRotCnt::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Tick(fTimeDelta);
}

HRESULT CUI_NodeRotCnt::Render()
{
	if (nullptr == m_pTextureCom[TEXTURE_DIFFUSE])
		return E_FAIL;

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
	_float2 vNewPos = { vPos.x + g_iWinSizeX*0.5f + 80.f, g_iWinSizeY*0.5f - vPos.y - 20.f };

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if(m_szInfo != nullptr)
	{
		pGameInstance->Render_Font(TEXT("Font_Basic0"), m_szInfo,
				vNewPos /* position */,
				0.f, _float2(0.8f, 0.8f)/* size */,
				XMVectorSet(1.f, 1.f, 1.f, 1.f)/* color */);
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_NodeRotCnt::SetUp_Components()
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

HRESULT CUI_NodeRotCnt::SetUp_ShaderResources()
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

CUI_NodeRotCnt * CUI_NodeRotCnt::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeRotCnt*	pInstance = new CUI_NodeRotCnt(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeRotCnt");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeRotCnt::Clone(void * pArg)
{
	CUI_NodeRotCnt*	pInstance = new CUI_NodeRotCnt(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeRotCnt");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeRotCnt::Free()
{
	__super::Free();

	Safe_Delete_Array(m_szInfo);
}
