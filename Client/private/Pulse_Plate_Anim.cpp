#include "stdafx.h"
#include "Pulse_Plate_Anim.h"
#include "GameInstance.h"
#include "ControlMove.h"
#include "Interaction_Com.h"
#include "Kena.h"

CPulse_Plate_Anim::CPulse_Plate_Anim(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CPulse_Plate_Anim::CPulse_Plate_Anim(const CPulse_Plate_Anim & rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CPulse_Plate_Anim::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPulse_Plate_Anim::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_bRenderActive = true;

	m_pModelCom->Set_AnimIndex(0);
	return S_OK;
}

HRESULT CPulse_Plate_Anim::Late_Initialize(void * pArg)
{
	m_pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena"));
	if (m_pKena == nullptr)
		return S_OK;

	assert(m_pKena != nullptr && "CRot::Late_Initialize");

	m_pKenaTransform = dynamic_cast<CTransform*>(m_pKena->Get_TransformCom());
	assert(m_pKenaTransform != nullptr && "CPulse_Plate_Anim::Late_Initialize");

	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	_float3 vSize = _float3(2.5f, 0.7f, 2.5f);

	CPhysX_Manager *pPhysX = CPhysX_Manager::GetInstance();

	CPhysX_Manager::PX_BOX_DESC BoxDesc;
	BoxDesc.pActortag = m_szCloneObjectTag;
	BoxDesc.eType = BOX_STATIC;
	BoxDesc.vPos = vPos;
	BoxDesc.vSize = _float3(2.1f, 0.1f, 2.1f);
	BoxDesc.vRotationAxis = _float3(0.f, 0.f, 0.f);
	BoxDesc.fDegree = 0.f;
	BoxDesc.isGravity = true;
	BoxDesc.eFilterType = PX_FILTER_TYPE::PULSE_PLATE;
	BoxDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	BoxDesc.fDensity = 0.2f;
	BoxDesc.fMass = 150.f;
	BoxDesc.fLinearDamping = 10.f;
	BoxDesc.fAngularDamping = 5.f;
	BoxDesc.bCCD = false;
	BoxDesc.fDynamicFriction = 0.5f;
	BoxDesc.fStaticFriction = 0.5f;
	BoxDesc.fRestitution = 0.1f;

	pPhysX->Create_Box(BoxDesc, Create_PxUserData(this, true, COL_PULSE_PLATE));
	m_pRendererCom->Set_PhysXRender(true);


	/*CPhysX_Manager::PX_SPHERE_DESC PxSphereDesc;
	PxSphereDesc.eType = SPHERE_STATIC;
	PxSphereDesc.pActortag = m_szCloneObjectTag;
	PxSphereDesc.vPos = vPos;
	PxSphereDesc.fRadius = vPivotScale.x;
	PxSphereDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	PxSphereDesc.fDensity = 1.f;
	PxSphereDesc.fAngularDamping = 0.5f;
	
	PxSphereDesc.fLinearDamping = 1.f;
	PxSphereDesc.bCCD = true;
	PxSphereDesc.eFilterType = PX_FILTER_TYPE::PULSE_PLATE;
	PxSphereDesc.fDynamicFriction = 0.5f;
	PxSphereDesc.fStaticFriction = 0.5f;
	PxSphereDesc.fRestitution = 0.1f;

	CPhysX_Manager::GetInstance()->Create_Sphere(PxSphereDesc, Create_PxUserData(this, false, COL_PULSE_PLATE));
*/
	// 여기 뒤에 세팅한 vPivotPos를 넣어주면된다.

	


	return S_OK;
}

void CPulse_Plate_Anim::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_bPlayerColl = false;			



	m_pTransformCom->Tick(fTimeDelta);
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CPulse_Plate_Anim::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	/*if(m_bPlayerColl && CGameInstance::GetInstance()->Key_Up(DIK_E))
		m_Gimmick_PulsePlateDelegate.broadcast(m_bPlayerColl);*/

	_bool bTest = true;

	if (CGameInstance::GetInstance()->Key_Up(DIK_E))
		m_Gimmick_PulsePlateDelegate.broadcast(bTest);

	if (m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CPulse_Plate_Anim::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}

	return S_OK;
}

