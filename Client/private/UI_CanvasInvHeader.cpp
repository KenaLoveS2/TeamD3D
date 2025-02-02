#include "stdafx.h"
#include "..\public\UI_CanvasInvHeader.h"
#include "GameInstance.h"
#include "Kena.h"
#include "Kena_Status.h"
#include "Camera_Player.h"
#include "UI_NodeCrystal.h"
#include "UI_NodeKarma.h"
#include "UI_NodeNumRots.h"


CUI_CanvasInvHeader::CUI_CanvasInvHeader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Canvas(pDevice, pContext)
	, m_pPlayer(nullptr)
	, m_bFirstOpen(false)
{
}

CUI_CanvasInvHeader::CUI_CanvasInvHeader(const CUI_CanvasInvHeader & rhs)
	: CUI_Canvas(rhs)
	, m_pPlayer(nullptr)
	, m_bFirstOpen(false)
{
}

HRESULT CUI_CanvasInvHeader::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CanvasInvHeader::Initialize(void * pArg)
{
	/* Get a Texture Size, and Make an Initial Matrix */
	XMStoreFloat4x4(&m_matInit, XMMatrixScaling((_float)g_iWinSizeX, 70.f, 1.f));

	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3((_float)g_iWinSizeX, 70.f, 1.f));
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components : CanvasInvHeader");
		return E_FAIL;
	}


	if (FAILED(Ready_Nodes()))
	{
		MSG_BOX("Failed To Ready Nodes : CanvasInvHeader");
		return E_FAIL;
	}

	//m_bActive = true;

	return S_OK;
}

void CUI_CanvasInvHeader::Tick(_float fTimeDelta)
{
	if (!m_bBindFinished)
	{
		if (FAILED(Bind()))
			return;
	}

	if (!m_bActive)
		return;

	/* Return To Play */
	if (CGameInstance::GetInstance()->Key_Down(DIK_M))
	{
		m_pRendererCom->Set_CaptureMode(false);
		CGameInstance::GetInstance()->Get_Back();
		m_bActive = false;
		CUI_ClientManager::GetInstance()->Get_Canvas(CUI_ClientManager::CANVAS_UPGRADE)->Set_Active(false);
		CUI_ClientManager::GetInstance()->Get_Canvas(CUI_ClientManager::CANVAS_HUD)->Set_Active(true);
		CUI_ClientManager::GetInstance()->Get_Canvas(CUI_ClientManager::CANVAS_AMMO)->Set_Active(true);
		CUI_ClientManager::GetInstance()->Get_Canvas(CUI_ClientManager::CANVAS_QUEST)->Set_Active(true);

		if (nullptr != m_pPlayer->Get_CameraPlayer())
			m_pPlayer->Get_CameraPlayer()->Set_MouseFix(true);


		if (!m_bFirstOpen && g_LEVEL == LEVEL_TESTPLAY)
		{
			if (false == m_pPlayer->Get_Status()->Get_SkillState(CKena_Status::SKILL_BOMB, 0))
			{
				/* Quest 0 - 3 Clear */
				CUI_ClientManager::UI_PRESENT tag = CUI_ClientManager::QUEST_CLEAR;
				_bool bStart = true;
				_float fIdx3 = 3.f;
				wstring wstr = L"";
				m_pPlayer->m_PlayerQuestDelegator.broadcast(tag, bStart, fIdx3, wstr);
				//CGameInstance::GetInstance()->Play_Sound(L"clear.ogg", 1.f, false, SOUND_UI);

			}
			m_bFirstOpen = true;
		}


		return;
	}

	if (nullptr != m_pPlayer)
	{
		static_cast<CUI_NodeKarma*>(m_vecNode[UI_KARMA])->Set_Karma(m_pPlayer->Get_Status()->Get_Karma());
		static_cast<CUI_NodeNumRots*>(m_vecNode[UI_NUMROTS])->Set_NumRots(m_pPlayer->Get_Status()->Get_RotCount());
		static_cast<CUI_NodeCrystal*>(m_vecNode[UI_CRYSTAL])->Set_Crystal(m_pPlayer->Get_Status()->Get_Crystal());

	}

	__super::Tick(fTimeDelta);
}

void CUI_CanvasInvHeader::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_CanvasInvHeader::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CUI_CanvasInvHeader::Bind()
{
	CKena* pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena"));

	if (pKena == nullptr)
		return E_FAIL;

	pKena->m_PlayerPtrDelegator.bind(this, &CUI_CanvasInvHeader::BindFunction);


	m_bBindFinished = true;
	return S_OK;
}

