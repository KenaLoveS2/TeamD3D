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

void CMonster::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
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

HRESULT CMonster::Call_EventFunction(const string & strFuncName)
{
	return S_OK;
}

void CMonster::Push_EventFunctions()
{
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

_float CMonster::DistanceBetweenPlayer()
{
	_float3 vPlayerPos = m_pKena->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	return _float3::Distance(vPos, vPlayerPos);
}

void CMonster::Free()
{
	__super::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRangeCol);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pFSM);
}
