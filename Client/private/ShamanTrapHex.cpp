#include "stdafx.h"
#include "..\public\ShamanTrapHex.h"
#include "GameInstance.h"
#include "ShamanTrapPlane.h"
#include "ShamanTrapGeo.h"

CShamanTrapHex::CShamanTrapHex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEffect_Mesh(pDevice, pContext)
{
}

CShamanTrapHex::CShamanTrapHex(const CShamanTrapHex& rhs)
	: CEffect_Mesh(rhs)
{
}

HRESULT CShamanTrapHex::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CShamanTrapHex::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	if (pArg == nullptr)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	}
	else
		memcpy(&GameObjectDesc, pArg, sizeof(CGameObject::GAMEOBJECTDESC));

	/* 모델을 별도로 설정해 주기 때문에 Desc 일부 변경 해줌 */
	m_eEFfectDesc.eMeshType = CEffect_Base::tagEffectDesc::MESH_END; // Mesh 생성 안함
	m_iTotalDTextureComCnt = 0;	m_iTotalMTextureComCnt = 0;

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	_matrix matiden = XMMatrixIdentity();
	m_pTransformCom->Set_WorldMatrix(matiden);

	Push_EventFunctions();
	m_eEFfectDesc.bActive = true;

	m_pPartBone = m_pModelCom->Get_BonePtr("Geo");
	m_eEFfectDesc.vColor = _float4(1.f, 0.f, 1.f, 0.3f);
	return S_OK;
}

void CShamanTrapHex::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Tick(fTimeDelta);
	m_pPart[GEO]->Tick(fTimeDelta);
	m_pPart[PLANE]->Tick(fTimeDelta);
	m_pModelCom->Play_Animation(fTimeDelta);
	m_pTransformCom->Turn(_float4(0.f, 1.f, 0.f, 0.f), fTimeDelta * 0.5f);
}

void CShamanTrapHex::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
	m_pPart[GEO]->Late_Tick(fTimeDelta);
	//m_pPart[PLANE]->Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CShamanTrapHex::Render()
{
	if (m_eEFfectDesc.bActive == false)
		return E_FAIL;

	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 4), E_FAIL);
	}

	return S_OK;
}

_bool CShamanTrapHex::IsActiveState()
{
	if (m_eEFfectDesc.bActive == false)
		return false;

	return false;
}

void CShamanTrapHex::ImGui_AnimationProperty()
{
	CEffect_Mesh::ImGui_AnimationProperty();
	if (ImGui::CollapsingHeader("ShamanTrapHex"))
	{
		ImGui::BeginTabBar("BossHunter Animation & State");

		if (ImGui::BeginTabItem("Animation"))
		{
			m_pModelCom->Imgui_RenderProperty();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

void CShamanTrapHex::Imgui_RenderProperty()
{
	if (ImGui::Button("Recompile"))
		m_pShaderCom->ReCompile();
	SetColor(m_vBaseColor, "BaseColor");
	m_eEFfectDesc.vColor = m_vBaseColor;
	SetColor(m_vEdgeColor, "EdgeColor");
	m_pPart[GEO]->Imgui_RenderProperty();
	//m_pPart[PLANE]->Imgui_RenderProperty();
}

HRESULT CShamanTrapHex::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxEffectAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_ShamanTrapHex", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);
	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(i, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/NonAnim/ShamanTrapDecal/T_Shaman_WallTextures_EdgeLines_Thin.png")), E_FAIL);
	}

	CGameInstance* p_game_instance = GET_INSTANCE(CGameInstance)
	CShamanTrapGeo::ShamanTrapDESC GeoTrapDesc;
	ZeroMemory(&GeoTrapDesc, sizeof(CShamanTrapGeo::ShamanTrapDESC));
	XMStoreFloat4x4(&GeoTrapDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	GeoTrapDesc.pSocket = m_pModelCom->Get_BonePtr("Geo");
	GeoTrapDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(GeoTrapDesc.pSocket);
	Safe_AddRef(m_pTransformCom);
	m_pPart[GEO] = p_game_instance->Clone_GameObject(TEXT("Prototype_GameObject_ShamanTrapGeo"), TEXT("ShamanTrapGeo"), &GeoTrapDesc);

	CShamanTrapPlane::ShamanTrapDESC ShamanTrapDesc;
	ZeroMemory(&ShamanTrapDesc, sizeof(CShamanTrapPlane::ShamanTrapDESC));
	XMStoreFloat4x4(&ShamanTrapDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	ShamanTrapDesc.pSocket = m_pModelCom->Get_BonePtr("Geo");
	ShamanTrapDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(ShamanTrapDesc.pSocket);
	Safe_AddRef(m_pTransformCom);
	m_pPart[PLANE] = p_game_instance->Clone_GameObject(TEXT("Prototype_GameObject_ShamanTrapPlane"), TEXT("ShamanTrapPlane"), &ShamanTrapDesc);
	RELEASE_INSTANCE(CGameInstance)

	return S_OK;
}

HRESULT CShamanTrapHex::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vEdgeLineColor", &m_vEdgeColor, sizeof(_float4)), E_FAIL);
	return S_OK;
}

void CShamanTrapHex::SetColor(_float4& vColor, const char* pTag)
{
	ImGui::Begin(pTag);
	ImGui::Text(pTag);
	static bool alpha_preview = true;
	static bool alpha_half_preview = false;
	static bool drag_and_drop = true;
	static bool options_menu = true;
	static bool hdr = false;
	ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
	static bool   ref_color = false;
	static ImVec4 ref_color_v(1.0f, 1.0f, 1.0f, 1.0f);
	static _float4 vSelectColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	vSelectColor = vColor;
	ImGui::ColorPicker4("CurColor##6", (float*)&vSelectColor, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color ? &ref_color_v.x : NULL);
	ImGui::ColorEdit4("Diffuse##5f", (float*)&vSelectColor, ImGuiColorEditFlags_DisplayRGB | misc_flags);
	vColor = vSelectColor;
	ImGui::End();
}

CShamanTrapHex* CShamanTrapHex::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShamanTrapHex * pInstance = new CShamanTrapHex(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CShamanTrapHex Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CShamanTrapHex::Clone(void* pArg)
{
	CShamanTrapHex * pInstance = new CShamanTrapHex(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CShamanTrapHex Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CShamanTrapHex::Free()
{
	CEffect_Mesh::Free();

	for(int i = 0; i < PARTS_END; i++)
		Safe_Release(m_pPart[i]);
}
