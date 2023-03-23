#include "stdafx.h"
#include "..\public\UI_NodeBossHP.h"
#include "GameInstance.h"
#include "UI_Event_Guage.h"

CUI_NodeBossHP::CUI_NodeBossHP(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
	, m_szTitle(nullptr)
{
}

CUI_NodeBossHP::CUI_NodeBossHP(const CUI_NodeBossHP & rhs)
	: CUI_Node(rhs)
	, m_szTitle(nullptr)
{
}

void CUI_NodeBossHP::Set_Title(wstring wstrName)
{
	m_bActive = true;

	Safe_Delete_Array(m_szTitle);

	m_szTitle = CUtile::Create_String(wstrName.c_str());
}

void CUI_NodeBossHP::Set_Guage(_float fGuage)
{
	m_vecEvents[EVENT_GUAGE]->Call_Event(fGuage);
}

HRESULT CUI_NodeBossHP::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeBossHP::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(275.f, 23.f, 1.f));
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}

	//m_bActive = true;

	/* Events */
	UIDESC* tDesc = (UIDESC*)pArg;
	m_vecEvents.push_back(CUI_Event_Guage::Create(tDesc->fileName));

	return S_OK;
}

void CUI_NodeBossHP::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (CGameInstance::GetInstance()->Key_Down(DIK_P))
	{
		static _float fGuage = 1.f;
		fGuage -= 0.1f;
		m_vecEvents[EVENT_GUAGE]->Call_Event(fGuage);
	}

	__super::Tick(fTimeDelta);
}

void CUI_NodeBossHP::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeBossHP::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	_float2 vNewPos = { vPos.x + g_iWinSizeX*0.5f - 50.f, g_iWinSizeY*0.5f - vPos.y - 60.f };

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_szTitle != nullptr)
	{
		pGameInstance->Render_Font(TEXT("Font_Basic0"), m_szTitle,
			vNewPos /* position */,
			0.f, _float2(1.f, 1.f)/* size */,
			XMVectorSet(1.f, 1.f, 1.f, 1.f)/* color */);
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_NodeBossHP::SetUp_Components()
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

HRESULT CUI_NodeBossHP::SetUp_ShaderResources()
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

	_float fAlpha = 1.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CUI_NodeBossHP * CUI_NodeBossHP::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeBossHP*	pInstance = new CUI_NodeBossHP(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeBossHP");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeBossHP::Clone(void * pArg)
{
	CUI_NodeBossHP*	pInstance = new CUI_NodeBossHP(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeBossHP");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeBossHP::Free()
{
	__super::Free();
	Safe_Delete_Array(m_szTitle);
}
