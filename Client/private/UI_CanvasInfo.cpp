#include "stdafx.h"
#include "UI_CanvasInfo.h"
#include "GameInstance.h"
#include "UI_NodeVideo.h"
#include "PostFX.h"
#include "WorldTrigger.h"
#include "Kena.h"

CUI_CanvasInfo::CUI_CanvasInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Canvas(pDevice, pContext)
{
}

CUI_CanvasInfo::CUI_CanvasInfo(const CUI_CanvasInfo& rhs)
	:CUI_Canvas(rhs)
{
}

HRESULT CUI_CanvasInfo::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CanvasInfo::Initialize(void* pArg)
{
	/* Get a Texture Size, and Make an Initial Matrix */
	XMStoreFloat4x4(&m_matInit, XMMatrixScaling(520.f, 840.f, 1.f));

	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(520.f, 840.f, 1.f));
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components : CanvasInfo");
		return E_FAIL;
	}

	if (FAILED(Ready_Nodes()))
	{
		MSG_BOX("Failed To Ready Nodes : CanvasInfo");
		return E_FAIL;
	}

	/*temp*/
	//m_bActive = true;

	return S_OK;
}

void CUI_CanvasInfo::Tick(_float fTimeDelta)
{
	if (!m_bBindFinished)
	{
		if (FAILED(Bind()))
		{
			//	MSG_BOX("Bind Failed");
			return;
		}
	}
	//m_bActive = true;

	static _float fIndex = 0.f;
	if(CPostFX::GetInstance()->Get_Capture() && !m_bCapture)
	{
		BindFunction(CUI_ClientManager::INFO_, fIndex);
		m_bCapture = true;
		fIndex += 1.f;
		if (fIndex > (_float)INFO_END)
			fIndex = 0.f;
	}

	if (!m_bActive)		
		return;

	__super::Tick(fTimeDelta);
}

void CUI_CanvasInfo::Late_Tick(_float fTimeDelta)
{
	if (!CPostFX::GetInstance()->Get_Capture())
		m_pRendererCom->Set_CaptureMode(false);

	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);

	/* Return To Play */
	if (CGameInstance::GetInstance()->Key_Down(DIK_SPACE))
	{
		CGameInstance::GetInstance()->Get_Back();
		CPostFX::GetInstance()->CaptureOff();
		CUI_Canvas* pCanvas = CUI_ClientManager::GetInstance()->Get_Canvas(CUI_ClientManager::CANVAS_AMMO);
		if (pCanvas != nullptr)
			pCanvas->Set_Active(true);
		m_bActive = false;
		m_bCapture = false;

		if (m_iTextureIdx == INFO_PORTAL)
		{
			/* Quest 1 - 2 Clear */
			CKena* pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena"));
			if (pKena != nullptr)
			{
				CUI_ClientManager::UI_PRESENT tag = CUI_ClientManager::QUEST_CLEAR;
				_bool bStart = true;
				_float fIdx = 2;
				wstring wstr = L"";
				pKena->m_PlayerQuestDelegator.broadcast(tag, bStart, fIdx, wstr);
				CGameInstance::GetInstance()->Play_Sound(L"clear.ogg", 1.f, false, SOUND_UI);
			}
		}
	}
}

HRESULT CUI_CanvasInfo::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CUI_CanvasInfo::Bind()
{
	CWorldTrigger* p_world_trigger = dynamic_cast<CWorldTrigger*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL,
		L"Layer_Effect", L"UIWorldTrigger"));
	if (p_world_trigger == nullptr)
		return E_FAIL;
	p_world_trigger->m_WorldTriggerDelegator.bind(this, &CUI_CanvasInfo::BindFunction);
	m_bBindFinished = true;
	return S_OK;
}

HRESULT CUI_CanvasInfo::Ready_Nodes()
{
	/* Video */
	{
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;
		string strCloneTag = "Node_InfoVideo";
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Node_Video", wstrCloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCloneTag);
		CGameInstance::GetInstance()->Add_String(wstrCloneTag);
	}

	return S_OK;
}

HRESULT CUI_CanvasInfo::SetUp_Components()
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

HRESULT CUI_CanvasInfo::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CUI::SetUp_ShaderResources();

	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
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
		if(FAILED(m_pShaderCom->Set_ShaderResourceView("g_LDRTexture", m_pRendererCom->Get_LDRTexture())))
			return E_FAIL;
	}

	return S_OK;
}

void CUI_CanvasInfo::BindFunction(CUI_ClientManager::UI_PRESENT eType, _float fValue)
{
	if(eType == CUI_ClientManager::INFO_)
	{
		m_bActive = true;
		//CGameInstance::GetInstance()->Set_SingleLayer(g_LEVEL, L"Layer_Canvas");
		CUI_Canvas* pCanvas = CUI_ClientManager::GetInstance()->Get_Canvas(CUI_ClientManager::CANVAS_AMMO);
		if(pCanvas != nullptr)
			pCanvas->Set_Active(false);

		_int iIndex = (_int)fValue;
		m_iTextureIdx = iIndex;
		switch (_int(fValue))
		{
		case INFO_FIGHTIN:
			static_cast<CUI_NodeVideo*>(m_vecNode[UI_VIDEO])->Play_Video(L"IntoTheFray", true, 0.05f /* speed */);
			break;
		case INFO_FLOWER:
			static_cast<CUI_NodeVideo*>(m_vecNode[UI_VIDEO])->Play_Video(L"HealthFlower", true, 0.05f /* speed */);
			break;
		case INFO_BIND:
			static_cast<CUI_NodeVideo*>(m_vecNode[UI_VIDEO])->Play_Video(L"RotActionSelector", true, 0.05f /* speed */);
			break;
		case INFO_PORTAL:
			static_cast<CUI_NodeVideo*>(m_vecNode[UI_VIDEO])->Play_Video(L"DashAbility", true, 0.05f /* speed */);
			break;
		case INFO_MINIGAME:
			static_cast<CUI_NodeVideo*>(m_vecNode[UI_VIDEO])->Play_Video(L"RusuFalls", true, 0.05f /* speed */);
			break;
		case INFO_ROTCARRY:
			static_cast<CUI_NodeVideo*>(m_vecNode[UI_VIDEO])->Play_Video(L"RotCarry", true, 0.05f /* speed */);
			break;
		}

	}
}

CUI_CanvasInfo* CUI_CanvasInfo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_CanvasInfo* pInstance = new CUI_CanvasInfo(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_CanvasInfo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_CanvasInfo::Clone(void* pArg)
{
	CUI_CanvasInfo* pInstance = new CUI_CanvasInfo(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_CanvasInfo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CanvasInfo::Free()
{
	__super::Free();
}
