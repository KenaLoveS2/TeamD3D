#include "stdafx.h"
#include "..\public\UI_CanvasAmmo.h"
#include "GameInstance.h"

/* Nodes */
#include "UI_NodeAmmoBombGuage.h"

/* Bind Object */
#include "Kena.h"

CUI_CanvasAmmo::CUI_CanvasAmmo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Canvas(pDevice, pContext)
{
}

CUI_CanvasAmmo::CUI_CanvasAmmo(const CUI_CanvasAmmo & rhs)
	: CUI_Canvas(rhs)
{
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

	/* Test */
	m_bActive = true;

	return S_OK;
}

void CUI_CanvasAmmo::Tick(_float fTimeDelta)
{
	if (!m_bBindFinished)
	{
		if (FAILED(Bind()))
		{
			MSG_BOX("Bind Failed");
			return;
		}
	}

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
	__super::Render();

	return S_OK;
}

HRESULT CUI_CanvasAmmo::Bind()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CKena* pKena = dynamic_cast<CKena*>(pGameInstance->Get_GameObjectPtr(pGameInstance->Get_CurLevelIndex(),L"Layer_Player", L"Kena"));
	if (pKena == nullptr)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	pKena->m_PlayerDelegator.bind(this, &CUI_CanvasAmmo::Function);

	RELEASE_INSTANCE(CGameInstance);

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
	str = "Node_BombFrame";
	tDesc.fileName.assign(str.begin(), str.end()); 
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_BombFrame", L"Node_BombFrame", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_BombGuage";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_BombGuage", L"Node_BombGuage", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);


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

void CUI_CanvasAmmo::Function(CUI_ClientManager::UI_PRESENT eType, _float fValue)
{
	switch (eType)
	{
	case CUI_ClientManager::AMMO_BOMB:
		static_cast<CUI_NodeAmmoBombGuage*>(m_vecNode[UI_BOMBGUAGE])->Set_Guage(fValue);
		break;
	}
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
