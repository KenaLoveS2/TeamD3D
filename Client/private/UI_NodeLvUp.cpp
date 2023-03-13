#include "stdafx.h"
#include "..\public\UI_NodeLvUp.h"
#include "GameInstance.h"
#include "UI_Event_Fade.h"

CUI_NodeLvUp::CUI_NodeLvUp(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
	,m_szTitle(nullptr)
	,m_szLevel(nullptr)
{
}

CUI_NodeLvUp::CUI_NodeLvUp(const CUI_NodeLvUp & rhs)
	:CUI_Node(rhs)
	, m_szTitle(nullptr)
	, m_szLevel(nullptr)
{
}

HRESULT CUI_NodeLvUp::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeLvUp::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled({ 100.f, 100.f, 1.f });
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components : CUI_NodeLvUp");
		return E_FAIL;
	}

	m_bActive = true;
	m_szTitle = CUtile::Create_StringAuto(L"Level");

	/* event */
	m_vecEvents.push_back(CUI_Event_Fade::Create(0.01f, 6.f));

	return S_OK;
}

HRESULT CUI_NodeLvUp::Late_Initialize(void * pArg)
{
	return S_OK;
}

void CUI_NodeLvUp::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (static_cast<CUI_Event_Fade*>(m_vecEvents[0])->Is_Appear())
	{
		m_matLocal._11 -= 0.6f;
		m_matLocal._22 -= 0.6f;
		//m_matLocal._42 -= 0.4f;

		if (m_matLocal._11 < m_matLocalOriginal._11)
		{
			m_matLocal._11 = m_matLocalOriginal._11;
			m_matLocal._22 = m_matLocalOriginal._22;
		}

		//if (m_matLokcal._42 < 280.f)
		//	m_matLocal._42 = 280.f;
	}

	__super::Tick(fTimeDelta);
}

void CUI_NodeLvUp::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeLvUp::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	{
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		_float2 vNewPos = { vPos.x + g_iWinSizeX*0.5f - 40.f, g_iWinSizeY*0.5f - vPos.y + 0.f };

		if (m_szTitle != nullptr)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->Render_Font(TEXT("Font_Comic"), m_szTitle,
				vNewPos /* position */,
				0.f, _float2(0.7f, 0.7f)/* size */,
				XMVectorSet(1.f, 1.f, 1.f, static_cast<CUI_Event_Fade*>(m_vecEvents[0])->Get_Alpha())/* color */);
			RELEASE_INSTANCE(CGameInstance);
		}
	}

	{
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		_float2 vNewPos = { vPos.x + g_iWinSizeX*0.5f -25.f, g_iWinSizeY*0.5f - vPos.y + 20.f };

		if (m_szLevel != nullptr)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->Render_Font(TEXT("Font_Comic"), m_szLevel,
				vNewPos /* position */,
				0.f, _float2(1.3f, 1.3f)/* size */,
				XMVectorSet(1.f, 1.f, 1.f, static_cast<CUI_Event_Fade*>(m_vecEvents[0])->Get_Alpha())/* color */);
			RELEASE_INSTANCE(CGameInstance);
		}
	}

	return S_OK;
}

void CUI_NodeLvUp::Appear(_uint iLevel)
{
	m_vecEvents[0]->Call_Event(true);

	//m_matLocal._42 = 320.f;
	m_matLocal._11 = 1.3f * m_matLocalOriginal._11;
	m_matLocal._22 = 1.3f * m_matLocalOriginal._22;


	Safe_Delete_Array(m_szLevel);
	m_szLevel = CUtile::Create_String(to_wstring(iLevel).c_str());


}

HRESULT CUI_NodeLvUp::SetUp_Components()
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

HRESULT CUI_NodeLvUp::SetUp_ShaderResources()
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

CUI_NodeLvUp * CUI_NodeLvUp::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeLvUp*	pInstance = new CUI_NodeLvUp(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeLvUp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeLvUp::Clone(void * pArg)
{
	CUI_NodeLvUp*	pInstance = new CUI_NodeLvUp(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeLvUp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeLvUp::Free()
{
	Safe_Delete_Array(m_szLevel);
	__super::Free();
}