HRESULT CUI_CanvasInvHeader::Ready_Nodes()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	string KarmaTag = "Node_Karma";
	CUI* pKarma = nullptr;
	CUI::UIDESC tKara;
	_tchar* karmaCloneTag = CUtile::StringToWideChar(KarmaTag);
	tKara.fileName = karmaCloneTag;
	pKarma = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Karma", karmaCloneTag, &tKara));
	if (FAILED(Add_Node(pKarma)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(KarmaTag);
	pGameInstance->Add_String(karmaCloneTag);

	string RotsTag = "Node_NumRots";
	CUI* pRots = nullptr;
	CUI::UIDESC tRots;
	_tchar* RotsCloneTag = CUtile::StringToWideChar(RotsTag);
	tRots.fileName = RotsCloneTag;
	pRots = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_NumRots", RotsCloneTag, &tRots));
	if (FAILED(Add_Node(pRots)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(RotsTag);
	pGameInstance->Add_String(RotsCloneTag);

	string CrysTag = "Node_Crystal";
	CUI* pCrys = nullptr;
	CUI::UIDESC tCrys;
	_tchar* CyrsCloneTag = CUtile::StringToWideChar(CrysTag);
	tCrys.fileName = CyrsCloneTag;
	pCrys = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Crystal", CyrsCloneTag, &tCrys));
	if (FAILED(Add_Node(pCrys)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(CrysTag);
	pGameInstance->Add_String(CyrsCloneTag);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;

}

HRESULT CUI_CanvasInvHeader::SetUp_Components()
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

HRESULT CUI_CanvasInvHeader::SetUp_ShaderResources()
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
		if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
			return E_FAIL;
	}

	if (m_pTextureCom[TEXTURE_MASK] != nullptr)
	{
		if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			return E_FAIL;
	}

	return S_OK;
}

void CUI_CanvasInvHeader::BindFunction(CUI_ClientManager::UI_PRESENT eType, CUI_ClientManager::UI_FUNCTION eFunc, CKena * pPlayer)
{
	if (eType == CUI_ClientManager::INV_)
	{
		m_bActive = true;
		m_pRendererCom->Set_CaptureMode(true);
		CGameInstance::GetInstance()->Set_SingleLayer(g_LEVEL, L"Layer_Canvas");
		CUI_ClientManager::GetInstance()->Get_Canvas(CUI_ClientManager::CANVAS_UPGRADE)->Set_Active(true);
		CUI_ClientManager::GetInstance()->Get_Canvas(CUI_ClientManager::CANVAS_HUD)->Set_Active(false);
		CUI_ClientManager::GetInstance()->Get_Canvas(CUI_ClientManager::CANVAS_AMMO)->Set_Active(false);
		CUI_ClientManager::GetInstance()->Get_Canvas(CUI_ClientManager::CANVAS_AIM)->Set_Active(false);
		CUI_ClientManager::GetInstance()->Get_Canvas(CUI_ClientManager::CANVAS_QUEST)->Set_Active(false);

		m_pPlayer = pPlayer;

		static_cast<CUI_NodeKarma*>(m_vecNode[UI_KARMA])->Set_Karma(m_pPlayer->Get_Status()->Get_Karma());
		static_cast<CUI_NodeNumRots*>(m_vecNode[UI_NUMROTS])->Set_NumRots(m_pPlayer->Get_Status()->Get_RotCount());
		static_cast<CUI_NodeCrystal*>(m_vecNode[UI_CRYSTAL])->Set_Crystal(m_pPlayer->Get_Status()->Get_Crystal());

		CUI_ClientManager::GetInstance()->Get_Canvas(CUI_ClientManager::CANVAS_UPGRADE)->Set_Caller(m_pPlayer);

		if (nullptr != m_pPlayer->Get_CameraPlayer())
			m_pPlayer->Get_CameraPlayer()->Set_MouseFix(false);
	}


}

CUI_CanvasInvHeader * CUI_CanvasInvHeader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_CanvasInvHeader*	pInstance = new CUI_CanvasInvHeader(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_CanvasInvHeader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_CanvasInvHeader::Clone(void * pArg)
{
	CUI_CanvasInvHeader*	pInstance = new CUI_CanvasInvHeader(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_CanvasInvHeader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CanvasInvHeader::Free()
{
	__super::Free();
}
