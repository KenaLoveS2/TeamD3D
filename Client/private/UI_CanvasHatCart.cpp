#include "stdafx.h"
#include "..\public\UI_CanvasHatCart.h"
#include "GameInstance.h"
#include "Json/json.hpp"
#include <fstream>
#include <codecvt>
#include <locale>
#include "Utile.h"
#include "UI_CanvasItemBar.h"
#include "UI_NodeCurrentCrystal.h"
#include "UI_NodeItemBox.h"
#include "Kena.h"
#include "Kena_Status.h"
#include "HatCart.h"


CUI_CanvasHatCart::CUI_CanvasHatCart(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Canvas(pDevice, pContext)
	, m_pPlayer(nullptr)
	, m_iPickedIndex(UI_ITEMBAR0)
{
}

CUI_CanvasHatCart::CUI_CanvasHatCart(const CUI_CanvasHatCart & rhs)
	:CUI_Canvas(rhs)
	, m_pPlayer(nullptr)
	, m_iPickedIndex(UI_ITEMBAR0)
{
}

HRESULT CUI_CanvasHatCart::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CanvasHatCart::Initialize(void * pArg)
{
	/* Get a Texture Size, and Make an Initial Matrix */
	XMStoreFloat4x4(&m_matInit, XMMatrixScaling(1.f, 1.f, 1.f));

	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled({ 1.f, 1.f, 1.f });
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

	m_matInit = m_pTransformCom->Get_WorldMatrixFloat4x4();
	//m_bActive = true;

	/* temp */
	for (_uint i = 0; i < MAX_HAT_COUNT; ++i)
		m_iHatCount[i] = 0;

	return S_OK;
}

void CUI_CanvasHatCart::Tick(_float fTimeDelta)
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

	Picking();

	__super::Tick(fTimeDelta);

}

void CUI_CanvasHatCart::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_CanvasHatCart::Render()
{
	if (m_pTextureCom[TEXTURE_DIFFUSE] == nullptr)
		return S_OK;

	__super::Render();

	return S_OK;
}

HRESULT CUI_CanvasHatCart::Bind()
{
	CHatCart* pCart = nullptr;
	if (g_LEVEL == LEVEL_GAMEPLAY)
	{
		pCart = dynamic_cast<CHatCart*>(CGameInstance::GetInstance()->
			Get_GameObjectPtr(g_LEVEL, L"Layer_Enviroment", L"test"));
	}
	else if(g_LEVEL == LEVEL_FINAL)
	{
		pCart = dynamic_cast<CHatCart*>(CGameInstance::GetInstance()->
			Get_GameObjectPtr(g_LEVEL, L"Layer_Enviroment", L"4_HatCart"));
	}

	if (pCart == nullptr)
		return E_FAIL;

	pCart->m_CartDelegator.bind(this, &CUI_CanvasHatCart::BindFunction);

	m_bBindFinished = true;
	return S_OK;
}

