#include "stdafx.h"
#include "..\public\Renderer.h"
#include "GameObject.h"
#include "Target_Manager.h"
#include "RenderTarget.h"
#include "Light_Manager.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "PipeLine.h"
#include "PostFX.h"
#include "GameInstance.h"
#include "Level_Manager.h"

CRenderer::CRenderer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
	, m_pTarget_Manager(CTarget_Manager::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
	, m_pLevel_Manager(CLevel_Manager::GetInstance())
{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pLevel_Manager);
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject * pGameObject)
{
	if (nullptr == pGameObject || 
		eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Add_DebugRenderGroup(CComponent * pComponent)
{
	if (nullptr == pComponent)
		return E_FAIL;

	m_DebugObject.push_back(pComponent);

	Safe_AddRef(pComponent);
	
	return S_OK;
}
#endif

HRESULT CRenderer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	D3D11_VIEWPORT			ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof ViewportDesc);

	_uint			iNumViewports = 1;

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	/* For.Target_Diffuse */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Diffuse"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;

	/* For.Target_Normal */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Normal"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, &_float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Depth */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Depth"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.f, 1.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_MtrlSpecular */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_MtrlSpecular"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	
	/* For.Target_MtrlAmbient */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_MtrlAmbient"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Shade */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Shade"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, &_float4(0.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;

	/* For.Target_Specular */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Specular"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;

	/* For.Target_HDR */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_HDR"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.8f, 0.8f, 0.8f, 0.f))))
		return E_FAIL;

	/* For.Target_LDR */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_LDR1"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(0.5f, 0.5f, 0.5f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_LDR2"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(0.5f, 0.5f, 0.5f, 1.f))))
		return E_FAIL;

	/* For.ModelViewer */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_ModelViewer"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, &_float4(0.f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;




	/* For.MRT_Deferred */ /* 디퍼드 렌더링(빛)을 수행하기위해 필요한 데이터들을 저장한 렌더타겟들. */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_MtrlSpecular"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_MtrlAmbient"))))
		return E_FAIL;
	// Model_Preview 렌더링용
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_ModelViewer"), TEXT("Target_ModelViewer"))))
		return E_FAIL;


	/* For.MRT_LightAcc */ /* 빛 연산의 결과를 저장할 렌더타겟들.  */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	// HDR 텍스쳐 렌더링용
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_HDR"), TEXT("Target_HDR"))))
		return E_FAIL;



	/* For. SHADOW */
	m_iShadowWidth = 8000;
	m_iShadowHeight = 4500;

	if (FAILED(Initialize_ShadowResources(m_iShadowWidth, m_iShadowHeight)))
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pShader_PostProcess = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_PostProcessVFX.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (m_pShader_PostProcess == nullptr)
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

#ifdef _DEBUG

	_float fSizeX = 200.f, fSizeY = 200.f;

	// For. Model
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Diffuse"), fSizeX * 0.5f, fSizeY * 0.5f, fSizeX, fSizeY)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Normal"), fSizeX * 0.5f, (fSizeY * 0.5f) + fSizeY , fSizeX, fSizeY)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Depth"), fSizeX * 0.5f, (fSizeY * 0.5f) + (fSizeY * 2.f), fSizeX, fSizeY)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_MtrlSpecular"), (fSizeX * 0.5f) + fSizeX, fSizeY * 0.5f, fSizeX, fSizeY)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_MtrlAmbient"), (fSizeX * 0.5f) + fSizeX, (fSizeY * 0.5f) + fSizeY, fSizeX, fSizeY)))
		return E_FAIL;

	// For. Lighting
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Shade"), (fSizeX * 0.5f) + fSizeX * 2.f, fSizeY * 0.5f, fSizeX, fSizeY)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Specular"), (fSizeX * 0.5f) + fSizeX * 2.f, (fSizeY * 0.5f) + fSizeY, fSizeX, fSizeY)))
		return E_FAIL;

	// For. Shadow
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_ShadowDepth"), (fSizeX * 0.5f) + fSizeX * 3.f, fSizeY * 0.5f, fSizeX, fSizeY)))
		return E_FAIL;
	/* For.ModelViewer */
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_ModelViewer"),1300.f, fSizeY * 0.5f, fSizeX*3, fSizeY*3)))
		return E_FAIL;


#endif
	
	return S_OK;
}