void CPulse_Plate_Anim::ImGui_PhysXValueProperty()
{
	__super::ImGui_PhysXValueProperty();

	_float3 vPxPivotScale = m_pTransformCom->Get_vPxPivotScale();

	float fScale[3] = { vPxPivotScale.x, vPxPivotScale.y, vPxPivotScale.z };
	ImGui::DragFloat3("PxScale", fScale, 0.01f, 0.1f, 100.0f);
	vPxPivotScale.x = fScale[0]; vPxPivotScale.y = fScale[1]; vPxPivotScale.z = fScale[2];
	CPhysX_Manager::GetInstance()->Set_ActorScaling(m_szCloneObjectTag, vPxPivotScale);
	m_pTransformCom->Set_PxPivotScale(vPxPivotScale);

	_float3 vPxPivot = m_pTransformCom->Get_vPxPivot();

	float fPos[3] = { vPxPivot.x, vPxPivot.y, vPxPivot.z };
	ImGui::DragFloat3("PxPivotPos", fPos, 0.01f, -100.f, 100.0f);
	vPxPivot.x = fPos[0]; vPxPivot.y = fPos[1]; vPxPivot.z = fPos[2];
	m_pTransformCom->Set_PxPivot(vPxPivot);
}

void CPulse_Plate_Anim::ImGui_AnimationProperty()
{
	m_pModelCom->Imgui_RenderProperty();
}

HRESULT CPulse_Plate_Anim::Add_AdditionalComponent(_uint iLevelIndex, const _tchar * pComTag, COMPONENTS_OPTION eComponentOption)
{
	__super::Add_AdditionalComponent(iLevelIndex, pComTag, eComponentOption);

	/* For.Com_CtrlMove */
	if (eComponentOption == COMPONENTS_CONTROL_MOVE)
	{
		if (FAILED(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_ControlMove"), pComTag,
			(CComponent**)&m_pControlMoveCom)))
			return E_FAIL;
	}
	/* For.Com_Interaction */
	else if (eComponentOption == COMPONENTS_INTERACTION)
	{
		if (FAILED(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_Interaction_Com"), pComTag,
			(CComponent**)&m_pInteractionCom)))
			return E_FAIL;
	}
	else
		return S_OK;

	return S_OK;
}

HRESULT CPulse_Plate_Anim::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	/*나중에  레벨 인덱스 수정해야됌*/
	if (m_EnviromentDesc.iCurLevel == 0)
		m_EnviromentDesc.iCurLevel = LEVEL_MAPTOOL;

	/* For.Com_Model */ 	/*나중에  레벨 인덱스 수정해야됌*/
	if (FAILED(__super::Add_Component(g_LEVEL, m_EnviromentDesc.szModelTag.c_str(), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom, nullptr, this)))
		return E_FAIL;

	/* For.Com_Shader */
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(),
		L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	return S_OK;
}

HRESULT CPulse_Plate_Anim::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;



	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CPulse_Plate_Anim::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	// To. DO 
	// 1) Gimmci_Plat 충돌 체크확인
	// 2) 펄스 했는지 확인후 브로드 캐스트 쏴주기

	m_bPlayerColl = true;

	

	return 0;
}

_int CPulse_Plate_Anim::Execute_TriggerTouchFound(CGameObject * pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	return 0;
}

_int CPulse_Plate_Anim::Execute_TriggerTouchLost(CGameObject * pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	return 0;
}

CPulse_Plate_Anim * CPulse_Plate_Anim::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPulse_Plate_Anim*		pInstance = new CPulse_Plate_Anim(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPulse_Plate_Anim");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPulse_Plate_Anim::Clone(void * pArg)
{
	CPulse_Plate_Anim*		pInstance = new CPulse_Plate_Anim(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPulse_Plate_Anim");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPulse_Plate_Anim::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);
}