HRESULT CUI_CanvasHatCart::Ready_Nodes()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* BG */
	{
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;

		string strCloneTag = "Node_HatCartBG";
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_HatCartBG", wstrCloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCloneTag);
		pGameInstance->Add_String(wstrCloneTag);
	}

	/* ItemBar */
	Json jLoad;
	string filePath = "../Bin/Data/Shop/Hat.json";
	ifstream file(filePath);
	if (file.fail())
		return E_FAIL;
	file >> jLoad;
	file.close();

	//using convert_type = codecvt_utf8<wchar_t>;
	//wstring_convert<convert_type> utf8_conv;

	size_t iNumHat;
	jLoad["NumHat"].get_to<size_t>(iNumHat);

	for (size_t i = 0; i < iNumHat; ++i)
	{
		string str = "Hat" + to_string(i);
		auto jInfo = jLoad[str];

		string sName;
		jInfo["Name"].get_to<string>(sName);
		wstring wstr = CUtile::utf8_to_wstring(sName);

		_int iPrice;
		jInfo["Price"].get_to<_int>(iPrice);

		m_vecHats.push_back({ wstr,iPrice });


		CUI* pUI = nullptr;
		CUI_CanvasItemBar::ITEMDESC tDesc;

		tDesc.wstrName = m_vecHats[i].first;
		tDesc.iPrice = m_vecHats[i].second;
		tDesc.iTextureIndex = (_int)i;
		tDesc.fileName = L"Canvas_ItemBar";
		tDesc.pParent = this;

		/* It is not real cloneTag. Just for list */
		string strCloneTag = "Canvas_ItemBar" + to_string(i);
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Canvas_ItemBar", 
			CUtile::Create_DummyString(), &tDesc));
		m_vecNodeCloneTag.push_back(strCloneTag);
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
	}

	/* ScrollLine */
	{
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;

		string strCloneTag = "Node_ScrollLine";
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_ScrollLine", wstrCloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCloneTag);
		pGameInstance->Add_String(wstrCloneTag);
	}

	/* ScrollBar */
	{
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;

		string strCloneTag = "Node_ScrollBar";
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_ScrollBar", wstrCloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCloneTag);
		pGameInstance->Add_String(wstrCloneTag);
	}

	/* UpSideBar */
	{
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;

		string strCloneTag = "Node_CartUpSide";
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_SideBar", wstrCloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCloneTag);
		pGameInstance->Add_String(wstrCloneTag);
	}

	/* DownSideBar */
	{
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;

		string strCloneTag = "Node_CartDownSide";
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_SideBar", wstrCloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCloneTag);
		pGameInstance->Add_String(wstrCloneTag);
	}

	/* Crystal */
	{
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;

		string strCloneTag = "Node_CurrentCrystal";
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_CurrentCrystal", wstrCloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCloneTag);
		pGameInstance->Add_String(wstrCloneTag);
		static_cast<CUI_NodeCurrentCrystal*>(pUI)->Set_Font(L"Font_Jangmi0", { 1.f, 1.f, 1.f, 1.f }, { 1.0f, 1.0f }, { 30.f, -10.f });

	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_CanvasHatCart::SetUp_Components()
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

HRESULT CUI_CanvasHatCart::SetUp_ShaderResources()
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

void CUI_CanvasHatCart::BindFunction(CUI_ClientManager::UI_PRESENT eType, CKena* pPlayer, _bool* Out_Open)
{
	if (eType == CUI_ClientManager::HATCART_)
	{
		m_bActive = !m_bActive;
		m_pPlayer = pPlayer;
		static_cast<CUI_NodeCurrentCrystal*>(m_vecNode[UI_CRYSTAL])->Set_Crystal(m_pPlayer->Get_Status()->Get_Crystal());

		*Out_Open = m_bActive;
	}
}
void CUI_CanvasHatCart::Picking()
{
	POINT pt = CUtile::GetClientCursorPos(g_hWnd);

	for (_uint i = UI_ITEMBAR0; i <= UI_ITEMBAR15; ++i)
	{
		_float4 vPos = m_vecNode[i]->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
		_float2 vPosConvert = { vPos.x + 0.5f*g_iWinSizeX, -vPos.y + 0.5f*g_iWinSizeY };

		_float3 vScale = 0.5f * m_vecNode[i]->Get_TransformCom()->Get_Scaled();
		RECT rc = { LONG(vPosConvert.x - vScale.x), LONG(vPosConvert.y - vScale.y),
			LONG(vPosConvert.x + vScale.x), LONG(vPosConvert.y + vScale.y) };
		
		if (PtInRect(&rc, pt))
		{
			if (CGameInstance::GetInstance()->Mouse_Down(DIM_LB))
			{
				if (m_iPickedIndex != i)
					static_cast<CUI_CanvasItemBar*>(m_vecNode[m_iPickedIndex])->BackToNormal();


				m_iPickedIndex = i;
				static_cast<CUI_CanvasItemBar*>(m_vecNode[m_iPickedIndex])->Clicked();

				Shopping();
			}
		}
	}
}
void CUI_CanvasHatCart::Shopping()
{
	_int iCrystal = m_pPlayer->Get_Status()->Get_Crystal();
	_int iPrice = m_vecHats[m_iPickedIndex- UI_ITEMBAR0].second;
	if (iCrystal >= iPrice && m_pPlayer->IsBuyPossible_RotHat())
	{
		m_pPlayer->Get_Status()->Set_Crystal(iCrystal - iPrice);
		static_cast<CUI_NodeCurrentCrystal*>(m_vecNode[UI_CRYSTAL])->Set_Crystal(m_pPlayer->Get_Status()->Get_Crystal());

		m_iHatCount[m_iPickedIndex- UI_ITEMBAR0] += 1;
		static_cast<CUI_CanvasItemBar*>(m_vecNode[m_iPickedIndex])->Buy(m_iHatCount[m_iPickedIndex - UI_ITEMBAR0]);

		m_pPlayer->Buy_RotHat(m_iPickedIndex - UI_ITEMBAR0);		
	}
}
CUI_CanvasHatCart * CUI_CanvasHatCart::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_CanvasHatCart*	pInstance = new CUI_CanvasHatCart(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_CanvasHatCart");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_CanvasHatCart::Clone(void * pArg)
{
	CUI_CanvasHatCart*	pInstance = new CUI_CanvasHatCart(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_CanvasHatCart");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CanvasHatCart::Free()
{
	m_vecHats.clear();
	__super::Free();
}
