#include "stdafx.h"
#include "UI_NodeTimeAtk.h"
#include "GameInstance.h"

CUI_NodeTimeAtk::CUI_NodeTimeAtk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Node(pDevice, pContext)
	, m_szTitle(nullptr)
	, m_bStart(false)
	, m_fTime(0.0f)
	, m_fTimeAcc(0.0f)
{
}

CUI_NodeTimeAtk::CUI_NodeTimeAtk(const CUI_NodeTimeAtk& rhs)
	: CUI_Node(rhs)
	, m_szTitle(nullptr)
	, m_bStart(false)
	, m_fTime(0.0f)
	, m_fTimeAcc(0.0f)
{
}

HRESULT CUI_NodeTimeAtk::Initialize_Prototype()
{
	return CUI_Node::Initialize_Prototype();
}

HRESULT CUI_NodeTimeAtk::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled({ 400.f, 200.f, 1.f });
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components : CUI_NodeTimeAtk");
		return E_FAIL;
	}

	//m_bActive = true;
	return S_OK;
}

HRESULT CUI_NodeTimeAtk::Late_Initialize(void* pArg)
{
	return S_OK;
}

void CUI_NodeTimeAtk::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (m_bStart)
	{
		m_fTimeAcc += fTimeDelta;
		if (m_fTimeAcc > 1.f)
		{
			m_fTime += 1.f;
			_int iMin = (_int)m_fTime / 60;
			_int iSec = (_int)m_fTime - iMin * 60;

			wstring minute = to_wstring(iMin);
			if (minute.length() < 2)
				minute = L"0" + minute;
			wstring second = to_wstring(iSec);
			if (second.length() < 2)
				second = L"0" + second;

			wstring wstr = minute + L" : " + second;

			Safe_Delete_Array(m_szTitle);
			m_szTitle = CUtile::Create_String(wstr.c_str());

			m_fTimeAcc = 0.f;
		}
	}


	__super::Tick(fTimeDelta);
}

void CUI_NodeTimeAtk::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeTimeAtk::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	{
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		_float2 vNewPos = { vPos.x + g_iWinSizeX * 0.5f - 60.0f, g_iWinSizeY * 0.5f - vPos.y - 15.f };
		if (m_szTitle != nullptr)
		{
			//Font_Jangmi0 //Font_JungE0 // Font_SR0
			CGameInstance::GetInstance()->Render_Font(TEXT("Font_SR0"), m_szTitle,
				vNewPos /* position */,
				0.f, _float2(0.7f, 0.7f)/* size */,
				XMVectorSet(1.f, 1.f, 1.f, 1.f));
		}
	}

	return S_OK;
}

HRESULT CUI_NodeTimeAtk::SetUp_Components()
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

HRESULT CUI_NodeTimeAtk::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

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

	return S_OK;
}

CUI_NodeTimeAtk* CUI_NodeTimeAtk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_NodeTimeAtk* pInstance = new CUI_NodeTimeAtk(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeTimeAtk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_NodeTimeAtk::Clone(void* pArg)
{
	CUI_NodeTimeAtk* pInstance = new CUI_NodeTimeAtk(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeTimeAtk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeTimeAtk::Free()
{
	__super::Free();
	Safe_Delete_Array(m_szTitle);
}
