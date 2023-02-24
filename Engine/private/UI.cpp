#include "stdafx.h"
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

	XMStoreFloat4x4(&m_matInit, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matParentInit, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matLocal, XMMatrixIdentity());

}

_fmatrix CUI::Get_WorldMatrix()
{
	return m_pTransformCom->Get_WorldMatrix();
}

_fmatrix CUI::Get_InitMatrix()
{
	return XMLoadFloat4x4(&m_matInit);
}

void CUI::Set_Parent(CUI* pUI)
{
	m_pParent = pUI;
	//Safe_AddRef(m_pParent);

	XMStoreFloat4x4(&m_matParentInit, m_pParent->Get_InitMatrix());
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

	//if (pArg != nullptr)
	//{
	//	ZeroMemory(&m_tSpriteInfo, sizeof m_tSpriteInfo);
	//	memcpy(&m_tSpriteInfo, pArg, sizeof m_tSpriteInfo);
	//}
	//else
	{
		m_tSpriteInfo.iXFrames = 1;
		m_tSpriteInfo.iYFrames = 1;
		m_tSpriteInfo.iXFrameNow = 0;
		m_tSpriteInfo.iYFrameNow = 0;
		m_tSpriteInfo.fAnimTime = 0.f;
		m_tSpriteInfo.fAnimTimeAcc = 0.f;
		m_tSpriteInfo.bLoop = false;
		m_tSpriteInfo.bFinished = false;

		m_tUVMoveInfo.fDeltaTime = 0.f;
		m_tUVMoveInfo.fDeltaTimeAcc = 0.f;
		m_tUVMoveInfo.vSpeed = {1.f, 1.f};
		m_tUVMoveInfo.vDelta = { 0.f, 0.f };


		m_tDefaultRenderInfo.fAlpha = 1.f;
		m_tDefaultRenderInfo.fDeltaTime = 0.f;
		m_tDefaultRenderInfo.fDeltaTimeAcc = 0.f;
		m_tDefaultRenderInfo.vColor = { 1.f, 1.f, 1.f, 1.f };
		m_tDefaultRenderInfo.vMinColor = { 0.f, 0.f ,0.f ,0.f };


	}

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

bool	texture_getter(void* data, int index, const char** output)
{
	vector<string>*	 pVec = (vector<string>*)data;
	*output = (*pVec)[index].c_str();
	return true;
}
void CUI::Imgui_RenderingSetting()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	vector<wstring>* pTags = pGameInstance->Get_UIWString(CUI_Manager::WSTRKEY_TEXTURE_PROTOTAG);
	vector<string>* pNames = pGameInstance->Get_UIString(CUI_Manager::STRKEY_TEXTURE_NAME);
	vector<string>* pPasses = pGameInstance->Get_UIString(CUI_Manager::STRKEY_RENDERPASS);
	RELEASE_INSTANCE(CGameInstance);

	_uint iNumTextures = (_uint)pTags->size();

	/* Diffuse */
	static int selected_Diffuse = 0;
	if (ImGui::ListBox(" : Diffuse", &selected_Diffuse, texture_getter, pNames, iNumTextures, 5))
	{
		if (FAILED(Set_Texture(CUI::TEXTURE_DIFFUSE, (*pTags)[selected_Diffuse])))
			MSG_BOX("Failed To Set Diffuse Texture : UIEditor");
	}

	/* Mask */
	static int selected_Mask = 0;
	if (ImGui::ListBox(" : Mask", &selected_Mask, texture_getter, pNames, iNumTextures, 5))
	{
		if (FAILED(Set_Texture(CUI::TEXTURE_MASK, (*pTags)[selected_Mask])))
			MSG_BOX("Failed To Set Mask Texture : UIEditor");
	}

	/* RenderPass */
	static int selected_Pass = 0;
	_uint iNumPasses = (_uint)pPasses->size();
	if (ImGui::ListBox(" : RenderPass", &selected_Pass, texture_getter, pPasses, iNumPasses, 5))
	{
		Set_RenderPass(selected_Pass);
	}

	/* For Shader Variables (Depending on RenderPass. */
	/* if it doesnt work, then it might be not related to the selected Renderpass. */
	ImGui::Separator();
	ImGui::Text("Sprite Animation Setting");
	static int size[2];
	size[0] = m_tSpriteInfo.iXFrames;
	size[1] = m_tSpriteInfo.iYFrames;
	if (ImGui::SliderInt("XFrames", &size[0], 1, 20))
		m_tSpriteInfo.iXFrames = size[0];
	if(ImGui::SliderInt("YFrames", &size[1], 1, 20))
		m_tSpriteInfo.iYFrames = size[1];

	static float fTime;
	fTime = m_tSpriteInfo.fAnimTime;
	if (ImGui::SliderFloat("Duration", &fTime, 0.f, 30.f))
		m_tSpriteInfo.fAnimTime = fTime;

	static bool bLoop;
	bLoop = m_tSpriteInfo.bLoop;
	if (ImGui::Checkbox("IsLoop", &bLoop))
	{
		m_tSpriteInfo.bLoop = bLoop;
		m_tSpriteInfo.bFinished = false;
		m_tSpriteInfo.fAnimTimeAcc = 0.f;
		m_tSpriteInfo.iXFrameNow = 0;
		m_tSpriteInfo.iYFrameNow = 0;

	}

	ImGui::Separator();
	ImGui::Text("UV Animation Setting");

	static float fUVTime;
	fUVTime = m_tUVMoveInfo.fDeltaTime;
	if (ImGui::SliderFloat("UV Duration", &fUVTime, 0.f, 30.f))
		m_tUVMoveInfo.fDeltaTime = fUVTime;

	static float fUVSpeed[2];
	fUVSpeed[0] = m_tUVMoveInfo.vSpeed.x;
	fUVSpeed[1] = m_tUVMoveInfo.vSpeed.y;
	if (ImGui::SliderFloat("UV Speed", fUVSpeed, -50.f, 50.f))
	{
		m_tUVMoveInfo.vSpeed.x = fUVSpeed[0];
		m_tUVMoveInfo.vSpeed.y = fUVSpeed[1];
	}

	ImGui::Separator();
	ImGui::Text("Default RenderSetting");

	static float fAlpha;
	fAlpha = m_tDefaultRenderInfo.fAlpha;

	static float vColor[4];
	vColor[0] = m_tDefaultRenderInfo.vColor.x;
	vColor[1] = m_tDefaultRenderInfo.vColor.y;
	vColor[2] = m_tDefaultRenderInfo.vColor.z;
	vColor[3] = m_tDefaultRenderInfo.vColor.w;
	if (ImGui::SliderFloat4("Color", vColor, 0.f, 1.f))
	{
		m_tDefaultRenderInfo.vColor.x = vColor[0];
		m_tDefaultRenderInfo.vColor.y = vColor[1];
		m_tDefaultRenderInfo.vColor.z = vColor[2];
		m_tDefaultRenderInfo.vColor.w = vColor[3];
	}

	static float vMinColor[4];
	vMinColor[0] = m_tDefaultRenderInfo.vMinColor.x;
	vMinColor[1] = m_tDefaultRenderInfo.vMinColor.y;
	vMinColor[2] = m_tDefaultRenderInfo.vMinColor.z;
	vMinColor[3] = m_tDefaultRenderInfo.vMinColor.w;
	if (ImGui::SliderFloat4("MinColor", vMinColor, 0.f, 1.f))
	{
		m_tDefaultRenderInfo.vMinColor.x = vMinColor[0];
		m_tDefaultRenderInfo.vMinColor.y = vMinColor[1];
		m_tDefaultRenderInfo.vMinColor.z = vMinColor[2];
		m_tDefaultRenderInfo.vMinColor.w = vMinColor[3];
	}

	static float fDeltaTime;
	fDeltaTime = m_tDefaultRenderInfo.fDeltaTime;
	if (ImGui::SliderFloat("DefaultDuration", &fDeltaTime, 0.f, 30.f))
		m_tDefaultRenderInfo.fDeltaTime = fDeltaTime;

	ImGui::Separator();
}

void CUI::Free()
{
	__super::Free();

	//Safe_Release(m_pParent);

	for (_uint i = 0; i < TEXTURE_END; ++i)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
