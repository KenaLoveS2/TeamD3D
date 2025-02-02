#include "stdafx.h"
#include "..\public\BackGround.h"
#include "GameInstance.h"
#include "UI_MousePointer.h"

CBackGround::CBackGround(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
	, m_iTextureIndex(0)
{
}

CBackGround::CBackGround(const CBackGround & rhs)
	: CGameObject(rhs)
	, m_iTextureIndex(0)
{
}

HRESULT CBackGround::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackGround::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));	

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;	

	m_fSizeX = (_float)g_iWinSizeX;
	m_fSizeY = (_float)g_iWinSizeY;

	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.5f;	
		
	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,
		XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));	

	m_fAlpha = 0.f;
	m_fAlphaDelta = 0.01f;
	m_fSpeed = 0.f;
	m_fTimeAcc = 0.f;
	m_fTime = 0.1f;

	return S_OK;
}

void CBackGround::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fTime = 0.01f;
	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc > m_fTime)
	{
		m_fAlpha += m_fAlphaDelta;
		if (m_fAlpha > 1.f || m_fAlpha< 0.f)
			m_fAlphaDelta *= -1;

		m_fSpeed += fTimeDelta * 0.005f;

		m_fTimeAcc = 0.f;
	}
}

void CBackGround::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);	

	if(g_LEVEL == LEVEL_LOGO)
	{
		::SetCursor(NULL);
		CUI_MousePointer* pMouse = static_cast<CUI_MousePointer*>(CGameInstance::GetInstance()->Get_GameObjectPtr(
			g_LEVEL, L"Layer_Canvas", L"Clone_MousePointer"));
		if (pMouse != nullptr)
			pMouse->Set_Active(true);
	}

	m_pRendererCom && m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CBackGround::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CBackGround::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_Logo"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_Noise"), TEXT("Com_NoiseTexture"),
		(CComponent**)&m_pNoiseTexture)))
		return E_FAIL;


	///* For.Com_Tranform */
	//

	//if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Transform"), TEXT("Com_Tranform"),
	//	(CComponent**)&m_pTransformCom, &TransformDesc)))
	//	return E_FAIL;
	

	return S_OK;
}

HRESULT CBackGround::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	// m_pShaderCom->Set_Matrix("g_WorldMatrix", &m_WorldMatrix);
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (LEVEL_LOGO == CGameInstance::GetInstance()->Get_CurLevelIndex())
		m_iTextureIndex = 0;
	else
		m_iTextureIndex = 1;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
		return E_FAIL;

	if (FAILED(m_pNoiseTexture->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture")))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Set_RawValue("g_fSpeedX", &m_fSpeed, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;



	return S_OK;
}

CBackGround * CBackGround::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CBackGround*		pInstance = new CBackGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBackGround::Clone(void * pArg)
{
	CBackGround*		pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBackGround::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pNoiseTexture);

}
