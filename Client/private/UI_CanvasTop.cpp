#include "stdafx.h"
#include "..\public\UI_CanvasTop.h"
#include "GameInstance.h"
#include "Kena.h"
#include "Kena_Status.h"
#include "UI_NodeLvUp.h"
#include "UI_NodeEffect.h"
#include "UI_NodeRotFrontGuage.h"
#include "UI_NodeRotCnt.h"
#include "UI_NodeRotArrow.h"
#include "UI_NodeBossHP.h"
#include "UI_NodeMood.h"
#include "BossWarrior.h"
#include "BossShaman.h"
#include "BossHunter.h"

CUI_CanvasTop::CUI_CanvasTop(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Canvas(pDevice,pContext)
	, m_iRotNow(0)
	, m_iRotMax(0)
	, m_bRotLvUp(false)
	, m_iRotLv(1)
{
}

CUI_CanvasTop::CUI_CanvasTop(const CUI_CanvasTop & rhs)
	: CUI_Canvas(rhs)
	, m_iRotNow(0)
	, m_iRotMax(0)
	, m_bRotLvUp(false)
	, m_iRotLv(1)
{
}

HRESULT CUI_CanvasTop::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CanvasTop::Initialize(void * pArg)
{
	/* Get a Texture Size, and Make an Initial Matrix */
	XMStoreFloat4x4(&m_matInit, XMMatrixScaling((_float)g_iWinSizeX, 70.f, 1.f));

	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3((_float)g_iWinSizeX, 70.f, 1.f));
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components : CanvasUpgrade");
		return E_FAIL;
	}


	if (FAILED(Ready_Nodes()))
	{
		MSG_BOX("Failed To Ready Nodes : CanvasUpgrade");
		return E_FAIL;
	}

	m_bActive = true;

	return S_OK;
}

void CUI_CanvasTop::Tick(_float fTimeDelta)
{
	if (!m_bBindFinished)
	{
		if (FAILED(Bind()))
			return;
	}

	if (!m_bActive)
		return;

	static_cast<CUI_NodeRotArrow*>(m_vecNode[UI_ROTARROW])->Set_Arrow(
		static_cast<CUI_NodeRotFrontGuage*>(m_vecNode[UI_ROTGUAGE])->Get_CurrentGuagePosition()
	);

	if (m_bRotLvUp == true)
	{
		_float fGuageAlpha = -1.f;
		if (true == static_cast<CUI_NodeRotFrontGuage*>(m_vecNode[UI_ROTGUAGE])->If_DisAppear_Get_Alpha(&fGuageAlpha))
		{
			if (fGuageAlpha < 0.3f)
			{
				static_cast<CUI_NodeEffect*>(m_vecNode[UI_EFFECT_BACKFLARE])->Start_Effect(m_vecNode[UI_ROTLVUP], 0.0f, 0.0f);
				static_cast<CUI_NodeEffect*>(m_vecNode[UI_EFFECT_FRONTFLAREROUND])->Start_Effect(m_vecNode[UI_ROTLVUP], 0.0f, 0.0f);
				static_cast<CUI_NodeEffect*>(m_vecNode[UI_EFFECT_FRONTFLARE])->Start_Effect(m_vecNode[UI_ROTLVUP], 0.0f, 0.0f);

				static_cast<CUI_NodeLvUp*>(m_vecNode[UI_ROTLVUP])->Appear(m_iRotLv);
			}
		}
		else if (true == m_vecNode[UI_ROTLVUP]->Is_Active())
		{
			_float fAlpha = static_cast<CUI_NodeLvUp*>(m_vecNode[UI_ROTLVUP])->Get_Alpha();

			static_cast<CUI_NodeEffect*>(m_vecNode[UI_EFFECT_BACKFLARE])->Set_Alpha(fAlpha);
			static_cast<CUI_NodeEffect*>(m_vecNode[UI_EFFECT_FRONTFLAREROUND])->Set_Alpha(fAlpha);
			static_cast<CUI_NodeEffect*>(m_vecNode[UI_EFFECT_FRONTFLARE])->Set_Alpha(fAlpha);

		}
		else if (false == m_vecNode[UI_ROTGUAGE]->Is_Active() && false == m_vecNode[UI_ROTLVUP]->Is_Active())
		{
			m_bRotLvUp = false;
			static_cast<CUI_NodeRotFrontGuage*>(m_vecNode[UI_ROTGUAGE])->Set_GuageZero();

			m_vecNode[UI_EFFECT_BACKFLARE]->Set_Active(false);
			m_vecNode[UI_EFFECT_FRONTFLAREROUND]->Set_Active(false);
			m_vecNode[UI_EFFECT_FRONTFLARE]->Set_Active(false);
		}
	}

	if (CGameInstance::GetInstance()->Key_Down(DIK_K))
	{
		static _uint iState = CUI_NodeMood::STATE_HIT;
		iState++;
		iState %= CUI_NodeMood::STATE_END;

		m_vecNode[UI_MOOD]->Set_Active(true);
		static_cast<CUI_NodeMood*>(m_vecNode[UI_MOOD])->MoodOn((CUI_NodeMood::STATE)iState);
	}

	//if (CGameInstance::GetInstance()->Key_Down(DIK_K))
	//{
	//	static_cast<CUI_NodeLvUp*>(m_vecNode[UI_ROTLVUP])->Appear(1);
	//}

	__super::Tick(fTimeDelta);
}

