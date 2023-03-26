#include "stdafx.h"
#include "..\public\E_KenaPulse.h"
#include "GameInstance.h"
#include "Effect_Trail.h"
#include "E_KenaPulseCloud.h"
#include "E_KenaPulseDot.h"
#include "Kena.h"
#include "Kena_Status.h"
#include "Monster.h"

CE_KenaPulse::CE_KenaPulse(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_KenaPulse::CE_KenaPulse(const CE_KenaPulse & rhs)
	:CEffect_Mesh(rhs)
{
	
}

void CE_KenaPulse::Set_InitMatrixScaled(_float3 vScale)
{
	_float4 vRight, vUp, vLook;

	XMStoreFloat4(&vRight, XMVector3Normalize(XMLoadFloat4x4(&m_InitWorldMatrix).r[0]) * vScale.x);
	XMStoreFloat4(&vUp, XMVector3Normalize(XMLoadFloat4x4(&m_InitWorldMatrix).r[1])  * vScale.y);
	XMStoreFloat4(&vLook, XMVector3Normalize(XMLoadFloat4x4(&m_InitWorldMatrix).r[2])  *vScale.z);

	memcpy(&m_InitWorldMatrix.m[0][0], &vRight, sizeof vRight);
	memcpy(&m_InitWorldMatrix.m[1][0], &vUp, sizeof vUp);
	memcpy(&m_InitWorldMatrix.m[2][0], &vLook, sizeof vLook);
}

_float3 CE_KenaPulse::Get_InitMatrixScaled()
{
	return _float3(XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_InitWorldMatrix).r[0])),
		XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_InitWorldMatrix).r[1])),
		XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_InitWorldMatrix).r[2])));
}

void CE_KenaPulse::Set_Child()
{
	CEffect_Base* pEffectBase = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_KenaPulseCloud", L"KenaPulseCloud"));
	NULL_CHECK_RETURN(pEffectBase, );
	m_vecChild.push_back(pEffectBase);

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_KenaPulseDot", L"KenaPulseDot"));
	NULL_CHECK_RETURN(pEffectBase, );
	m_vecChild.push_back(pEffectBase);

	for (auto* pChild : m_vecChild)
		pChild->Set_Parent(this);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CE_KenaPulse::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_KenaPulse::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	/* Component */
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_PulseShield_Dissolve"), L"Com_DissolveTexture", (CComponent**)&m_pDissolveTexture, this), E_FAIL);

	/* For.Com_Shader */
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxEffectModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom), E_FAIL);
	Set_ModelCom(m_eEFfectDesc.eMeshType);
	/* ~Component */

	Set_Child();
	m_eEFfectDesc.vColor = XMVectorSet(0.0f, 116.f, 255.f, 255.f) / 255.f;
	m_eStatus.eState = CE_KenaPulse::tagMyStatus::STATE_DEFAULT;
	m_eEFfectDesc.bActive = false;
	memcpy(&m_SaveInitWorldMatrix, &m_InitWorldMatrix, sizeof(_float4x4));
	return S_OK;
}

HRESULT CE_KenaPulse::Late_Initialize(void * pArg)
{	
	/* kena Status */
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pKena = (CKena*)pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Player"), TEXT("Kena"));

	m_pStatus = m_pKena->Get_Status();
	m_eStatus.fCurHp = (_float)m_pStatus->Get_Shield();
	m_eStatus.fMaxHp = (_float)m_pStatus->Get_MaxShield();
	RELEASE_INSTANCE(CGameInstance);
	/* Kena Status */

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	m_pTriggerDAta = Create_PxTriggerData(m_szCloneObjectTag, this, TRIGGER_PULSE, CUtile::Float_4to3(vPos), 1.f);
	CPhysX_Manager::GetInstance()->Create_Trigger(m_pTriggerDAta);

	_float3 vOriginPos = _float3(0.f, 0.f, 0.f);
	_float3 vPivotScale = _float3(1.0f, 0.0f, 1.f);
	_float3 vPivotPos = _float3(0.f, 0.f, 0.f);

	// Capsule X == radius , Y == halfHeight
	CPhysX_Manager::PX_SPHERE_DESC PxSphereDesc;
	PxSphereDesc.eType = SPHERE_DYNAMIC;
	PxSphereDesc.pActortag = m_szCloneObjectTag;
	PxSphereDesc.vPos = vOriginPos;
	PxSphereDesc.fRadius = vPivotScale.x;
	PxSphereDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	PxSphereDesc.fDensity = 1.f;
	PxSphereDesc.fAngularDamping = 0.5f;
	PxSphereDesc.fMass = 59.f;
	PxSphereDesc.fLinearDamping = 1.f;
	PxSphereDesc.bCCD = true;
	PxSphereDesc.eFilterType = PX_FILTER_TYPE::PLAYER_BODY;
	PxSphereDesc.fDynamicFriction = 0.5f;
	PxSphereDesc.fStaticFriction = 0.5f;
	PxSphereDesc.fRestitution = 0.1f;

	CPhysX_Manager::GetInstance()->Create_Sphere(PxSphereDesc, Create_PxUserData(this, false, COL_PLAYER));

	_smatrix	matPivot = XMMatrixTranslation(vPivotPos.x, vPivotPos.y, vPivotPos.z);
	m_pTransformCom->Add_Collider(PxSphereDesc.pActortag, matPivot);

	return S_OK;
}

