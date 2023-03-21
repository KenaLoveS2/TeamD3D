#include "stdafx.h"
#include "..\public\UI_NodeScrollBar.h"
#include "GameInstance.h"

CUI_NodeScrollBar::CUI_NodeScrollBar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
{
}

CUI_NodeScrollBar::CUI_NodeScrollBar(const CUI_NodeScrollBar & rhs)
	: CUI_Node(rhs)
{
}

HRESULT CUI_NodeScrollBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeScrollBar::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(112.f, 112.f, 1.f));
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

void CUI_NodeScrollBar::Tick(_float fTimeDelta)
{

	POINT pt = CUtile::GetClientCursorPos(g_hWnd);
	_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float2 vPosConvert = { vPos.x + 0.5f*g_iWinSizeX, -vPos.y + 0.5f*g_iWinSizeY };
	// left, top, right, bottom 
	RECT rc = { LONG(vPosConvert.x - 10.f), LONG(vPosConvert.y - m_matLocal._22 * 0.7f),
		LONG(vPosConvert.x + 100.f), LONG(vPosConvert.y + m_matLocal._22 * 0.7f) };

	if (PtInRect(&rc, pt))
	{
		if (CGameInstance::GetInstance()->Mouse_Pressing(DIM_LB))
		{
			if (CGameInstance::GetInstance()->Get_DIMouseMove(DIMS_Y))
			{
				_float vMousePos = 0.5f*g_iWinSizeY - pt.y;

				if (vMousePos < -270.f || vMousePos > 330.f)
					return;

				m_matLocal._42 = vMousePos;

				/* Move Canvas */
				_float fRatio = (330.f - vMousePos) / (330.f + 270.f); 
				_float fY = CUtile::FloatLerp(m_pParent->Get_InitMatrixFloat4x4()._42, 1050.f, fRatio); 
				_float4 vParent = m_pParent->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
				vParent.y = fY;
				m_pParent->Set_Position(vParent);
			}
		}
	}

	__super::Tick(fTimeDelta);
}

void CUI_NodeScrollBar::Late_Tick(_float fTimeDelta)
{
	//__super::Late_Tick(fTimeDelta);
	CUI::Late_Tick(fTimeDelta);

	m_pTransformCom->Set_WorldMatrix_float4x4(m_matLocal);

	for (auto e : m_vecEvents)
		e->Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && m_bActive)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_NodeScrollBar::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeScrollBar::SetUp_Components()
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

HRESULT CUI_NodeScrollBar::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CUI::SetUp_ShaderResources(); /* Events Resourece Setting */

	_float fAlpha = 1.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
		return E_FAIL;

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

CUI_NodeScrollBar * CUI_NodeScrollBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeScrollBar*	pInstance = new CUI_NodeScrollBar(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeScrollBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeScrollBar::Clone(void * pArg)
{
	CUI_NodeScrollBar*	pInstance = new CUI_NodeScrollBar(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeScrollBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeScrollBar::Free()
{
	__super::Free();
}
