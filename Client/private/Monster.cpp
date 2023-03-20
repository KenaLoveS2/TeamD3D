#include "stdafx.h"
#include "Monster.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "RotForMonster.h"

#include "UI_MonsterHP.h"
#include "Camera.h"
#include "Camera_Player.h"


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

_vector CMonster::Get_Position()
{
	return m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
}

_vector CMonster::Get_FocusPosition()
{
	return m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)
		+ XMVectorSet(0.f, 6.f * m_pTransformCom->Get_vPxPivotScale().y, 0.f, 0.f);
}

HRESULT CMonster::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)

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


	m_pKena = (CKena*)pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Player"),TEXT("Kena"));

	/* Hit */
	m_pKenaHit = dynamic_cast<CE_KenaHit*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_KenaHit", CUtile::Create_DummyString()));
	NULL_CHECK_RETURN(m_pKenaHit, E_FAIL);

	RELEASE_INSTANCE(CGameInstance)
	m_bRotable = true;
	return S_OK;
}

HRESULT CMonster::Late_Initialize(void * pArg)
{

	/* Is In Camera? */
	return S_OK;
}

void CMonster::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

/* #ifdef _DEBUG
	if (nullptr != m_pUIHPBar)
		m_pUIHPBar->Imgui_RenderProperty();

	static _float fGuage = 1.f;
	if (CGameInstance::GetInstance()->Key_Down(DIK_I))
	{
		fGuage -= 0.1f;
		m_pUIHPBar->Set_Guage(fGuage);
	}
#endif */
		
	m_fDissolveTime += fTimeDelta * 0.2f * m_bDying;
		
	m_pEnemyWisp ? m_pEnemyWisp->Tick(fTimeDelta) : 0;
	m_pKenaHit ? m_pKenaHit->Tick(fTimeDelta) : 0;		
	
	m_vKenaPos = m_pKena ? m_pKena->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION) : m_vKenaPos;
}

void CMonster::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	m_pEnemyWisp ? m_pEnemyWisp->Late_Tick(fTimeDelta) : 0;
	m_pKenaHit ? m_pKenaHit->Late_Tick(fTimeDelta) : 0;
			
	/* calculate camera */
	_float4 vCamLook = CGameInstance::GetInstance()->Get_CamLook_Float4();
	_float4 vCamPos = CGameInstance::GetInstance()->Get_CamPosition();
	_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float fDistance = _float4::Distance(vCamPos, vPos);
	_float4 vDir = XMVector3Normalize(vPos - vCamPos);

	if (fDistance <= 10.f)
	{
		if(!m_bBind && (XMVectorGetX(XMVector3Dot(vDir, vCamLook)) > cosf(XMConvertToRadians(20.f))))
			Call_RotIcon();
	} 

	Call_MonsterFocusIcon();		
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

	if(ImGui::Button("AddShaderValue"))
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
			pGameInstance->Add_ShaderValueObject(g_LEVEL, this);
			RELEASE_INSTANCE(CGameInstance)
	}

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

void CMonster::Calc_RootBoneDisplacement(_fvector vDisplacement)
{
	_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vPos = vPos + vDisplacement;
	m_pTransformCom->Set_Translation(vPos, vDisplacement);
}

void CMonster::Bind(CRotForMonster * pGameObject[], _int iRotCnt)
{
	m_bBind = true;
	for (_int i = 0; i<iRotCnt; ++i)
		m_pRotForMonster[i] = pGameObject[i];
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
	if (nullptr == m_pKena || m_bSpawn == false || m_bDying || m_bDeath)
		return;

	m_pKena->Call_FocusRotIcon(this);
}

void CMonster::Call_MonsterFocusIcon()
{
	if (nullptr == m_pKena || !m_bSpawn || m_bDying || m_bDeath)
		return;

	if (m_pTransformCom->Calc_Distance_XZ(CGameInstance::GetInstance()->Get_CamPosition()) > 10.f)
		return;

	CCamera*		pCamera = CGameInstance::GetInstance()->Get_WorkCameraPtr();
	if (pCamera->Get_TransformCom()->Calc_InRange(XMConvertToRadians(120.f), m_pTransformCom) == true)
		m_pKena->Smooth_Targeting(this);
}

HRESULT CMonster::Ready_EnemyWisp(const _tchar* szEnemyWispCloneTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	m_pEnemyWisp = dynamic_cast<CEnemyWisp*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_EnemyWisp", szEnemyWispCloneTag));
	NULL_CHECK_RETURN(m_pEnemyWisp, E_FAIL );
	m_pEnemyWisp->Set_Parent(this);
		
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CMonster::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxAnimMonsterModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Texture_Dissolve", L"Com_Dissolve_Texture", (CComponent**)&m_pDissolveTextureCom), E_FAIL);
	
	
	return S_OK;
}

HRESULT CMonster::SetUp_UI(_float fOffsetY)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI_Billboard::BBDESC tDesc;
	tDesc.fileName = L"UI_Monster_Normal_HP";
	tDesc.pOwner = this;
	tDesc.vCorrect.y = m_pTransformCom->Get_vPxPivotScale().y + fOffsetY;

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

	Safe_Release(m_pDissolveTextureCom);
	Safe_Release(m_pMonsterStatusCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pFSM);

	Safe_Release(m_pEnemyWisp);
	Safe_Release(m_pKenaHit);	
}

_int CMonster::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	if (pTarget && m_bSpawn)
	{
		if ((iColliderIndex == (_int)COL_PLAYER_WEAPON || iColliderIndex == (_int)COL_PLAYER_ARROW) && m_pKena->Get_State(CKena::STATE_ATTACK))
		{
			m_pUIHPBar->Set_Active(true);
			WeakleyHit();
			m_pMonsterStatusCom->UnderAttack(m_pKena->Get_KenaStatusPtr());
			m_pUIHPBar->Set_Guage(m_pMonsterStatusCom->Get_PercentHP());
			
			m_bStronglyHit = true;

			m_pKenaHit->Set_Active(true);
			m_pKenaHit->Set_Position(vCollisionPos);

			if (m_pKena->Get_State(CKena::STATE_HEAVYATTACK) == false)
			{
				dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->TimeSleep(0.15f);
				dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->Camera_Shake(0.003f, 5);
			}
			else
			{
				dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->TimeSleep(0.5f);
				dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->Camera_Shake(0.005f, 5);
			}
		}
	}

	return 0;
}

HRESULT CMonster::Bind_Dissolove(CShader* pShader)
{	
	if (FAILED(pShader->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float)))) return E_FAIL;
	if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(pShader, "g_DissolveTexture"))) return E_FAIL;
	
	return S_OK;
}

void CMonster::Set_Dying(_uint iDeathAnimIndex)
{
	m_pModelCom->ResetAnimIdx_PlayTime(iDeathAnimIndex);
	m_pModelCom->Set_AnimIndex(iDeathAnimIndex);

	m_pKena->Dead_FocusMonsterIcon(this);	
	m_bDying = true;
	m_pUIHPBar->Set_Active(false);
	m_pTransformCom->Clear_Actor();
}