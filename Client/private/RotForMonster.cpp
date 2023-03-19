#include "stdafx.h"
#include "..\public\RotForMonster.h"
#include "GameInstance.h"
#include "Rot.h"
#include "Bone.h"
#include "Monster.h"
#include "E_RotTrail.h"

CRotForMonster::CRotForMonster(ID3D11Device* pDevice, ID3D11DeviceContext* p_context)
	:CGameObject(pDevice, p_context)
{
}

CRotForMonster::CRotForMonster(const CRotForMonster& rhs)
	:CGameObject(rhs)
{
}

HRESULT CRotForMonster::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CRotForMonster::Initialize(void* pArg)
{
	// 평소에 IDLE 상태로 딱 있다가 스폰되면서 몬스터 감싸는 형식

	ZeroMemory(&m_Desc, sizeof(DESC));
	if(pArg != nullptr)
	{
		memcpy(&m_Desc, pArg, sizeof(DESC));
	}

	CGameObject::GAMEOBJECTDESC		GaemObjectDesc;
	ZeroMemory(&GaemObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	GaemObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GaemObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	FAILED_CHECK_RETURN(__super::Initialize(&GaemObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_FSM(), E_FAIL);
	FAILED_CHECK_RETURN(Set_RotTrail(), E_FAIL);

	m_pModelCom->Set_AnimIndex(CRot::IDLE);
	m_pModelCom->Set_AllAnimCommonType();

	return S_OK;
}

HRESULT CRotForMonster::Late_Initialize(void* pArg)
{
	// Capsule X == radius , Y == halfHeight
	CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
	PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
	PxCapsuleDesc.pActortag = m_szCloneObjectTag;
	PxCapsuleDesc.isGravity = true;
	PxCapsuleDesc.vPos = _float3(0.f, 0.f, 0.f);
	PxCapsuleDesc.fRadius = 0.1f;
	PxCapsuleDesc.fHalfHeight = 0.04f;
	PxCapsuleDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	PxCapsuleDesc.fDensity = 1.f;
	PxCapsuleDesc.fAngularDamping = 0.5f;
	PxCapsuleDesc.fMass = 10.f;
	PxCapsuleDesc.fLinearDamping = 10.f;
	PxCapsuleDesc.fDynamicFriction = 0.5f;
	PxCapsuleDesc.fStaticFriction = 0.5f;
	PxCapsuleDesc.fRestitution = 0.1f;
	PxCapsuleDesc.eFilterType = FILTER_DEFULAT;

	CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, true, COL_DEFAULT));

	// 여기 뒤에 세팅한 vPivotPos를 넣어주면된다.
	m_pTransformCom->Connect_PxActor_Gravity(m_szCloneObjectTag, _float3(0.f, 0.15f, 0.f));

	return S_OK;
}

void CRotForMonster::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	
	if (m_pFSM)
		m_pFSM->Tick(fTimeDelta);

	if (!m_bBind)
		return;

	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();
	m_pModelCom->Play_Animation(fTimeDelta);
	m_pTransformCom->Tick(fTimeDelta);

	/* Trail */
	if (m_pRotTrail != nullptr)
		m_pRotTrail->Tick(fTimeDelta);
	/* Trail */
}

void CRotForMonster::Late_Tick(_float fTimeDelta)
{
	if (!m_bBind)
		return;

	__super::Late_Tick(fTimeDelta);

	/* Trail */
	if (m_pRotTrail != nullptr)
	{
		// rot_headTip_a_jnt
		CBone*	pWispBonePtr = m_pModelCom->Get_BonePtr("rot_headTip_a_jnt");
		_matrix SocketMatrix = pWispBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
		_matrix matWorldSocket = SocketMatrix * m_pTransformCom->Get_WorldMatrix();

		m_pRotTrail->Get_TransformCom()->Set_WorldMatrix(matWorldSocket);
		m_pRotTrail->Late_Tick(fTimeDelta);
	}
	/* Trail */

	if (m_pRendererCom != nullptr)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CRotForMonster::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	 iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");

		if (i == 0)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 1);
		}
		else	if (i == 1)
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
		else	if (i == 2)
		{
			// 머리카락 모르겠음.
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_ALPHA, "g_AlphaTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 2);
		}
	}

	return S_OK;
}

HRESULT CRotForMonster::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 3);

	return S_OK;
}

void CRotForMonster::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();
}

void CRotForMonster::ImGui_AnimationProperty()
{
	CGameObject::ImGui_AnimationProperty();
}

void CRotForMonster::ImGui_ShaderValueProperty()
{
	CGameObject::ImGui_ShaderValueProperty();
}

void CRotForMonster::ImGui_PhysXValueProperty()
{
	CGameObject::ImGui_PhysXValueProperty();
}

HRESULT CRotForMonster::Call_EventFunction(const string& strFuncName)
{
	return CGameObject::Call_EventFunction(strFuncName);
}

void CRotForMonster::Push_EventFunctions()
{
	CGameObject::Push_EventFunctions();
}

void CRotForMonster::Bind(_bool bBind, CMonster * pGameObject)
{
	m_bBind = bBind;
	m_pTarget = pGameObject;
}

