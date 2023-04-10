#include "stdafx.h"
#include "..\public\E_ShamanIceDagger.h"
#include "GameInstance.h"
#include "E_RectTrail.h"
#include "BossShaman.h"
#include "Model.h"

CE_ShamanIceDagger::CE_ShamanIceDagger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_ShamanIceDagger::CE_ShamanIceDagger(const CE_ShamanIceDagger& rhs)
	: CEffect_Mesh(rhs)
{
}

HRESULT CE_ShamanIceDagger::Initialize_Prototype(const _tchar* pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_ShamanIceDagger::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 8.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Effects(), E_FAIL);

	m_fHDRValue = 2.f;
	m_eEFfectDesc.fFrame[0] = 20.f;
	m_eEFfectDesc.bActive = false;
	m_eEFfectDesc.vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f);
	m_pTransformCom->Set_Scaled(_float3(0.1f, 0.1f, 0.1f));
	m_pTransformCom->Rotation(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XMConvertToRadians(90.0f));

	return S_OK;
}

HRESULT CE_ShamanIceDagger::Late_Initialize(void* pArg)
{
	m_pKena = (CKena*)CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena");
	NULL_CHECK_RETURN(m_pKena, E_FAIL);

	_float3 vPos = _float3(0.f, 0.f, 0.f);
	_float3 vPivotScale = _float3(0.15f, 0.f, 1.f);
	_float3 vPivotPos = _float3(0.f, 0.f, 0.f);

	// Capsule X == radius , Y == halfHeight
	CPhysX_Manager::PX_SPHERE_DESC PxSphereDesc;
	PxSphereDesc.eType = SPHERE_DYNAMIC;
	PxSphereDesc.pActortag = m_szCloneObjectTag;
	PxSphereDesc.vPos = vPos;
	PxSphereDesc.fRadius = vPivotScale.x;
	PxSphereDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	PxSphereDesc.fDensity = 1.f;
	PxSphereDesc.fAngularDamping = 0.5f;
	PxSphereDesc.fMass = 59.f;
	PxSphereDesc.fLinearDamping = 1.f;
	PxSphereDesc.bCCD = true;
	PxSphereDesc.eFilterType = PX_FILTER_TYPE::MONSTER_WEAPON;
	PxSphereDesc.fDynamicFriction = 0.5f;
	PxSphereDesc.fStaticFriction = 0.5f;
	PxSphereDesc.fRestitution = 0.1f;

	CPhysX_Manager::GetInstance()->Create_Sphere(PxSphereDesc, Create_PxUserData(this, false, COL_MONSTER_WEAPON));

	_smatrix	matPivot = XMMatrixTranslation(vPivotPos.x, vPivotPos.y, vPivotPos.z);
	m_pTransformCom->Add_Collider(PxSphereDesc.pActortag, matPivot);


	return S_OK;
}

void CE_ShamanIceDagger::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	CTransform::TRANSFORMDESC eTransform = m_pTransformCom->Get_TransformDesc();
	eTransform.fSpeedPerSec = 8.f;
	m_pTransformCom->Set_TransformDesc(eTransform);

	m_fTimeDelta += fTimeDelta;
	__super::Tick(fTimeDelta);

	if (m_bChase == false)
		Tick_IceDagger(fTimeDelta);
	else
		Tick_Chase(fTimeDelta);

	if (m_bFinalposition)
	{
		m_fIdle2Chase += fTimeDelta;
		if (m_fIdle2Chase > 3.f + (m_iIndex * 0.1f))
		{
			m_bFinalposition = false;
			m_bChase = true;
			m_fIdle2Chase = 0.0f;
		}
	}

	m_pTransformCom->Tick(fTimeDelta);

	/* MoveMentTrail */
	{
		m_vecChild[CHILD_TRAIL]->Set_Active(true);
		_vector vPos = m_pTransformCom->Get_Position();
		dynamic_cast<CE_RectTrail*>(m_vecChild[CHILD_TRAIL])->Trail_InputPos(vPos);
	}
}

void CE_ShamanIceDagger::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CE_ShamanIceDagger::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	if (m_pModelCom != nullptr)
		m_pModelCom->Render(m_pShaderCom, 0, nullptr, 3);

	return S_OK;
}

