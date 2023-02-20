#include "..\public\UI.h"
#include "Shader.h"
#include "Renderer.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "GameInstance.h"

CUI::CUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
	, m_pParent(nullptr)
	, m_bActive(false)
	, m_iRenderPass(0)
{
}

CUI::CUI(const CUI & rhs)
	: CGameObject(rhs)
	, m_pParent(nullptr)
	, m_bActive(false)
	, m_iRenderPass(0)
{
	m_TextureComTag[TEXTURE_DIFFUSE]	= L"Com_DiffuseTexture";
	m_TextureComTag[TEXTURE_MASK]		= L"Com_MaskTexture";

	XMStoreFloat4x4(&m_matParentInit, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matLocal, XMMatrixIdentity());

}

_fmatrix CUI::Get_WorldMatrix()
{
	return m_pTransformCom->Get_WorldMatrix();
}

void CUI::Set_Parent(CUI* pUI)
{
	m_pParent = pUI;
	Safe_AddRef(m_pParent);

	XMStoreFloat4x4(&m_matParentInit, m_pParent->Get_WorldMatrix());
}

HRESULT CUI::Set_Texture(TEXTURE_TYPE eType, wstring textureComTag)
{
	if (nullptr == m_pTextureCom[eType])
	{
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), textureComTag.c_str(), m_TextureComTag[eType].c_str(),
			(CComponent**)&m_pTextureCom[eType])))
			return E_FAIL;
	}
	else
	{
		/* release previous texture */
		auto iter = find_if(m_Components.begin(), m_Components.end(), CTag_Finder(m_TextureComTag[eType].c_str()));
		if (m_Components.end() == iter)
			return E_FAIL;
		Safe_Release(m_pTextureCom[eType]);
		Safe_Release(m_pTextureCom[eType]);
		m_Components.erase(iter);

		/* add new texture */
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), textureComTag.c_str(), m_TextureComTag[eType].c_str(),
			(CComponent**)&m_pTextureCom[eType])))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX("Failed To Initialize Prototype : CUI");
		return E_FAIL;
	}

	for (_uint i = 0; i < TEXTURE_END; ++i)
		m_pTextureCom[i] = nullptr;

	return S_OK;
}

HRESULT CUI::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX("Failed To Initialize Clone : CUI");
		return E_FAIL;
	}

	for (_uint i = 0; i < TEXTURE_END; ++i)
		m_pTextureCom[i] = nullptr;

	//ZeroMemory(&m_tDesc, sizeof UIDESC);

	//_uint numViewport = 1;
	//D3D11_VIEWPORT	viewport;
	//ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	//m_pContext->RSGetViewports(&numViewport, &viewport);
	//_float fWinSizeX = viewport.Width;
	//_float fWinSizeY = viewport.Height;
	//XMStoreFloat4x4(&m_tDesc.ViewMatrix, XMMatrixIdentity());
	//XMStoreFloat4x4(&m_tDesc.ProjMatrix, XMMatrixOrthographicLH(fWinSizeX, fWinSizeY, 0.f, 1.f));

	//if (pArg == nullptr)
	//{
	//	m_tDesc.fSizeX = fWinSizeX;
	//	m_tDesc.fSizeY = fWinSizeY;

	//	m_tDesc.fPosX = m_tDesc.fSizeX * 0.5f;
	//	m_tDesc.fPosY = m_tDesc.fSizeY * 0.5f;

	//	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,
	//		XMVectorSet(m_tDesc.fPosX - (_float)fWinSizeX * 0.5f, -m_tDesc.fPosY + (_float)fWinSizeY * 0.5f, 0.f, 1.f));
	//}
	//else
	//{
	//	memcpy(&m_tDesc, pArg, sizeof UIDESC);
	//	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,
	//		XMVectorSet(m_tDesc.fPosX - (_float)fWinSizeX * 0.5f, -m_tDesc.fPosY + (_float)fWinSizeY * 0.5f, 0.f, 1.f));

	//}

	return S_OK;
}

void CUI::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI::Render()
{
	__super::Render();

	return S_OK;
}

void CUI::Free()
{
	__super::Free();

	Safe_Release(m_pParent);

	for (_uint i = 0; i < TEXTURE_END; ++i)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
