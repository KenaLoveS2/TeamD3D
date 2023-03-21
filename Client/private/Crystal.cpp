#include "stdafx.h"
#include "..\public\Crystal.h"
#include "GameInstance.h"
#include "ControlMove.h"
#include "Interaction_Com.h"
#include "Effect_Base.h"
#include "E_PulseObject.h"
#include "Pulse_Plate_Anim.h"
#include "ControlRoom.h"

CCrystal::CCrystal(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CCrystal::CCrystal(const CCrystal & rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CCrystal::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCrystal::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_bRenderActive = true;
	return S_OK;
}

HRESULT CCrystal::Late_Initialize(void * pArg)
{
#ifdef FOR_MAP_GIMMICK
	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	CPhysX_Manager *pPhysX = CPhysX_Manager::GetInstance();

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
	
	CGameInstance*	pGameInstance = CGameInstance::GetInstance();
	CEffect_Base* pEffectObj = nullptr;

	/*Recived_PulseE*/
	CE_PulseObject::E_PulseObject_DESC PulseObj_Desc;		
	ZeroMemory(&PulseObj_Desc, sizeof(PulseObj_Desc));
	PulseObj_Desc.eObjType = CE_PulseObject::PULSE_OBJ_RECIVE;	// 0번  :PULSE_OBJ_RECIVE
	PulseObj_Desc.fIncreseRatio = 1.3f;
	PulseObj_Desc.fPulseMaxSize = 2.f;
	PulseObj_Desc.vResetSize = _float3(0.25f, 0.25f, 0.25f);
	PulseObj_Desc.vResetPos = _float4(vPos.x, vPos.y+1.5f, vPos.z, vPos.w);
	pEffectObj = dynamic_cast<CEffect_Base*>(pGameInstance->
		Clone_GameObject(L"Prototype_GameObject_PulseObject", L"Crystal_Recived_PulseE"));
	NULL_CHECK_RETURN(pEffectObj, E_FAIL);
	static_cast<CE_PulseObject*>(pEffectObj)->Set_PulseObject_DESC(PulseObj_Desc);
	m_VecCrystal_Effect.push_back(pEffectObj);

	/*Deliver_PulseE*/
	ZeroMemory(&PulseObj_Desc, sizeof(PulseObj_Desc));
	PulseObj_Desc.eObjType = CE_PulseObject::PULSE_OBJ_DELIVER; // 1번  :PULSE_OBJ_DELIVER
	PulseObj_Desc.fIncreseRatio = 1.07f;
	PulseObj_Desc.fPulseMaxSize = 10.f;
	PulseObj_Desc.vResetSize = _float3(1.f, 1.f, 1.f);
	PulseObj_Desc.vResetPos = vPos;
	pEffectObj = dynamic_cast<CEffect_Base*>(pGameInstance->
		Clone_GameObject(L"Prototype_GameObject_PulseObject", L"Crystal_Deliver_PulseE"));
	NULL_CHECK_RETURN(pEffectObj, E_FAIL);
	static_cast<CE_PulseObject*>(pEffectObj)->Set_PulseObject_DESC(PulseObj_Desc);
	m_VecCrystal_Effect.push_back(pEffectObj);

	for (auto& pEffectObj : m_VecCrystal_Effect)
	{
		pEffectObj->Late_Initialize();
		pEffectObj->Set_Active(false);
	}
#endif

	if (m_EnviromentDesc.iRoomIndex == 2 &&
		!lstrcmp(L"2_Water_GimmickCrystal01", m_szCloneObjectTag))
	{
		m_pControlRoom = dynamic_cast<CControlRoom*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_ControlRoom", L"ControlRoom"));
		assert(m_pControlRoom != nullptr  && "CPulse_Plate_Anim::Late_Initialize(void * pArg)");
		m_pControlRoom->Add_Gimmick_TrggerObj(m_szCloneObjectTag, this);
	}
	
	return S_OK;
}

void CCrystal::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_VecCrystal_Effect.size() == 0)
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

	if (m_EnviromentDesc.iRoomIndex == 2 && 
		true == m_bGimmickActive && 
		 !lstrcmp(m_szCloneObjectTag,L"2_Water_GimmickCrystal01") && nullptr != m_pControlRoom)
	{	
		if(m_VecCrystal_Effect[1]->Get_Active() == true)
			m_pControlRoom->Trigger_Active(m_EnviromentDesc.iRoomIndex, CEnviromentObj::Gimmick_TYPE_GO_UP, true);
	}

}

