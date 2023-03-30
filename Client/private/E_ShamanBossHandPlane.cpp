#include "stdafx.h"
#include "..\public\E_ShamanBossHandPlane.h"
#include "GameInstance.h"
#include "E_ShamanBossPlate.h"

CE_ShamanBossHandPlane::CE_ShamanBossHandPlane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_ShamanBossHandPlane::CE_ShamanBossHandPlane(const CE_ShamanBossHandPlane& rhs)
	: CEffect_Mesh(rhs)
{
}

HRESULT CE_ShamanBossHandPlane::Initialize_Prototype(const _tchar* pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_ShamanBossHandPlane::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 4.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	m_eEFfectDesc.bActive = false;
	m_fHDRValue = 2.f;
	m_eEFfectDesc.vColor = XMVectorSet(153.f, 195.f, 255.f, 255.f) / 255.f;
	m_pTransformCom->Set_Scaled(_float3(11.f, 11.f, 11.f));
	//m_pTransformCom->Set_Scaled(_float3(8.f, 8.f, 8.f));
	return S_OK;
}

HRESULT CE_ShamanBossHandPlane::Late_Initialize(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Base* pEffectBase = nullptr;

	/* Plate */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_ShamanBossPlate", L"Hand_Plate"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	dynamic_cast<CE_ShamanBossPlate*>(pEffectBase)->Set_EffectType(CE_ShamanBossPlate::EFFECT_HANDSET);
	m_vecChild.push_back(pEffectBase);

	return S_OK;
}

void CE_ShamanBossHandPlane::Tick(_float fTimeDelta)
{
	for (auto& pChild : m_vecChild)
		pChild->Set_Active(m_eEFfectDesc.bActive);

	if (m_eEFfectDesc.bActive == false)
	{
		m_fTimeDelta = 0.0f;
		return;
	}
	else
		m_fTimeDelta += fTimeDelta;

	__super::Tick(fTimeDelta);
}

void CE_ShamanBossHandPlane::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);

	if (m_pParent != nullptr)
	{
		/*  Billboard */
		_float4 vCamLook = CGameInstance::GetInstance()->Get_CamLook_Float4();
		_float4 vCamUp = CGameInstance::GetInstance()->Get_CamUp_Float4();
		_float4 vCamRight = CGameInstance::GetInstance()->Get_CamRight_Float4();
		_float4 vPos = m_pTransformCom->Get_Position();

		_float4 vDir = XMVector3Normalize(vCamLook - vPos);
		_float3 vScaled = _float3(8.f, 8.f, 8.f);

		m_pTransformCom->Set_Right(vCamRight * -1.f * vScaled.x);
		m_pTransformCom->Set_Up(vDir * vScaled.y);
		m_pTransformCom->Set_Look(vCamUp * vScaled.x);

		_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		_float4x4 WorldMatrix_float4x4;
		XMStoreFloat4x4(&WorldMatrix_float4x4, WorldMatrix);
		m_vecChild[0]->Set_WorldMatrix(WorldMatrix_float4x4);
		/*  Billboard */

		//_vector vLook = XMVector3Normalize(WorldMatrix.r[2]);
		//m_vecChild[0]->Get_TransformCom()->Turn(vDir, fTimeDelta);
	}

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CE_ShamanBossHandPlane::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_pModelCom != nullptr)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, 0, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 0, nullptr, 0);
	}

	return S_OK;
}

void CE_ShamanBossHandPlane::Imgui_RenderProperty()
{
	if (ImGui::Button("RE")) m_pShaderCom->ReCompile();
	ImGui::InputInt("ShamanTexture", (_int*)&m_iShamanTexture);
	ImGui::InputFloat("HDRValue", &m_fHDRValue);

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

	ImGui::ColorPicker4("CurColor##4", (float*)&vSelectColor, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color ? &ref_color_v.x : NULL);
	ImGui::ColorEdit4("Diffuse##2f", (float*)&vSelectColor, ImGuiColorEditFlags_DisplayRGB | misc_flags);
	m_eEFfectDesc.vColor = vSelectColor;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (ImGui::CollapsingHeader("Transform : "))
	{
		ImGuizmo::BeginFrame();
		static float snap[3] = { 1.f, 1.f, 1.f };
		static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
		if (ImGui::RadioButton("MyTranslate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("MyRotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("MyScale", mCurrentGizmoOperation == ImGuizmo::SCALE))
			mCurrentGizmoOperation = ImGuizmo::SCALE;

		static bool useSnap(false);
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];

		const _matrix& matWorld = m_pTransformCom->Get_WorldMatrix();

		ImGuizmo::DecomposeMatrixToComponents((_float*)&matWorld, matrixTranslation, matrixRotation, matrixScale);
		ImGui::InputFloat3("MyTranslate", matrixTranslation);
		ImGui::InputFloat3("MyRotate", matrixRotation);
		ImGui::InputFloat3("MyScale", matrixScale);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, (_float*)&matWorld);

		ImGuiIO& io = ImGui::GetIO();
		RECT		rt;
		GetClientRect(g_hWnd, &rt);
		POINT		LT{ rt.left, rt.top };
		ClientToScreen(g_hWnd, &LT);
		ImGuizmo::SetRect((_float)LT.x, (_float)LT.y, io.DisplaySize.x, io.DisplaySize.y);

		_float4x4		matView, matProj;
		XMStoreFloat4x4(&matView, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
		XMStoreFloat4x4(&matProj, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

		ImGuizmo::Manipulate(
			reinterpret_cast<_float*>(&matView),
			reinterpret_cast<_float*>(&matProj),
			mCurrentGizmoOperation,
			mCurrentGizmoMode,
			(_float*)&matWorld,
			nullptr, useSnap ? &snap[0] : nullptr);

		m_pTransformCom->Set_WorldMatrix(matWorld);
	}
	RELEASE_INSTANCE(CGameInstance);

}

void CE_ShamanBossHandPlane::Reset()
{
}

HRESULT CE_ShamanBossHandPlane::SetUp_ShaderResources()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	FAILED_CHECK_RETURN(m_pShamanTextureCom->Bind_ShaderResource(m_pShaderCom, "g_ShamanTexture", m_iShamanTexture), E_FAIL);

	return S_OK;
}

HRESULT CE_ShamanBossHandPlane::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxBossModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_EffectShaman"), TEXT("Com_ShamanTexture"), (CComponent**)&m_pShamanTextureCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_BossHandPlate"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	m_eEFfectDesc.eMeshType = CEffect_Base::tagEffectDesc::MESH_ETC;

	return S_OK;
}

CE_ShamanBossHandPlane* CE_ShamanBossHandPlane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath)
{
	CE_ShamanBossHandPlane* pInstance = new CE_ShamanBossHandPlane(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_ShamanBossHandPlane Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CE_ShamanBossHandPlane::Clone(void* pArg)
{
	CE_ShamanBossHandPlane* pInstance = new CE_ShamanBossHandPlane(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_ShamanBossHandPlane Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_ShamanBossHandPlane::Free()
{
	__super::Free();

	Safe_Release(m_pShamanTextureCom);
}
