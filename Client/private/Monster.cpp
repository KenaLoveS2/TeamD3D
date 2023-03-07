#include "stdafx.h"
#include "Monster.h"
#include "GameInstance.h"
#include "FSMComponent.h"

CMonster::CMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster & rhs)
	: CGameObject(rhs)
{
}

const _double & CMonster::Get_AnimationPlayTime()
{
	return m_pModelCom->Get_PlayTime();
}

HRESULT CMonster::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	if (pArg == nullptr)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 7.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	}
	else
		memcpy(&GameObjectDesc, pArg, sizeof(CGameObject::GAMEOBJECTDESC));

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State(), E_FAIL);

	Push_EventFunctions();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)

	m_pKena = pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Player"),TEXT("Kena"));

	RELEASE_INSTANCE(CGameInstance)
	return S_OK;
}

HRESULT CMonster::Late_Initialize(void * pArg)
{
	return S_OK;
}

void CMonster::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pKena)
		m_vKenaPos = m_pKena->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
}

void CMonster::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CMonster::Render()
{
	return S_OK;
}

HRESULT CMonster::RenderShadow()
{
	return S_OK;
}

void CMonster::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	ImGui::Text("Distance to Player :	%f", DistanceBetweenPlayer());
}

void CMonster::ImGui_AnimationProperty()
{
	__super::ImGui_AnimationProperty();
}

void CMonster::ImGui_ShaderValueProperty()
{
	__super::ImGui_ShaderValueProperty();

	if (ImGui::Button("Recompile"))
	{
		m_pShaderCom->ReCompile();
		m_pRendererCom->ReCompile();
	}
}

void CMonster::ImGui_PhysXValueProperty()
{
	__super::ImGui_PhysXValueProperty();
}

HRESULT CMonster::Call_EventFunction(const string & strFuncName)
{
	return S_OK;
}

void CMonster::Push_EventFunctions()
{
}

void CMonster::Calc_RootBoneDisplacement(_fvector vDisplacement)
{
	_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vPos = vPos + vDisplacement;
	m_pTransformCom->Set_Translation(vPos, vDisplacement);
}

_bool CMonster::AnimFinishChecker(_uint eAnim, _double FinishRate)
{
	return m_pModelCom->Find_Animation(eAnim)->Get_PlayRate() >= FinishRate;
}

_bool CMonster::AnimIntervalChecker(_uint eAnim, _double StartRate, _double FinishRate)
{
	if (m_pModelCom->Find_Animation(eAnim)->Get_PlayRate() > StartRate &&
		m_pModelCom->Find_Animation(eAnim)->Get_PlayRate() <= FinishRate)
	{
		return true;
	}

	return false;
}

_bool CMonster::DistanceTrigger(_float distance)
{
	_float3 vPlayerPos = m_pKena->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_float fDistance = _float3::Distance(vPos, vPlayerPos);

	if (distance >= fDistance)
		return true;
	else
		return false;
}

_bool CMonster::IntervalDistanceTrigger(_float min, _float max)
{
	_float3 vPlayerPos = m_pKena->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_float fDistance = _float3::Distance(vPos, vPlayerPos);

	if (min <= fDistance && fDistance <= max)
		return true;
	else
		return false;
}

_bool CMonster::TimeTrigger(_float Time1, _float Time2)
{
	if (Time1 >= Time2)
		return true;
	else
		return false;	
}

_float CMonster::DistanceBetweenPlayer()
{
	_float3 vPlayerPos = m_pKena->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	return  _float3::Distance(vPos, vPlayerPos);
}

_float CMonster::Calc_PlayerLookAtDirection()
{
	_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_float4 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	_float4 vEyeDirection = vPos - m_vKenaPos;
	vEyeDirection.Normalize();
	vEyeDirection.w = 0.f;

	_float fFBLookRatio = vEyeDirection.Dot(vLook);
	_float fLRLookRatio = vEyeDirection.Dot(vRight);

	if (fFBLookRatio <= 0.f)
	{
		if (fLRLookRatio >= -0.5f && fLRLookRatio <= 0.5f)
			m_PlayerLookAt_Dir = FRONT;
		else	if (fLRLookRatio >= -1.f && fLRLookRatio <= -0.5f)
			m_PlayerLookAt_Dir = RIGHT;
		else if(fLRLookRatio >= 0.5f && fLRLookRatio <= 1.f)
			m_PlayerLookAt_Dir = LEFT;
	}
	else 
	{
		if (fLRLookRatio >= -0.5f && fLRLookRatio <= 0.5f)
			m_PlayerLookAt_Dir = BACK;
		else	if (fLRLookRatio >= -1.f && fLRLookRatio <= -0.5f)
			m_PlayerLookAt_Dir = RIGHT;
		else if (fLRLookRatio >= 0.5f && fLRLookRatio <= 1.f)
			m_PlayerLookAt_Dir = LEFT;
	}

	return fLRLookRatio;
}

void CMonster::AdditiveAnim(_float fTimeDelta)
{
}

void CMonster::Free()
{
	__super::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pFSM);
}
