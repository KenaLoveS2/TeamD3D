#include "stdafx.h"
#include "Monster.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "UI_MonsterHP.h"
#include "Camera.h"
#include "Kena.h"

CMonster::CMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
	, m_pUIHPBar(nullptr)
{
}

CMonster::CMonster(const CMonster & rhs)
	: CGameObject(rhs)
	, m_pUIHPBar(nullptr)
{
}

const _double & CMonster::Get_AnimationPlayTime()
{
	return m_pModelCom->Get_PlayTime();
}

_fvector CMonster::Get_Position()
{
	return m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
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

	m_bRotable = true;

	return S_OK;
}

HRESULT CMonster::Late_Initialize(void * pArg)
{
	FAILED_CHECK_RETURN(SetUp_UI(), E_FAIL);

	/* Is In Camera? */




	return S_OK;
}

void CMonster::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

#ifdef _DEBUG
	if (nullptr != m_pUIHPBar)
		m_pUIHPBar->Imgui_RenderProperty();

	static _float fGuage = 1.f;
	if (CGameInstance::GetInstance()->Key_Down(DIK_I))
	{
		fGuage -= 0.1f;
		m_pUIHPBar->Set_Guage(fGuage);
	}
#endif

	if (m_pKena)
		m_vKenaPos = m_pKena->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
}

void CMonster::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	/* calculate camera */
	_vector vCamLook = CGameInstance::GetInstance()->Get_WorkCameraPtr()->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	_vector vCamPos = CGameInstance::GetInstance()->Get_WorkCameraPtr()->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);

	_vector vDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) - vCamPos);
	if (50.f >= XMVectorGetX(XMVector3Length(vDir)) && (XMVectorGetX(XMVector3Dot(vDir, vCamLook)) > cosf(XMConvertToRadians(20.f))))
		Call_RotIcon();
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

	if (ImGui::Button("BIND"))
		m_bBind = true;

	if (ImGui::Button("STRONGLYDMG"))
		m_bStronglyHit = true;

	if (ImGui::Button("WEALKYDMG"))
		m_bWeaklyHit = true;
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

void CMonster::Call_RotIcon()
{
	if (nullptr == m_pKena)
		return;

	static_cast<CKena*>(m_pKena)->Call_RotIcon(this);
}

HRESULT CMonster::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxAnimMonsterModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	return S_OK;
}

HRESULT CMonster::SetUp_UI()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI_MonsterHP::BBDESC tDesc;
	tDesc.fileName = L"UI_Monster_Normal_HP";
	tDesc.pOwner = this;
	tDesc.vCorrect.y = m_pTransformCom->Get_vPxPivotScale().y + 0.2f ;

	if (FAILED(pGameInstance->Clone_GameObject(g_LEVEL, L"Layer_UI",
		TEXT("Prototype_GameObject_UI_MonsterHP"),
		CUtile::Create_DummyString(), &tDesc, (CGameObject**)&m_pUIHPBar)))
	{
		MSG_BOX("Failed To make UI");
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pMonsterStatusCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pFSM);
}

_int CMonster::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	if (pTarget)
	{
		if (iColliderIndex == COL_PLAYER_WEAPON)
		{
			// Ÿ��
		}
	}

	return 0;
}