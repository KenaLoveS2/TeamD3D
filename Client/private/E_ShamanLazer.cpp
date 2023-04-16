#include "stdafx.h"
#include "..\public\E_ShamanLazer.h"
#include "GameInstance.h"
#include "E_P_ExplosionGravity.h"
#include "E_LazerTrail.h"
#include "ShamanTrapHex.h"
#include "Camera_Player.h"

CE_ShamanLazer::CE_ShamanLazer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_ShamanLazer::CE_ShamanLazer(const CE_ShamanLazer & rhs)
	:CEffect(rhs)
{
	
}

HRESULT CE_ShamanLazer::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(__super::Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_ShamanLazer::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;
	FAILED_CHECK_RETURN(SetUp_Effects(), E_FAIL);

	m_eEFfectDesc.bActive = false;
	m_eEFfectDesc.vScale = XMVectorSet(0.8f, 0.8f, 0.8f, 1.f);
	Reset();

	CGameInstance::GetInstance()->Copy_Sound(TEXT("Mon_BossShaman_LaserCharge.ogg"), m_szCopySoundKey_Charge);
	CGameInstance::GetInstance()->Copy_Sound(TEXT("Mon_BossShaman_LaserFire1.ogg"), m_szCopySoundKey_Fire1);	
	CGameInstance::GetInstance()->Copy_Sound(TEXT("Mon_BossShaman_LaserFire2.ogg"), m_szCopySoundKey_Fire2);

	return S_OK;
}

HRESULT CE_ShamanLazer::Late_Initialize(void * pArg)
{	
	return S_OK;
}

void CE_ShamanLazer::Tick(_float fTimeDelta)
{
 	if (m_eEFfectDesc.bActive == false)
	{
		Reset();
		return;
	}

	if (m_pShamanTrapHex == nullptr)
		m_pShamanTrapHex = (CShamanTrapHex*)(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Monster", L"ShamanTrapHex_0"));

	__super::Tick(fTimeDelta);

	if (m_pPathTrail) m_pPathTrail->Tick(fTimeDelta);
	TurnOnLazer(fTimeDelta);
}

void CE_ShamanLazer::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
	{
		Reset();
		return;
	}

	if (m_pPathTrail) m_pPathTrail->Late_Tick(fTimeDelta);
	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_ShamanLazer::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	return S_OK;
}

void CE_ShamanLazer::Reset()
{
 	for (auto& pChild : m_vecChild)
	{
		pChild->Set_Active(false);
		if (dynamic_cast<CEffect*>(pChild))
			pChild->ResetSprite();
	}

	m_eEFfectDesc.bActive = false;
	m_eEFfectDesc.vScale = XMVectorSet(0.1f, 0.1f, 0.1f, 1.f);
	m_vecChild[CHILD_LINE]->Set_Scale(XMVectorSet(10.f, 2.f, 1.f, 1.f));
	m_vecChild[CHILD_SP]->Set_Scale(XMVectorSet(3.f, 3.f, 3.f, 1.f));
	m_fDurationTime = 0.0f;
	m_fTimeDelta = 0.0f;
	m_bFinalState = false;

	m_bLaserCharge = false;
	m_bLaserFire1 = false;
	m_bLaserFire2 = false;
}

_int CE_ShamanLazer::Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	return 0;
}

void CE_ShamanLazer::ImGui_PhysXValueProperty()
{
	__super::ImGui_PhysXValueProperty();
}

HRESULT CE_ShamanLazer::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CE_ShamanLazer::SetUp_Components()
{	
	return S_OK;
}

HRESULT CE_ShamanLazer::SetUp_Effects()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEffect_Base*  pEffectBase = nullptr;

	// TYPE_BOSS_GATHER
	{
		pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_ExplosionGravity", L"Lazer_Gravity"));
		NULL_CHECK_RETURN(pEffectBase, E_FAIL);
		dynamic_cast<CE_P_ExplosionGravity*>(pEffectBase)->Set_Option(CE_P_ExplosionGravity::TYPE_BOSS_GATHER);
		m_vecChild.push_back(pEffectBase);
	}

	{
		m_pPathTrail = dynamic_cast<CE_LazerTrail*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_LazerTrail", L"Lazer_Trail"));
		NULL_CHECK_RETURN(m_pPathTrail, E_FAIL);
	}

	RELEASE_INSTANCE(CGameInstance);

	for (auto& pChild : m_vecChild)
		pChild->Set_Parent(this);

	return S_OK;
}

