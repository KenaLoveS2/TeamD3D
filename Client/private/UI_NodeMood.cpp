#include "stdafx.h"
#include "UI_NodeMood.h"
#include "GameInstance.h"
#include "UI_Event_Fade.h"

CUI_NodeMood::CUI_NodeMood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Node(pDevice, pContext)
	, m_eState(STATE_END)
	, m_vColor(_float4{1.f, 1.f, 1.f, 1.f})
{
}

CUI_NodeMood::CUI_NodeMood(const CUI_NodeMood& rhs)
	: CUI_Node(rhs)
	, m_eState(STATE_END)
	, m_vColor(_float4{ 1.f, 1.f, 1.f, 1.f })
{
}

HRESULT CUI_NodeMood::Initialize_Prototype()
{
	return CUI_Node::Initialize_Prototype();
}

HRESULT CUI_NodeMood::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled({ _float(g_iWinSizeX), _float(g_iWinSizeY), 1.f });
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components : CUI_NodeLvUp");
		return E_FAIL;
	}

	//m_bActive = true;

	/* event */
	m_vecEvents.push_back(CUI_Event_Fade::Create(0.08f, 0.5f));

	return S_OK;
}

HRESULT CUI_NodeMood::Late_Initialize(void* pArg)
{
	return S_OK;
}

void CUI_NodeMood::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	if (ImGui::Button("Recompile"))
	{
		m_pRendererCom->ReCompile();
		m_pShaderCom->ReCompile();
	}

	{
		static bool alpha_preview = true;
		static bool alpha_half_preview = false;
		static bool drag_and_drop = true;
		static bool options_menu = true;
		static bool hdr = false;

		ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

		static bool   ref_color = false;
		static ImVec4 ref_color_v(1.0f, 1.0f, 1.0f, 1.0f);

		static _float4 vSelectColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		vSelectColor = m_vColor;

		if (ImGui::ColorPicker4("CurColor##4", (float*)&vSelectColor, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color ? &ref_color_v.x : NULL))
			m_vColor = vSelectColor;
		if (ImGui::ColorEdit4("Diffuse##2f", (float*)&vSelectColor, ImGuiColorEditFlags_DisplayRGB | misc_flags))
			m_vColor = vSelectColor;

		_float4 vColor = m_vColor;
		ImGui::InputFloat3("color%", (float*)&vColor, "%.3f", ImGuiInputTextFlags_ReadOnly);
	}
#endif


	if (!m_bActive)
		return;

	if (static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->Is_End())
		m_bActive = false;



	__super::Tick(fTimeDelta);
}

void CUI_NodeMood::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeMood::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CUI_NodeMood::MoodOn(STATE eState)
{
	m_bActive = true;
	m_eState = eState;

	switch(m_eState)
	{
	case STATE_HIT:
		m_vColor = _float4{0.388f, 0.024f, 0.024f, 1.0f};
		m_iTextureIdx = 0;
		break;
	case STATE_PARRY:
		m_vColor = _float4{ 0.439f, 0.662f, 0.914f, 1.0f };
		m_iTextureIdx = 0;
		break;
	case STATE_HEAL:
		m_vColor = _float4{ 0.835f, 0.883f, 0.883f, 1.0f };
		m_iTextureIdx = 0;
		break;
	case STATE_FADE:
		m_vColor = _float4{ 0.f, 0.f, 0.f , 1.0f };
		m_iTextureIdx = 1;
		break;
	case STATE_DAZZLE:
		m_vColor = _float4{ 1.f, 1.f, 1.f , 1.0f };
		m_iTextureIdx = 1;
		break;

	}

	static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->Change_Data(0.08f, 0.5f);
	m_vecEvents[EVENT_FADE]->Call_Event(true);

}

HRESULT CUI_NodeMood::SetUp_Components()
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

HRESULT CUI_NodeMood::SetUp_ShaderResources()
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
		if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_Texture",m_iTextureIdx)))
			return E_FAIL;
	}

	if (m_pTextureCom[TEXTURE_MASK] != nullptr)
	{
		if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	_float fAmount = 0.6f; /* Max Alpha Control */
	if (FAILED(m_pShaderCom->Set_RawValue("g_fAmount", &fAmount, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CUI_NodeMood* CUI_NodeMood::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_NodeMood* pInstance = new CUI_NodeMood(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeMood");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_NodeMood::Clone(void* pArg)
{
	CUI_NodeMood* pInstance = new CUI_NodeMood(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeMood");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeMood::Free()
{
	__super::Free();
}
