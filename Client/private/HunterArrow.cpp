#include "stdafx.h"
#include "..\public\HunterArrow.h"
#include "BossHunter.h"

CHunterArrow::CHunterArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonsterWeapon(pDevice, pContext)
{
}

CHunterArrow::CHunterArrow(const CHunterArrow& rhs)
	: CMonsterWeapon(rhs)
{	
}

HRESULT CHunterArrow::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHunterArrow::Initialize(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_WeaponDesc, pArg, sizeof(m_WeaponDesc));

	CGameObject::GAMEOBJECTDESC GameObjDesc;
	ZeroMemory(&GameObjDesc, sizeof(CGameObject::GAMEOBJECTDESC));
	GameObjDesc.TransformDesc.fSpeedPerSec = 17.f;
	
	if (FAILED(__super::Initialize(&GameObjDesc)))
		return E_FAIL;

	m_iNumMeshes = m_pModelCom->Get_NumMeshes();
	m_pTransformCom->Set_Position(m_vInvisiblePos);

	return S_OK;
}

HRESULT CHunterArrow::Late_Initialize(void* pArg)
{
	CPhysX_Manager* pPhysX = CPhysX_Manager::GetInstance();

	{
		CPhysX_Manager::PX_SPHERE_DESC PxSphereDesc;
		PxSphereDesc.eType = SPHERE_DYNAMIC;
		PxSphereDesc.pActortag = m_szCloneObjectTag;
		PxSphereDesc.fRadius = 0.1f;
		PxSphereDesc.isGravity = false;
		PxSphereDesc.bCCD = false;
		PxSphereDesc.eFilterType = PX_FILTER_TYPE::MONSTER_WEAPON;

		PxSphereDesc.fStaticFriction = 0.5f;
		PxSphereDesc.fDynamicFriction = 0.5f;
		PxSphereDesc.fRestitution = 0.1f;
		PxSphereDesc.fDensity = 0.1f;
		PxSphereDesc.fAngularDamping = 0.1f;
		PxSphereDesc.fMass = 1.f;
		PxSphereDesc.fLinearDamping = 0.1f;

		pPhysX->Create_Sphere(PxSphereDesc, Create_PxUserData(m_WeaponDesc.pOwnerMonster, false, COL_MONSTER_WEAPON));
		
		XMStoreFloat4x4(&m_ColliderPivotMatrix, XMMatrixTranslation(m_vColliderPivotPos.x, m_vColliderPivotPos.y, m_vColliderPivotPos.z));		
		m_pTransformCom->Add_Collider(PxSphereDesc.pActortag, m_ColliderPivotMatrix);
	}

	{
		CPhysX_Manager::PX_SPHERE_DESC PxSphereDesc;
		PxSphereDesc.eType = SPHERE_DYNAMIC;
		PxSphereDesc.pActortag = CUtile::Create_DummyString();
		PxSphereDesc.fRadius = 0.2f;
		PxSphereDesc.isGravity = false;
		PxSphereDesc.bCCD = false;
		PxSphereDesc.eFilterType = PX_FILTER_TYPE::MONSTER_WEAPON;

		PxSphereDesc.fStaticFriction = 0.5f;
		PxSphereDesc.fDynamicFriction = 0.5f;
		PxSphereDesc.fRestitution = 0.1f;
		PxSphereDesc.fDensity = 0.1f;
		PxSphereDesc.fAngularDamping = 0.1f;
		PxSphereDesc.fMass = 1.f;
		PxSphereDesc.fLinearDamping = 0.1f;

		pPhysX->Create_Sphere(PxSphereDesc, Create_PxUserData(this, false, COLLISON_DUMMY));
		m_pTransformCom->Add_Collider(PxSphereDesc.pActortag, m_ColliderPivotMatrix);
	}

	return S_OK;
}

void CHunterArrow::Tick(_float fTimeDelta)
{
	if (m_eArrowState == STATE_END) return;

	__super::Tick(fTimeDelta);
		
	// CHunterArrow::Imgui_RenderProperty();

	ArrowProc(fTimeDelta);
	Update_Collider(fTimeDelta);
}

void CHunterArrow::Late_Tick(_float fTimeDelta)
{	
	if (m_eArrowState == STATE_END) return;

	__super::Late_Tick(fTimeDelta);
	
	m_pRendererCom && m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CHunterArrow::Render()
{
	if (FAILED(__super::Render())) return E_FAIL;
	if (FAILED(SetUp_ShaderResources())) return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Render(m_pShaderCom, i, nullptr, 0);
	}

	return S_OK;
}

HRESULT CHunterArrow::RenderShadow()
{
	if (FAILED(__super::RenderShadow())) return E_FAIL;
	if (FAILED(SetUp_ShadowShaderResources())) return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, nullptr, 2);

	return S_OK;
}