void CCrystal::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

#ifdef FOR_MAP_GIMMICK
	if (m_VecCrystal_Effect.size() != 0)
	{ 
		for (auto& pEffectObj : m_VecCrystal_Effect)
		{
			if (pEffectObj != nullptr)
				pEffectObj->Late_Tick(fTimeDelta);
		}
	}
#endif

	if (m_pRendererCom && m_bRenderActive)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CCrystal::Render()
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

			if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_COMP_H_R_AO] != nullptr && (*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_EMISSIVE] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_H_R_AO, "g_HRAOTexture"), E_FAIL);
				float f = 1.f;
				FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fStonePulseIntensity", &f, sizeof(float)), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 4), E_FAIL);
			}
			else if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_COMP_H_R_AO] != nullptr)
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

			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 4), E_FAIL);
			if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_COMP_H_R_AO] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_H_R_AO, "g_HRAOTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 7), E_FAIL);
			}
			else	if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_COMP_E_R_AO] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_E_R_AO, "g_ERAOTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 8), E_FAIL);
			}
			else if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_AMBIENT_OCCLUSION] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 9), E_FAIL);
			}
			else
			{
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 4), E_FAIL);
			}
		}
	}

	return S_OK;
}

HRESULT CCrystal::RenderShadow()
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
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 2), E_FAIL);
		}
	}

	return S_OK;
}

HRESULT CCrystal::Add_AdditionalComponent(_uint iLevelIndex, const _tchar * pComTag, COMPONENTS_OPTION eComponentOption)
{
	__super::Add_AdditionalComponent(iLevelIndex, pComTag, eComponentOption);

	/* For.Com_CtrlMove */
	if (eComponentOption == COMPONENTS_CONTROL_MOVE)
	{
		if (FAILED(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_ControlMove"), pComTag,
			(CComponent**)&m_pControlMoveCom)))
			return E_FAIL;
	}
	/* For.Com_Interaction */
	else if (eComponentOption == COMPONENTS_INTERACTION)
	{
		if (FAILED(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_Interaction_Com"), pComTag,
			(CComponent**)&m_pInteractionCom)))
			return E_FAIL;
	}
	else
		return S_OK;

	return S_OK;
}

_int CCrystal::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	return 0;
}

_int CCrystal::Execute_TriggerTouchFound(CGameObject * pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (pTarget == m_VecCrystal_Effect[1] || iColliderIndex != (_int)TRIGGER_PULSE)
		return 0;

	if (!lstrcmp(m_szCloneObjectTag, L"2_Water_GimmickCrystal01"))
	{
		if(true == m_bGimmickActive)
			m_VecCrystal_Effect[0]->Set_Active(true);

		return 0;
	}
	m_VecCrystal_Effect[0]->Set_Active(true);

	return 0;
}

_int CCrystal::Execute_TriggerTouchLost(CGameObject * pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (iColliderIndex == (_int)TRIGGER_PULSE)
		_bool b = false;
	return 0;
}

HRESULT CCrystal::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (m_EnviromentDesc.iCurLevel == 0)
		m_EnviromentDesc.iCurLevel = LEVEL_MAPTOOL;
	/* For.Com_Shader */
	/*나중에  레벨 인덱스 수정해야됌*/
	/* For.Com_Model */ 	/*나중에  레벨 인덱스 수정해야됌*/
	if (FAILED(__super::Add_Component(g_LEVEL, m_EnviromentDesc.szModelTag.c_str(), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (m_pModelCom->Get_IStancingModel())
	{
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModelInstance"), TEXT("Com_Shader"),
			(CComponent**)&m_pShaderCom)))
			return E_FAIL;
		m_iShaderOption = 1;
	}
	else
	{
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModelTess"), TEXT("Com_Shader"),
			(CComponent**)&m_pShaderCom)))
			return E_FAIL;
		m_iShaderOption = 4;
	}


	return S_OK;
}

HRESULT CCrystal::SetUp_ShaderResources()
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

HRESULT CCrystal::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CCrystal * CCrystal::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCrystal*		pInstance = new CCrystal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCrystal");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCrystal::Clone(void * pArg)
{
		CCrystal*		pInstance = new CCrystal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCrystal");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCrystal::Free()
{
	

	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);

	for (auto &pEffect : m_VecCrystal_Effect)
		Safe_Release(pEffect);
	m_VecCrystal_Effect.clear();




}
