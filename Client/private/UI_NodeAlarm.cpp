#include "stdafx.h"
#include "UI_NodeAlarm.h"
#include "GameInstance.h"
#include "UI_Event_Fade.h"

CUI_NodeAlarm::CUI_NodeAlarm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Node(pDevice, pContext)
	, m_fTime(0.0f)
	, m_fTimeAcc(0.0f)
	, m_fTerm(0.0f)
	, m_szTitle(nullptr)
	, m_szDesc(nullptr)
{
}

CUI_NodeAlarm::CUI_NodeAlarm(const CUI_NodeAlarm& rhs)
	:CUI_Node(rhs)
	, m_fTime(0.0f)
	, m_fTimeAcc(0.0f)
	, m_fTerm(0.0f)
	, m_szTitle(nullptr)
	, m_szDesc(nullptr)
{
}

void CUI_NodeAlarm::Set_Alarm(TYPE eType, _float fTerm)
{
	if (false == m_bActive)
	{
		m_bActive = true;

		Safe_Delete_Array(m_szTitle);
		Safe_Delete_Array(m_szDesc);

		switch (eType)
		{
		case TYPE_CHEST:
			m_szTitle = CUtile::Create_String(L"보물 상자");
			m_szDesc = CUtile::Create_String(L"누군가의 전리품을 획득");
			break;
		case TYPE_HAT:
			m_szTitle = CUtile::Create_String(L"신상 모자");
			m_szDesc = CUtile::Create_String(L"새로운 모자를 구매");
			break;
		}

		m_matLocal._41 += m_fIntervalX;
		m_fTimeAcc = 0.0f;
		m_fTerm = fTerm;
		m_iTextureIdx = eType;
		static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->Change_Data(0.05f, 2.f);
		m_vecEvents[EVENT_FADE]->Call_Event(true);

	}
}

HRESULT CUI_NodeAlarm::Initialize_Prototype()
{
	return CUI_Node::Initialize_Prototype();
}

HRESULT CUI_NodeAlarm::Initialize(void* pArg)
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

	/* event */
	m_vecEvents.push_back(CUI_Event_Fade::Create(0.08f, 2.f));
	m_fIntervalX = -30.f;

	return S_OK;
}

HRESULT CUI_NodeAlarm::Late_Initialize(void* pArg)
{
	return S_OK;
}

void CUI_NodeAlarm::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->Is_End())
		m_bActive = false;

	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc > m_fTerm)
		return;

	if (m_matLocal._41 >= m_matLocalOriginal._41)
		m_matLocal._41 = m_matLocalOriginal._41;
	else
		m_matLocal._41 += 0.5f * (m_fTerm - m_fTimeAcc);

	__super::Tick(fTimeDelta);
}

void CUI_NodeAlarm::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	//if (m_fTimeAcc < m_fTerm)
	//	return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeAlarm::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	{
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		_float2 vNewPos = { vPos.x + g_iWinSizeX * 0.5f - 180.0f, g_iWinSizeY * 0.5f - vPos.y - 20.f};
		//m_szKeyDesc = CUtile::Create_StringAuto(L"부식령 날리기");
		if (m_szTitle != nullptr)
		{
			//Font_Jangmi0 //Font_JungE0 // Font_SR0
			CGameInstance::GetInstance()->Render_Font(TEXT("Font_SR0"), m_szTitle,
				vNewPos /* position */,
				0.f, _float2(0.8f, 0.8f)/* size */,
				XMVectorSet(1.f, 1.f, 1.f,
					static_cast<CUI_Event_Fade*>(m_vecEvents[0])->Get_Alpha())/* color */);
		}
	}

	{
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		_float2 vNewPos = { vPos.x + g_iWinSizeX * 0.5f - 180.0f, g_iWinSizeY * 0.5f - vPos.y + 40.f };
		//m_szKeyDesc = CUtile::Create_StringAuto(L"부식령 날리기");
		if (m_szDesc != nullptr)
		{
			//Font_Jangmi0 //Font_JungE0 // Font_SR0
			CGameInstance::GetInstance()->Render_Font(TEXT("Font_SR0"), m_szDesc,
				vNewPos /* position */,
				0.f, _float2(0.5f, 0.5f)/* size */,
				XMVectorSet(1.f, 1.f, 1.f, 
						static_cast<CUI_Event_Fade*>(m_vecEvents[0])->Get_Alpha())/* color */);
		}
	}
	return S_OK;
}

HRESULT CUI_NodeAlarm::SetUp_Components()
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

HRESULT CUI_NodeAlarm::SetUp_ShaderResources()
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

CUI_NodeAlarm* CUI_NodeAlarm::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_NodeAlarm* pInstance = new CUI_NodeAlarm(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeAlarm");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_NodeAlarm::Clone(void* pArg)
{
	CUI_NodeAlarm* pInstance = new CUI_NodeAlarm(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeAlarm");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeAlarm::Free()
{
	__super::Free();

	Safe_Delete_Array(m_szTitle);
	Safe_Delete_Array(m_szDesc);
}
