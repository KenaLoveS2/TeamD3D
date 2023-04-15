#include "stdafx.h"
#include "..\public\Meditation_Spot.h"
#include "GameInstance.h"
#include "ControlMove.h"
#include "Interaction_Com.h"
#include "Kena.h"
#include "AnimationState.h"
#include "Kena_State.h"
#include "Kena_Status.h"
#include "E_P_Meditation_Spot.h"

CMeditation_Spot::CMeditation_Spot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CMeditation_Spot::CMeditation_Spot(const CMeditation_Spot& rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CMeditation_Spot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMeditation_Spot::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Effect(), E_FAIL);

	m_bRenderActive = true;
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STATIC_SHADOW, this);
	return S_OK;
}

HRESULT CMeditation_Spot::Late_Initialize(void* pArg)
{
	m_pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena"));
	NULL_CHECK_RETURN(m_pKena, E_FAIL);

	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	_float3 vSize = _float3(1.51f, 0.39f, 1.34f);

	CPhysX_Manager* pPhysX = CPhysX_Manager::GetInstance();
	CPhysX_Manager::PX_BOX_DESC BoxDesc;
	BoxDesc.pActortag = m_szCloneObjectTag;
	BoxDesc.eType = BOX_STATIC;
	BoxDesc.vPos = vPos;
	BoxDesc.vSize = vSize;
	BoxDesc.vRotationAxis = _float3(0.f, 0.f, 0.f);
	BoxDesc.fDegree = 0.f;
	BoxDesc.isGravity = false;
	BoxDesc.eFilterType = PX_FILTER_TYPE::FILTER_DEFULAT;
	BoxDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	BoxDesc.fDensity = 0.2f;
	BoxDesc.fMass = 150.f;
	BoxDesc.fLinearDamping = 10.f;
	BoxDesc.fAngularDamping = 5.f;
	BoxDesc.bCCD = false;
	BoxDesc.fDynamicFriction = 0.5f;
	BoxDesc.fStaticFriction = 0.5f;
	BoxDesc.fRestitution = 0.1f;

	pPhysX->Create_Box(BoxDesc, Create_PxUserData(this, false, COL_ENVIROMENT));
	pPhysX->Create_Trigger(Create_PxTriggerData(m_szCloneObjectTag, this, TRIGGER_MEDITATIONSPOT, vPos, 2.f));

	//m_pModelCom->InstanceModelPosInit(m_pTransformCom->Get_WorldMatrix());

	if (m_pModelCom->Get_IStancingModel() == true && m_pModelCom->Get_UseTriangleMeshActor())
	{
		m_pModelCom->Create_Px_InstTriangle(m_pTransformCom);
	}
	else if (m_pModelCom->Get_IStancingModel() == false && m_pModelCom->Get_UseTriangleMeshActor())
	{
		m_pModelCom->Create_PxTriangle(Create_PxUserData(this, false, COL_ENVIROMENT));
	}
	else
		return S_OK;

	return S_OK;
}

