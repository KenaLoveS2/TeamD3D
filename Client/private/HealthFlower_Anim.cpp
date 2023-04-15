#include "stdafx.h"
#include "HealthFlower_Anim.h"
#include "GameInstance.h"
#include "Utile.h"
#include "ControlMove.h"
#include "Interaction_Com.h"
#include "Kena.h"
#include "Kena_State.h"
#include "Kena_Status.h"
#include "Camera_Player.h"
#include "E_P_ExplosionGravity.h"
#include "RotForMonster.h"

CHealthFlower_Anim::CHealthFlower_Anim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CHealthFlower_Anim::CHealthFlower_Anim(const CHealthFlower_Anim& rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CHealthFlower_Anim::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHealthFlower_Anim::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	FAILED_CHECK_RETURN(SetUp_Effects(), E_FAIL);

	m_bRenderActive = true;

	const _float4 vPos = _float4(-10.f, 0.f, -10.f, 1.f);
	m_pTransformCom->Set_Position(vPos);

	m_pModelCom->Set_AnimIndex((_uint)CHealthFlower_Anim::OPEN_LOOP);

	return S_OK;
}

HRESULT CHealthFlower_Anim::Late_Initialize(void* pArg)
{
	m_pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena"));
	NULL_CHECK_RETURN(m_pKena, E_FAIL);

	m_pCamera = dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr());
	NULL_CHECK_RETURN(m_pCamera, E_FAIL);

	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	vPos.y += 0.4f;

	_float3 vSize = _float3(0.36f, 0.45f, 0.33f);

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

	pPhysX->Create_Box(BoxDesc, Create_PxUserData(this, false, COL_HEALTHFLOWER));
	pPhysX->Create_Trigger(Create_PxTriggerData(m_szCloneObjectTag, this, TRIGGER_HEALTHFLOWER, vPos, 15.f));

	CRotForMonster** p = m_pKena->Get_RotForMonstrPtr();
	for (_uint i = 0; i < 8; i++)
	{
		m_pBindRots[i] = p[i];
	}

	return S_OK;
}

void CHealthFlower_Anim::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

#ifdef FOR_MAP_GIMMICK
	m_eCurState = Check_State();
	Update_State(fTimeDelta); 
	
	m_pTransformCom->Tick(fTimeDelta);
	if (m_pExplosionGravity)	m_pExplosionGravity->Tick(fTimeDelta);
#endif

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CHealthFlower_Anim::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
#ifdef FOR_MAP_GIMMICK
	if (m_pExplosionGravity)	m_pExplosionGravity->Late_Tick(fTimeDelta);

	if (m_ePreState != m_eCurState)
		m_ePreState = m_eCurState;
#endif

	/*Culling*/
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 vCamPos = CGameInstance::GetInstance()->Get_CamPosition();
	_vector camPos = XMLoadFloat4(&vCamPos);
	const _vector	 vDir = camPos - vPos;

	_float f = XMVectorGetX(XMVector4Length(vDir));
	if (100.f <= XMVectorGetX(XMVector4Length(vDir)))
		m_bRenderCheck = false;
	if (m_bRenderCheck == true)
		m_bRenderCheck = CGameInstance::GetInstance()->isInFrustum_WorldSpace(vPos, 100.f);

	if (m_pRendererCom && m_bRenderActive &&m_bRenderCheck ==false)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CHealthFlower_Anim::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (i == 2 || i==3) 
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 33),E_FAIL);
		}
		else if (i == 4)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 14), E_FAIL);
		}
		else
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 0),E_FAIL);
		}
	}
	return S_OK;
}

void CHealthFlower_Anim::ImGui_AnimationProperty()
{
	if (ImGui::Button("Reset Anim"))
	{
		m_bUsed = false;
		m_eCurState = OPEN_LOOP;
	}

	m_pTransformCom->Imgui_RenderProperty_ForJH();
	m_pModelCom->Imgui_RenderProperty();
}

void CHealthFlower_Anim::ImGui_PhysXValueProperty()
{
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

HRESULT CHealthFlower_Anim::Add_AdditionalComponent(_uint iLevelIndex, const _tchar* pComTag, COMPONENTS_OPTION eComponentOption)
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

_int CHealthFlower_Anim::Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (iColliderIndex == (_uint)COL_PLAYER)
	{
		m_bKenaDetected = true;
		m_pKena->Set_RotActionPossible(true);
	}

	return 0;
}

_int CHealthFlower_Anim::Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (iColliderIndex == (_uint)COL_PLAYER)
	{
		m_bKenaDetected = false;
		m_pKena->Set_RotActionPossible(false);
	}

	return 0;
}

HRESULT CHealthFlower_Anim::SetUp_Effects()
{
#ifdef FOR_MAP_GIMMICK

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	/// <ExplosionGravity / Particle>
	m_pExplosionGravity = dynamic_cast<CE_P_ExplosionGravity*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_ExplosionGravity", L"HealthFlower_P"));
	NULL_CHECK_RETURN(m_pExplosionGravity, E_FAIL);
	m_pExplosionGravity->Set_Parent(this);
	m_pExplosionGravity->Set_Option(CE_P_ExplosionGravity::TYPE::TYPE_HEALTHFLOWER);
	/// <ExplosionGravity / Particle>

#endif
	return S_OK;
}

