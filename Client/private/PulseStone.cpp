#include "stdafx.h"
#include "..\public\PulseStone.h"
#include "GameInstance.h"
#include "ControlMove.h"

#include "E_PulseObject.h"
#include "CPortalPlane.h"
#include "Pulse_Plate_Anim.h"

CPulseStone::CPulseStone(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CPulseStone::CPulseStone(const CPulseStone & rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CPulseStone::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPulseStone::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_bRenderActive = true;
	return S_OK;
}

HRESULT CPulseStone::Late_Initialize(void* pArg)
{
	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	CPhysX_Manager* pPhysX = CPhysX_Manager::GetInstance();

	CPhysX_Manager::PX_BOX_DESC BoxDesc;
	BoxDesc.pActortag = m_szCloneObjectTag;
	BoxDesc.eType = BOX_DYNAMIC;
	BoxDesc.vPos = CUtile::Float_4to3(vPos);
	BoxDesc.vSize = _float3(0.9f, 1.25f, 0.9f);
	BoxDesc.vRotationAxis = _float3(0.f, 0.f, 0.f);
	BoxDesc.fDegree = 0.f;
	BoxDesc.isGravity = false;
	BoxDesc.eFilterType = PX_FILTER_TYPE::FITLER_ENVIROMNT;
	BoxDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	BoxDesc.fDensity = 0.2f;
	BoxDesc.fMass = 150.f;
	BoxDesc.fLinearDamping = 10.f;
	BoxDesc.fAngularDamping = 5.f;
	BoxDesc.bCCD = false;
	BoxDesc.fDynamicFriction = 0.5f;
	BoxDesc.fStaticFriction = 0.5f;
	BoxDesc.fRestitution = 0.1f;
	BoxDesc.bKinematic = true;

	pPhysX->Create_Box(BoxDesc, Create_PxUserData(this, true, COL_ENVIROMENT));
	m_pTransformCom->Connect_PxActor_Gravity(m_szCloneObjectTag);
	m_pTransformCom->Set_Position(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Base* pEffectObj = nullptr;


	/*Recived_PulseE*/
	CE_PulseObject::E_PulseObject_DESC PulseObj_Desc;
	ZeroMemory(&PulseObj_Desc, sizeof(PulseObj_Desc));
	PulseObj_Desc.eObjType = CE_PulseObject::PULSE_OBJ_RECIVE;	// 0번  :PULSE_OBJ_RECIVE
	PulseObj_Desc.fIncreseRatio = 1.1f;
	PulseObj_Desc.fPulseMaxSize = 2.f;
	PulseObj_Desc.vResetSize = _float3(0.25f, 0.25f, 0.25f);
	PulseObj_Desc.vResetPos = _float4(vPos.x, vPos.y + 1.5f, vPos.z, vPos.w);
	pEffectObj = dynamic_cast<CEffect_Base*>(pGameInstance->
		Clone_GameObject(L"Prototype_GameObject_PulseObject", 
			CUtile::Create_CombinedString(m_szCloneObjectTag, TEXT("PulseStone_Recived_PulseE"))));

	NULL_CHECK_RETURN(pEffectObj, E_FAIL);
	static_cast<CE_PulseObject*>(pEffectObj)->Set_PulseObject_DESC(PulseObj_Desc);
	m_VecCrystal_Effect.push_back(pEffectObj);

	/*Deliver_PulseE*/
	ZeroMemory(&PulseObj_Desc, sizeof(PulseObj_Desc));
	PulseObj_Desc.eObjType = CE_PulseObject::PULSE_OBJ_DELIVER; // 1번  :PULSE_OBJ_DELIVER
	PulseObj_Desc.fIncreseRatio = 1.05f;
	PulseObj_Desc.fPulseMaxSize = 10.f;
	PulseObj_Desc.vResetSize = _float3(1.f, 1.f, 1.f);
	PulseObj_Desc.vResetPos = vPos;
	pEffectObj = dynamic_cast<CEffect_Base*>(pGameInstance->
		Clone_GameObject(L"Prototype_GameObject_PulseObject", 
			CUtile::Create_CombinedString(m_szCloneObjectTag, TEXT("PulseStone_Deliver_PulseE"))));

	NULL_CHECK_RETURN(pEffectObj, E_FAIL);
	static_cast<CE_PulseObject*>(pEffectObj)->Set_PulseObject_DESC(PulseObj_Desc);
	m_VecCrystal_Effect.push_back(pEffectObj);

	for (auto& pEffectObj : m_VecCrystal_Effect)
	{
		pEffectObj->Late_Initialize();
		pEffectObj->Set_Active(false);
	}


	if (m_EnviromentDesc.iRoomIndex == 6)
	{
		m_pRenderFalsePortal = dynamic_cast<CPortalPlane*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL,
			L"Layer_Enviroment", L"MG_CrystalGimmick_Portal"));

		assert(m_pRenderFalsePortal != nullptr && "CPulseStone::Late_Initialize(void* pArg)");

		

	}

	return S_OK;
}

