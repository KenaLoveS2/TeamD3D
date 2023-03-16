#include "stdafx.h"
#include "..\public\UI_CanvasHUD.h"
#include "GameInstance.h"

/* Nodes */
#include "UI_NodeHUDHP.h"
#include "UI_NodeHUDHPBar.h"
#include "UI_NodeHUDShield.h"
#include "UI_NodeHUDPip.h"
#include "UI_NodeHUDRot.h"
#include "UI_NodeEffect.h"

/* Bind Object */
#include "Kena.h"
#include "Kena_State.h"

CUI_CanvasHUD::CUI_CanvasHUD(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Canvas(pDevice, pContext)
{
}

CUI_CanvasHUD::CUI_CanvasHUD(const CUI_CanvasHUD & rhs)
	: CUI_Canvas(rhs)
{
}

HRESULT CUI_CanvasHUD::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CanvasHUD::Initialize(void * pArg)
{
	/* Get a Texture Size, and Make an Initial Matrix */
	XMStoreFloat4x4(&m_matInit, XMMatrixScaling(670.f, 200.f, 1.f));

	if (FAILED(__super::Initialize(pArg)))
	{
		m_tDesc.vSize = { (_float)g_iWinSizeX, (_float)g_iWinSizeY };
		m_tDesc.vPos = { g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f };
		m_pTransformCom->Set_Scaled(_float3(m_tDesc.vSize.x, m_tDesc.vSize.y, 1.f));
		m_pTransformCom->Set_Scaled(_float3(670.f, 200.f, 1.f));

		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,
			XMVectorSet(0.f, 0.f, 0.f, 1.f));
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}

	if (FAILED(Ready_Nodes()))
	{
		MSG_BOX("Failed To Ready Nodes");
		return E_FAIL;
	}

	/* Test */
	m_bActive = true;

	m_Pips[PIP_1] = UI_PIPGAUGE1;
	m_Pips[PIP_2] = UI_PIPGAUGE2;
	m_Pips[PIP_3] = UI_PIPGAUGE3;

	m_iNumPips = 1;
	m_iNumPipsNow = m_iNumPips;

	m_vecNode[UI_PIPBAR1]->Set_Active(true);
	m_vecNode[UI_PIPGAUGE1]->Set_Active(true);
	
	static_cast<CUI_NodeHUDRot*>(m_vecNode[UI_ROT])->Change_RotIcon(0);

	return S_OK;
}

void CUI_CanvasHUD::Tick(_float fTimeDelta)
{
	if (!m_bBindFinished)
	{
		if (FAILED(Bind()))
		{
		//	MSG_BOX("Bind Failed");
			return;
		}
	}

	if (!m_bActive)
		return;


	__super::Tick(fTimeDelta);

	//for (auto node : m_vecNode)
	//	node->Tick(fTimeDelta);
}

void CUI_CanvasHUD::Late_Tick(_float fTimeDelta)
{
	/* Code */
	if (!m_bActive)
		return;
	/* ~Code */

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_CanvasHUD::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CUI_CanvasHUD::Bind()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CKena* pKena = dynamic_cast<CKena*>(pGameInstance->Get_GameObjectPtr(pGameInstance->Get_CurLevelIndex(),L"Layer_Player", L"Kena"));
	RELEASE_INSTANCE(CGameInstance);

	if (pKena == nullptr)
		return E_FAIL;

	pKena->Get_State()->m_PlayerDelegator.bind(this, &CUI_CanvasHUD::Function);
	pKena->m_PlayerDelegator.bind(this, &CUI_CanvasHUD::Function);

	m_bBindFinished = true;
	return S_OK;
}


HRESULT CUI_CanvasHUD::Ready_Nodes()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI* pUI = nullptr;
	CUI::UIDESC tDesc;
	string str;
	wstring wstr;

	/* Note : the reason using unstable temp address variable "fileName" is that
	fileName(CloneTag) needed while it cloned when loading the data.
	(The cloneTag is stored after the clone process.)
	*/
	str = "Node_HPBar";
	tDesc.fileName.assign(str.begin(), str.end()); /* this file name doesn't exist eternally.(지역변수) */
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_HPBar", L"Node_HPBar", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_HP";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_HP", L"Node_HP", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_ShieldBar";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_ShieldBar", L"Node_ShieldBar", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_Shield";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Shield", L"Node_Shield", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_RotIcon";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_RotIcon", L"Node_RotIcon", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	/************ Pip : Max 3 *************/
	str = "Node_PipBar1"; 
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_PipBar", L"Node_PipBar1", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_PipGuage1";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_PipGuage", L"Node_PipGuage1", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_PipBar2";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_PipBar", L"Node_PipBar2", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_PipGuage2";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_PipGuage", L"Node_PipGuage2", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_PipBar3";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_PipBar", L"Node_PipBar3", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_PipGuage3";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_PipGuage", L"Node_PipGuage3", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);
	/************	~ Pip *************/

	/* Effects are stored in the canvas of UI that need it. */
	/* But Call through ClientManager because Node doesn't have to know the canvas belong to.*/
	str = "Node_EffectPip";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Effect", L"Node_EffectPip", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);
	CUI_ClientManager::GetInstance()->Set_Effect(CUI_ClientManager::EFFECT_PIPFULL, static_cast<CUI_NodeEffect*>(pUI));


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CUI_CanvasHUD::SetUp_Components()
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

HRESULT CUI_CanvasHUD::SetUp_ShaderResources()
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

void CUI_CanvasHUD::Function(CUI_ClientManager::UI_PRESENT eType, CUI_ClientManager::UI_FUNCTION eFunc, _float fValue)
{
	switch (eFunc)
	{
	case CUI_ClientManager::FUNC_DEFAULT:
		Default(eType, fValue);
		break;
	case CUI_ClientManager::FUNC_LEVELUP:
		LevelUp(eType, (_int)fValue);
		break;
	}
}