CHealthFlower_Anim::ANIMATION CHealthFlower_Anim::Check_State()
{
	ANIMATION	eState = m_ePreState;
	const _uint	iKenaState = m_pKena->Get_AnimationStateIndex();

	switch (eState)
	{
	case CHealthFlower_Anim::OPEN_LOOP:
		{	
			if (m_bUsed == false && m_bInteractable == true)
			{
				if (iKenaState == (_uint)CKena_State::ROT_ACTION || iKenaState == (_uint)CKena_State::ROT_ACTION_AIM_STATE)
				{
					m_bUsed = true;
					eState = CHealthFlower_Anim::BIND;
					m_pModelCom->Set_AnimIndex((_uint)eState);

					Start_Bind();
				}
			}

			break;
		}
	case CHealthFlower_Anim::BIND:
		{	
			if (m_pModelCom->Get_AnimationFinish())
			{
				eState = CHealthFlower_Anim::OPEN;
				m_pModelCom->Set_AnimIndex((_uint)eState);

				_int	iMaxHP = m_pKena->Get_Status()->Get_MaxHP();
				_int	iHalfAmount = _int(floor(iMaxHP / 2.f));
				m_pKena->Get_Status()->Add_HealAmount(iHalfAmount);

				/* PARTICLE */
				m_pExplosionGravity->UpdateParticle(m_pTransformCom->Get_Position());

				CUI_ClientManager::UI_PRESENT tag = CUI_ClientManager::TOP_MOOD_HEAL;
				_float fDefault = 1.f; 
				m_pKena->m_Delegator.broadcast(tag, fDefault);

				End_Bind();
			}

			break;
		}
	case CHealthFlower_Anim::OPEN:
		{
			if (m_pModelCom->Get_AnimationFinish() == true)
			{
				eState = CHealthFlower_Anim::OPEN_LOOP;
				m_pModelCom->Set_AnimIndex((_uint)eState);				
			}

			break;
		}
	}

	return eState;
}

void CHealthFlower_Anim::Update_State(_float fTimeDelta)
{
	switch (m_eCurState)
	{
	case CHealthFlower_Anim::OPEN_LOOP:
		{
			if (m_bKenaDetected == true && m_bUsed == false)
			{
				m_bInteractable = Check_CameraRay();
				m_pKena->Set_RotActionPossible(m_bInteractable);
			}

			break;
		}
	case CHealthFlower_Anim::BIND:
		{			
			m_fDissolveTime = m_pModelCom->Get_AnimationProgress();
			CUtile::Saturate<_float>(m_fDissolveTime, 0.f, 1.f);
			break;
		}
	case CHealthFlower_Anim::OPEN:
		{
			break;
		}
	}
}

_bool CHealthFlower_Anim::Check_CameraRay()
{
	_float3		vCamPos = CGameInstance::GetInstance()->Get_CamPosition_Float3();
	_float3		vCamLook = XMVector3Normalize(CGameInstance::GetInstance()->Get_CamLook_Float3());

	if (CPhysX_Manager::GetInstance()->Raycast_CollisionTarget(vCamPos, vCamLook, 15.f, nullptr, this))
	{
		m_pKena->Call_FocusRotIconFlower(this);
		return true;
	}

	return false;
}

HRESULT CHealthFlower_Anim::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	/*나중에  레벨 인덱스 수정해야됌*/
	if (m_EnviromentDesc.iCurLevel == 0)
		m_EnviromentDesc.iCurLevel = g_LEVEL;

	/* For.Com_Model */ 	/*나중에  레벨 인덱스 수정해야됌*/
	if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Model_HealthFlower"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom, nullptr, this)))
		return E_FAIL;

	m_pModelCom->SetUp_Material(2, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/HealthFlower/T_HealthFlower_E.png"));
	m_pModelCom->SetUp_Material(3, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/HealthFlower/T_HealthFlower_E.png"));

	/* For.Com_Shader */
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	/* For.DissolveTex */
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Texture_Dissolve", L"COM_DISSOLVETEX", (CComponent**)&m_pDissolveTexture), E_FAIL);

	return S_OK;
}

HRESULT CHealthFlower_Anim::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bUsed, sizeof(_bool)), E_FAIL);

	m_bUsed&& Bind_Dissolve(m_pShaderCom);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CHealthFlower_Anim::Bind_Dissolve(CShader* pShader)
{
	FAILED_CHECK_RETURN(pShader->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float)), E_FAIL);
	FAILED_CHECK_RETURN(m_pDissolveTexture->Bind_ShaderResource(pShader, "g_DissolveTexture"), E_FAIL);

	return S_OK;
}

CHealthFlower_Anim* CHealthFlower_Anim::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHealthFlower_Anim* pInstance = new CHealthFlower_Anim(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHealthFlower_Anim");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHealthFlower_Anim::Clone(void* pArg)
{
	CHealthFlower_Anim* pInstance = new CHealthFlower_Anim(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHealthFlower_Anim");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHealthFlower_Anim::Free()
{
	__super::Free();

	Safe_Release(m_pDissolveTexture);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);
	Safe_Release(m_pExplosionGravity);
}


void CHealthFlower_Anim::Start_Bind()
{
	for (_uint i = 0; i < 8; ++i)
	{
		m_pBindRots[i] ? m_pBindRots[i]->Start_BindFlower(this) : 0;
	}
}

void CHealthFlower_Anim::End_Bind()
{	
	for (_uint i = 0; i < 8; ++i)
	{
		m_pBindRots[i] ? m_pBindRots[i]->End_BindFlower() : 0;
	}
	
	ZeroMemory(&m_pBindRots, sizeof(m_pBindRots));
}