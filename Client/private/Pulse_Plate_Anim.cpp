#include "stdafx.h"
#include "Pulse_Plate_Anim.h"

#include "CinematicCamera.h"
#include "GameInstance.h"
#include "ControlMove.h"
#include "Interaction_Com.h"
#include "Kena.h"
#include "ControlRoom.h"

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

	m_pModelCom->Set_AnimIndex(3);
	return S_OK;
}

HRESULT CPulse_Plate_Anim::Late_Initialize(void * pArg)
{
	m_pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena"));
	if (m_pKena == nullptr)
		return S_OK;

	assert(m_pKena != nullptr && "Pulse_Plate_Anim::Late_Initialize");

	m_pKenaTransform = dynamic_cast<CTransform*>(m_pKena->Get_TransformCom());
	assert(m_pKenaTransform != nullptr && "CPulse_Plate_Anim::Late_Initialize");

	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	_float3 vSize = _float3(2.5f, 0.7f, 2.5f);

	CPhysX_Manager *pPhysX = CPhysX_Manager::GetInstance();
	// RoomIndex 2번은 현재 사이즈 포스가 맞음
	// 1번만 조정하면됌
	CPhysX_Manager::PX_BOX_DESC BoxDesc;
	BoxDesc.pActortag = m_szCloneObjectTag;
	BoxDesc.eType = BOX_STATIC;		// 원래는 박스 스태틱으로 만들어야함
	BoxDesc.vPos = vPos;
	if (m_EnviromentDesc.iRoomIndex == 1)
		BoxDesc.vPos.y = -0.25f;

	BoxDesc.vSize = _float3(2.74f, 0.25f, 2.45f);
	BoxDesc.vRotationAxis = _float3(0.f, 0.f, 0.f);
	BoxDesc.fDegree = 0.f;
	BoxDesc.isGravity = false;
	BoxDesc.eFilterType = PX_FILTER_TYPE::FILTER_DEFULAT;
	BoxDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	BoxDesc.fDensity = 0.2f;
	BoxDesc.fMass = 150.f;
	BoxDesc.fLinearDamping = 10.f;
	BoxDesc.fAngularDamping = 5.f;
	BoxDesc.bCCD = false;
	BoxDesc.fDynamicFriction = 0.5f;
	BoxDesc.fStaticFriction = 0.5f;
	BoxDesc.fRestitution = 0.1f;

	pPhysX->Create_Box(BoxDesc, Create_PxUserData(this, false, COL_PULSE_PLATE));

	CPhysX_Manager::GetInstance()->Create_Trigger(
		Create_PxTriggerData(m_szCloneObjectTag, this, TRIGGER_PULSE_PLATE, vPos, 4.f));

	m_pControlRoom =dynamic_cast<CControlRoom*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_ControlRoom", L"ControlRoom"));
	assert(m_pControlRoom != nullptr  && "CPulse_Plate_Anim::Late_Initialize(void * pArg)");
	m_pControlRoom->Add_Gimmick_TrggerObj(m_szCloneObjectTag, this);


	return S_OK;
}

void CPulse_Plate_Anim::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Pulse_Plate_AnimControl(fTimeDelta);

	m_pTransformCom->Tick(fTimeDelta);
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CPulse_Plate_Anim::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	if (m_pRendererCom && m_bRenderActive)
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
	if (FAILED(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_PulsePlateAnim", TEXT("Com_Model"),
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

void CPulse_Plate_Anim::Pulse_Plate_AnimControl(_float fTimeDelta)
{
	if (m_bPlayerColl && 
		CGameInstance::GetInstance()->Key_Up(DIK_E))
	{
		m_pControlRoom->PulsePlate_Down_Active(m_EnviromentDesc.iRoomIndex,true);
		m_pModelCom->Set_AnimIndex(2);
		if(m_EnviromentDesc.iRoomIndex == 1 && !m_bFirstCinema)
		{
			CGameInstance::GetInstance()->Work_Camera(TEXT("CINE_CAM0"));
			dynamic_cast<CCinematicCamera*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->Play();
			m_bFirstCinema = true;
		}

		if (m_EnviromentDesc.iRoomIndex == 2 && !m_bSecondCinema)
		{
			//CGameInstance::GetInstance()->Work_Camera(TEXT("CINE_CAM1"));
			//dynamic_cast<CCinematicCamera*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->Play();
			//m_bSecondCinema = true;
		}
	}

	if (m_bPlayerColl && (m_pModelCom->Get_AnimIndex() == 2 && m_pModelCom->Get_AnimationFinish())
		|| (m_pModelCom->Get_AnimIndex() == 0 && m_pModelCom->Get_AnimationFinish()))
	{
		m_pModelCom->Set_AnimIndex(1);
	}
	else if (false == m_bPlayerColl && (m_pModelCom->Get_AnimIndex() == 4 && m_pModelCom->Get_AnimationFinish()))
	{
		m_pModelCom->Set_AnimIndex(3);
	}
}

_int CPulse_Plate_Anim::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{	
	return 0;
}

_int CPulse_Plate_Anim::Execute_TriggerTouchFound(CGameObject * pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	m_bPlayerColl = true;

	m_pModelCom->Set_AnimIndex(0);	

	return 0;
}

_int CPulse_Plate_Anim::Execute_TriggerTouchLost(CGameObject * pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	m_bPlayerColl = false;
	m_pModelCom->Set_AnimIndex(4);
	m_pControlRoom->PulsePlate_Down_Active(m_EnviromentDesc.iRoomIndex, false);
	
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