HRESULT CRotForMonster::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxAnimRotModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Rot", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);
	m_pModelCom->Set_RootBone("Rot_RIG");

	//  0 : Body
	//	 1 : Eye
	//	 2 : Hair
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Rot/rh_body_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(2, WJTextureType_ALPHA, TEXT("../Bin/Resources/Anim/Rot/rot_fur_ALPHA.png")), E_FAIL);

	return S_OK;
}

HRESULT CRotForMonster::SetUp_ShaderResources()
{ 
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	return S_OK;
}

HRESULT CRotForMonster::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	return S_OK;
}

HRESULT CRotForMonster::SetUp_FSM()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("IDLE")
		.AddState("IDLE")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(CRot::IDLE);
	})	
		.AddTransition("IDLE to READY_BIND", "READY_BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})

		.AddState("READY_BIND")
		.OnStart([this]()
	{
		_float4 vPos = _float4(m_pTarget->Get_Position()) + m_Desc.vPivotPos;
		vPos.w = 1.f;
		m_pTransformCom->Set_Position(vPos);
		m_pTransformCom->LookAt_NoUpDown(m_pTarget->Get_Position());
		_int iRand = rand() % 7;
		switch(iRand)
		{
		case 0:
			m_pModelCom->Set_AnimIndex(CRot::TELEPORT1);
			break;
		case 1:
			m_pModelCom->Set_AnimIndex(CRot::TELEPORT2);
			break;
		case 2:
			m_pModelCom->Set_AnimIndex(CRot::TELEPORT3);
			break;
		case 3:
			m_pModelCom->Set_AnimIndex(CRot::TELEPORT4);
			break;
		case 4:
			m_pModelCom->Set_AnimIndex(CRot::TELEPORT5);
			break;
		case 5:
			m_pModelCom->Set_AnimIndex(CRot::TELEPORT6);
			break;
		case 6:
			m_pModelCom->Set_AnimIndex(CRot::TELEPORT7);
			break;
		case 7:
			m_pModelCom->Set_AnimIndex(CRot::TELEPORT8);
			break;
		}
	})
		.AddTransition("READY_BIND to BIND_MONSTER", "BIND_MONSTER")
		.Predicator([this]()
	{
		return AnimFinishChecker(CRot::TELEPORT1) ||
			AnimFinishChecker(CRot::TELEPORT2) ||
			AnimFinishChecker(CRot::TELEPORT3) ||
			AnimFinishChecker(CRot::TELEPORT4) ||
			AnimFinishChecker(CRot::TELEPORT5) ||
			AnimFinishChecker(CRot::TELEPORT6) ||
			AnimFinishChecker(CRot::TELEPORT7) ||
			AnimFinishChecker(CRot::TELEPORT8);
	})

		.AddState("BIND_MONSTER")
		.OnStart([this]()
	{
		m_pRotTrail->Set_Active(true);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(CRot::SQUISH_U);
		if(m_pTarget != nullptr)
		{
			CBone* pBone = m_pTarget->Get_Model()->Get_BonePtr("BindJoint");

			_matrix			SocketMatrix = 
				pBone->Get_OffsetMatrix() * 
				pBone->Get_CombindMatrix() * 
				m_pTarget->Get_Model()->Get_PivotMatrix() *
				m_pTarget->Get_TransformCom()->Get_WorldMatrix();

			SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
			SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
			SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

			_float4x4 pivotMatrix;
			XMStoreFloat4x4(&pivotMatrix, SocketMatrix);
			_float4 vPos = _float4(pivotMatrix._41, pivotMatrix._42, pivotMatrix._43, 1.f);
			m_pTransformCom->Chase(vPos, fTimeDelta, CUtile::Get_RandomFloat(0.1f, 1.f), true);
		}
	})
		.OnExit([this]()
	{
		m_pRotTrail->Set_Active(false);
	})

		.AddTransition("BIND_MONSTER to IDLE", "IDLE")
		.Predicator([this]()
	{
		return !m_bBind;
	})

		.Build();

	if (m_pFSM)
		return S_OK;
	else
		return E_FAIL;
}

HRESULT CRotForMonster::Set_RotTrail()
{	
	/* Set Trail */
	CE_RotTrail* pRotTrail = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	wstring strMyCloneTag = this->Get_ObjectCloneName();
	strMyCloneTag += L"_Trail";

	pRotTrail = dynamic_cast<CE_RotTrail*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_RotTrail", strMyCloneTag.c_str()));
	NULL_CHECK_RETURN(pRotTrail, E_FAIL);
	pRotTrail->Set_Parent(this);
	m_pRotTrail = pRotTrail;

	RELEASE_INSTANCE(CGameInstance);
	/* Set Trail */

	return S_OK;
}

_bool CRotForMonster::AnimFinishChecker(_uint eAnim, _double FinishRate)
{
	return m_pModelCom->Find_Animation(eAnim)->Get_PlayRate() >= FinishRate;
}

CRotForMonster* CRotForMonster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRotForMonster*	pInstance = new CRotForMonster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CRotForMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRotForMonster::Clone(void* pArg)
{
	CRotForMonster*	pInstance = new CRotForMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CRotForMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRotForMonster::Free()
{
	__super::Free();

	Safe_Release(m_pFSM);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pRotTrail);
}
