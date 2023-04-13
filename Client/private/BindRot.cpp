#include "stdafx.h"
#include "..\public\BindRot.h"
#include "GameInstance.h"
#include "Monster.h"
#include "E_RotTrail.h"
#include "E_TeleportRot.h"

CBindRot::CBindRot(ID3D11Device* pDevice, ID3D11DeviceContext* p_context)
	: CRot_Base(pDevice, p_context)
{
}

CBindRot::CBindRot(const CBindRot& rhs)
	: CRot_Base(rhs)
{
}

HRESULT CBindRot::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CBindRot::Initialize(void* pArg)
{
	ZeroMemory(&m_Desc, sizeof(DESC));
	if(pArg != nullptr)
	{
		memcpy(&m_Desc, pArg, sizeof(DESC));
	}
		
	FAILED_CHECK_RETURN(__super::Initialize(nullptr), E_FAIL);
	FAILED_CHECK_RETURN(Set_RotTrail(), E_FAIL);
	
	m_pTransformCom->Set_Speed(5.f);

	return S_OK;
}

void CBindRot::Tick(_float fTimeDelta)
{
	m_pTeleportRot->Tick(fTimeDelta);

	__super::Tick(fTimeDelta);
	
	if (m_pFSM)
		m_pFSM->Tick(fTimeDelta);

	if (!m_bBind)
		return;

	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();
	m_pModelCom->Play_Animation(fTimeDelta);
	
	m_pTransformCom->Tick(fTimeDelta);
	m_pRotTrail->Tick(fTimeDelta);
}

void CBindRot::Late_Tick(_float fTimeDelta)
{
	m_pTeleportRot->Late_Tick(fTimeDelta);

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

void CBindRot::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();
}

void CBindRot::ImGui_AnimationProperty()
{
	CGameObject::ImGui_AnimationProperty();
}

void CBindRot::ImGui_ShaderValueProperty()
{
	CGameObject::ImGui_ShaderValueProperty();
}

void CBindRot::ImGui_PhysXValueProperty()
{
	CGameObject::ImGui_PhysXValueProperty();
}

void CBindRot::Bind(_bool bBind, CMonster* pGameObject)
{
	m_bBind = bBind;
	m_pTarget = pGameObject;
}

HRESULT CBindRot::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("IDLE")
		.AddState("IDLE")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(IDLE);
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
		
		m_iRandTeleportAnimIndex = rand() % 7;
		TurnOn_TeleportEffect(vPos, m_iTeleportAnimIndexTable[m_iRandTeleportAnimIndex]);		
	})
		.AddTransition("READY_BIND to BIND_MONSTER", "BIND_MONSTER")
		.Predicator([this]()
	{
		return AnimFinishChecker(m_iTeleportAnimIndexTable[m_iRandTeleportAnimIndex]);
	})

		.AddState("BIND_MONSTER")
		.OnStart([this]()
	{
		m_pRotTrail->Set_Active(true);
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_CROSS + m_iRandTeleportAnimIndex % 2], 0.5f);
		m_fAttackSoundTime = CUtile::Get_RandomFloat(1.f, 2.5f);
		m_bAttackSound = true;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(SQUISH_U);
		if(m_pTarget != nullptr)
		{
			CBone* pBone = m_pTarget->Get_Model()->Get_BonePtr("BindJoint");
			NULL_CHECK_RETURN(pBone, );

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

			if (m_bAttackSound)
			{
				m_fAttackSoundTimeCheck += fTimeDelta;
				if (m_fAttackSoundTimeCheck >= m_fAttackSoundTime)
				{
					m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_BOMB_SPAWN], 0.5f);
					m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_CROSS + rand() % 2], 0.5f);
					m_fAttackSoundTimeCheck = 0.f;
				}
			}
		}
	})
		.OnExit([this]()
	{
		m_bAttackSound = false;
		m_pRotTrail->Set_Active(false);
		TurnOn_TeleportEffect(m_pTransformCom->Get_Position(), IDLE);
		m_pTransformCom->Set_Position(m_vInvisiblePos);
	})
		.AddTransition("BIND_MONSTER to IDLE", "IDLE")
		.Predicator([this]()
	{
		return !m_bBind && m_pTeleportRot->Get_Active() == false;
	})

		.Build();

	if (m_pFSM)
		return S_OK;
	else
		return E_FAIL;
}

HRESULT CBindRot::Set_RotTrail()
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

_bool CBindRot::AnimFinishChecker(_uint eAnim, _double FinishRate)
{
	return m_pModelCom->Find_Animation(eAnim)->Get_PlayRate() >= FinishRate;
}

CBindRot* CBindRot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBindRot*	pInstance = new CBindRot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CBindRot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBindRot::Clone(void* pArg)
{
	CBindRot*	pInstance = new CBindRot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBindRot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBindRot::Free()
{
	__super::Free();

	Safe_Release(m_pRotTrail);
}