HRESULT CRenderer::Initialize(void * pArg, CGameObject * pOwner)
{
	if (FAILED(__super::Initialize(pArg, pOwner)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Initialize_ShadowResources(_uint iWidth, _uint iHeight)
{
	// For.Target_ShadowDepth
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, L"Target_ShadowDepth", iWidth, iHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	m_pTarget_Manager->Get_Target(L"Target_ShadowDepth")->SetResizable(false);

	// For.MRT_ShadowDepth(Shadow)
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_LightDepth", L"Target_ShadowDepth")))
		return E_FAIL;

	// shadow depth용 depthStencil view 생성
	{
		ID3D11Texture2D*		pDepthStencilTexture = nullptr;

		D3D11_TEXTURE2D_DESC	TextureDesc;
		ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

		TextureDesc.Width = iWidth;
		TextureDesc.Height = iHeight;
		TextureDesc.MipLevels = 1;
		TextureDesc.ArraySize = 1;
		TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		TextureDesc.SampleDesc.Quality = 0;
		TextureDesc.SampleDesc.Count = 1;

		TextureDesc.Usage = D3D11_USAGE_DEFAULT;
		TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		TextureDesc.CPUAccessFlags = 0;
		TextureDesc.MiscFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
			return E_FAIL;

		if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pShadowDepthStencilView)))
			return E_FAIL;

		Safe_Release(pDepthStencilTexture);
	}

	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{
	if (FAILED(Render_Shadow()))
		return E_FAIL;
	if (FAILED(Render_NonAlphaBlend()))
		return E_FAIL;
	if (FAILED(Render_LightAcc()))
		return E_FAIL;

	if(CPostFX::GetInstance()->IsPostFXOn())
	{
		if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_HDR"))))
			return E_FAIL;
		if (FAILED(Render_Priority()))
			return E_FAIL;
		if (FAILED(Render_Blend()))
			return E_FAIL;
		if (FAILED(Render_NonLight()))
			return E_FAIL;
		if (FAILED(Render_AlphaBlend()))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_HDR"))))
			return E_FAIL;
		if (FAILED(Render_HDR()))
			return E_FAIL;
		if (FAILED(Render_PostProcess()))
			return E_FAIL;
	}
	else
	{
		if (FAILED(Render_Priority()))
			return E_FAIL;
		if (FAILED(Render_Blend()))
			return E_FAIL;
		if (FAILED(Render_NonLight()))
			return E_FAIL;
		if (FAILED(Render_AlphaBlend()))
			return E_FAIL;
	}

	if (FAILED(Render_UI()))
		return E_FAIL;

#ifdef _DEBUG


	// Model Viewer
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_ModelViewer"))))
		return E_FAIL;
	
	if (FAILED(Render_Viewer()))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_ModelViewer"))))
		return E_FAIL;

	// ~ Model Viewer

	if (FAILED(Render_DebugObject()))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
		if (pGameInstance->Get_DIKeyState(DIK_F8) & 0x8000)
		{
			m_bDebugRender = !m_bDebugRender;
		}
	RELEASE_INSTANCE(CGameInstance);

	if (nullptr != m_pTarget_Manager && m_bDebugRender)
	{
		m_pTarget_Manager->Render_Debug(TEXT("MRT_Deferred"));
		m_pTarget_Manager->Render_Debug(TEXT("MRT_LightAcc"));
		m_pTarget_Manager->Render_Debug(TEXT("MRT_LightDepth"));
		m_pTarget_Manager->Render_Debug(TEXT("MRT_ModelViewer"));
	
	}
#endif

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Shadow()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pContext->ClearDepthStencilView(m_pShadowDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	D3D11_VIEWPORT			ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof ViewportDesc);

	_uint			iNumViewports = 1;
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	D3D11_VIEWPORT			DepthViewportDesc = ViewportDesc;
	DepthViewportDesc.Width = (_float)m_iShadowWidth;
	DepthViewportDesc.Height = (_float)m_iShadowHeight;
	m_pContext->RSSetViewports(iNumViewports, &DepthViewportDesc);

	/* Shadow */
	if (FAILED(m_pTarget_Manager->Begin_MRTwithDepthStencil(m_pContext, TEXT("MRT_LightDepth"), m_pShadowDepthStencilView)))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_SHADOW])
	{
		if (nullptr != pGameObject)
			pGameObject->RenderShadow();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_SHADOW].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_LightDepth"))))
		return E_FAIL;

	m_pContext->RSSetViewports(iNumViewports, &ViewportDesc);

	return S_OK;
}