void CE_ShamanIceDagger::Imgui_RenderProperty()
{
	__super::ImGui_PhysXValueProperty();

	ImGui::InputFloat("HDRValue", &m_fHDRValue);ImGui::SameLine();
	if (ImGui::Button("RE")) m_pShaderCom->ReCompile(); 

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
	ImGui::SetNextItemWidth(100);
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

void CE_ShamanIceDagger::ImGui_PhysXValueProperty()
{
	__super::ImGui_PhysXValueProperty();
}

void CE_ShamanIceDagger::Reset()
{
	//for (auto& pChild : m_vecChild)
	//	pChild->Set_Active(false);
	m_vecChild[CHILD_BREAK]->Set_Effect(m_pTransformCom->Get_Position(), true);
	m_vecChild[CHILD_TRAIL]->Set_Active(false);

	m_pTransformCom->Set_PositionY(-1000.f);
	m_pKenaPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	m_fDurateionTime = 0.0f;
	m_fIdle2Chase = 0.0f;
	m_eEFfectDesc.bActive = false;
	m_bFinalposition = false;
	m_bChase = false;
}

void CE_ShamanIceDagger::Set_IceDaggerMatrix()
{
	_float4 vCamLook = CGameInstance::GetInstance()->Get_CamLook_Float4();
	_float4 vCamUp = CGameInstance::GetInstance()->Get_CamUp_Float4();
	_float4 vCamRight = CGameInstance::GetInstance()->Get_CamRight_Float4();

	_float4 vLook = XMVector3Normalize(vCamLook - m_pTransformCom->Get_Position());
	_float3 vScaled = m_pTransformCom->Get_Scaled();

	m_pTransformCom->Set_Right(vCamRight * -1.f * vScaled.x);
	m_pTransformCom->Set_Up(vLook * vScaled.y);
	m_pTransformCom->Set_Look(vCamUp * vScaled.x);
}

void CE_ShamanIceDagger::Tick_IceDagger(_float fTimeDelta)
{
	if (m_pParent == nullptr) return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CModel* pModel = dynamic_cast<CModel*>(m_pParent->Find_Component(L"Com_Model"));
	CTransform* pTransform = m_pParent->Get_TransformCom();

	m_pUpdateMatrix = m_pCenterBone->Get_CombindMatrix() * pModel->Get_PivotMatrix() * pTransform->Get_WorldMatrix();
	_vector vDir = XMVector3Normalize(m_pUpdateMatrix.r[0] * 1.0f) * m_fRange;
	vDir = XMVector3TransformNormal(vDir, XMMatrixRotationAxis(XMVector3Normalize(m_pUpdateMatrix.r[2]), m_fAngle));

	_vector vMovePos = m_pUpdateMatrix.r[3] + vDir;
	_float fDistance = XMVectorGetX(XMVector3Length(vMovePos - XMLoadFloat4(&m_pTransformCom->Get_Position())));
	Set_IceDaggerMatrix();

	if (fDistance < 0.1f)
	{
		m_bFinalposition = true;
		m_pKenaPosition = m_pKena->Get_TransformCom()->Get_Position();
	}
	else
		m_pTransformCom->Set_Position(vMovePos);

	RELEASE_INSTANCE(CGameInstance);
}

void CE_ShamanIceDagger::Tick_Chase(_float fTimeDelta)
{
	if (m_bFinalposition == false && m_bChase == false)
		return;

	m_pKenaPosition = XMVectorSetY(m_pKenaPosition, m_pKena->Get_TransformCom()->Get_PositionY() + 1.0f);
	m_pTransformCom->Chase(m_pKenaPosition, fTimeDelta, 0.1f, true);

	Set_IceDaggerMatrix();

	_bool bReset = TurnOffSystem(m_fDurateionTime, 3.f, fTimeDelta, false);
	if (bReset == true) Reset();
}

HRESULT CE_ShamanIceDagger::SetUp_ShaderResources()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

// 	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDissolve, sizeof(_bool)), E_FAIL);
// 	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float)), E_FAIL);

	return S_OK;
}

HRESULT CE_ShamanIceDagger::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxBossModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Model_IceDagger"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	m_eEFfectDesc.eMeshType = CEffect_Base::tagEffectDesc::MESH_ETC;

	return S_OK;
}

HRESULT CE_ShamanIceDagger::SetUp_Effects()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEffect_Base* pEffectBase = nullptr;

	/* Child_ trail */
	{
		wstring strCloneTag = m_szCloneObjectTag;
		strCloneTag += L"CloudTrail";
		_tchar* szCloneTag = CUtile::Create_StringAuto(strCloneTag.c_str());

		pEffectBase = dynamic_cast<CE_RectTrail*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_RectTrail", szCloneTag));
		NULL_CHECK_RETURN(pEffectBase, E_FAIL);
		pEffectBase->Set_Parent(this);
		dynamic_cast<CE_RectTrail*>(pEffectBase)->SetUp_Option(CE_RectTrail::OBJ_TRAIL);
		dynamic_cast<CE_RectTrail*>(pEffectBase)->Set_ObjectTrail(10.f, _float4(1.f, 1.f, 1.f, 0.4f));
		m_vecChild.push_back(pEffectBase);
	}

	/* Explosion Effect */
	{
		wstring strCloneTag = m_szCloneObjectTag;
		strCloneTag += L"CommonCircleSp";
		_tchar* szCloneTag = CUtile::Create_StringAuto(strCloneTag.c_str());

		pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_CommonCircleSp", szCloneTag));
		NULL_CHECK_RETURN(pEffectBase, E_FAIL);
		pEffectBase->Set_Parent(this);
		m_vecChild.push_back(pEffectBase);
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CE_ShamanIceDagger* CE_ShamanIceDagger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath)
{
	CE_ShamanIceDagger* pInstance = new CE_ShamanIceDagger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_ShamanIceDagger Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CE_ShamanIceDagger::Clone(void* pArg)
{
	CE_ShamanIceDagger* pInstance = new CE_ShamanIceDagger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_ShamanIceDagger Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_ShamanIceDagger::Free()
{
	__super::Free();
}
