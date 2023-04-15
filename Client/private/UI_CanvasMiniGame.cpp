#include "stdafx.h"
#include "UI_CanvasMiniGame.h"
#include "GameInstance.h"
#include "UI_NodeVictory.h"

CUI_CanvasMiniGame::CUI_CanvasMiniGame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Canvas(pDevice, pContext)
	, m_bResultShow(false)
	, m_fTime(0.0f)
	, m_fTimeAcc(0.0f)
	, m_szHit(nullptr)
	, m_szTime(nullptr)
	, m_szResult(nullptr)
	, m_szReward(nullptr)
	, m_eCurState(STATE_END)
{
}

CUI_CanvasMiniGame::CUI_CanvasMiniGame(const CUI_CanvasMiniGame& rhs)
	:CUI_Canvas(rhs)
	, m_bResultShow(false)
	, m_fTime(0.0f)
	, m_fTimeAcc(0.0f)
	, m_szHit(nullptr)
	, m_szTime(nullptr)
	, m_szResult(nullptr)
	, m_szReward(nullptr)
	, m_eCurState(STATE_END)
{
}

HRESULT CUI_CanvasMiniGame::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CanvasMiniGame::Initialize(void* pArg)
{
	/* Get a Texture Size, and Make an Initial Matrix */
	XMStoreFloat4x4(&m_matInit, XMMatrixScaling(520.f, 840.f, 1.f));

	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(520.f, 840.f, 1.f));
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components : CUI_CanvasMiniGame");
		return E_FAIL;
	}

	if (FAILED(Ready_Nodes()))
	{
		MSG_BOX("Failed To Ready Nodes : CUI_CanvasMiniGame");
		return E_FAIL;
	}

	/*temp*/
	//m_bActive = true;
	m_fTime = 1.f;

	return S_OK;
}

void CUI_CanvasMiniGame::Tick(_float fTimeDelta)
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

	if (!m_bResultShow)
	{
		m_fTimeAcc += fTimeDelta;
		m_fTime = 1.f;
		if (m_fTimeAcc > m_fTime)
		{
			static_cast<CUI_NodeVictory*>(m_vecNode[UI_VICTORY])->Sign_On();
			m_bResultShow = true;
		}
	}
	else
	{

	}



	__super::Tick(fTimeDelta);


}

void CUI_CanvasMiniGame::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_CanvasMiniGame::Render()
{
	__super::Render();

	{
		//m_szTime = CUtile::Create_String(L"소요 시간 : 10 분 20초");
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		_float2 vNewPos = { vPos.x + g_iWinSizeX * 0.5f - 210.0f, g_iWinSizeY * 0.5f - vPos.y -150.f };
		if (m_szTime != nullptr)
		{
			//Font_Jangmi0 //Font_JungE0 // Font_SR0
			CGameInstance::GetInstance()->Render_Font(TEXT("Font_SR0"), m_szTime,
				vNewPos /* position */,
				0.f, _float2(1.0f, 1.0f)/* size */,
				XMVectorSet(1.f, 1.f, 1.f, 1.f));
		}
	}
	{
		//m_szHit = CUtile::Create_String(L"적중 타겟 : 10 / 10");

		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		_float2 vNewPos = { vPos.x + g_iWinSizeX * 0.5f - 210.0f, g_iWinSizeY * 0.5f - vPos.y - 60.f };
		if (m_szHit != nullptr)
		{
			//Font_Jangmi0 //Font_JungE0 // Font_SR0
			CGameInstance::GetInstance()->Render_Font(TEXT("Font_SR0"), m_szHit,
				vNewPos /* position */,
				0.f, _float2(1.0f, 1.0f)/* size */,
				XMVectorSet(1.f, 1.f, 1.f, 1.f));
		}

	}

	{
		//m_szResult = CUtile::Create_String(L"최종 결과 : 10305034");
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		_float2 vNewPos = { vPos.x + g_iWinSizeX * 0.5f - 210.0f, g_iWinSizeY * 0.5f - vPos.y + 30.f };
		if (m_szResult != nullptr)
		{
			//Font_Jangmi0 //Font_JungE0 // Font_SR0
			CGameInstance::GetInstance()->Render_Font(TEXT("Font_SR0"), m_szResult,
				vNewPos /* position */,
				0.f, _float2(1.0f, 1.0f)/* size */,
				XMVectorSet(1.f, 1.f, 1.f, 1.f));
		}
	}
	//{
	//	m_szReward = CUtile::Create_String(L"보상");
	//	_float4 vPos;
	//	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	//	_float2 vNewPos = { vPos.x + g_iWinSizeX * 0.5f - 180.0f, g_iWinSizeY * 0.5f - vPos.y + 40.f };
	//	if (m_szReward != nullptr)
	//	{
	//		//Font_Jangmi0 //Font_JungE0 // Font_SR0
	//		CGameInstance::GetInstance()->Render_Font(TEXT("Font_SR0"), m_szReward,
	//			vNewPos /* position */,
	//			0.f, _float2(1.0f, 1.0f)/* size */,
	//			XMVectorSet(1.f, 1.f, 1.f, 1.f));
	//	}
	//}

	return S_OK;
}

HRESULT CUI_CanvasMiniGame::Bind()
{
	m_bBindFinished = true;

	return S_OK;
}

HRESULT CUI_CanvasMiniGame::Ready_Nodes()
{
	/* TimeAtk */
	{
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;
		string strCloneTag = "Node_TimeAtk";
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Node_TimeAtk", wstrCloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCloneTag);
		CGameInstance::GetInstance()->Add_String(wstrCloneTag);
	}

	/* HitCount */
	{
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;
		string strCloneTag = "Node_HitCount";
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Node_HitCount", wstrCloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCloneTag);
		CGameInstance::GetInstance()->Add_String(wstrCloneTag);
	}

	/* Victory */
	{
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;
		string strCloneTag = "Node_Victory";
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Node_Victory", wstrCloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCloneTag);
		CGameInstance::GetInstance()->Add_String(wstrCloneTag);
	}

	return S_OK;
}

HRESULT CUI_CanvasMiniGame::SetUp_Components()
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

HRESULT CUI_CanvasMiniGame::SetUp_ShaderResources()
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

void CUI_CanvasMiniGame::BindFunction(CUI_ClientManager::UI_PRESENT eType, _float fValue)
{
}

CUI_CanvasMiniGame* CUI_CanvasMiniGame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_CanvasMiniGame* pInstance = new CUI_CanvasMiniGame(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_CanvasMiniGame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_CanvasMiniGame::Clone(void* pArg)
{
	CUI_CanvasMiniGame* pInstance = new CUI_CanvasMiniGame(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_CanvasMiniGame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CanvasMiniGame::Free()
{
	__super::Free();

	Safe_Delete_Array(m_szHit);
	Safe_Delete_Array(m_szTime);
	Safe_Delete_Array(m_szResult);
	Safe_Delete_Array(m_szReward);

}