HRESULT CRenderer::Render_NonAlphaBlend()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	/* Diffuse + Normal */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Deferred"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHABLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_NONALPHABLEND].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_Deferred"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	/* Shade */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_LightAcc"))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;	

	if (FAILED(m_pShader->Set_ShaderResourceView("g_NormalTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Normal")))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_ShaderResourceView("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth")))))
		return E_FAIL;

	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);	

	if (FAILED(m_pShader->Set_Matrix("g_ProjMatrixInv", &pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Matrix("g_ViewMatrixInv", &pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_vCamPosition", &pPipeLine->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CPipeLine);

	/* 빛 갯수만큼 사각형 버퍼(셰이드타겟의 전체 픽셀을 갱신할 수 있는 사이즈로 그려지는 정점버퍼. )를 그린다. */
	m_pLight_Manager->Render_Light(m_pVIBuffer, m_pShader);
	
	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_LightAcc"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	if (FAILED(m_pShader->Set_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Set_ShaderResourceView("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Diffuse")))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_ShaderResourceView("g_ShadeTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Shade")))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_ShaderResourceView("g_SpecularTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_ShaderResourceView("g_MtrlSpecularTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_MtrlSpecular")))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_ShaderResourceView("g_MtrlAmbientTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_MtrlAmbient")))))
		return E_FAIL;

	/* For. Shadow */
	if (FAILED(m_pShader->Set_ShaderResourceView("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth")))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_ShaderResourceView("g_ShadowTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_ShadowDepth")))))
		return E_FAIL;

	CGameInstance* pInst = GET_INSTANCE(CGameInstance)

	if(pInst->Get_CameraFar() != nullptr)
	{
		if (FAILED(m_pShader->Set_RawValue("g_fFar", pInst->Get_CameraFar(), sizeof(float))))
			return E_FAIL;

		const float fWidth = static_cast<float>(m_iShadowWidth);
		const float fHeight = static_cast<float>(m_iShadowHeight);

		if (FAILED(m_pShader->Set_RawValue("g_fTexcelSizeX", &fWidth, sizeof(float))))
			return E_FAIL;

		if (FAILED(m_pShader->Set_RawValue("g_fTexcelSizeY", &fHeight, sizeof(float))))
			return E_FAIL;
	}

	if (FAILED(m_pShader->Set_Matrix("g_ViewMatrixInv", &pInst->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Matrix("g_ProjMatrixInv", &pInst->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Matrix("g_LightProjMatrix", &pInst->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Matrix("g_LightViewMatrix", &pInst->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance)

	m_pShader->Begin(3);
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		pGameObject && pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_AlphaBlend()
{
	m_RenderObjects[RENDER_ALPHABLEND].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
	{
		return pSour->Get_CamDistance() > pDest->Get_CamDistance();
	});

	for (auto& pGameObject : m_RenderObjects[RENDER_ALPHABLEND])
	{
		pGameObject && pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_ALPHABLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_HDR()
{
	CRenderTarget* pLDR1 = m_pTarget_Manager->Get_Target(L"Target_LDR1");
	CRenderTarget* pLDR2 = m_pTarget_Manager->Get_Target(L"Target_LDR2");
	pLDR1->Clear();
	pLDR2->Clear();

	CPostFX::GetInstance()->PostProcessing(m_pTarget_Manager->Get_SRV(L"Target_HDR"), pLDR1->Get_RTV());
	return S_OK;
}

HRESULT CRenderer::Render_PostProcess()
{
	if (FAILED(m_pShader_PostProcess->Set_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader_PostProcess->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader_PostProcess->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	CRenderTarget* pLDRSour = m_pTarget_Manager->Get_Target(L"Target_LDR1");
	CRenderTarget* pLDRDest = m_pTarget_Manager->Get_Target(L"Target_LDR2");
	ID3D11RenderTargetView*	pBackBufferView = nullptr;
	ID3D11DepthStencilView*		pDepthStencilView = nullptr;
	m_pContext->OMGetRenderTargets(1, &pBackBufferView, &pDepthStencilView);
	m_pContext->GSSetShader(nullptr, nullptr, 0);
	
	m_pContext->OMSetRenderTargets(1, &pBackBufferView, pDepthStencilView);
	Safe_Release(pBackBufferView);
	Safe_Release(pDepthStencilView);

	// LDR to Backbuffer
	if (FAILED(m_pShader_PostProcess->Set_ShaderResourceView("g_LDRTexture", pLDRSour->Get_SRV())))
		return E_FAIL;

	m_pShader_PostProcess->Begin(0);
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	///* Sorting */
	//for (auto& pGameObject : m_RenderObjects[RENDER_UI])
	//{
	// list.sort();
	//}

	for (auto& pGameObject : m_RenderObjects[RENDER_UI])
	{
		pGameObject && pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Viewer()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_VIEWER])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_VIEWER].clear();


	return S_OK;

}


#ifdef _DEBUG
HRESULT CRenderer::Render_DebugObject()
{
	for (auto& pComponent : m_DebugObject)
	{
		pComponent && pComponent->Render();

		Safe_Release(pComponent);
	}

	m_DebugObject.clear();

	return S_OK;	
}
#endif

CRenderer * CRenderer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRenderer*		pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRenderer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CRenderer * CRenderer::Clone(void * pArg, CGameObject * pOwner)
{
	AddRef();
	return this;
}

void CRenderer::Free()
{
	__super::Free();
#ifdef _DEBUG
	for (auto& pComponent : m_DebugObject)
		Safe_Release(pComponent);

	m_DebugObject.clear();
#endif
	for (_uint i = 0; i < RENDER_END; ++i)
	{
		for (auto& pGameObject : m_RenderObjects[i])
			Safe_Release(pGameObject);

		m_RenderObjects[i].clear();
	}

	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pShader_PostProcess);

	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pShadowDepthStencilView);
}
