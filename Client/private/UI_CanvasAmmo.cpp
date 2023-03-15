#include "stdafx.h"
#include "..\public\UI_CanvasAmmo.h"
#include "GameInstance.h"
#include "UI_ClientManager.h"
#include "UI_NodeEffect.h"

/* Canvas */
#include "UI_CanvasAim.h"

/* Nodes */
#include "UI_NodeAmmoBombGuage.h"
#include "UI_NodeAmmoArrowGuage.h"

/* Bind Object */
#include "Kena.h"
#include "Kena_State.h"
#include "Kena_Status.h"

CUI_CanvasAmmo::CUI_CanvasAmmo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Canvas(pDevice, pContext)
{
}

CUI_CanvasAmmo::CUI_CanvasAmmo(const CUI_CanvasAmmo & rhs)
	: CUI_Canvas(rhs)
{
}

void CUI_CanvasAmmo::ConnectToAimUI(AIM_UI eUIType, _int iParam, CUI* pUI)
{
	CUI_CanvasAim* pCanvas = static_cast<CUI_CanvasAim*>(CUI_ClientManager::GetInstance()->Get_Canvas(CUI_ClientManager::CANVAS_AIM));
	if (pCanvas == nullptr)
		return;

	_int arrowIndex = m_iNumArrowNow - 1;
	_int bombIndex = m_iNumBombNow - 1; /* Bombs order is different from arrow order */
	if (pUI != nullptr)
	{
		if (m_vecNode[m_Bombs[BOMB_1]] == pUI)
			bombIndex = BOMB_1;
		else
			bombIndex = BOMB_2;
	}
	

	switch (eUIType)
	{
	case AIM_ARROW:
		if (arrowIndex >= 0)
			pCanvas->Set_Arrow(arrowIndex, 1); /* 1 == full */
		break;
	case AIM_BOMB:
		if (bombIndex >= 0)
			pCanvas->Set_Bomb(bombIndex, 1); /* 1 == full */
		break;
	default:
		MSG_BOX("Wrong Input: CUI_CanvasAmmo");
		break;
	}


}
HRESULT CUI_CanvasAmmo::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CanvasAmmo::Initialize(void * pArg)
{
	/* Get a Texture Size, and Make an Initial Matrix */
	XMStoreFloat4x4(&m_matInit, XMMatrixScaling(304.f, 222.f, 1.f));

	if (FAILED(__super::Initialize(pArg)))
		m_pTransformCom->Set_Scaled(_float3(304.f, 222.f, 1.f));

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

	/* Arrow */
	m_iNumArrows = 4;
	m_iNumArrowNow = m_iNumArrows;

	/* temp */
	m_bActive = true;
	m_vecNode[UI_ARROWGUAGE]->Set_Active(true);

	/* Bomb */
	m_iNumBombs		= 1;
	m_iNumBombNow	= m_iNumBombs;
	m_Bombs[BOMB_1] = UI_BOMBGUAGE1;
	m_Bombs[BOMB_2] = UI_BOMBGUAGE2;


	return S_OK;
}

void CUI_CanvasAmmo::Tick(_float fTimeDelta)
{
	if (!m_bBindFinished)
	{
		if (FAILED(Bind()))
		{
			//MSG_BOX("Bind Failed");
			return;
		}
	}

	/* Code */

	/* ~Code */

	__super::Tick(fTimeDelta);

}

