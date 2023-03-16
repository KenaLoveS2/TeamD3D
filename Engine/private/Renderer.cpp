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

#include "stb_image.h"

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

void CRenderer::Imgui_Render()
{
	ImGui::Checkbox("SHADOW", &m_bDynamicShadow);
	ImGui::Checkbox("SSAO", &m_bSSAO);
	ImGui::Checkbox("DISTORT", &m_bDistort);
	ImGui::Checkbox("FILMTONEMAPPING", &m_bFilmTonemapping);
	if(ImGui::Button("ReCompile"))
	{
		ReCompile();
	}
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject * pGameObject)
{
	if (nullptr == pGameObject || 
		eRenderGroup >= RENDER_END)
		return E_FAIL;

	if (!m_bDynamicShadow && eRenderGroup == RENDER_SHADOW)
	{
		if(!m_RenderObjects[eRenderGroup].empty())
		{
			for (auto& pGameObject : m_RenderObjects[RENDER_SHADOW])
			{
				Safe_Release(pGameObject);
			}
			m_RenderObjects[RENDER_SHADOW].clear();
		}
	}
	else
	{
		m_RenderObjects[eRenderGroup].push_back(pGameObject);
		Safe_AddRef(pGameObject);
	}
		
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
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Diffuse"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(0.f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;

	/* For.Target_Normal */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Normal"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, &_float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Depth */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Depth"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.f, 1.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_MtrlAmbient */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_MtrlAmbient"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Shade */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Shade"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, &_float4(0.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;

	/* For.Target_Specular */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Specular"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, &_float4(0.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;

	/* For.Target_SSAO */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_SSAO"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_HDR */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_HDR"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.8f, 0.8f, 0.8f, 0.f))))
		return E_FAIL;

	/* For.Target_LDR */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_LDR1"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(0.5f, 0.5f, 0.5f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_LDR2"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(0.5f, 0.5f, 0.5f, 1.f))))
		return E_FAIL;

	// SSAO
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_SSAO"), TEXT("Target_SSAO"))))
		return E_FAIL;

	/* For.MRT_Deferred */ /* ���۵� ������(��)�� �����ϱ����� �ʿ��� �����͵��� ������ ����Ÿ�ٵ�. */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_MtrlAmbient"))))
		return E_FAIL;

	/* For.MRT_LightAcc */ /* �� ������ ����� ������ ����Ÿ�ٵ�.  */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	// HDR �ؽ��� ��������
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_HDR"), TEXT("Target_HDR"))))
		return E_FAIL;

	/* For. SHADOW */
	m_iShadowWidth = 8192;
	m_iShadowHeight = 8192;

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

	m_pShader_SSAO = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_SSAO.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (m_pShader_SSAO == nullptr)
		return  E_FAIL;

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
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_MtrlAmbient"), (fSizeX * 0.5f) + fSizeX, fSizeY * 0.5f, fSizeX, fSizeY)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_SSAO"), (fSizeX * 0.5f) + fSizeX, (fSizeY * 0.5f) + fSizeY, fSizeX, fSizeY)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_HDR"), (fSizeX * 0.5f) + fSizeX, (fSizeY * 0.5f) + (fSizeY * 2.f), fSizeX, fSizeY)))
		return E_FAIL;
	
	// For. Lighting
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Shade"), (fSizeX * 0.5f) + fSizeX * 2.f, fSizeY * 0.5f, fSizeX, fSizeY)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Specular"), (fSizeX * 0.5f) + fSizeX * 2.f, (fSizeY * 0.5f) + fSizeY, fSizeX, fSizeY)))
		return E_FAIL;

	// For. Shadow
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_ShadowDepth"), (fSizeX * 0.5f) + fSizeX * 3.f, fSizeY * 0.5f, fSizeX, fSizeY)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_StaticShadowDepth"), (fSizeX * 0.5f) + fSizeX * 3.f, (fSizeY * 0.5f) + fSizeY, fSizeX, fSizeY)))
		return E_FAIL;