void CUI_CanvasTop::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_CanvasTop::Render()
{
	if (m_pTextureCom[TEXTURE_DIFFUSE] == nullptr)
		return S_OK;

	__super::Render();

	return S_OK;
}

HRESULT CUI_CanvasTop::Bind()
{
	
	CKena* pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL,
		L"Layer_Player", L"Kena"));
	if (pKena == nullptr)
		return E_FAIL;

	/* Boss Warrior Bind */
	//CBossWarrior* pBossWarrior = dynamic_cast<CBossWarrior*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL,
	//	L"Layer_Monster", L"BossWarrior_0"));
	//if (pBossWarrior == nullptr)
	//	return E_FAIL;

	/* Boss Shaman Bind */
	//CBossShaman* pShaman = dynamic_cast<CBossShaman*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL,
	//	L"Layer_Monster", L"BossShaman_0"));
	//if (pShaman == nullptr)
	//	return E_FAIL;

	/* Boss Hunter Bind */
	CBossHunter* pBossHunter = dynamic_cast<CBossHunter*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL,
		L"Layer_Monster", L"BossHunter_0"));
	if (pBossHunter == nullptr)
		return E_FAIL;

	pKena->Get_Status()->m_StatusDelegator.bind(this, &CUI_CanvasTop::BindFunction);
	//pBossWarrior->m_BossWarriorDelegator.bind(this, &CUI_CanvasTop::BindFunction);
	//pShaman->m_BossShamanDelegator.bind(this, &CUI_CanvasTop::BindFunction);
	pBossHunter->m_BossHunterDelegator.bind(this, &CUI_CanvasTop::BindFunction);

	m_bBindFinished = true;
	return S_OK;
}

