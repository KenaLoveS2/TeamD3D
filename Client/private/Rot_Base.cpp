#include "stdafx.h"
#include "..\public\Rot_Base.h"
#include "CameraForRot.h"
#include "E_TeleportRot.h"
#include "Kena.h"
#include "Kena_Status.h"
#include "RotHat.h"
#include "GameInstance.h"

CRot_Base::CRot_Base(ID3D11Device* pDevice, ID3D11DeviceContext* p_context)
	: CGameObject(pDevice, p_context)
	, m_pGameInstance(CGameInstance::GetInstance())
{
}

CRot_Base::CRot_Base(const CRot_Base& rhs)
	: CGameObject(rhs)
	, m_pGameInstance(CGameInstance::GetInstance())
{
}

const _double& CRot_Base::Get_AnimationPlayTime()
{
	return m_pModelCom->Get_PlayTime();
}

HRESULT CRot_Base::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CRot_Base::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC GaemObjectDesc;
	ZeroMemory(&GaemObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));
	GaemObjectDesc.TransformDesc.fSpeedPerSec = CUtile::Get_RandomFloat(1.3f, 2.f);
	GaemObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	FAILED_CHECK_RETURN(__super::Initialize(&GaemObjectDesc), E_FAIL);
	
	FAILED_CHECK_RETURN(Create_CopySound(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Effects(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State(), E_FAIL);
	
	m_pModelCom->Set_AnimIndex(IDLE);
	m_pModelCom->Set_AllAnimCommonType();
	Push_EventFunctions();
		
	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	return S_OK;
}

HRESULT CRot_Base::Late_Initialize(void * pArg)
{	
	FAILED_CHECK_RETURN(Setup_KenaParamters(), E_FAIL);

	// Capsule X == radius , Y == halfHeight
	CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
	PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
	PxCapsuleDesc.pActortag = m_szCloneObjectTag;
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

	CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, true, COL_ROT));

	m_pTransformCom->Connect_PxActor_Gravity(m_szCloneObjectTag, _float3(0.f, 0.15f, 0.f));
				
	return S_OK;
}

void CRot_Base::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CRot_Base::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);	
}

HRESULT CRot_Base::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		if (i == 0)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 27), E_FAIL);
		}
		else if (i == 1)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 32), E_FAIL);
		}
		else if (i == 2)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_ALPHA, "g_AlphaTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 31), E_FAIL);
		}
	}

	return S_OK;
}

HRESULT CRot_Base::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 11);

	return S_OK;
}

HRESULT CRot_Base::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_Rot", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);
	m_pModelCom->Set_RootBone("Rot_RIG");
	
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Rot/rh_body_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(2, WJTextureType_ALPHA, TEXT("../Bin/Resources/Anim/Rot/rot_fur_ALPHA.png")), E_FAIL);

	return S_OK;
}

HRESULT CRot_Base::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	return S_OK;
}

HRESULT CRot_Base::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	return S_OK;
}