void CPulseStone::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_VecCrystal_Effect.size() == 0 || m_bRenderActive == false)
		return;

	if (static_cast<CE_PulseObject*>(m_VecCrystal_Effect[0])->Get_Finish())
	{
		m_VecCrystal_Effect[1]->Set_Active(true);
	}

	for (auto& pEffectObj : m_VecCrystal_Effect)
	{
		if (pEffectObj != nullptr)
			pEffectObj->Tick(fTimeDelta);
	}

	if (m_VecCrystal_Effect[1]->Get_Active() == true)
		m_pRenderFalsePortal->Set_GimmickRender(true);

}

void CPulseStone::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_VecCrystal_Effect.size() != 0)
	{
		for (auto& pEffectObj : m_VecCrystal_Effect)
		{
			if (pEffectObj != nullptr)
				pEffectObj->Late_Tick(fTimeDelta);
		}
	}

	if (m_pRendererCom && m_bRenderActive)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CPulseStone::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (m_pModelCom->Get_IStancingModel())
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);

			if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_COMP_H_R_AO] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_H_R_AO, "g_HRAOTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 2), E_FAIL);
			}
			else	if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_COMP_E_R_AO] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_E_R_AO, "g_ERAOTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 5), E_FAIL);
			}
			else if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_AMBIENT_OCCLUSION] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 6), E_FAIL);
			}
			else
			{
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 1), E_FAIL);
			}
		}
	}
	else
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 15), E_FAIL);
		}
	}

	return S_OK;
}

HRESULT CPulseStone::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (m_pModelCom->Get_IStancingModel())
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 0), E_FAIL);
		}
	}
	else
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 21), E_FAIL);
		}
	}

	return S_OK;
}


_int CPulseStone::Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (pTarget == m_VecCrystal_Effect[1] || iColliderIndex != (_int)TRIGGER_PULSE)
		return 0;

	if (true == m_bGimmickActive)
		m_VecCrystal_Effect[0]->Set_Active(true);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Interactive_Crystal_Activated.ogg", 0.5f, false);

	return 0;
}

_int CPulseStone::Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (iColliderIndex == (_int)TRIGGER_PULSE)
		_bool b = false;
	return 0;
}

HRESULT CPulseStone::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;


	/*나중에  레벨 인덱스 수정해야됌*/
	if (m_EnviromentDesc.iCurLevel == 0)
		m_EnviromentDesc.iCurLevel = LEVEL_MAPTOOL;

	/* For.Com_Model */ 	/*나중에  레벨 인덱스 수정해야됌*/
	if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, m_EnviromentDesc.szModelTag.c_str(), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModelInstance"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CPulseStone::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(float)), E_FAIL);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CPulseStone::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(float)), E_FAIL);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CPulseStone * CPulseStone::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPulseStone*		pInstance = new CPulseStone(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPulseStone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPulseStone::Clone(void * pArg)
{
	CPulseStone*		pInstance = new CPulseStone(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPulseStone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPulseStone::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	
	for (auto& pEffect : m_VecCrystal_Effect)
		Safe_Release(pEffect);
	m_VecCrystal_Effect.clear();

}