HRESULT CUI_CanvasTop::Ready_Nodes()
{
	/* RotGet, RotLevelUp, MapTitle, */

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	{ /* MapTitle */
		string strTag = "Node_MapTitle";
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;
		_tchar* cloneTag = CUtile::StringToWideChar(strTag);
		tDesc.fileName = cloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_MapTitle", cloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strTag);
		pGameInstance->Add_String(cloneTag);
	}
	
	{ /* CardBackFlare */
		string strRing = "Node_EffectBackFlare";
		CUI::UIDESC tDescRing;
		_tchar* tagRing = CUtile::StringToWideChar(strRing);
		tDescRing.fileName = tagRing;
		CUI_NodeEffect* pEffectUI
			= static_cast<CUI_NodeEffect*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Effect", tagRing, &tDescRing));
		if (FAILED(Add_Node(pEffectUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strRing);
		pGameInstance->Add_String(tagRing);
		m_vecEffects.push_back(pEffectUI);
	}

	{ /* RotLvUpCard */
		string strTag = "Node_RotLvUpCard";
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;
		_tchar* cloneTag = CUtile::StringToWideChar(strTag);
		tDesc.fileName = cloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_RotLvUpCard", cloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strTag);
		pGameInstance->Add_String(cloneTag);
	}

	{ /* FrontFlareRound  */
		string strRing = "Node_EffectFrontFlareRound";
		CUI::UIDESC tDescRing;
		_tchar* tagRing = CUtile::StringToWideChar(strRing);
		tDescRing.fileName = tagRing;
		CUI_NodeEffect* pEffectUI
			= static_cast<CUI_NodeEffect*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Effect", tagRing, &tDescRing));
		if (FAILED(Add_Node(pEffectUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strRing);
		pGameInstance->Add_String(tagRing);
		m_vecEffects.push_back(pEffectUI);
	}

	{ /* FrontFlare */
		string strRing = "Node_EffectFrontFlare";
		CUI::UIDESC tDescRing;
		_tchar* tagRing = CUtile::StringToWideChar(strRing);
		tDescRing.fileName = tagRing;
		CUI_NodeEffect* pEffectUI
			= static_cast<CUI_NodeEffect*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Effect", tagRing, &tDescRing));
		if (FAILED(Add_Node(pEffectUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strRing);
		pGameInstance->Add_String(tagRing);
		m_vecEffects.push_back(pEffectUI);
	}


	{ /* NodeRotMax */
		string strTag = "Node_RotMaxCnt";
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;
		_tchar* cloneTag = CUtile::StringToWideChar(strTag);
		tDesc.fileName = cloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_RotMaxCnt", cloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strTag);
		pGameInstance->Add_String(cloneTag);
	}

	{ /* RotGuage */
		string strTag = "Node_RotFrontGuage";
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;
		_tchar* cloneTag = CUtile::StringToWideChar(strTag);
		tDesc.fileName = cloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_RotFrontGuage", cloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strTag);
		pGameInstance->Add_String(cloneTag);
	}

	{ /* RotArrow */
		string strTag = "Node_RotArrow";
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;
		_tchar* cloneTag = CUtile::StringToWideChar(strTag);
		tDesc.fileName = cloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_RotArrow", cloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strTag);
		pGameInstance->Add_String(cloneTag);
	}

	{ /* BossHP */
		string strTag = "Node_BossHP";
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;
		_tchar* cloneTag = CUtile::StringToWideChar(strTag);
		tDesc.fileName = cloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_BossHP", cloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strTag);
		pGameInstance->Add_String(cloneTag);
	}

	{ /* PlayerMood */
		string strTag = "Node_Mood";
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;
		_tchar* cloneTag = CUtile::StringToWideChar(strTag);
		tDesc.fileName = cloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Mood", cloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strTag);
		pGameInstance->Add_String(cloneTag);
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_CanvasTop::SetUp_Components()
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

HRESULT CUI_CanvasTop::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

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

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CUI_CanvasTop::BindFunction(CUI_ClientManager::UI_PRESENT eType, _float fValue)
{
	m_bActive = true;

	switch (eType)
	{
	case CUI_ClientManager::TOP_ROTMAX:
		m_iRotMax = (_uint)fValue;
		static_cast<CUI_NodeRotCnt*>(m_vecNode[UI_ROTCNT])->Set_Info(m_iRotMax);
		break;
	case CUI_ClientManager::TOP_ROTCUR:
		m_iRotNow = (_uint)fValue;
		static_cast<CUI_NodeRotArrow*>(m_vecNode[UI_ROTARROW])->Set_Info(m_iRotNow);
		break;
	case CUI_ClientManager::TOP_ROTGET:
		static_cast<CUI_NodeRotFrontGuage*>(m_vecNode[UI_ROTGUAGE])->Set_Guage(fValue);
		break;
	case CUI_ClientManager::TOP_ROT_LVUP:
		m_bRotLvUp = true;
		m_iRotLv = (_uint)fValue;
		break;
	case CUI_ClientManager::TOP_BOSS:
		if (fValue >= 10.f)
		{
			_int iValue = (_int)fValue;
			switch (iValue)
			{
			case 10:
				static_cast<CUI_NodeBossHP*>(m_vecNode[UI_BOSSHP])->Set_Title(L"워리어");
				break;
			case 20:
				static_cast<CUI_NodeBossHP*>(m_vecNode[UI_BOSSHP])->Set_Title(L"가면 제작자");
				break;
			case 30:
				static_cast<CUI_NodeBossHP*>(m_vecNode[UI_BOSSHP])->Set_Title(L"헌터");
				break;
			}
		}
		else if (fValue == -1.f)
			m_vecNode[UI_BOSSHP]->Set_Active(false);
		else
			static_cast<CUI_NodeBossHP*>(m_vecNode[UI_BOSSHP])->Set_Guage(fValue);
		break;
	}

}

CUI_CanvasTop * CUI_CanvasTop::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_CanvasTop*	pInstance = new CUI_CanvasTop(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_CanvasTop");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_CanvasTop::Clone(void * pArg)
{
	CUI_CanvasTop*	pInstance = new CUI_CanvasTop(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_CanvasTop");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CanvasTop::Free()
{
	__super::Free();

	m_vecEffects.clear();
}