CRot_Base* CRot_Base::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRot_Base*	pInstance = new CRot_Base(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CRot_Base");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRot_Base::Clone(void* pArg)
{
	CRot_Base*	pInstance = new CRot_Base(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CRot_Base");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRot_Base::Free()
{
	__super::Free();

	Safe_Release(m_pFSM);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
		
	Safe_Release(m_pTeleportRot);
}

HRESULT CRot_Base::SetUp_Effects()
{
	CEffect_Base* pEffectBase = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pTeleportRot = dynamic_cast<CE_TeleportRot*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_TeleportRot", CUtile::Create_DummyString()));
	NULL_CHECK_RETURN(m_pTeleportRot, E_FAIL);
//	m_pTeleportRot->Set_Parent(this);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}


void CRot_Base::Push_EventFunctions()
{
	Play_Rot1Sound(true, 0.f);
	Play_Rot2Sound(true, 0.f);
	Play_Rot3Sound(true, 0.f);
	Play_Rot4Sound(true, 0.f);
	Play_Rot5Sound(true, 0.f);
	Play_Rot6Sound(true, 0.f);
	Play_Rot7Sound(true, 0.f);
	Play_Rot8Sound(true, 0.f);
	Play_Rot9Sound(true, 0.f);
	Play_Rot10Sound(true, 0.f);
	Play_Rot11Sound(true, 0.f);
	Play_Rot12Sound(true, 0.f);
	Play_MoveSound(true, 0.f);	
}

HRESULT CRot_Base::Create_CopySound()
{
	_tchar szSoundTable[COPY_SOUND_KEY_END][64] = {
		TEXT("Rot (1).ogg"), TEXT("Rot (2).ogg"), TEXT("Rot (3).ogg"), TEXT("Rot (4).ogg"), TEXT("Rot (5).ogg"), TEXT("Rot (6).ogg"),
		TEXT("Rot (7).ogg"), TEXT("Rot (8).ogg"), TEXT("Rot (9).ogg"), TEXT("Rot (10).ogg"), TEXT("Rot (11).ogg"), TEXT("Rot (12).ogg"),
		TEXT("Rot_Bait_Movement.ogg"), TEXT("Rot_Cross.ogg"),		
	};

	_tchar szTemp[MAX_PATH] = { 0, };
	for (_uint i = 0; i < (_uint)COPY_SOUND_KEY_END; i++)
	{
		if(FAILED(CGameInstance::GetInstance()->Copy_Sound(szSoundTable[i], szTemp)))
			return E_FAIL;

		m_pCopySoundKey[i] = CUtile::Create_StringAuto(szTemp);
	}	

	return S_OK;
}

void CRot_Base::Play_Rot1Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CRot_Base::Play_Rot1Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_1], 0.5f);
}

void CRot_Base::Play_Rot2Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CRot_Base::Play_Rot2Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_2], 0.6f);
}

void CRot_Base::Play_Rot3Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CRot_Base::Play_Rot3Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_3], 0.7f);
}

void CRot_Base::Play_Rot4Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CRot_Base::Play_Rot4Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_4], 0.6f);
}

void CRot_Base::Play_Rot5Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CRot_Base::Play_Rot5Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_5], 0.6f);
}

void CRot_Base::Play_Rot6Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CRot_Base::Play_Rot6Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_6], 0.6f);
}

void CRot_Base::Play_Rot7Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CRot_Base::Play_Rot7Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_7], 0.6f);
}

void CRot_Base::Play_Rot8Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CRot_Base::Play_Rot8Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_8], 0.6f);
}

void CRot_Base::Play_Rot9Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CRot_Base::Play_Rot9Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_9], 0.6f);
}

void CRot_Base::Play_Rot10Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CRot_Base::Play_Rot10Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_10], 0.8f);
}

void CRot_Base::Play_Rot11Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CRot_Base::Play_Rot11Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_11], 0.8f);
}

void CRot_Base::Play_Rot12Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CRot_Base::Play_Rot12Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_12], 0.6f);
}

void CRot_Base::Play_MoveSound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CRot_Base::Play_MoveSound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_MOVE], 0.3f);
}

void CRot_Base::TurnOn_TeleportEffect(_float4 vTeleportPos, _uint iAnimindex)
{
	m_pTeleportRot->Set_Active(true);
	m_pTeleportRot->Set_Position(vTeleportPos);
	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_4], 1.f);

	m_pModelCom->ResetAnimIdx_PlayTime(iAnimindex);
	m_pModelCom->Set_AnimIndex(iAnimindex);
}

HRESULT CRot_Base::Setup_KenaParamters()
{
	m_pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena"));
	assert(m_pKena != nullptr && "CRot_Base::Late_Initialize");

	m_pKenaTransform = dynamic_cast<CTransform*>(m_pKena->Get_TransformCom());
	assert(m_pKenaTransform != nullptr && "CRot_Base::Late_Initialize");

	m_pkenaState = dynamic_cast<CKena_Status*>(m_pKena->Get_Status());
	assert(m_pkenaState != nullptr && "CRot_Base::Late_Initialize");

	return S_OK;
}