void CUI_CanvasAmmo::Late_Tick(_float fTimeDelta)
{
	/* Code */

	/* ~Code */

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_CanvasAmmo::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	/* Arrow Count */
	_float4 vPos;

	wchar_t cnt[10];
	_itow_s(m_iNumArrowNow, cnt, 10);

	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Render_Font(TEXT("Font_Comic"), cnt, _float2(vPos.x + g_iWinSizeX * 0.5f - 23.f, -vPos.y + g_iWinSizeY * 0.5f - 20.f), 0.f, _float2(1.2f, 1.2f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	RELEASE_INSTANCE(CGameInstance);



	return S_OK;
}

HRESULT CUI_CanvasAmmo::Bind()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CKena* pKena = dynamic_cast<CKena*>(pGameInstance->Get_GameObjectPtr(pGameInstance->Get_CurLevelIndex(), L"Layer_Player", L"Kena"));
	RELEASE_INSTANCE(CGameInstance);

	if (pKena == nullptr)
		return E_FAIL;

	pKena->Get_Status()->m_StatusDelegator.bind(this, &CUI_CanvasAmmo::Function);
	//pKena->Get_State()->m_PlayerDelegator.bind(this, &CUI_CanvasAmmo::Function);
	//pKena->m_PlayerDelegator.bind(this, &CUI_CanvasAmmo::Function);


	//pKena->m_PlayerAmmoDelegator.bind(this, &CUI_CanvasAmmo::NewFunction);

	m_bBindFinished = true;
	return S_OK;
}

HRESULT CUI_CanvasAmmo::Ready_Nodes()
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
	str = "Node_BombFrame1";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_BombFrame", L"Node_BombFrame1", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_BombGuage1";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_BombGuage", L"Node_BombGuage1", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_BombFrame2";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_BombFrame", L"Node_BombFrame2", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_BombGuage2";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_BombGuage", L"Node_BombGuage2", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_ArrowGuage";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_ArrowGuage", L"Node_ArrowGuage", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	/* Effects are stored in the canvas of UI that need it. */
	/* But Call through ClientManager because Node doesn't have to know the canvas belong to.*/
	str = "Node_EffectBomb";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Effect", L"Node_EffectBomb", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);
	CUI_ClientManager::GetInstance()->Set_Effect(CUI_ClientManager::EFFECT_BOMBFULL, static_cast<CUI_NodeEffect*>(pUI));

	str = "Node_EffectArrow";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Effect", L"Node_EffectArrow", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);
	CUI_ClientManager::GetInstance()->Set_Effect(CUI_ClientManager::EFFECT_ARROWFULL, static_cast<CUI_NodeEffect*>(pUI));



	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CUI_CanvasAmmo::SetUp_Components()
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

HRESULT CUI_CanvasAmmo::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI::SetUp_ShaderResources();

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

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CUI_CanvasAmmo::Function(CUI_ClientManager::UI_PRESENT eType, CUI_ClientManager::UI_FUNCTION eFunc, _float fValue)
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

void CUI_CanvasAmmo::Function(CUI_ClientManager::UI_PRESENT eType, _float fValue)
{
	CUI_CanvasAim* pCanvas = static_cast<CUI_CanvasAim*>(CUI_ClientManager::GetInstance()->Get_Canvas(CUI_ClientManager::CANVAS_AIM));

	switch (eType)
	{
	case CUI_ClientManager::AMMO_ARROW:
		m_iNumArrowNow = (_uint)fValue;
		pCanvas->Set_Arrow(m_iNumArrowNow, 0);
		break;
	case CUI_ClientManager::AMMO_ARROWCOOL:
		static_cast<CUI_NodeAmmoArrowGuage*>(m_vecNode[UI_ARROWGUAGE])->Set_Guage(fValue);
		break;
	case CUI_ClientManager::AMMO_ARROWEFFECT:
		static_cast<CUI_NodeEffect*>(m_vecNode[UI_ARROWEFFECT])->Start_Effect(m_vecNode[UI_ARROWGUAGE], 0.f, 35.f);
		break;

	case CUI_ClientManager::AMMO_ARROWRECHARGE:
		m_iNumArrowNow = (_uint)fValue;
		pCanvas->Set_Arrow(m_iNumArrowNow, 1);
		break;
	}
}

void CUI_CanvasAmmo::NewFunction(CUI_ClientManager::UI_PRESENT eType, _float fVal1, _float fVal2, _float fVal3, _float fVal4)
{
	switch (eType)
	{
	case CUI_ClientManager::AMMO_ARROW:
		/* curArrowCnt, maxArrowCnt, curArrowCoolTime, initArrowCoolTime */
		m_iNumArrowNow = (_uint)fVal1;
		
		static_cast<CUI_NodeAmmoArrowGuage*>(m_vecNode[UI_ARROWGUAGE])->Set_Guage(fVal3 / fVal4);

		break;
	}





}

void CUI_CanvasAmmo::Default(CUI_ClientManager::UI_PRESENT eType, _float fValue)
{
	CUI_CanvasAim* pCanv = static_cast<CUI_CanvasAim*>(CUI_ClientManager::GetInstance()->Get_Canvas(CUI_ClientManager::CANVAS_AIM));
	if (nullptr == pCanv)
		return;

	/* Shoot a Bomb or Arrow */
	_uint iIndex = 0;
	switch (eType)
	{
	case CUI_ClientManager::AMMO_BOMB:
		if (m_iNumBombNow == 0)
			return;
		m_iNumBombNow -= 1;
		if (m_iNumBombs == 2)
			if (static_cast<CUI_NodeAmmoBombGuage*>(m_vecNode[m_Bombs[BOMB_2]])->Is_Full())
				iIndex = BOMB_2;
			else
				iIndex = BOMB_1;
		else
			iIndex = m_iNumBombNow;

		static_cast<CUI_NodeAmmoBombGuage*>(m_vecNode[m_Bombs[iIndex]])->Set_Guage(0.f);

		/*Connect Wiht Cavnas Aim's Bomb */
		pCanv->Set_Bomb(iIndex, 0);
		break;

	case CUI_ClientManager::AMMO_ARROW:
		if (m_iNumArrowNow == 0)
			return;
		m_iNumArrowNow -= 1;
		static_cast<CUI_NodeAmmoArrowGuage*>(m_vecNode[UI_ARROWGUAGE])->Set_Guage(-1.f);

		/* Connect With Canvas Aim's Arrow */
		pCanv->Set_Arrow(m_iNumArrowNow, 0);
		break;
	}

}

void CUI_CanvasAmmo::LevelUp(CUI_ClientManager::UI_PRESENT eType, _int iLevel)
{
	switch (eType)
	{
	case CUI_ClientManager::AMMO_BOMB:
		if (1 == iLevel)
		{
			m_vecNode[UI_BOMBFRAME1]->Set_Active(true);
			m_vecNode[UI_BOMBGUAGE1]->Set_Active(true);
		}
		else if (2 == iLevel)
		{
			m_iNumBombs = 2;
			m_iNumBombNow = m_iNumBombs;

			/* Todo : All Full State  & Move */
			m_vecNode[UI_BOMBFRAME2]->Set_Active(true);
			m_vecNode[UI_BOMBGUAGE2]->Set_Active(true);
			static_cast<CUI_Node*>(m_vecNode[m_Bombs[BOMB_2]])->ReArrangeX();
			static_cast<CUI_Node*>(m_vecNode[m_Bombs[BOMB_2]-1])->ReArrangeX(); /* Frame */

		}
		break;
	case CUI_ClientManager::AMMO_ARROW:
		if (1 == iLevel)
		{
			/* When player gets arrow, ammo Opens. */
			m_bActive = true;
			m_vecNode[UI_ARROWGUAGE]->Set_Active(true);
		}
		else if(2 == iLevel)
		{
			m_iNumArrows = 5;
			m_iNumArrowNow = m_iNumArrows;
		}
		break;
	}

	/* Connect To Aim */
	static_cast<CUI_CanvasAim*>(CUI_ClientManager::GetInstance()
		->Get_Canvas(CUI_ClientManager::CANVAS_AIM))->LevelUp(eType, iLevel);
}

CUI_CanvasAmmo * CUI_CanvasAmmo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_CanvasAmmo*	pInstance = new CUI_CanvasAmmo(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_CanvasAmmo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_CanvasAmmo::Clone(void * pArg)
{
	CUI_CanvasAmmo*	pInstance = new CUI_CanvasAmmo(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_CanvasAmmo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CanvasAmmo::Free()
{
	__super::Free();
}
