#include "stdafx.h"
#include "..\public\E_RectTrail.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Kena.h"
#include "Monster.h"
#include "Kena_Staff.h"
#include "BossShaman.h"

CE_RectTrail::CE_RectTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Trail(pDevice, pContext)
{
}

CE_RectTrail::CE_RectTrail(const CE_RectTrail & rhs)
	: CEffect_Trail(rhs)
{
}

HRESULT CE_RectTrail::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_RectTrail::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_pVITrailBufferCom = CVIBuffer_Trail::Create(m_pDevice, m_pContext, 100);
	m_eEFfectDesc.bActive = false;

	SetUp_Option(m_eType);

	return S_OK;
}

HRESULT CE_RectTrail::Late_Initialize(void * pArg)
{
	return S_OK;
}

void CE_RectTrail::Tick(_float fTimeDelta)
{
#pragma region Test
	//if( dynamic_cast<CKena_Staff*>(m_pParent))
	//{
	//	ImGui::Begin("RectTrail");

	//	if (ImGui::Button("Recompile"))
	//		m_pShaderCom->ReCompile();

	//	ImGui::InputFloat("DiffuseTexture", &m_eEFfectDesc.fFrame[0]);
	//	ImGui::InputFloat("MaskTexture", &m_eEFfectDesc.fMaskFrame[0]);
	//	ImGui::InputFloat("Life", &m_eEFfectDesc.fLife);
	//	ImGui::InputFloat("Width", &m_eEFfectDesc.fWidth);

	//	static bool alpha_preview = true;
	//	static bool alpha_half_preview = false;
	//	static bool drag_and_drop = true;
	//	static bool options_menu = true;
	//	static bool hdr = false;

	//	ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

	//	static bool   ref_color = false;
	//	static ImVec4 ref_color_v(1.0f, 1.0f, 1.0f, 1.0f);

	//	static _float4 vSelectColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	//	vSelectColor = m_eEFfectDesc.vColor;

	//	ImGui::ColorPicker4("CurColor##6", (float*)&vSelectColor, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color ? &ref_color_v.x : NULL);
	//	ImGui::ColorEdit4("Diffuse##5f", (float*)&vSelectColor, ImGuiColorEditFlags_DisplayRGB | misc_flags);
	//	m_eEFfectDesc.vColor = vSelectColor;

	//	ImGui::End();
	//}
#pragma endregion Test

	if (m_pParent && !lstrcmp(m_pParent->Get_ObjectCloneName(), L"S_IceDagger_0"))
	{
		ImGui::Begin("RectTrail");

		if (ImGui::Button("Recompile"))
			m_pShaderCom->ReCompile();

		ImGui::InputFloat("DiffuseTexture", &m_eEFfectDesc.fFrame[0]);
		ImGui::InputFloat("MaskTexture", &m_eEFfectDesc.fMaskFrame[0]);
		ImGui::InputFloat("Life", &m_eEFfectDesc.fLife);
		ImGui::InputFloat("Width", &m_eEFfectDesc.fWidth);
		ImGui::InputInt("Pass", &m_eEFfectDesc.iPassCnt);
		ImGui::InputFloat("HDRValue", &m_fHDRValue);
		ImGui::InputFloat4("vColor", (_float*)&m_eEFfectDesc.vColor);

		ImGui::End();
	}

	__super::Tick(fTimeDelta);

	if (m_eType == CE_RectTrail::OBJ_BODY_SHAMAN)
		TurnOffSystem(m_fDurationTime, 8.f, fTimeDelta);

	if (m_eEFfectDesc.eTextureRenderType == CEffect_Base::tagEffectDesc::TEX_SPRITE)
		Tick_Sprite(m_fTimeDelta, fTimeDelta);

	Set_KenaStaffOption();

	for (auto& vPosition : m_vecProPos)
		vPosition.w -= fTimeDelta;

	m_vecProPos.erase(remove_if(m_vecProPos.begin(), m_vecProPos.end(), [](const _float4& vPosition) {
		return vPosition.w <= 0.0f;
	}), m_vecProPos.end());

	if (m_pParent != nullptr)
		m_pVITrailBufferCom->Trail_Tick((_int)m_eType, m_pParent, fTimeDelta);
}

void CE_RectTrail::Late_Tick(_float fTimeDelta)
{
 	__super::Trail_LateTick(fTimeDelta);
}

HRESULT CE_RectTrail::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	m_pShaderCom->Begin(m_eEFfectDesc.iPassCnt);
	m_pVITrailBufferCom->Render();

	return S_OK;
}

void CE_RectTrail::Imgui_RenderProperty()
{
}

HRESULT CE_RectTrail::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	//if (FAILED(m_pVITrailBufferCom->Bind_ShaderResouce(m_pShaderCom, "g_InfoMatrix")))
	//	return E_FAIL;
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_Time", &m_fTimeDelta, sizeof(_float)), E_FAIL);
	return S_OK;
}

