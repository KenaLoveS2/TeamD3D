#include "stdafx.h"
#include "..\public\UI_NodeSkillVideo.h"
#include "GameInstance.h"
#include <windows.h>
//#include <d3d11.h>
//#include <mfapi.h>
//#include <mfidl.h>
//#include <mfreadwrite.h>
//#include <Mferror.h>
//#include <atlbase.h>
//
//#pragma comment(lib, "d3d11.lib")
//#pragma comment(lib, "mf.lib")
//#pragma comment(lib, "mfplat.lib")
//#pragma comment(lib, "mfreadwrite.lib")

// MFPlay 찾아보기
//https://learn.microsoft.com/ko-kr/windows/win32/medfound/mfplay-tutorial--video-playback
CUI_NodeSkillVideo::CUI_NodeSkillVideo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
{
}

CUI_NodeSkillVideo::CUI_NodeSkillVideo(const CUI_NodeSkillVideo & rhs)
	: CUI_Node(rhs)
{
}


HRESULT CUI_NodeSkillVideo::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeSkillVideo::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(100.f, 100.f, 1.f));
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}

	if (FAILED(Initialize_MF()))
	{
		MSG_BOX("Failed To Create Video Source");
		return E_FAIL;
	}

	m_bActive = true;
	return S_OK;
}

void CUI_NodeSkillVideo::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Tick(fTimeDelta);
}

void CUI_NodeSkillVideo::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeSkillVideo::Render()
{
	if (nullptr == m_pTextureCom[TEXTURE_DIFFUSE])
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}
HRESULT CUI_NodeSkillVideo::SetUp_Components()
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

HRESULT CUI_NodeSkillVideo::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
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


HRESULT CUI_NodeSkillVideo::Initialize_MF()
{
	//// Initialize Media Foundation
	//HRESULT hr = S_OK;
	//hr = MFStartup(MF_VERSION);
	//if (FAILED(hr))
	//{
	//	MSG_BOX("Failed To Initialize");
	//	return E_FAIL;
	//}

	//IMFMediaSource* pSource = NULL;
	//IMFTopology* pTopology = NULL;

	//// Load Video File
	//MF_OBJECT_TYPE ObjectType = MF_OBJECT_INVALID;
	//IMFSourceResolver* pResolver = NULL;
	//if (FAILED(MFCreateSourceResolver(&pResolver)))
	//{
	//	MSG_BOX("Failed To Create SourceResolver");
	//	return E_FAIL;
	//}
	//if (FAILED(pResolver->CreateObjectFromURL(L"../Bin/video.mp4", MF_RESOLUTION_MEDIASOURCE, NULL, &ObjectType, (IUnknown**)&pSource)))
	//{
	//	MSG_BOX("Failed To Load Video");
	//	return E_FAIL;
	//}

	//// Create Topology
	//if (FAILED(MFCreateTopology(&pTopology)))
	//{
	//	MSG_BOX("Failed To Create topology");
	//	return E_FAIL;
	//}

	//// Create/Add Node
	//IMFStreamDescriptor* pSD = NULL;
	//IMFActivate* pSinkActivate = NULL;
	//IMFMediaSink* pSink = NULL;
	//IMFTransform* pDecoder = NULL;

	//// Add Source Node
	//IMFPresentationDescriptor* pPD = NULL;
	//if (FAILED(pSource->CreatePresentationDescriptor(&pPD)))
	//{
	//	MSG_BOX("Failed To Add Source Node");
	//	return E_FAIL;
	//}

	//DWORD cStreams = 0;
	//if (FAILED(pPD->GetStreamDescriptorCount(&cStreams)))
	//{
	//	MSG_BOX("Failed To GetSTreamD...");
	//	return E_FAIL;
	//}

	//for (DWORD i = 0; i < cStreams; i++)
	//{
	//	BOOL fSelected = FALSE;
	//	IMFStreamDescriptor* pSD = NULL;
	//	hr = pPD->GetStreamDescriptorByIndex(i, &fSelected, &pSD);

	//	if (fSelected)
	//	{
	//		IMFTopologyNode* pNode = NULL;
	//		hr = MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &pNode);

	//		hr = pTopology->AddNode(pNode);

	//	}
	//}

	//// Add Sink Node
	//hr = MFCreateAudioRendererActivate(&pSinkActivate);

	//hr = pSinkActivate->ActivateObject(IID_PPV_ARGS(&pSink));
	//
	//IMFTopologyNode* pNode = NULL;
	//hr = MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &pNode);

	//hr = pTopology->AddNode(pNode);
	//

	//IMFMediaSession* pSession = NULL;

	//// 미디어 세션 생성
	//hr = MFCreateMediaSession(NULL, &pSession);
	//// 미디어 세션 초기화 // 토폴로지 설정 실패
	//hr = pSession->SetTopology(0, pTopology);

	//// 재생 시작
	//hr = pSession->Start(NULL, NULL);


	//// 재생 중인 미디어 세션을 정지하고 해제
	//if (pSession != NULL)
	//{
	//	hr = pSession->Stop();
	//	if (FAILED(hr))
	//	{
	//		// 미디어 세션 정지 실패
	//	}
	//	pSession->Shutdown();
	//	pSession->Release();
	//	pSession = NULL;
	//}

	//// 토폴로지 해제
	//if (pTopology != NULL)
	//{
	//	pTopology->Release();
	//	pTopology = NULL;
	//}

	//// 미디어 소스 해제
	//if (pSource != NULL)
	//{
	//	pSource->Release();
	//	pSource = NULL;
	//}

	//// 미디어 Sink 해제
	//if (pSink != NULL)
	//{
	//	pSink->Release();
	//	pSink = NULL;
	//}

	//// 미디어 Sink Activate 해제
	//if (pSinkActivate != NULL)
	//{
	//	pSinkActivate->Release();
	//	pSinkActivate = NULL;
	//}

	//// 미디어 Transform 해제
	//if (pDecoder != NULL)
	//{
	//	pDecoder->Release();
	//	pDecoder = NULL;
	//}

	return S_OK;
}

CUI_NodeSkillVideo * CUI_NodeSkillVideo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeSkillVideo*	pInstance = new CUI_NodeSkillVideo(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeSkillVideo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeSkillVideo::Clone(void * pArg)
{
	CUI_NodeSkillVideo*	pInstance = new CUI_NodeSkillVideo(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeSkillVideo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeSkillVideo::Free()
{
	__super::Free();
}
