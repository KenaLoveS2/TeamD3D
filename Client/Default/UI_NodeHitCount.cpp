#include "stdafx.h"
#include "UI_NodeHitCount.h"
#include "GameInstance.h"
#include "Kena.h"
CUI_NodeHitCount::CUI_NodeHitCount(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Node(pDevice, pContext)
	, m_szTitle(nullptr)
	, m_iNumTargets(0)
	, m_iNumHits(0)
	, m_fTime(0.f)
	, m_fTimeAcc(0.f)
	, m_bFinished(false)
{
}

CUI_NodeHitCount::CUI_NodeHitCount(const CUI_NodeHitCount& rhs)
	: CUI_Node(rhs)
	, m_szTitle(nullptr)
	, m_iNumTargets(0)
	, m_iNumHits(0)
	, m_fTime(0.f)
	, m_fTimeAcc(0.f)
	, m_bFinished(false)
{
}

void CUI_NodeHitCount::Counting()
{
	CKena* pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena"));

	if (pKena != nullptr)
		m_iNumHits = pKena->Get_Hits();
	
	Safe_Delete_Array(m_szTitle);

	wstring hit = to_wstring(m_iNumHits);
	if (hit.length() < 2)
		hit = L"0" + hit;
	wstring targets = to_wstring(m_iNumTargets);
	if (targets.length() < 2)
		targets = L"0" + targets;
	wstring wstr = hit + L" / " + targets;
	m_szTitle = CUtile::Create_String(wstr.c_str());

	m_fTimeAcc = 0.f;
}

HRESULT CUI_NodeHitCount::Initialize_Prototype()
{
	return CUI_Node::Initialize_Prototype();
}

HRESULT CUI_NodeHitCount::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled({ 400.f, 200.f, 1.f });
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components : CUI_NodeLvUp");
		return E_FAIL;
	}

	m_iNumTargets = 19;
	//m_bActive = true;
	return S_OK;
}

HRESULT CUI_NodeHitCount::Late_Initialize(void* pArg)
{
	return S_OK;
}

void CUI_NodeHitCount::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	Counting();


	__super::Tick(fTimeDelta);
}

void CUI_NodeHitCount::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeHitCount::Render()
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

HRESULT CUI_NodeHitCount::SetUp_Components()
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

HRESULT CUI_NodeHitCount::SetUp_ShaderResources()
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

CUI_NodeHitCount* CUI_NodeHitCount::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_NodeHitCount* pInstance = new CUI_NodeHitCount(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeHitCount");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_NodeHitCount::Clone(void* pArg)
{
	CUI_NodeHitCount* pInstance = new CUI_NodeHitCount(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeHitCount");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeHitCount::Free()
{
	__super::Free();
	Safe_Delete_Array(m_szTitle);
}