void CMeditation_Spot::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	/* Kena Interact */
	if (m_bKenaDetected == true)
	{
		CAnimationState* pAnimState = m_pKena->Get_AnimationStateMachine();
		if (pAnimState->Get_CurrentAnimIndex() == (_uint)CKena_State::MEDITATE_LOOP)
		{
			CKena_Status* pStatus = m_pKena->Get_Status();
			_int	iHP = pStatus->Get_HP();

			if (m_bUsed == false)
			{
				m_pMeditationSpotEffect->TurnOff_Meditation(true);

				if (pAnimState->Get_AnimationProgress() > 0.4f)
				{

					m_bKenaDetected = false;
					m_bUsed = true;

					_int	iMaxHP = pStatus->Get_MaxHP();
					_float	fHPRate = pStatus->Get_PercentHP();

					pStatus->Set_MaxHP(iMaxHP + 50);
					pStatus->Add_HealAmount(pStatus->Get_MaxHP() - iHP);

					/* NEED : UI HP GUAGE UP */
					//m_pKena->m_Delegator
					CUI_ClientManager::UI_PRESENT eUpgrade = CUI_ClientManager::HUD_HP_UPGRADE;
					fHPRate = pStatus->Get_PercentHP();
					pStatus->m_StatusDelegator.broadcast(eUpgrade, fHPRate);
				}
			}
			else
			{
				m_pMeditationSpotEffect->TurnOff_Meditation(true);

				if (iHP + 1 <= pStatus->Get_MaxHP())
					pStatus->Add_HealAmount(1);
			}
		}
	}

	if (m_bPulseTest)
		m_fEmissivePulse += 0.05f;

	if (!m_bPulseTest)
	{
		m_fEmissivePulse -= 0.05f;
		if (m_fEmissivePulse <= 0.f)
		{
			m_fEmissivePulse = 0.f;
			m_bPulseTest = !m_bPulseTest;
		}
	}

	if (m_fEmissivePulse >= 2.f)
		m_bPulseTest = !m_bPulseTest;

	if (m_bOncePosUpdate == false && m_bRenderActive)
	{
		m_pModelCom->InstanceModelPosInit(m_pTransformCom->Get_WorldMatrix());
		m_bOncePosUpdate = true;
	}

	m_pTransformCom->Tick(fTimeDelta);
	if (m_pMeditationSpotEffect) m_pMeditationSpotEffect->Tick(fTimeDelta);
}

void CMeditation_Spot::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	_matrix  WolrdMat = m_pTransformCom->Get_WorldMatrix();

	if (m_pRendererCom && m_bRenderActive && false == m_pModelCom->Culling_InstancingMeshs(200.f, WolrdMat))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_CINE, this);
#endif
	}

	if (m_pMeditationSpotEffect) m_pMeditationSpotEffect->Late_Tick(fTimeDelta);
}

HRESULT CMeditation_Spot::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (m_pModelCom->Get_IStancingModel())
	{
		if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_ShrineOfDeath_MainRock")
		{
			// 매터리얼 잘못들어가서 수정되야하는 친구임 일단 이렇게 수정
			for (_uint i = 0; i < iNumMeshes; ++i)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 0, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 0, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 0, WJTextureType_EMISSIVE, "g_EmissiveTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 0, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fMeditation_SpotPulseIntensity", &m_fEmissivePulse, sizeof(float)), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 3), E_FAIL);
			}
		}
		else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_ShrineOfDeath_Rock_02" || m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_ShrineOfDeath_Rock_03")
		{
			// 매터리얼 잘못들어가서 수정되야하는 친구임 일단 이렇게 수정
			for (_uint i = 0; i < iNumMeshes; ++i)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 1, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 1, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 1, WJTextureType_EMISSIVE, "g_EmissiveTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 1, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fMeditation_SpotPulseIntensity", &m_fEmissivePulse, sizeof(float)), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 3), E_FAIL);
			}
		}
		else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_ShrineOfLife")
		{
			for (_uint i = 0; i < iNumMeshes; ++i)
			{
				if (i == 0)
				{
					FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
					FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
					FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVEMASK, "g_EmissiveTexture"), E_FAIL);
					FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture"), E_FAIL);
					FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fMeditation_SpotPulseIntensity", &m_fEmissivePulse, sizeof(float)), E_FAIL);
					FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 3), E_FAIL);
				}
				else if (i == 1)
				{
					FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
					FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
					FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture"), E_FAIL);
					FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 6), E_FAIL);
				}
				else if (i == 2)
				{
					FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
					FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
					FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture"), E_FAIL);
					FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 7), E_FAIL);
				}
			}
		}
		else
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
					FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fMeditation_SpotPulseIntensity", &m_fEmissivePulse, sizeof(float)), E_FAIL);
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
	}
	else
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 15), E_FAIL);
		}
	}

	return S_OK;
}

HRESULT CMeditation_Spot::RenderShadow()
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