void CE_KenaPulse::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fTimeDelta += fTimeDelta;
	if (m_bDesolve)
		m_fDissolveTime += fTimeDelta;
	else
		m_fDissolveTime = 0.0f;

	Set_Status();

	if (m_eEFfectDesc.bActive == false)
   		return;

	_float3 vScaled = m_pTransformCom->Get_Scaled();
	switch (m_ePulseType)
	{
	case Client::CE_KenaPulse::PULSE_DEFAULT:

		if (m_bNoActive == true) // Pulse 끝
		{
			for (auto& pChild : m_vecChild)
				pChild->Set_Active(false);

			m_fDissolveTime += fTimeDelta;
			_float3 vScale = Get_InitMatrixScaled();
			Set_InitMatrixScaled(vScale * 1.3f);

			PxRigidActor*		pActor = m_pTransformCom->Get_ActorList()->front().pActor;
			CPhysX_Manager::GetInstance()->Set_ScalingSphere(pActor, 0.001f);

			_float4 vPos;
			XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
			CPhysX_Manager::GetInstance()->Set_ActorPosition(m_pTriggerDAta->pTriggerStatic, CUtile::Float_4to3(vPos));

			if (vScale.x <= 8.f)
				CPhysX_Manager::GetInstance()->Set_ScalingSphere(m_pTriggerDAta->pTriggerStatic, vScale.x *3.5f);

			if (m_fDissolveTime > 1.f)
			{
				m_eEFfectDesc.bActive = false;
				m_bNoActive = false;
				memcpy(&m_InitWorldMatrix, &m_SaveInitWorldMatrix, sizeof(_float4x4));
				m_fDissolveTime = 0.0f;
				CPhysX_Manager::GetInstance()->Set_ScalingSphere(m_pTriggerDAta->pTriggerStatic, 0.001f);
			}
		}

		if (m_bNoActive == false && m_eEFfectDesc.bActive == true)
		{
			PxRigidActor*		pActor = m_pTransformCom->Get_ActorList()->front().pActor;
			CPhysX_Manager::GetInstance()->Set_ScalingSphere(pActor, 1.f);

			for (auto& pChild : m_vecChild)
				pChild->Set_Active(true);
		}
		break;

	case Client::CE_KenaPulse::PULSE_PARRY:
		m_eEFfectDesc.iPassCnt = 0;
		m_fTimeDelta += fTimeDelta;

		for (auto& pChild : m_vecChild)
			pChild->Set_Active(false);

		if (vScaled.x > 5.f)
		{
			m_eEFfectDesc.bActive = false;
			m_eEFfectDesc.iPassCnt = 1;

			m_ePulseType = CE_KenaPulse::PULSE_DEFAULT;
			m_pTransformCom->Set_WorldMatrix_float4x4(m_SaveInitWorldMatrix);
		}
		else
		{
			vScaled *= fTimeDelta + 1.1f;
			m_pTransformCom->Set_Scaled(vScaled);
		}
		break;
	}
	m_pTransformCom->Tick(fTimeDelta);
}

void CE_KenaPulse::Late_Tick(_float fTimeDelta)
{
  	if (m_eEFfectDesc.bActive == false || (m_ePulseType == CE_KenaPulse::PULSE_DEFAULT && m_bPulseZero == true))
  		return;

	if (m_ePulseType == CE_KenaPulse::PULSE_DEFAULT && m_pParent != nullptr)
		Set_Matrix();
	
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

		if(m_ePulseType != CE_KenaPulse::PULSE_DEFAULT)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EFFECT, this);
	}

	if (m_eStatus.eState == CE_KenaPulse::tagMyStatus::STATE_DAMAGE)
	{
		m_eStatus.fStateDurationTime += fTimeDelta * 1.5f;
		if (m_eStatus.fStateDurationTime > 2.f)
		{
			m_eStatus.eState = CE_KenaPulse::tagMyStatus::STATE_DEFAULT;
			m_eStatus.fStateDurationTime = 0.0f;
		}
	}
}

