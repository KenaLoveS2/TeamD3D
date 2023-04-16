#include "stdafx.h"
#include "UI_NodeReward.h"
#include "GameInstance.h"
#include "Effect_Particle_Base.h"

CUI_NodeReward::CUI_NodeReward(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Node(pDevice, pContext)
	, m_pParticle(nullptr)
	, m_bOpen(false)
	, m_fTime(0.f)
	, m_fTimeAcc(0.f)
{
}

CUI_NodeReward::CUI_NodeReward(const CUI_NodeReward& rhs)
	: CUI_Node(rhs)
	, m_pParticle(nullptr)
	, m_bOpen(false)
	, m_fTime(0.f)
	, m_fTimeAcc(0.f)
{
}

HRESULT CUI_NodeReward::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeReward::Initialize(void* pArg)
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

	/* Ready Effect */
	_tchar* cloneTag = CUtile::Create_StringAuto(L"Particle_Reward");
	m_pParticle =
		static_cast<CEffect_Particle_Base*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_Effect_Particle_Base", cloneTag, cloneTag));
	if (m_pParticle == nullptr)
		MSG_BOX("failed to create effect : bossHP");

	return S_OK;
}

void CUI_NodeReward::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (!m_bOpen)
	{
		m_fTimeAcc += fTimeDelta;
		if (m_fTimeAcc > 1.f)
		{
			m_bOpen = true;

			m_iTextureIdx = 1;
			_float4 vPos = m_pTransformCom->Get_Position();
			vPos.z = 0.f;
			m_pParticle->Activate(vPos);
		}
	}

	__super::Tick(fTimeDelta);

	//if (ImGui::Button("particle on"))
	//{
	//	m_pParticle->Activate(this);
	//	//m_pParticle->Activate_BufferUpdate();
	//}
	m_pParticle->Imgui_RenderProperty();

	m_pParticle->Tick(fTimeDelta);
}

void CUI_NodeReward::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	CUI::Late_Tick(fTimeDelta);

	/* Calculate with Parent(Canvas) WorldMatrix (Scale, Translation) */
	if (m_pParent != nullptr)
	{
		_float4x4 matWorldParent;
		XMStoreFloat4x4(&matWorldParent, m_pParent->Get_WorldMatrix());

		_matrix matParentTrans = XMMatrixTranslation(matWorldParent._41, matWorldParent._42, matWorldParent._43);

		float fRatioX = matWorldParent._11 / m_matParentInit._11;
		float fRatioY = matWorldParent._22 / m_matParentInit._22;
		_matrix matParentScale = XMMatrixScaling(fRatioX, fRatioY, 1.f);
		_smatrix matWorld = m_matLocal * matParentScale * matParentTrans;
		m_pTransformCom->Set_WorldMatrix(matWorld);
	}

	for (auto e : m_vecEvents)
		e->Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && m_bActive)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UIHDR, this);

	m_pParticle->Late_Tick(fTimeDelta);

}

HRESULT CUI_NodeReward::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeReward::SetUp_Components()
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

HRESULT CUI_NodeReward::SetUp_ShaderResources()
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

CUI_NodeReward* CUI_NodeReward::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_NodeReward* pInstance = new CUI_NodeReward(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeReward");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_NodeReward::Clone(void* pArg)
{
	CUI_NodeReward* pInstance = new CUI_NodeReward(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeReward");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeReward::Free()
{
	__super::Free();
	Safe_Release(m_pParticle);
}
