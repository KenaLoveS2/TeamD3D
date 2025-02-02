#include "stdafx.h"
#include "..\public\ShamanTrapGeo.h"
#include "GameInstance.h"
#include "ShamanTrapHex.h"

CShamanTrapGeo::CShamanTrapGeo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEffect(pDevice, pContext)
{
}

CShamanTrapGeo::CShamanTrapGeo(const CShamanTrapGeo& rhs)
	: CEffect(rhs)
{
}

HRESULT CShamanTrapGeo::Initialize_Prototype(const _tchar* pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CShamanTrapGeo::Initialize(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_Desc, pArg, sizeof(m_Desc));

	CGameObject::GAMEOBJECTDESC GameObjDesc;
	ZeroMemory(&GameObjDesc, sizeof(CGameObject::GAMEOBJECTDESC));
	GameObjDesc.TransformDesc.fSpeedPerSec = 3.f;
	GameObjDesc.TransformDesc.fRotationPerSec = 0.f;

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	_matrix matiden = XMMatrixIdentity();
	m_pTransformCom->Set_WorldMatrix(matiden);

	Push_EventFunctions();
	m_eEFfectDesc.bActive = true;

	m_vPivotPos = _float4(0.f, 0.f, 0.f, 0.f);
	m_vPivotRot = _float4(-1.575f, 0.f, -0.525f, 0.f);
	m_fHDRValue = 3.f;

	m_vecChild[0]->Set_Parent(this);
	m_vecChild[0]->Set_HDRValue(3.f);
	return S_OK;
}

void CShamanTrapGeo::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (dynamic_cast<CShamanTrapHex*>(m_pParent)->Get_Active() == true)
	{
		m_eEFfectDesc.bActive = true;
		for (auto& pChild : m_vecChild)
			pChild->Set_Active(true);
	}
}

void CShamanTrapGeo::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);

	if (m_pParent)	Set_Matrix();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CShamanTrapGeo::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	return S_OK;
}

_bool CShamanTrapGeo::IsActiveState()
{
	if (m_eEFfectDesc.bActive == false)
		return false;

	return false;
}

void CShamanTrapGeo::ImGui_AnimationProperty()
{
}

void CShamanTrapGeo::Imgui_RenderProperty()
{
	/*ImGui::Begin("hamanTrapGeo");
	_float3 vPivot = m_vPivotPos;
	float fPos[3] = { vPivot.x, vPivot.y, vPivot.z };
	ImGui::DragFloat3("PivotPos", fPos, 0.01f, -100.f, 100.0f);
	m_vPivotPos.x = fPos[0];
	m_vPivotPos.y = fPos[1];
	m_vPivotPos.z = fPos[2];
	_float3 vRotPivot = m_vPivotRot;
	float fRotPos[3] = { vRotPivot.x, vRotPivot.y, vRotPivot.z };
	ImGui::DragFloat3("PivotRot", fRotPos, 0.01f, -100.f, 100.0f);
	m_vPivotRot.x = fRotPos[0];
	m_vPivotRot.y = fRotPos[1];
	m_vPivotRot.z = fRotPos[2];
	ImGui::End();*/
	ImGui::Begin("ShamanTrapGeo");
	if (ImGui::Button("Recompile"))
		m_pShaderCom->ReCompile();

	static bool alpha_preview = true;
	static bool alpha_half_preview = false;
	static bool drag_and_drop = true;
	static bool options_menu = true;
	static bool hdr = false;
	ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
	static bool   ref_color = false;
	static ImVec4 ref_color_v(1.0f, 1.0f, 1.0f, 1.0f);
	static _float4 vSelectColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	vSelectColor = m_eEFfectDesc.vColor;
	ImGui::ColorPicker4("CurColor##6", (float*)&vSelectColor, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color ? &ref_color_v.x : NULL);
	ImGui::ColorEdit4("Diffuse##5f", (float*)&vSelectColor, ImGuiColorEditFlags_DisplayRGB | misc_flags);
	m_eEFfectDesc.vColor = vSelectColor;

	//ImGui::Begin("ShamanTrapPlane");
	//_float3 vPivot = m_vPivotPos;
	//float fPos[3] = { vPivot.x, vPivot.y, vPivot.z };
	//ImGui::DragFloat3("PivotPos", fPos, 0.01f, -100.f, 100.0f);
	//m_vPivotPos.x = fPos[0];
	//m_vPivotPos.y = fPos[1];
	//m_vPivotPos.z = fPos[2];
	//_float3 vRotPivot = m_vPivotRot;
	//float fRotPos[3] = { vRotPivot.x, vRotPivot.y, vRotPivot.z };
	//ImGui::DragFloat3("PivotRot", fRotPos, 0.01f, -100.f, 100.0f);
	//m_vPivotRot.x = fRotPos[0];
	//m_vPivotRot.y = fRotPos[1];
	//m_vPivotRot.z = fRotPos[2];
	ImGui::End();
}

HRESULT CShamanTrapGeo::SetUp_Components()
{
	//FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxEffectModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);
	//FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_ShamanTrap_DecalGeo_Rescale", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	//m_iNumMeshes = m_pModelCom->Get_NumMeshes();
	//for (_uint i = 0; i < m_iNumMeshes; ++i)
	//	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(i, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Anim/Enemy/Boss_TrapAsset/Noise_cloudsmed.png")), E_FAIL);

	return S_OK;
}

HRESULT CShamanTrapGeo::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);


	return S_OK;
}

CShamanTrapGeo* CShamanTrapGeo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath)
{
	CShamanTrapGeo * pInstance = new CShamanTrapGeo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CShamanTrapGeo Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CShamanTrapGeo::Clone(void* pArg)
{
	CShamanTrapGeo * pInstance = new CShamanTrapGeo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CShamanTrapGeo Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CShamanTrapGeo::Free()
{
	__super::Free();
}