HRESULT CMeditation_Spot::RenderCine()
{
	if (FAILED(__super::RenderCine()))
		return E_FAIL;

	if (FAILED(__super::SetUp_CineShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (m_pModelCom->Get_IStancingModel())
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 12), E_FAIL);
		}
	}
	else
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 20), E_FAIL);
		}
	}

	return S_OK;
}

void CMeditation_Spot::Imgui_RenderProperty()
{
	if (ImGui::Button("Emissive"))
		m_bPulseTest = true;
}

void CMeditation_Spot::ImGui_ShaderValueProperty()
{
	__super::ImGui_ShaderValueProperty();
	ImGui::Text(CUtile::WstringToString(m_EnviromentDesc.szModelTag).c_str());
	m_pModelCom->Imgui_MaterialPath();
	m_pTransformCom->Imgui_RenderProperty();
}

void CMeditation_Spot::ImGui_PhysXValueProperty()
{
	ImGui::Text("CMeditation_Spot::ImGui_PhysXValueProperty");

	PxRigidActor* pActor = CPhysX_Manager::GetInstance()->Find_StaticActor(m_szCloneObjectTag);

	PxShape* pShape = nullptr;
	pActor->getShapes(&pShape, sizeof(PxShape));
	PxBoxGeometry& Geometry = pShape->getGeometry().box();
	PxVec3& fScale = Geometry.halfExtents;

	/* Scale */
	ImGui::BulletText("Scale Setting");
	ImGui::DragFloat("Scale X", &fScale.x, 0.01f);
	ImGui::DragFloat("Scale Y", &fScale.y, 0.01f);
	ImGui::DragFloat("Scale Z", &fScale.z, 0.01f);

	pShape->setGeometry(Geometry);
}

HRESULT CMeditation_Spot::Add_AdditionalComponent(_uint iLevelIndex, const _tchar* pComTag, COMPONENTS_OPTION eComponentOption)
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

_int CMeditation_Spot::Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	if (iColliderIndex == (_int)COL_PULSE)
		_bool b = false;

	return 0;
}

_int CMeditation_Spot::Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (pTarget != nullptr)
	{
		if (iColliderIndex == (_int)COL_PULSE)
			_bool b = false;

		if (iColliderIndex == (_int)COL_PLAYER)
		{
			m_bKenaDetected = true;
			m_pKena->Set_MeditationPossible(true);
		}
	}

	return 0;
}

_int CMeditation_Spot::Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (pTarget != nullptr)
	{
		if (iColliderIndex == (_int)COL_PULSE)
			_bool b = false;

		if (iColliderIndex == (_int)COL_PLAYER)
		{
			m_bKenaDetected = false;
			m_pKena->Set_MeditationPossible(false);
		}
	}

	return 0;
}

HRESULT CMeditation_Spot::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	/*나중에  레벨 인덱스 수정해야됌*/
	if (m_EnviromentDesc.iCurLevel == 0)
		m_EnviromentDesc.iCurLevel = LEVEL_MAPTOOL;

	/* For.Com_Model */ 	/*나중에  레벨 인덱스 수정해야됌*/
	if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_Meditation_Stump", TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModelInstance"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMeditation_Spot::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(float)), E_FAIL);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CMeditation_Spot::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(float)), E_FAIL);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CMeditation_Spot::Ready_Effect()
{
#ifdef FOR_MAP_GIMMICK
	m_pMeditationSpotEffect = (CE_P_Meditation_Spot*)CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_P_Meditation", CUtile::Create_DummyString());
	NULL_CHECK_RETURN(m_pMeditationSpotEffect, E_FAIL);
	m_pMeditationSpotEffect->Set_Parent(this);
	m_pMeditationSpotEffect->Late_Initialize(nullptr);
#endif

	return S_OK;
}

CMeditation_Spot* CMeditation_Spot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMeditation_Spot* pInstance = new CMeditation_Spot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMeditation_Spot");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMeditation_Spot::Clone(void* pArg)
{
	CMeditation_Spot* pInstance = new CMeditation_Spot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMeditation_Spot");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMeditation_Spot::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);

	Safe_Release(m_pMeditationSpotEffect);

	int a = 0;
}
