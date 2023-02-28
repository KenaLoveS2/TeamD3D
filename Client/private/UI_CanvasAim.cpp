#include "stdafx.h"
#include "..\public\UI_CanvasAim.h"
#include "GameInstance.h"

/* Nodes */
#include "UI_NodeAimLine.h"
#include "UI_NodeAimArrow.h"
#include "UI_NodeAimBomb.h"

/* Bind Object */
#include "Kena.h"

CUI_CanvasAim::CUI_CanvasAim(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Canvas(pDevice, pContext)
{
}

CUI_CanvasAim::CUI_CanvasAim(const CUI_CanvasAim & rhs)
	: CUI_Canvas(rhs)
{
}

void CUI_CanvasAim::Set_Arrow(_int iArrowIndex, _int iState)
{
	static_cast<CUI_NodeAimArrow*>(m_vecNode[m_Arrows[iArrowIndex]])
		->Set_State((CUI_NodeAimArrow::STATE)iState);
}

void CUI_CanvasAim::Set_Bomb(_int iBombIndex, _int iState)
{
	static_cast<CUI_NodeAimBomb*>(m_vecNode[m_Bombs[iBombIndex]])
		->Set_State((CUI_NodeAimBomb::STATE)iState);
}

void CUI_CanvasAim::LevelUp(CUI_ClientManager::UI_PRESENT eType, _int iLevel)
{
	switch (eType)
	{
	case CUI_ClientManager::AMMO_ARROW:
		if (1 == iLevel)
		{
			m_vecNode[UI_LINELEFT]->Set_Active(true);
			m_vecNode[UI_LINERIGHT]->Set_Active(true);
			m_vecNode[UI_ARROW1]->Set_Active(true);
			m_vecNode[UI_ARROW2]->Set_Active(true);
			m_vecNode[UI_ARROW3]->Set_Active(true);
			m_vecNode[UI_ARROW4]->Set_Active(true);
		}
		else if(2 == iLevel)
		{
			/* Todo : 1 ~ 4 Move & All Full State */
			m_vecNode[UI_ARROW5]->Set_Active(true);
			for (_uint i = 0; i < ARROW_END; ++i)
			{
				static_cast<CUI_Node*>(m_vecNode[m_Arrows[i]])->ReArrangeX_Reverse(i, ARROW_END, 0.f);
				static_cast<CUI_NodeAimArrow*>(m_vecNode[m_Arrows[i]])
					->Set_State((CUI_NodeAimArrow::STATE_FULL));

			}
		}
		break;
	case CUI_ClientManager::AMMO_BOMB:
		if (1 == iLevel)
			m_vecNode[UI_BOMB1]->Set_Active(true);
		else if(2 == iLevel)
		{	/* Todo : 1 Move & All Full State */
			m_vecNode[UI_BOMB2]->Set_Active(true);
			for (_uint i = 0; i < BOMB_END; ++i)
			{
				static_cast<CUI_Node*>(m_vecNode[m_Bombs[i]])->ReArrangeX_Reverse(i, BOMB_END, 0.f);
				static_cast<CUI_NodeAimBomb*>(m_vecNode[m_Bombs[i]])
					->Set_State((CUI_NodeAimBomb::STATE_FULL));
			}
		}
		break;
	}
}

HRESULT CUI_CanvasAim::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CanvasAim::Initialize(void * pArg)
{
	/* Get a Texture Size, and Make an Initial Matrix */
	XMStoreFloat4x4(&m_matInit, XMMatrixScaling(112.f, 112.f, 1.f));

	if (FAILED(__super::Initialize(pArg)))
		m_pTransformCom->Set_Scaled(_float3(112.f, 112.f, 1.f));

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

	/* Ordering */
	m_Arrows[ARROW_1] = UI_ARROW1;
	m_Arrows[ARROW_2] = UI_ARROW2;
	m_Arrows[ARROW_3] = UI_ARROW3;
	m_Arrows[ARROW_4] = UI_ARROW4;
	m_Arrows[ARROW_5] = UI_ARROW5;

	m_Bombs[BOMB_1] = UI_BOMB1;
	m_Bombs[BOMB_2] = UI_BOMB2;


	/* Todo : Player Push Shift Key -> true, else -> false */
	m_bActive = true;

	/* Temp */
	m_bStart = false;
	m_fTime = 0.5f;
	m_fTimeAcc = 0.f;

	return S_OK;
}

void CUI_CanvasAim::Tick(_float fTimeDelta)
{
	if (!m_bBindFinished)
	{
		if (FAILED(Bind()))
		{
			MSG_BOX("Bind Failed");
			return;
		}
	}

	/* Code */

	/* Aiming .... */
	if (m_bStart)
	{
		m_fTime = 1.f;
		m_fTimeAcc += fTimeDelta;
		_float3 vScaleNow = m_pTransformCom->Get_Scaled();
		if (vScaleNow.x >= 10.f)
		{
			_float3 vNewScale = (1 - m_fTimeAcc) * m_vOriginalSettingScale;
			m_pTransformCom->Set_Scaled(vNewScale);
		}
		else
		{
			m_bStart = false;
			m_fTimeAcc = 0.f;
			m_pTransformCom->Set_Scaled(m_vOriginalSettingScale);
			static_cast<CUI_NodeAimLine*>(m_vecNode[UI_LINELEFT])->Shrink(false);
			static_cast<CUI_NodeAimLine*>(m_vecNode[UI_LINERIGHT])->Shrink(false);
		}
	}

	/* ~Code */

	__super::Tick(fTimeDelta);
}

void CUI_CanvasAim::Late_Tick(_float fTimeDelta)
{
	/* Code */

	/* ~Code */

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_CanvasAim::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CanvasAim::Bind()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CKena* pKena = dynamic_cast<CKena*>(pGameInstance->Get_GameObjectPtr(pGameInstance->Get_CurLevelIndex(), L"Layer_Player", L"Kena"));
	RELEASE_INSTANCE(CGameInstance);
	
	if (pKena == nullptr)
		return E_FAIL;

	pKena->m_PlayerDelegator.bind(this, &CUI_CanvasAim::Function);


	m_bBindFinished = true;
	return S_OK;
}

HRESULT CUI_CanvasAim::Ready_Nodes()
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
	str = "Node_AimLineLeft";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_AimLine", L"Node_AimLineLeft", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_AimLineRight";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_AimLine", L"Node_AimLineRight", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_AimArrow1";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_AimArrow", L"Node_AimArrow1", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_AimArrow2";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_AimArrow", L"Node_AimArrow2", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_AimArrow3";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_AimArrow", L"Node_AimArrow3", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_AimArrow4";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_AimArrow", L"Node_AimArrow4", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_AimArrow5";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_AimArrow", L"Node_AimArrow5", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_AimBomb1";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_AimBomb", L"Node_AimBomb1", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_AimBomb2";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_AimBomb", L"Node_AimBomb2", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CUI_CanvasAim::SetUp_Components()
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

HRESULT CUI_CanvasAim::SetUp_ShaderResources()
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

void CUI_CanvasAim::Function(CUI_ClientManager::UI_PRESENT eType, CUI_ClientManager::UI_FUNCTION eFunc, _float fValue)
{
	switch (eType)
	{
	case CUI_ClientManager::AIM_:
		m_bStart = true;
		m_fTimeAcc = 0.f;
		m_pTransformCom->Set_Scaled(m_vOriginalSettingScale);
		static_cast<CUI_NodeAimLine*>(m_vecNode[UI_LINELEFT])->Shrink(true);
		static_cast<CUI_NodeAimLine*>(m_vecNode[UI_LINERIGHT])->Shrink(true);
		break;
	}
}

CUI_CanvasAim * CUI_CanvasAim::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_CanvasAim*	pInstance = new CUI_CanvasAim(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_CanvasAim");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_CanvasAim::Clone(void * pArg)
{
	CUI_CanvasAim*	pInstance = new CUI_CanvasAim(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_CanvasAim");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CanvasAim::Free()
{
	__super::Free();
}