void CE_RectTrail::SetUp_Option(RECTTRAILTYPE eType)
{
	m_eEFfectDesc.IsTrail = true;
	m_eEFfectDesc.fWidth = 1.f;
	m_eType = eType;

	switch (eType)
	{
	case Client::CE_RectTrail::OBJ_KENA:
		m_eEFfectDesc.fFrame[0] = 126.f;
		m_eEFfectDesc.iPassCnt = 11;
		m_eEFfectDesc.fLife = 0.5f;
		m_eEFfectDesc.vColor = XMVectorSet(114.f, 227.f, 255.f, 255.f) / 255.f;
		break;

	case Client::CE_RectTrail::OBJ_MONSTER:
		m_eEFfectDesc.fFrame[0] = 126.f;
		m_eEFfectDesc.iPassCnt = 11;
		m_eEFfectDesc.fLife = 0.5f;
		m_eEFfectDesc.vColor = XMVectorSet(255.f, 120.f, 120.f, 255.f) / 255.f;
		break;

	case Client::CE_RectTrail::OBJ_BOSS:
		m_eEFfectDesc.fFrame[0] = 126.f;
		m_eEFfectDesc.iPassCnt = 11;
		m_eEFfectDesc.fLife = 0.5f;
		m_eEFfectDesc.fWidth = 5.f;
		m_eEFfectDesc.vColor = XMVectorSet(255.f, 127.f, 255.f, 255.f) / 255.f;
		break;

	case Client::CE_RectTrail::OBJ_B_SHAMAN: // snow
		m_eEFfectDesc.fFrame[0] = 31.f;
		m_eEFfectDesc.iPassCnt = 16;
		m_eEFfectDesc.fLife = 0.5f;
		m_eEFfectDesc.fWidth = 4.f;
		m_eEFfectDesc.vColor = XMVectorSet(255.f, 255.f, 255.f, 255.f) / 255.f;
		m_eEFfectDesc.iSeparateWidth = 2;
		m_eEFfectDesc.iSeparateHeight = 2;
		m_fHDRValue = 3.5f;
		break;

	case Client::CE_RectTrail::OBJ_W_SHAMAN:
		m_eEFfectDesc.fFrame[0] = 126.f;
		m_eEFfectDesc.iPassCnt = 17;
		m_eEFfectDesc.fLife = 0.3f;
		m_eEFfectDesc.fWidth = 15.f;
		m_eEFfectDesc.vColor = XMVectorSet(1.f, 0.2f, 1.f, 0.5f);
		m_fHDRValue = 3.f;
		break;

	case Client::CE_RectTrail::OBJ_B_HUNTER: 
		m_eEFfectDesc.fFrame[0] = 31.f;
		m_eEFfectDesc.iPassCnt = 11; // 13 
		m_eEFfectDesc.fLife = 0.5f;
		m_eEFfectDesc.fWidth = 2.f;
		m_eEFfectDesc.vColor = XMVectorSet(255.f, 255.f, 255.f, 255.f) / 255.f;
		break;

	case Client::CE_RectTrail::OBJ_BODY_SHAMAN:
		m_eEFfectDesc.fFrame[0] = 126.f;
		m_eEFfectDesc.iPassCnt = 11; // 13 
		m_eEFfectDesc.fLife = 0.5f;
		m_eEFfectDesc.fWidth = 3.f;
		m_eEFfectDesc.vColor = XMVectorSet(255.f, 0.0f, 0.0f, 255.f) / 255.f;
		break;

	case Client::CE_RectTrail::OBJ_ROTWISP:
		m_eEFfectDesc.fFrame[0] = 126.f;
		m_eEFfectDesc.iPassCnt = 11;
		m_eEFfectDesc.fLife = 0.5f;
		m_eEFfectDesc.vColor = XMVectorSet(0.3f, 0.3f, 0.8f, 1.f);
		break;

	case Client::CE_RectTrail::OBJ_DEFAULT:
		m_eEFfectDesc.fFrame[0] = 126.f;
		m_eEFfectDesc.iPassCnt = 11;
		m_eEFfectDesc.fLife = 0.5f;
		m_eEFfectDesc.vColor = XMVectorSet(255.f, 255.f, 255.f, 255.f) / 255.f;
		break;

	case Client::CE_RectTrail::OBJ_TRAIL:
		m_eEFfectDesc.fFrame[0] = 26.f;
		m_eEFfectDesc.iPassCnt = 14;
		m_eEFfectDesc.fLife = 0.5f;
		m_eEFfectDesc.fWidth = 13.f;
		m_eEFfectDesc.vColor = XMVectorSet(255.f, 32.f, 0.f, 102.f) / 255.f;

		m_eEFfectDesc.eTextureRenderType = CEffect_Base::tagEffectDesc::TEX_SPRITE;
		m_eEFfectDesc.iSeparateWidth = 8;
		m_eEFfectDesc.iSeparateHeight = 8;
		m_eEFfectDesc.fTimeDelta = 0.3f;
		break;

	}
}

void CE_RectTrail::Set_TexRandomPrint()
{
	m_eEFfectDesc.fWidthFrame = floor(CUtile::Get_RandomFloat(0.0f, m_eEFfectDesc.iSeparateWidth * 1.0f));
	m_eEFfectDesc.fHeightFrame = floor(CUtile::Get_RandomFloat(0.0f, m_eEFfectDesc.iSeparateHeight * 1.0f));
}

void CE_RectTrail::Set_KenaStaffOption()
{
	if (dynamic_cast<CKena_Staff*>(m_pParent))
	{
		CKena* pKena = (CKena*)CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Player"), TEXT("Kena"));

		if (pKena->Get_State(CKena::STATE_PERFECTATTACK) == true)
			m_eEFfectDesc.vColor = XMVectorSet(255.f, 89.0f, 0.0f, 255.f) / 255.f;
		else
			m_eEFfectDesc.vColor = XMVectorSet(114.f, 227.f, 255.f, 255.f) / 255.f;
	}
}

CE_RectTrail * CE_RectTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_RectTrail * pInstance = new CE_RectTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_RectTrail Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_RectTrail::Clone(void * pArg)
{
	CE_RectTrail * pInstance = new CE_RectTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_RectTrail Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_RectTrail::Free()
{
	__super::Free();
}