#endif

	CreateTexture(L"../Bin/Resources/PostProcess/Flare_white.png", m_pFlareTexture);

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

	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, L"Target_StaticShadowDepth", iWidth, iHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	m_pTarget_Manager->Get_Target(L"Target_ShadowDepth")->SetResizable(false);

	m_pTarget_Manager->Get_Target(L"Target_StaticShadowDepth")->SetResizable(false);

	// For.MRT_ShadowDepth(Shadow)
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_LightDepth", L"Target_ShadowDepth")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_StaticLightDepth", L"Target_StaticShadowDepth")))
		return E_FAIL;

	// shadow depth�� depthStencil view ����
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

		if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pStaticShadowDepthStencilView)))
			return E_FAIL;

		Safe_Release(pDepthStencilTexture);
	}

	// shadow depth�� depthStencil view ����
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

HRESULT CRenderer::ReCompile()
{
	HRESULT hr;
	hr = m_pShader->ReCompile();
	hr = m_pShader_SSAO->ReCompile();
	hr = m_pShader_PostProcess->ReCompile();

	if (hr == E_FAIL)
		return  E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{
	if(m_bStaticShadow)
	{
		if (FAILED(Render_StaticShadow()))
			return E_FAIL;
		m_bStaticShadow = false;
	}

	if(m_bDynamicShadow)
	{
		if (FAILED(Render_Shadow()))
			return E_FAIL;
	}

	if (FAILED(Render_NonAlphaBlend()))
		return E_FAIL;

	if(m_bSSAO)
	{
		if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_SSAO"))))
			return E_FAIL;
		if (FAILED(Render_SSAO()))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_SSAO"))))
			return E_FAIL;
	}

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
	if (FAILED(Render_UILast()))
		return E_FAIL;

#ifdef _DEBUG
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
		m_pTarget_Manager->Render_Debug(TEXT("MRT_StaticLightDepth"));
		m_pTarget_Manager->Render_Debug(TEXT("MRT_SSAO"));
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

HRESULT CRenderer::Render_StaticShadow()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pContext->ClearDepthStencilView(m_pStaticShadowDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	D3D11_VIEWPORT			ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof ViewportDesc);

	_uint			iNumViewports = 1;
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	D3D11_VIEWPORT			DepthViewportDesc = ViewportDesc;
	DepthViewportDesc.Width = (_float)m_iShadowWidth;
	DepthViewportDesc.Height = (_float)m_iShadowHeight;
	m_pContext->RSSetViewports(iNumViewports, &DepthViewportDesc);

	/* Shadow */
	if (FAILED(m_pTarget_Manager->Begin_MRTwithDepthStencil(m_pContext, TEXT("MRT_StaticLightDepth"), m_pStaticShadowDepthStencilView)))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_STATIC_SHADOW])
	{
		if (nullptr != pGameObject)
			pGameObject->RenderShadow();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_STATIC_SHADOW].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_StaticLightDepth"))))
		return E_FAIL;

	m_pContext->RSSetViewports(iNumViewports, &ViewportDesc);

	return S_OK;
}

