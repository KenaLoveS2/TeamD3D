#include "stdafx.h"
#include "..\public\E_KenaPulseCloud.h"
#include "Shader.h"
#include "Effect_Trail.h"
#include "GameInstance.h"
#include "Camera.h"

CE_KenaPulseCloud::CE_KenaPulseCloud(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_KenaPulseCloud::CE_KenaPulseCloud(const CE_KenaPulseCloud & rhs)
	: CEffect(rhs)
{
}

HRESULT CE_KenaPulseCloud::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_KenaPulseCloud::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;
	m_fInitSpriteCnt = _float2(1.0f, 1.0f);
	m_eEFfectDesc.vScale = XMVectorSet(3.f, 3.f, 1.f, 1.f);
	return S_OK;
}

void CE_KenaPulseCloud::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//ImGui::Begin("cloud");

	//if (ImGui::Button("test"))
	//	m_pShaderCom->ReCompile();	

	//static bool alpha_preview = true;
	//static bool alpha_half_preview = false;
	//static bool drag_and_drop = true;
	//static bool options_menu = true;
	//static bool hdr = false;

	//ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

	//static bool   ref_color = false;
	//static ImVec4 ref_color_v(1.0f, 1.0f, 1.0f, 1.0f);

	//static _float4 vSelectColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	//vSelectColor = m_eEFfectDesc.vColor;

	//ImGui::ColorPicker4("CurColor##6", (float*)&vSelectColor, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color ? &ref_color_v.x : NULL);
	//ImGui::ColorEdit4("Diffuse##5f", (float*)&vSelectColor, ImGuiColorEditFlags_DisplayRGB | misc_flags);
	//m_eEFfectDesc.vColor = vSelectColor;

	//ImGui::End();
}

void CE_KenaPulseCloud::Late_Tick(_float fTimeDelta)
{
	if (m_pParent != nullptr)
		Set_Matrix();

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_KenaPulseCloud::Render()
{
	if (m_eEFfectDesc.bActive == false)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;
	
	return S_OK;
}

CE_KenaPulseCloud* CE_KenaPulseCloud::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_KenaPulseCloud* pInstnace = new CE_KenaPulseCloud(pDevice, pContext);

	if (FAILED(pInstnace->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaPulseCloud Create Failed");
		Safe_Release(pInstnace);
	}
	return pInstnace;
}

CGameObject * CE_KenaPulseCloud::Clone(void * pArg)
{
	CE_KenaPulseCloud* pInstnace = new CE_KenaPulseCloud(*this);

	if (FAILED(pInstnace->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaPulseCloud Clone Failed");
		Safe_Release(pInstnace);
	}
	return pInstnace;
}

void CE_KenaPulseCloud::Free()
{
	__super::Free();
}
