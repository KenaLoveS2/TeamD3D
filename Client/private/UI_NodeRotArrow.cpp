#include "stdafx.h"
#include "..\public\UI_NodeRotArrow.h"
#include "GameInstance.h"
#include "UI_Canvas.h"
#include "UI_Event_Fade.h"

CUI_NodeRotArrow::CUI_NodeRotArrow(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
	, m_iRots(0)
	, m_szInfo(nullptr)
{
}

CUI_NodeRotArrow::CUI_NodeRotArrow(const CUI_NodeRotArrow & rhs)
	:CUI_Node(rhs)
	, m_iRots(0)
	, m_szInfo(nullptr)
{
}

void CUI_NodeRotArrow::Set_Info(_int iRots)
{
	m_vecEvents[EVENT_FADE]->Call_Event(true);

	m_iRots = iRots;

	Safe_Delete_Array(m_szInfo);
	m_szInfo = CUtile::Create_String(to_wstring(m_iRots).c_str());
}

void CUI_NodeRotArrow::Set_Arrow(_float fX)
{
	m_matLocal._41 = fX;
}

HRESULT CUI_NodeRotArrow::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeRotArrow::Initialize(void * pArg)
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

	m_vecEvents.push_back(CUI_Event_Fade::Create(0.05f, 4.f));
	return S_OK;
}

void CUI_NodeRotArrow::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	/* Move From Current Position To Dest Position */
	if (nullptr != m_pParent)
	{
		//static_cast<CUI_Canvas*>(m_pParent)->node
	}


	__super::Tick(fTimeDelta);
}

void CUI_NodeRotArrow::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeRotArrow::Render()
{
	if (nullptr == m_pTextureCom[TEXTURE_DIFFUSE])
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	//if (FAILED(SetUp_ShaderResources()))
	//{
	//	MSG_BOX("Failed To Setup ShaderResources : CUI_NodeSkillName");
	//	return E_FAIL;
	//}

	//m_pShaderCom->Begin(m_iRenderPass);
	//m_pVIBufferCom->Render();

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	_float2 vNewPos = { vPos.x + g_iWinSizeX*0.5f-13.f, g_iWinSizeY*0.5f - vPos.y};

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	Safe_Delete_Array(m_szInfo);
	m_szInfo = CUtile::Create_String(to_wstring(m_iRots).c_str());
	if (m_szInfo != nullptr)
	{
		pGameInstance->Render_Font(TEXT("Font_Basic0"), m_szInfo,
			vNewPos /* position */,
			0.f, _float2(1.f, 1.f)/* size */,
			XMVectorSet(1.f, 1.f, 1.f, static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->Get_Alpha())/* color */);
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_NodeRotArrow::SetUp_Components()
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

HRESULT CUI_NodeRotArrow::SetUp_ShaderResources()
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

	//_float fAlpha = 1.f;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
	//	return E_FAIL;
	_float4 vColor = { 1.f, 1.f, 1.f, 1.f };
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CUI_NodeRotArrow * CUI_NodeRotArrow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeRotArrow*	pInstance = new CUI_NodeRotArrow(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeRotArrow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeRotArrow::Clone(void * pArg)
{
	CUI_NodeRotArrow*	pInstance = new CUI_NodeRotArrow(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeRotArrow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeRotArrow::Free()
{
	__super::Free();

	Safe_Delete_Array(m_szInfo);
}
