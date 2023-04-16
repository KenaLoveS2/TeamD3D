#include "stdafx.h"
#include "UI_CanvasMiniGame.h"
#include "GameInstance.h"
#include "UI_NodeVictory.h"
#include "UI_NodeReward.h"
#include "Kena.h"
#include "Kena_Status.h"

CUI_CanvasMiniGame::CUI_CanvasMiniGame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Canvas(pDevice, pContext)
	, m_bResultShow(false)
	, m_fTime(0.0f)
	, m_fTimeAcc(0.0f)
	//, m_szHit(nullptr)
	//, m_szTime(nullptr)
	//, m_szResult(nullptr)
	//, m_szReward(nullptr)
	, m_iCurState(STATE_END)
	, m_iTimeLeft(0)
	, m_iHitCount(0)
	, m_iResult(0)
	, m_bCountTime(false)
	, m_bCountHit(false)
	, m_bReward(false)

{
}

CUI_CanvasMiniGame::CUI_CanvasMiniGame(const CUI_CanvasMiniGame& rhs)
	:CUI_Canvas(rhs)
	, m_bResultShow(false)
	, m_fTime(0.0f)
	, m_fTimeAcc(0.0f)
	//, m_szHit(nullptr)
	//, m_szTime(nullptr)
	//, m_szResult(nullptr)
	//, m_szReward(nullptr)
	, m_iCurState(STATE_END)
	, m_iTimeLeft(0)
	, m_iHitCount(0)
	, m_iResult(0)
	, m_bCountTime(false)
	, m_bCountHit(false)
	, m_bReward(false)
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

	if (ImGui::Button("minigame test"))
	{
		m_bActive = true;
		m_bResultShow = false;
		m_iCurState = -1;
		m_bReward = false;

		m_iTimeLeft = 300;
		m_iHitCount = 30;
		m_iResult = 0;

	}

	if (!m_bActive)
		return;

	if (CGameInstance::GetInstance()->Key_Down(DIK_SPACE))
	{
		/* Temp code  */
		CKena* pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena"));
		if (pKena != nullptr)
			pKena->Get_Status()->Set_Crystal(pKena->Get_Status()->Get_Crystal() + m_iResult);

		m_bActive = false;
	}

	if (!m_bResultShow)
	{
		m_fTimeAcc += fTimeDelta;
		m_fTime = 1.5f;
		if (m_fTimeAcc > m_fTime)
		{
			m_iCurState = STATE_VICTORY;
			m_fTimeAcc = 0.0f;
			m_bResultShow = true;
			static_cast<CUI_NodeVictory*>(m_vecNode[UI_VICTORY])->Sign_On();
		}
	}
	else
	{
		m_fTimeAcc += fTimeDelta;
		if (m_fTimeAcc > m_fTime && m_iCurState <= STATE_END)
		{
			m_fTimeAcc = 0.f;
			m_iCurState = min(m_iCurState + 1, (_uint)STATE_END);
			CalculateFunction(m_iCurState);
		}

		if (m_bCountTime)
		{
			m_iTimeLeft -= 5;
			m_iResult += 5 * 5;
			if (m_iTimeLeft <= 0)
			{
				m_iTimeLeft = 0;
				m_bCountTime = false;
				m_bCountHit = true;
			}
		}

		if (m_bCountHit)
		{
			m_iHitCount -= 1;
			m_iResult += 1 * 10;
			if (m_iHitCount <= 0)
			{
				m_iHitCount = 0;
				m_bCountHit = false;
				m_bReward = true;
			}
		}

		if (m_bReward == true)
		{
			static_cast<CUI_NodeReward*>(m_vecNode[UI_REWARD])->Open();
			m_bReward = false;
		}
	}

	__super::Tick(fTimeDelta);


}

void CUI_CanvasMiniGame::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	//__super::Late_Tick(fTimeDelta);
	CUI::Late_Tick(fTimeDelta);
	for (auto e : m_vecEvents)
		e->Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && m_bActive)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UIHDR, this);

		for (auto node : m_vecNode)
			node->Late_Tick(fTimeDelta);

		/* Nodes added to RenderList After the canvas. */
		/* So It can be guaranteed that Canvas Draw first */

	}

}