void CE_ShamanLazer::Imgui_RenderProperty()
{
	ImGui::Checkbox("Active", &m_eEFfectDesc.bActive);
}

void CE_ShamanLazer::TurnOnLazer( _float fTimeDelta)
{
	if (m_vecChild[CHILD_SP]->Get_Active() == true)
	{
		Calculate_Path(fTimeDelta);
		m_pPathTrail->Set_Active(true);
		Play_LaserSound(&m_bLaserFire1, m_szCopySoundKey_Fire1, 0.5f);
		Play_LaserSound(&m_bLaserFire2, m_szCopySoundKey_Fire2, 0.5f);
		
		m_fDurationTime += fTimeDelta;
		if (m_fDurationTime > 1.f)
		{
			m_eEFfectDesc.bActive = false;
			m_fDurationTime = 0.0f;
			Reset();
		}
		m_bFinalState = true;
	}
	else
	{
		m_vecChild[CHILD_P]->Set_Active(m_eEFfectDesc.bActive);
		_float4 vPos = m_SpawnPos;
		m_vecChild[CHILD_P]->Set_Position(vPos);

		m_fDurationTime += fTimeDelta;
		if (m_fDurationTime > 0.5f)
		{
			m_vecChild[CHILD_LINE]->Set_Active(true);
			_float3 fLineScale = m_vecChild[CHILD_LINE]->Get_vScale();

			if (fLineScale.x <= 0.0f)
			{
				m_vecChild[CHILD_LINE]->Set_Active(false);
				m_vecChild[CHILD_P]->Set_Active(false);
				m_vecChild[CHILD_SP]->Set_Active(true);

				m_fDurationTime = 0.0f;
			}
			else
			{
				_float4 vScale = m_vecChild[CHILD_LINE]->Get_vScale();
				vScale.x -= fTimeDelta * m_pTransformCom->Get_TransformDesc().fSpeedPerSec * 2.f;
				m_vecChild[CHILD_LINE]->Set_Scale(vScale);
				Set_AddScale(fTimeDelta * -1.4f);
			}
		}
	}
}

void CE_ShamanLazer::Set_SpawnPos(_float4 vPos)
{
	if (m_pParent == nullptr)
		return;

	_vector vLook = m_pParent->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	m_SpawnPos = vPos += vLook * 2.0f;
	m_pTransformCom->Set_Position(vPos);
	m_eEFfectDesc.bActive = true;

	Play_LaserSound(&m_bLaserCharge, m_szCopySoundKey_Charge, 1.f);
}

void CE_ShamanLazer::Calculate_Path(_float fTimeDelta)
{
	if (dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr()) == nullptr)
		return;

	m_matDummy = m_pTransformCom->Get_WorldMatrix();

	_float4	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3	vScale = m_pTransformCom->Get_Scaled();
	_float4	vTargetPos = m_pShamanTrapHex->Get_TransformCom()->Get_Position();

	m_pPathTrail->Add_BezierCurve(vPos, vTargetPos, fTimeDelta);
}

CE_ShamanLazer * CE_ShamanLazer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_ShamanLazer * pInstance = new CE_ShamanLazer(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_ShamanLazer Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_ShamanLazer::Clone(void * pArg)
{
	CE_ShamanLazer * pInstance = new CE_ShamanLazer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_ShamanLazer Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_ShamanLazer::Free()
{
	__super::Free();

	Safe_Release(m_pPathTrail);
}

void CE_ShamanLazer::Play_LaserSound(_bool *pCheckFlag, _tchar* pSoundKey, _float fVolume)
{
	if (*pCheckFlag == false)
	{
		CGameInstance::GetInstance()->Play_Sound(pSoundKey, fVolume);
		*pCheckFlag = true;
	}
}