HRESULT CE_KenaPulse::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_pModelCom != nullptr && m_pShaderCom != nullptr)
		m_pModelCom->Render(m_pShaderCom, 0, nullptr, m_eEFfectDesc.iPassCnt);

	return S_OK;
}

void CE_KenaPulse::Reset()
{
	dynamic_cast<CE_KenaPulseCloud*>(m_vecChild.front())->ResetSprite();
	dynamic_cast<CE_KenaPulseDot*>(m_vecChild.back())->Set_ShapePosition();

	m_bNoActive = false;
	m_bDesolve = true;
	m_fDissolveTime = 0.f;

	memcpy(&m_InitWorldMatrix, &m_SaveInitWorldMatrix, sizeof(_float4x4));
	Set_InitMatrixScaled(Get_InitMatrixScaled());

	CPhysX_Manager::GetInstance()->Set_ScalingSphere(m_pTriggerDAta->pTriggerStatic, 0.001f);
}

_int CE_KenaPulse::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	_bool bRealAttack = false;
	if (iColliderIndex == (_uint)COL_MONSTER_WEAPON && (bRealAttack = ((CMonster*)pTarget)->IsRealAttack()))
	{
		if (m_pKena->Get_State(CKena::STATE_PULSE) == false)
			return 0;

		// KenaPulse 공격력 깎기
		CStatus*	pStatus = dynamic_cast<CMonster*>(pTarget)->Get_MonsterStatusPtr();

		if (pStatus->Get_Attack() > 10)
			m_pKena->Set_State(CKena::STATE_HEAVYHIT, true);
		else
			m_pKena->Set_State(CKena::STATE_COMMONHIT, true);

		m_pKena->Set_AttackObject(pTarget);
		//
		CKena:: DAMAGED_FROM		eDir = CKena::DAMAGED_FROM_END;
		CTransform*	pTargetTransCom = pTarget->Get_TransformCom();
		_float4		vDir = pTargetTransCom->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		vDir.Normalize();

		_float			fFrontBackAngle = vDir.Dot(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));

		if (fFrontBackAngle >= 0.f)
			eDir = CKena::DAMAGED_FRONT;
		else
			eDir = CKena::DAMAGED_BACK;

		m_pKena->Set_DamagedDir(eDir);
		//
		m_pStatus->Under_Shield(pStatus);
		m_eStatus.eState = STATUS::STATE_DAMAGE;
	}
	return 0;
}

void CE_KenaPulse::ImGui_PhysXValueProperty()
{
	__super::ImGui_PhysXValueProperty();
}

HRESULT CE_KenaPulse::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	/* Kena Status */
	m_fHpRatio = m_eStatus.fCurHp / m_eStatus.fMaxHp;
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_HpRatio", &m_fHpRatio, sizeof _float), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_PulseState", &m_eStatus.eState, sizeof _uint), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_DamageDurationTime", &m_eStatus.fStateDurationTime, sizeof _float), E_FAIL);

	/* Kena Status */
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDesolve, sizeof(_bool)), E_FAIL);

	if (m_bDesolve)
	{
		FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float)), E_FAIL);
		FAILED_CHECK_RETURN(m_pDissolveTexture->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", 3), E_FAIL);
	}

	return S_OK;
}

void CE_KenaPulse::Imgui_RenderProperty()
{
	static _int iType = 0;
	ImGui::RadioButton("Default", &iType, 0);
	ImGui::RadioButton("Pulse", &iType, 1);
	ImGui::RadioButton("Bomb", &iType, 2);

	if (iType == 0)
		m_ePulseType = CE_KenaPulse::PULSE_DEFAULT;
	else if (iType == 1)
		m_ePulseType = CE_KenaPulse::PULSE_PARRY;

	if (ImGui::Button("Active"))
		m_eEFfectDesc.bActive = !m_eEFfectDesc.bActive;

}

void CE_KenaPulse::Set_Status()
{
	m_eStatus.fCurHp = m_pStatus->Get_Shield();
	m_eStatus.fMaxHp = m_pStatus->Get_MaxShield();

	if (m_eStatus.fCurHp <= 0.0f)
	{
		for (auto& pChild : m_vecChild)
			pChild->Set_Active(false);
		m_bPulseZero = true;
		Reset();
		m_eEFfectDesc.bActive = false;
	}
	else
		m_bPulseZero = false;

	if (m_fDissolveTime > 3.f)
	{
		m_bDesolve = false;
		m_fDissolveTime = 0.0f;
	}
}

CE_KenaPulse * CE_KenaPulse::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_KenaPulse * pInstance = new CE_KenaPulse(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaPulse Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_KenaPulse::Clone(void * pArg)
{
	CE_KenaPulse * pInstance = new CE_KenaPulse(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaPulse Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_KenaPulse::Free()
{
	__super::Free();

	Safe_Release(m_pDissolveTexture);
	
}
