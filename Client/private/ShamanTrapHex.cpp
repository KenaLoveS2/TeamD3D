#include "stdafx.h"
#include "..\public\ShamanTrapHex.h"
#include "GameInstance.h"
#include "ShamanTrapPlane.h"
#include "ShamanTrapGeo.h"
#include "FakeShaman.h"

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
	m_vEdgeColor =	_float4(1.f, 0.f, 1.f, 40.f / 255.f);
	m_vBaseColor =	_float4(1.f, 0.f, 1.f, 40.f / 255.f);
	m_eEFfectDesc.vColor = _float4(1.f, 0.f, 1.f, 40.f / 255.f);
	return S_OK;
}

void CShamanTrapHex::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Tick(fTimeDelta);

	for(int i = 0; i < PARTS_END; ++i)
		m_pPart[i]->Tick(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta);

	// Turn On After Animation
	//m_pTransformCom->Turn(_float4(0.f, 1.f, 0.f, 0.f), fTimeDelta * 0.25f);
}

void CShamanTrapHex::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);

	for (int i = 0; i < PARTS_END; ++i)
		m_pPart[i]->Late_Tick(fTimeDelta);

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
}

_float4 CShamanTrapHex::Get_JointBonePos()
{
	// 로테이션이 0,0,0 에서 가능한 뼈, pos는 y값은 수정을 해야할수도 있습니다.
	CBone* pBone = m_pModelCom->Get_BonePtr("joint6_end");
	_matrix			SocketMatrix =
		pBone->Get_OffsetMatrix() *
		pBone->Get_CombindMatrix() *
		m_pModelCom->Get_PivotMatrix() *
		m_pTransformCom->Get_WorldMatrix();
	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
	_float4x4 pivotMatrix;
	XMStoreFloat4x4(&pivotMatrix, SocketMatrix);
	_float4 vPos = _float4(pivotMatrix._41, pivotMatrix._42, pivotMatrix._43, 1.f);
	return vPos;
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
	{
		CShamanTrapGeo::ShamanTrapDESC Desc;
		ZeroMemory(&Desc, sizeof(CShamanTrapGeo::ShamanTrapDESC));
		XMStoreFloat4x4(&Desc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
		Desc.pSocket = m_pModelCom->Get_BonePtr("Geo");
		Desc.pTargetTransform = m_pTransformCom;
		Safe_AddRef(Desc.pSocket);
		Safe_AddRef(m_pTransformCom);
		m_pPart[GEO] = p_game_instance->Clone_GameObject(TEXT("Prototype_GameObject_ShamanTrapGeo"), TEXT("ShamanTrapGeo"), &Desc);
	}

	//{
	//	CShamanTrapPlane::ShamanTrapDESC Desc;
	//	ZeroMemory(&Desc, sizeof(CShamanTrapPlane::ShamanTrapDESC));
	//	XMStoreFloat4x4(&Desc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	//	Desc.pSocket = m_pModelCom->Get_BonePtr("Geo");
	//	Desc.pTargetTransform = m_pTransformCom;
	//	Safe_AddRef(Desc.pSocket);
	//	Safe_AddRef(m_pTransformCom);
	//	m_pPart[PLANE] = p_game_instance->Clone_GameObject(TEXT("Prototype_GameObject_ShamanTrapPlane"), TEXT("ShamanTrapPlane"), &Desc);
	//}

	{
		CFakeShaman::ShamanTrapDESC Desc;
		ZeroMemory(&Desc, sizeof(CShamanTrapPlane::ShamanTrapDESC));
		XMStoreFloat4x4(&Desc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
		Desc.pSocket = m_pModelCom->Get_BonePtr("joint3_end");
		Desc.pTargetTransform = m_pTransformCom;
		Desc.vPivotRot = _float4(0.f, 2.1f, 0.f, 0.f);
		Safe_AddRef(Desc.pSocket);
		Safe_AddRef(m_pTransformCom);
		m_pPart[SHAMAN_0] = p_game_instance->Clone_GameObject(TEXT("Prototype_GameObject_BossFakeShaman"), TEXT("FakeShaman_0"), &Desc);
	}

	{
		CFakeShaman::ShamanTrapDESC Desc;
		ZeroMemory(&Desc, sizeof(CShamanTrapPlane::ShamanTrapDESC));
		XMStoreFloat4x4(&Desc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
		Desc.pSocket = m_pModelCom->Get_BonePtr("joint4_end");
		Desc.pTargetTransform = m_pTransformCom;
		Desc.vPivotRot = _float4(0.f, -2.1f, 0.f, 0.f);
		Safe_AddRef(Desc.pSocket);
		Safe_AddRef(m_pTransformCom);
		m_pPart[SHAMAN_1] = p_game_instance->Clone_GameObject(TEXT("Prototype_GameObject_BossFakeShaman"), TEXT("FakeShaman_1"), &Desc);
	}

	{
		CFakeShaman::ShamanTrapDESC Desc;
		ZeroMemory(&Desc, sizeof(CShamanTrapPlane::ShamanTrapDESC));
		XMStoreFloat4x4(&Desc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
		Desc.pSocket = m_pModelCom->Get_BonePtr("joint5_end");
		Desc.pTargetTransform = m_pTransformCom;
		Desc.vPivotRot = _float4(0.f, -1.1f, 0.f, 0.f);
		Safe_AddRef(Desc.pSocket);
		Safe_AddRef(m_pTransformCom);
		m_pPart[SHAMAN_2] = p_game_instance->Clone_GameObject(TEXT("Prototype_GameObject_BossFakeShaman"), TEXT("FakeShaman_2"), &Desc);
	}

	{
		CFakeShaman::ShamanTrapDESC Desc;
		ZeroMemory(&Desc, sizeof(CShamanTrapPlane::ShamanTrapDESC));
		XMStoreFloat4x4(&Desc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
		Desc.pSocket = m_pModelCom->Get_BonePtr("joint2_end");
		Desc.vPivotRot = _float4(0.f, 3.25f, 0.f, 0.f);
		Desc.pTargetTransform = m_pTransformCom;
		Safe_AddRef(Desc.pSocket);
		Safe_AddRef(m_pTransformCom);
		m_pPart[SHAMAN_3] = p_game_instance->Clone_GameObject(TEXT("Prototype_GameObject_BossFakeShaman"), TEXT("FakeShaman_3"), &Desc);
	}

	{
		CFakeShaman::ShamanTrapDESC Desc;
		ZeroMemory(&Desc, sizeof(CShamanTrapPlane::ShamanTrapDESC));
		XMStoreFloat4x4(&Desc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
		Desc.pSocket = m_pModelCom->Get_BonePtr("joint7_end");
		Desc.vPivotRot = _float4(0.f, 1.1f, 0.f, 0.f);
		Desc.pTargetTransform = m_pTransformCom;
		Safe_AddRef(Desc.pSocket);
		Safe_AddRef(m_pTransformCom);
		m_pPart[SHAMAN_4] = p_game_instance->Clone_GameObject(TEXT("Prototype_GameObject_BossFakeShaman"), TEXT("FakeShaman_4"), &Desc);
	}

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
