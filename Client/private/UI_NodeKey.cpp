#include "stdafx.h"
#include "UI_NodeKey.h"
#include "GameInstance.h"
#include "UI_Event_Fade.h"

CUI_NodeKey::CUI_NodeKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Node(pDevice, pContext)
	, m_szKeyDesc(nullptr)
	, m_fTime(0.0f)
	, m_fTimeAcc(0.0f)
	, m_fTerm(0.0f)
{
}

CUI_NodeKey::CUI_NodeKey(const CUI_NodeKey& rhs)
	:CUI_Node(rhs)
	, m_szKeyDesc(nullptr)
	, m_fTime(0.0f)
	, m_fTimeAcc(0.0f)
	, m_fTerm(0.0f)
{
}

void CUI_NodeKey::Set_Key(wstring wstr, KEYTYPE eType, _float fTerm)
{
	//m_bActive = false;
	//m_iRenderPass = 3;
	if (false == m_bActive)
	{
		m_bActive = true;

		m_matLocal._41 += m_fIntervalX;
		m_fTimeAcc = 0.0f;
		m_fTerm = fTerm;

		Safe_Delete_Array(m_szKeyDesc);
		m_szKeyDesc = CUtile::Create_String(wstr.c_str());
		m_iTextureIdx = eType;

		static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->Change_Data(0.05f, 50.f);
		m_vecEvents[EVENT_FADE]->Call_Event(true);
	}
}

void CUI_NodeKey::SetOff_Key()
{
	static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->Change_Data(0.1f, 0.f);
	static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->FadeOut();
}

HRESULT CUI_NodeKey::Initialize_Prototype()
{
	return CUI_Node::Initialize_Prototype();
}

HRESULT CUI_NodeKey::Initialize(void* pArg)
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

	m_fIntervalX = -10.f;

	return S_OK;
}

HRESULT CUI_NodeKey::Late_Initialize(void* pArg)
{
	return S_OK;
}

void CUI_NodeKey::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->Is_End())
		m_bActive = false;

	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc < m_fTerm)
		return;

	if (m_matLocal._41 >= m_matLocalOriginal._41)
		m_matLocal._41 = m_matLocalOriginal._41;
	else
		m_matLocal._41 += 1.f * (m_fTimeAcc - m_fTerm);

	__super::Tick(fTimeDelta);
}

void CUI_NodeKey::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (m_fTimeAcc < m_fTerm)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeKey::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	{
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		_float2 vNewPos = { vPos.x + g_iWinSizeX * 0.5f + 30.0f, g_iWinSizeY * 0.5f - vPos.y - 12.0f};
		//m_szKeyDesc = CUtile::Create_StringAuto(L"부식령 날리기");
		if (m_szKeyDesc != nullptr)
		{
			//Font_Jangmi0 //Font_JungE0 // Font_SR0
			CGameInstance::GetInstance()->Render_Font(TEXT("Font_SR0"), m_szKeyDesc,
				vNewPos /* position */,
				0.f, _float2(0.5f, 0.5f)/* size */,
				XMVectorSet(1.f, 1.f, 1.f, 
					static_cast<CUI_Event_Fade*>(m_vecEvents[0])->Get_Alpha())/* color */);
		}
	}

	return S_OK;
}

HRESULT CUI_NodeKey::SetUp_Components()
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

HRESULT CUI_NodeKey::SetUp_ShaderResources()
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

	return S_OK;
}

CUI_NodeKey* CUI_NodeKey::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_NodeKey* pInstance = new CUI_NodeKey(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeKey");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_NodeKey::Clone(void* pArg)
{
	CUI_NodeKey* pInstance = new CUI_NodeKey(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeKey");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeKey::Free()
{
	__super::Free();
	Safe_Delete_Array(m_szKeyDesc);
}
