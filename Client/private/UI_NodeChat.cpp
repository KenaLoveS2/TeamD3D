#include "stdafx.h"
#include "..\public\UI_NodeChat.h"
#include "GameInstance.h"

CUI_NodeChat::CUI_NodeChat(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
	, m_szChat(nullptr)
	, m_fCorrectX(0.f)
	, m_fCorrectY(0.f)
{
}

CUI_NodeChat::CUI_NodeChat(const CUI_NodeChat & rhs)
	: CUI_Node(rhs)
	, m_szChat(nullptr)
	, m_fCorrectX(0.f)
	, m_fCorrectY(0.f)
{
}

void CUI_NodeChat::Set_String(wstring wstr, _float fCorrectY)
{
	Safe_Delete_Array(m_szChat);
	m_szChat = CUtile::Create_String(wstr.c_str());
	
	size_t length = wstr.length();
	m_fCorrectX = 10.f * _float(length-1);

	m_fCorrectY = fCorrectY;
}

HRESULT CUI_NodeChat::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeChat::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(100.f, 100.f, 1.f));
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components : CUI_NodeLetterBox");
		return E_FAIL;
	}
	
	m_matLocal._11 = 0.f;
	m_matLocal._22 = 0.f;

	return S_OK;
}

HRESULT CUI_NodeChat::Late_Initialize(void * pArg)
{
	return S_OK;
}

void CUI_NodeChat::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Tick(fTimeDelta);

}

void CUI_NodeChat::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;
	
	__super::Late_Tick(fTimeDelta);

}

HRESULT CUI_NodeChat::Render()
{
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
	_float2 vNewPos = { vPos.x + g_iWinSizeX*0.5f - m_fCorrectX, g_iWinSizeY*0.5f - vPos.y + m_fCorrectY };

	if (nullptr != m_szChat)
	{
		CGameInstance::GetInstance()->Render_Font(TEXT("Font_Jangmi0"), m_szChat,
			vNewPos /* position */,
			0.f, _float2(0.8f, 0.8f)/* size */,
			{ 1.f, 1.f, 1.f, 1.f }/* color */);
	}

	return S_OK;

}

HRESULT CUI_NodeChat::SetUp_Components()
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

HRESULT CUI_NodeChat::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

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

	return S_OK;
}

CUI_NodeChat * CUI_NodeChat::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeChat*	pInstance = new CUI_NodeChat(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeChat");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeChat::Clone(void * pArg)
{
	CUI_NodeChat*	pInstance = new CUI_NodeChat(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeChat");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeChat::Free()
{
	Safe_Delete_Array(m_szChat);

	__super::Free();
}
