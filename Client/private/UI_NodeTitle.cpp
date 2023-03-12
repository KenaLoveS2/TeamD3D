#include "stdafx.h"
#include "..\public\UI_NodeTitle.h"
#include "GameInstance.h"

CUI_NodeTitle::CUI_NodeTitle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
	, m_fSpeed(0.f)
	, m_bStart(false)
	, m_fDurTime(0.f)
	, m_fDurTimeAcc(0.f)
	, m_fAlpha(0.f)
	, m_szTitle(nullptr)
{
}

CUI_NodeTitle::CUI_NodeTitle(const CUI_NodeTitle & rhs)
	: CUI_Node(rhs)
	, m_fSpeed(0.f)
	, m_bStart(false)
	, m_fDurTime(0.f)
	, m_fDurTimeAcc(0.f)
	, m_fAlpha(0.f)
	, m_szTitle(nullptr)
{
}

HRESULT CUI_NodeTitle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeTitle::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3((_float)g_iWinSizeX, 100.f, 1.f));
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components : CUI_NodeLetterBox");
		return E_FAIL;
	}

	m_fSpeed = 0.01f;
	m_bActive = true;
	m_fDurTime = 6.f;

	return S_OK;
}

HRESULT CUI_NodeTitle::Late_Initialize(void * pArg)
{

	return S_OK;
}

void CUI_NodeTitle::Tick(_float fTimeDelta)
{
	if (CGameInstance::GetInstance()->Key_Down(DIK_L))
		Appear(wstring{ L"루수의 산" }, 1);

	if (!m_bActive)
		return;

	if (m_bStart)
	{
		m_fAlpha += m_fSpeed;
		if (m_fSpeed > 0.0f)
		{
			if (m_fAlpha >= 1.0f) /* Appear Ended */
			{
				m_fAlpha = 1.0f;
				m_bStart = false;
				m_fDurTimeAcc = 0.0f;
			}
		}
		else 
		{
			if (m_fAlpha <= 0.0f) /* Disappear Ended */
			{
				m_fAlpha = 0.0f;
				m_bStart = false;
				m_fDurTimeAcc = 0.0f;
				m_bActive = false;
				m_fSpeed = -m_fSpeed;
			}
		}

	}
	else
	{
		m_fDurTimeAcc += fTimeDelta;
		if (m_fDurTimeAcc > m_fDurTime)
		{
			m_bStart = true;
			m_fSpeed = -m_fSpeed;
		}
	}



	__super::Tick(fTimeDelta);
}

void CUI_NodeTitle::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);

}

HRESULT CUI_NodeTitle::Render()
{
	if (nullptr == m_pTextureCom[TEXTURE_DIFFUSE])
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
	{
		MSG_BOX("Failed To Setup ShaderResources : CUI_NodeSkill");
		return E_FAIL;
	}

	m_pShaderCom->Begin(m_iRenderPass);
	m_pVIBufferCom->Render();

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	_float2 vNewPos = { vPos.x + g_iWinSizeX*0.5f - 90.f, g_iWinSizeY*0.5f - vPos.y + 10.f};

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Render_Font(TEXT("Font_Basic0"), m_szTitle,
		vNewPos /* position */,
		0.f, _float2(1.5f, 1.5f)/* size */,
		XMVectorSet(1.f ,1.f, 1.f, m_fAlpha)/* color */);
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CUI_NodeTitle::Appear(wstring title, _uint iIndex)
{
	Safe_Delete_Array(m_szTitle);
	m_szTitle = CUtile::Create_String(title.c_str());
	m_iTextureIdx = iIndex;


	m_bActive = true;
	m_bStart = true;
	m_fSpeed = 0.01f;

}

HRESULT CUI_NodeTitle::SetUp_Components()
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

HRESULT CUI_NodeTitle::SetUp_ShaderResources()
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
		if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIdx)))
			return E_FAIL;
	}

	if (m_pTextureCom[TEXTURE_MASK] != nullptr)
	{
		if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CUI_NodeTitle * CUI_NodeTitle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeTitle*	pInstance = new CUI_NodeTitle(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeTitle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeTitle::Clone(void * pArg)
{
	CUI_NodeTitle*	pInstance = new CUI_NodeTitle(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeTitle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeTitle::Free()
{
	__super::Free();

	Safe_Delete_Array(m_szTitle);
}