HRESULT CUI_CanvasMiniGame::Render()
{
	__super::Render();

	if (m_iCurState == STATE_TIME_TITLE)
	{
		wstring wstr = L"남은 시간 : ";

		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		_float2 vNewPos = { vPos.x + g_iWinSizeX * 0.5f - 210.0f, g_iWinSizeY * 0.5f - vPos.y - 150.f };
		//if (m_szTime != nullptr)
		//{
			//Font_Jangmi0 //Font_JungE0 // Font_SR0
		CGameInstance::GetInstance()->Render_Font(TEXT("Font_SR0"), wstr.c_str(),
			vNewPos /* position */,
			0.f, _float2(1.0f, 1.0f)/* size */,
			XMVectorSet(1.f, 1.f, 1.f, 1.f));
	}

	if(m_iCurState >= STATE_TIME)
	{
		_int iMin = (_int)m_iTimeLeft / 60;
		_int iSec = (_int)m_iTimeLeft - iMin * 60;

		wstring minute = to_wstring(iMin);
		if (minute.length() < 2)
			minute = L"0" + minute;
		wstring second = to_wstring(iSec);
		if (second.length() < 2)
			second = L"0" + second;
		wstring wstr = L"남은 시간 : " + minute + L"분 " + second + L"초";

		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		_float2 vNewPos = { vPos.x + g_iWinSizeX * 0.5f - 210.0f, g_iWinSizeY * 0.5f - vPos.y -150.f };
		//if (m_szTime != nullptr)
		//{
			//Font_Jangmi0 //Font_JungE0 // Font_SR0
			CGameInstance::GetInstance()->Render_Font(TEXT("Font_SR0"), wstr.c_str(),
				vNewPos /* position */,
				0.f, _float2(1.0f, 1.0f)/* size */,
				XMVectorSet(1.f, 1.f, 1.f, 1.f));
		//}
	}

	if (m_iCurState == STATE_HIT_TITLE)
	{
		wstring wstr = L"적중 타겟 : ";

		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		_float2 vNewPos = { vPos.x + g_iWinSizeX * 0.5f - 210.0f, g_iWinSizeY * 0.5f - vPos.y - 60.f };
		//if (m_szHit != nullptr)
		//{
			//Font_Jangmi0 //Font_JungE0 // Font_SR0
		CGameInstance::GetInstance()->Render_Font(TEXT("Font_SR0"), wstr.c_str(),
			vNewPos /* position */,
			0.f, _float2(1.0f, 1.0f)/* size */,
			XMVectorSet(1.f, 1.f, 1.f, 1.f));
	}

	if(m_iCurState >= STATE_HIT)
	{
		wstring wstr = L"적중 타겟 : " + to_wstring(m_iHitCount);
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		_float2 vNewPos = { vPos.x + g_iWinSizeX * 0.5f - 210.0f, g_iWinSizeY * 0.5f - vPos.y - 60.f };
		//if (m_szHit != nullptr)
		//{
			//Font_Jangmi0 //Font_JungE0 // Font_SR0
			CGameInstance::GetInstance()->Render_Font(TEXT("Font_SR0"), wstr.c_str(),
				vNewPos /* position */,
				0.f, _float2(1.0f, 1.0f)/* size */,
				XMVectorSet(1.f, 1.f, 1.f, 1.f));
		//}

	}

	if(m_iCurState == STATE_RESULT_TITLE)
	{
		wstring wstr = L"최종 결과 : ";

		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		_float2 vNewPos = { vPos.x + g_iWinSizeX * 0.5f - 210.0f, g_iWinSizeY * 0.5f - vPos.y + 30.f };
		//if (m_szResult != nullptr)
		//{
			//Font_Jangmi0 //Font_JungE0 // Font_SR0
			CGameInstance::GetInstance()->Render_Font(TEXT("Font_SR0"), wstr.c_str(),
				vNewPos /* position */,
				0.f, _float2(1.0f, 1.0f)/* size */,
				XMVectorSet(1.f, 1.f, 1.f, 1.f));
		//}
	}

	if (m_iCurState > STATE_RESULT_TITLE)
	{
		wstring wstr = L"최종 결과 : " + to_wstring(m_iResult);

		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		_float2 vNewPos = { vPos.x + g_iWinSizeX * 0.5f - 210.0f, g_iWinSizeY * 0.5f - vPos.y + 30.f };
		//if (m_szResult != nullptr)
		//{
			//Font_Jangmi0 //Font_JungE0 // Font_SR0
		CGameInstance::GetInstance()->Render_Font(TEXT("Font_SR0"), wstr.c_str(),
			vNewPos /* position */,
			0.f, _float2(1.0f, 1.0f)/* size */,
			XMVectorSet(1.f, 1.f, 1.f, 1.f));
		//}
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

	/* Reward */
	{
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;
		string strCloneTag = "Node_Reward";
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Node_Reward", wstrCloneTag, &tDesc));
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
	switch (eType)
	{
	case CUI_ClientManager::MINIGAME_:
		break;
	}

}

void CUI_CanvasMiniGame::CalculateFunction(_int iState)
{
	switch (iState)
	{
	case STATE_RESULT_TITLE:
	{
		break;
	}
	case STATE_CAL_TIME:
	{
		m_bCountTime = true;
		break;
	}
	case STATE_CAL_TARGETS:
	{
		//m_bCountHit = true;
		break;
	}
	}
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

	//Safe_Delete_Array(m_szHit);
	//Safe_Delete_Array(m_szTime);
	//Safe_Delete_Array(m_szResult);
	//Safe_Delete_Array(m_szReward);

}