void CUI_CanvasHUD::Default(CUI_ClientManager::UI_PRESENT eType, _float fValue)
{
	_int iIndex = 0;
	_float fVal = fValue;
	switch (eType)
	{
	case CUI_ClientManager::HUD_HP:
		static_cast<CUI_NodeHUDHP*>(m_vecNode[UI_HPGUAGE])->Set_Guage(fValue);
		break;
	case CUI_ClientManager::HUD_SHIELD:
		static_cast<CUI_NodeHUDShield*>(m_vecNode[UI_SHIELD])->Set_Guage(fValue);
		break;
	case CUI_ClientManager::HUD_PIP:
		/* PIP_3	PIP_2	PIP_1 */	
		/* guage add -> */
		/* guage use <- */
		if (fValue == 0.f) /* Use All */
		{
			if (m_iNumPipsNow == 0)
				return;

			/* if m_iNuMPipsNow == 3, use at PIP_1 -> index == 0*/
			/* pipsNow/index : 3,0 / 2,1 / 1,2 */
			iIndex = m_iNumPips - m_iNumPipsNow;
			m_iNumPipsNow -= 1;
			
			CUI_NodeHUDPip* pCurPip = static_cast<CUI_NodeHUDPip*>(m_vecNode[m_Pips[iIndex]]);
			pCurPip->Set_Guage(fVal);

			/* when using pip energy, consume dir and charge dir is opposite. */
			/* so they need to be reorganized */
			if (iIndex - 1 >= 0)
			{
				CUI_NodeHUDPip* pNextPip = static_cast<CUI_NodeHUDPip*>(m_vecNode[m_Pips[iIndex-1]]);
				_float fGuage = pNextPip->Get_Guage();
				if (fGuage > 0.001f)
				{
					pCurPip->ReArrangeGuage(); /* Let Guage can be changed */
					pCurPip->Set_Guage(pNextPip->Get_Guage());
					pNextPip->Set_Guage(0.f);
				}
			}

		}
		else /* Gradually add */
		{
			if (m_iNumPipsNow == m_iNumPips)
				return;

			/* If m_iNumPipsNow == 0 , guage on at PIP_3 -> index==2 */
			/* pipsNow/index : 0,2 / 1,1 / 2,0*/
			iIndex = (m_iNumPips - 1) - m_iNumPipsNow;

			fVal = fValue - m_iNumPipsNow;
			static_cast<CUI_NodeHUDPip*>(m_vecNode[m_Pips[iIndex]])->Set_Guage(fVal);

			if(fVal >= 1.f)
				static_cast<CUI_NodeHUDRot*>(m_vecNode[UI_ROT])->Change_RotIcon(0);
		}
	
		break;
	case CUI_ClientManager::HUD_ROT:
		static_cast<CUI_NodeHUDRot*>(m_vecNode[UI_ROT])->Change_RotIcon(fValue);
		break;
	}
}

void CUI_CanvasHUD::LevelUp(CUI_ClientManager::UI_PRESENT eType, _int iLevel)
{
	switch (eType)
	{
	case CUI_ClientManager::HUD_HP:
		static_cast<CUI_NodeHUDHP*>(m_vecNode[UI_HPGUAGE])->Upgrade();
		static_cast<CUI_NodeHUDHPBar*>(m_vecNode[UI_HPBAR])->Upgrade();

		break;
	case CUI_ClientManager::HUD_SHIELD:
		/* rather make it longer, change color or stat upgrade would be better */
		break;
	case CUI_ClientManager::HUD_PIP:
		if (2 == iLevel)
		{
			m_vecNode[UI_PIPBAR2]->Set_Active(true);
			m_vecNode[UI_PIPGAUGE2]->Set_Active(true);

			m_iNumPips = 2;
			m_iNumPipsNow = m_iNumPips;

			static_cast<CUI_Node*>(m_vecNode[m_Pips[PIP_1]])->ReArrangeX();
			static_cast<CUI_Node*>(m_vecNode[m_Pips[PIP_1]-1])->ReArrangeX(); /* Frame */

			static_cast<CUI_NodeHUDPip*>(m_vecNode[m_Pips[PIP_1]])->Set_Guage(1.f);
		}
		else if (3 == iLevel)
		{
			m_vecNode[UI_PIPBAR3]->Set_Active(true);
			m_vecNode[UI_PIPGAUGE3]->Set_Active(true);

			m_iNumPips = 3;
			m_iNumPipsNow = m_iNumPips;

			static_cast<CUI_Node*>(m_vecNode[m_Pips[PIP_1]])->ReArrangeX();
			static_cast<CUI_Node*>(m_vecNode[m_Pips[PIP_1] - 1])->ReArrangeX(); /* Frame */
			static_cast<CUI_Node*>(m_vecNode[m_Pips[PIP_2]])->ReArrangeX();
			static_cast<CUI_Node*>(m_vecNode[m_Pips[PIP_2] - 1])->ReArrangeX(); /* Frame */

			static_cast<CUI_NodeHUDPip*>(m_vecNode[m_Pips[PIP_1]])->Set_Guage(1.f);
			static_cast<CUI_NodeHUDPip*>(m_vecNode[m_Pips[PIP_2]])->Set_Guage(1.f);
		}
		break;
		break;
	}
}

CUI_CanvasHUD * CUI_CanvasHUD::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_CanvasHUD*	pInstance = new CUI_CanvasHUD(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_CanvasHUD");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_CanvasHUD::Clone(void * pArg)
{
	CUI_CanvasHUD*	pInstance = new CUI_CanvasHUD(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_CanvasHUD");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CanvasHUD::Free()
{
	__super::Free();
}
