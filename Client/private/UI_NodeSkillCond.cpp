#include "stdafx.h"
#include "..\public\UI_NodeSkillCond.h"
#include "GameInstance.h"

CUI_NodeSkillCond::CUI_NodeSkillCond(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice,pContext)
	, m_szCondition(nullptr)
	, m_fFontPos(0.f)
	, m_eCondition(CSkillInfo::CHECK_END)
	, m_vColor{ 1.f, 1.f, 1.f, 1.f }
{
}

CUI_NodeSkillCond::CUI_NodeSkillCond(const CUI_NodeSkillCond & rhs)
	:CUI_Node(rhs)
	, m_szCondition(nullptr)
	, m_fFontPos(0.f)
	, m_eCondition(CSkillInfo::CHECK_END)
	, m_vColor{ 1.f, 1.f, 1.f, 1.f }
{
}

void CUI_NodeSkillCond::Set_Condition(CSkillInfo::SKILLDESC tDesc, CSkillInfo::CHECK eCheck)
{
	wstring msg;

	_float4 vWhite = { 1.f, 1.f, 1.f, 1.f };
	_float4 vRed = { 0.96f,0.5f,0.5f,1.f };

	m_eCondition = eCheck;
	switch (m_eCondition)
	{
	case CSkillInfo::CHECK_PREVSKILL:
		msg = L"선행 능력 필요";
		m_vColor = vRed;
		break;
	case CSkillInfo::CHECK_ROTLEVEL:
		msg = L"부식령 레벨 " + to_wstring(tDesc.conditions[CSkillInfo::CONDITION_ROTLEVEL]) + L" 필요";
		m_vColor = vRed;
		break;
	case  CSkillInfo::CHECK_KARMA:
		msg = L"추가 카르마 필요                              " + to_wstring(tDesc.conditions[CSkillInfo::CONDITION_KARMA]);
		m_vColor = vRed;
		break;
	case CSkillInfo::CHECK_UNLOCKED_AVAILABLE:
		msg = L"업그레이드 가능                               " + to_wstring(tDesc.conditions[CSkillInfo::CONDITION_KARMA]);
		m_vColor = vWhite;
		break;
	case CSkillInfo::CHECK_UNLOCKED_ALREADY:
		msg = L"잠금 해제됨";
		m_vColor = vWhite;
		break;
	default:
		msg = L"돌아가";
		m_vColor = vRed;
		break;
	}

	Safe_Delete_Array(m_szCondition);
	m_szCondition = CUtile::Create_String(msg.c_str());
}

HRESULT CUI_NodeSkillCond::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeSkillCond::Initialize(void * pArg)
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

void CUI_NodeSkillCond::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;


	__super::Tick(fTimeDelta);
}

void CUI_NodeSkillCond::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeSkillCond::Render()
{
	//if (nullptr == m_pTextureCom[TEXTURE_DIFFUSE])
	//	return E_FAIL;

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
	_float2 vNewPos = { vPos.x + g_iWinSizeX*0.5f - 445.f, g_iWinSizeY*0.5f - vPos.y - 20.f};

	if (nullptr != m_szCondition)
	{
		CGameInstance::GetInstance()->Render_Font(TEXT("Font_Basic0"), m_szCondition,
			vNewPos /* position */,
			0.f, _float2(0.8f, 0.8f)/* size */,
			XMLoadFloat4(&m_vColor)/* color */);
	}

	return S_OK;
}

HRESULT CUI_NodeSkillCond::SetUp_Components()
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

HRESULT CUI_NodeSkillCond::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	if (m_pTextureCom[TEXTURE_MASK] != nullptr)
	{
		if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			return E_FAIL;
	}


	_float fAlpha;
	if (m_eCondition == CSkillInfo::CHECK_KARMA || m_eCondition == CSkillInfo::CHECK_UNLOCKED_AVAILABLE)
		fAlpha = 1.f;
	else
		fAlpha = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CUI_NodeSkillCond * CUI_NodeSkillCond::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeSkillCond*	pInstance = new CUI_NodeSkillCond(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeSkillCond");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeSkillCond::Clone(void * pArg)
{
	CUI_NodeSkillCond*	pInstance = new CUI_NodeSkillCond(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeSkillCond");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeSkillCond::Free()
{
	__super::Free();

	Safe_Delete_Array(m_szCondition);
}