HRESULT CRenderer::Render_NonAlphaBlend()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

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

	if (FAILED(m_pShader->Set_ShaderResourceView("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Diffuse")))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_ShaderResourceView("g_NormalTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Normal")))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_ShaderResourceView("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth")))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_ShaderResourceView("g_MtrlAmbientTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_MtrlAmbient")))))
		return E_FAIL;
	
	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);	

	if (FAILED(m_pShader->Set_Matrix("g_ProjMatrixInv", &pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Matrix("g_ViewMatrixInv", &pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_vCamPosition", &pPipeLine->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CPipeLine);

	/* �� ������ŭ �簢�� ����(���̵�Ÿ���� ��ü �ȼ��� ������ �� �ִ� ������� �׷����� ��������. )�� �׸���. */
	m_pLight_Manager->Render_Light(m_pVIBuffer, m_pShader);
	
	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_LightAcc"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_SSAO()
{
	if (FAILED(m_pShader_SSAO->Set_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader_SSAO->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader_SSAO->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader_SSAO->Set_ShaderResourceView("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth")))))
		return E_FAIL;

	if (FAILED(m_pShader_SSAO->Set_ShaderResourceView("g_NormalTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Normal")))))
		return E_FAIL;

	if (FAILED(m_pShader_SSAO->Set_ShaderResourceView("g_AmbientTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_MtrlAmbient")))))
		return E_FAIL;

	CGameInstance* pInst = GET_INSTANCE(CGameInstance)

	if (FAILED(m_pShader_SSAO->Set_Matrix("g_ViewMatrixInv", &pInst->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader_SSAO->Set_Matrix("g_ProjMatrixInv", &pInst->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance)

	m_pShader_SSAO->Begin(0);
	m_pVIBuffer->Render();
		
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

	/* For. Shadow */
	if (FAILED(m_pShader->Set_RawValue("g_bShadow", &m_bDynamicShadow, sizeof(bool))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_ShaderResourceView("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth")))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_ShaderResourceView("g_ShadowTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_ShadowDepth")))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_ShaderResourceView("g_StaticShadowTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_StaticShadowDepth")))))
		return E_FAIL;

	/* For.SSAO*/
	if (FAILED(m_pShader->Set_RawValue("g_bSSAO", &m_bSSAO, sizeof(bool))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_ShaderResourceView("g_SSAOTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_SSAO")))))
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

	if (FAILED(m_pShader->Set_Matrix("g_DynamicLightViewMatrix", &pInst->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW))))
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
	for (auto& pGameObject : m_RenderObjects[RENDER_ALPHABLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Compute_CamDistance();
	}

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

	if (FAILED(m_pShader_PostProcess->Set_ShaderResourceView("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Diffuse")))))
		return E_FAIL;
	if (FAILED(m_pShader_PostProcess->Set_ShaderResourceView("g_NormalTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Normal")))))
		return E_FAIL;
	if (FAILED(m_pShader_PostProcess->Set_ShaderResourceView("g_ShadeTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Shade")))))
		return E_FAIL;
	if (FAILED(m_pShader_PostProcess->Set_ShaderResourceView("g_SpecularTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")))))
		return E_FAIL;
	if (FAILED(m_pShader_PostProcess->Set_ShaderResourceView("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth")))))
		return E_FAIL;

	CRenderTarget* pLDRTmp = pLDRSour;

	/* PostProcess */
	ID3D11ShaderResourceView* pLDRSour_SRV = pLDRSour->Get_SRV();
	ID3D11RenderTargetView* pLDRDest_RTV = pLDRDest->Get_RTV();

	// Test
	/***1***/
	if(m_bDistort)
	{
		if (FAILED(m_pShader_PostProcess->Set_ShaderResourceView("g_LDRTexture", pLDRSour_SRV)))
			return E_FAIL;
		m_pContext->OMSetRenderTargets(1, &pLDRDest_RTV, pDepthStencilView);
		pLDRSour = pLDRDest;
		pLDRDest = pLDRTmp;
		PostProcess_Distort();
	}

	/***2***/

	if(m_bFilmTonemapping)
	{
		pLDRSour_SRV = pLDRSour->Get_SRV();
		pLDRDest_RTV = pLDRDest->Get_RTV();
		if (FAILED(m_pShader_PostProcess->Set_ShaderResourceView("g_LDRTexture", pLDRSour_SRV)))
			return E_FAIL;
		m_pContext->OMSetRenderTargets(1, &pLDRDest_RTV, pDepthStencilView);
		pLDRTmp = pLDRSour;
		pLDRSour = pLDRDest;
		pLDRDest = pLDRTmp;
		PostProcess_FilmTonemapping();
	}

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