void CHunterArrow::Imgui_RenderProperty()
{
	CMonsterWeapon::Imgui_RenderProperty();

	ImGui::Begin("Monster_Weapon");
	_float3 vPivot = m_vHandPivotPos;
	float fPos[3] = { vPivot.x, vPivot.y, vPivot.z };
	ImGui::DragFloat3("Hand PivotPos", fPos, 0.01f, -100.f, 100.0f);
	m_vHandPivotPos.x = fPos[0];
	m_vHandPivotPos.y = fPos[1];
	m_vHandPivotPos.z = fPos[2];
	
	static char szTable[FIRE_TYPE_END][32] = {
		"Bow PivotPos - CHARGE",
		"Bow PivotPos - RAPID",
		"Bow PivotPos - SHOCK",
		"Bow PivotPos - SINGLE",
	};

	for (_uint i = 0; i < FIRE_TYPE_END; i++)
	{
		_float3 vPivot = m_vBowPivotPos[i];
		float fPos[3] = { vPivot.x, vPivot.y, vPivot.z };
		ImGui::DragFloat3(szTable[i], fPos, 0.01f, -100.f, 100.0f);
		m_vBowPivotPos[i].x = fPos[0];
		m_vBowPivotPos[i].y = fPos[1];
		m_vBowPivotPos[i].z = fPos[2];
	}

	_float3 vColPivot = m_vColliderPivotPos;
	float fColPos[3] = { vColPivot.x, vColPivot.y, vColPivot.z };
	ImGui::DragFloat3("Collider", fColPos, 0.01f, -100.f, 100.0f);
	m_vColliderPivotPos.x = fColPos[0];
	m_vColliderPivotPos.y = fColPos[1];
	m_vColliderPivotPos.z = fColPos[2];

	ImGui::End();
}

void CHunterArrow::ImGui_ShaderValueProperty()
{
	CMonsterWeapon::ImGui_ShaderValueProperty();
}

void CHunterArrow::ImGui_PhysXValueProperty()
{
	CMonsterWeapon::ImGui_PhysXValueProperty();
}

void CHunterArrow::Update_Collider(_float fTimeDelta)
{
	m_pTransformCom->Tick(fTimeDelta);
}

HRESULT CHunterArrow::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Boss_Hunter_Arrow"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_pModelCom->SetUp_Material(0, aiTextureType::WJTextureType_DIFFUSE, TEXT("../Bin/Resources/NonAnim/Boss_HunterArrow/Noise_cloudsmed_Normal.png"));

	return S_OK;
}

HRESULT CHunterArrow::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom) return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) return E_FAIL;
	
	/*_float4x4 SocketWorldMatrix;
	_float4x4 OriginWorld = m_pTransformCom->Get_WorldMatrixFloat4x4();
	XMStoreFloat4x4(&SocketWorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&m_SocketMatrix));
	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &SocketWorldMatrix))) return E_FAIL;*/

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_SocketMatrix", &m_SocketMatrix))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4)))) return E_FAIL;
		
	return S_OK;
}

HRESULT CHunterArrow::SetUp_ShadowShaderResources()
{
	if (nullptr == m_pShaderCom) return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_SocketMatrix", &m_SocketMatrix))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4)))) return E_FAIL;

	return S_OK;
}

CHunterArrow* CHunterArrow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHunterArrow* pInstance = new CHunterArrow(pDevice, pContext);
	
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHunterArrow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHunterArrow::Clone(void* pArg)
{
	CHunterArrow* pInstance = new CHunterArrow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHunterArrow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHunterArrow::Free()
{
	__super::Free();
}

void CHunterArrow::ArrowProc(_float fTimeDelta)
{
	switch (m_eArrowState)
	{
	case REDAY:
	{
		_matrix HandSocketMatrix = m_WeaponDesc.pSocket->Get_OffsetMatrix() * m_WeaponDesc.pSocket->Get_CombindMatrix() * XMLoadFloat4x4(&m_WeaponDesc.PivotMatrix);
		HandSocketMatrix = XMMatrixTranslation(m_vHandPivotPos.x, m_vHandPivotPos.y, m_vHandPivotPos.z) * HandSocketMatrix * m_WeaponDesc.pTargetTransform->Get_WorldMatrix();
		m_pTransformCom->Set_Position(HandSocketMatrix.r[3]);
		
		_matrix BowSocketMatrix = m_pBowBone->Get_OffsetMatrix() * m_pBowBone->Get_CombindMatrix() * XMLoadFloat4x4(&m_WeaponDesc.PivotMatrix);
		BowSocketMatrix = XMMatrixTranslation(m_vBowPivotPos[m_eFireType].x, m_vBowPivotPos[m_eFireType].y, m_vBowPivotPos[m_eFireType].z) * BowSocketMatrix * m_WeaponDesc.pTargetTransform->Get_WorldMatrix();
		m_pTransformCom->LookAt(BowSocketMatrix.r[3]);
	}	
	case FIRE:
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
		break;
	}	
	case FINISH:
	{
		// 이펙트 처리

		// 이펙트 처리 완료 후
		m_eArrowState = STATE_END;
		m_pTransformCom->Set_Position(m_vInvisiblePos);
		break;
	}	
	case STATE_END:
	{	
		// 대기 상태다
		break;
	}
	}
}

void CHunterArrow::Execute_Ready(FIRE_TYPE eFireType)
{
	m_eFireType = eFireType;
	m_eArrowState = REDAY;
}

void CHunterArrow::Execute_Fire()
{
	m_eArrowState = FIRE;
}

void CHunterArrow::Execute_Finish()
{
	m_eArrowState = FINISH;
}

_int CHunterArrow::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	if (m_eArrowState == FIRE)
	{
		m_eArrowState = FINISH;
	}

	return 0;
}