HRESULT CRenderer::Render_UILast()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_UILAST])
	{
		pGameObject && pGameObject->Render();
		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_UILAST].clear();

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

HRESULT CRenderer::PostProcess_Distort()
{
	// �� �� �� ���̴��� �� �� �ְԲ� �Ķ���� �����ٰ�.
	// ������ �����ϱ� ������ Begin -> Render �� �� �� ���������

	m_fDistortTime += TIMEDELTA;
	if (FAILED(m_pShader_PostProcess->Set_RawValue("g_Time", &m_fDistortTime, sizeof(float))))
		return E_FAIL;

	m_pShader_PostProcess->Begin(1);
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::PostProcess_FilmTonemapping()
{
	if (FAILED(m_pShader_PostProcess->Set_ShaderResourceView("g_FlareTexture", *m_pFlareTexture)))
		return E_FAIL;
	if (FAILED(m_pShader_PostProcess->Set_ShaderResourceView("g_LightDepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_ShadowDepth")))))
		return E_FAIL;
	m_pShader_PostProcess->Begin(2);
	m_pVIBuffer->Render();
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

	if (m_bPhysXRenderFlag)
		CPhysX_Manager::GetInstance()->Render();
	
	return S_OK;	
}
#endif


HRESULT CRenderer::CreateTexture(const _tchar* pTextureFilePath, ID3D11ShaderResourceView**&  OUT pTexture)
{
	pTexture = new ID3D11ShaderResourceView*();

	_tchar	szTexturePath[MAX_PATH] = TEXT("");
	wsprintf(szTexturePath, pTextureFilePath);
	_tchar			szExt[MAX_PATH] = TEXT("");
	_wsplitpath_s(szTexturePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);
	HRESULT		hr = 0;

	if (!lstrcmp(szExt, TEXT(".tga")))
		return E_FAIL;
	else if (!lstrcmp(szExt, TEXT(".dds")))
		hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szTexturePath, nullptr, pTexture);
	else if (!lstrcmp(szExt, TEXT(".hdr")))
	{
			int width, height, channels;
			char* pTexturePath = new char[MAX_PATH];
			CUtile::WideCharToChar(szTexturePath, pTexturePath);
			float* pData = stbi_loadf(pTexturePath, &width, &height, &channels, STBI_rgb_alpha);
			if (pData != nullptr)
			{
				D3D11_TEXTURE2D_DESC texDesc = {};
				texDesc.Width = width;
				texDesc.Height = height;
				texDesc.MipLevels = 1;
				texDesc.ArraySize = 1;
				texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				texDesc.SampleDesc.Count = 1;
				texDesc.Usage = D3D11_USAGE_DEFAULT;
				texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

				D3D11_SUBRESOURCE_DATA initData = {};
				initData.pSysMem = pData;
				initData.SysMemPitch = width * sizeof(float) * 4;

				ID3D11Texture2D* pTexture2D = nullptr;
				ID3D11ShaderResourceView* pSRV = nullptr;
				hr = m_pDevice->CreateTexture2D(&texDesc, &initData, &pTexture2D);
				if (SUCCEEDED(hr))
				{
					hr = m_pDevice->CreateShaderResourceView(pTexture2D, nullptr, &pSRV);
					if (SUCCEEDED(hr))
					{
						*pTexture = pSRV;
					}
					pTexture2D->Release();
				}

				stbi_image_free(pData);
				Safe_Delete_Array(pTexturePath);
			}
	}
	else
		hr = DirectX::CreateWICTextureFromFile(m_pDevice, szTexturePath, nullptr, pTexture);

		if (FAILED(hr))
		{
			Safe_Release(*pTexture);
			return E_FAIL;
		}

		return S_OK;
}

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
	Safe_Release(m_pShader_SSAO);

	Safe_Release(*m_pFlareTexture);
	Safe_Delete(m_pFlareTexture);

	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pShadowDepthStencilView);
	Safe_Release(m_pStaticShadowDepthStencilView);
}
