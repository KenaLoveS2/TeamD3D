#include "stdafx.h"
#include "..\public\E_P_ShockFrontEntended.h"
#include "GameInstance.h"

CE_P_ShockFrontEntended::CE_P_ShockFrontEntended(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_ShockFrontEntended::CE_P_ShockFrontEntended(const CE_P_ShockFrontEntended & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_ShockFrontEntended::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_P_ShockFrontEntended::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;
	return S_OK;
}

// CE_Warrior_ShockFrontExtended => 자식먼지()
HRESULT CE_P_ShockFrontEntended::Late_Initialize(void* pArg)
{
#pragma region Nouse
	//_float3	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	//_float3	vPivotScale = _float3(2.57f, 2.37f, 0.45f);
	//_float3	vPivotPos = _float3(0.f, 0.f, 0.f);

	//_smatrix	matPivot = XMMatrixRotationX(XM_PIDIV2);

	//CPhysX_Manager::PX_BOX_DESC PxBoxDesc;
	//ZeroMemory(&PxBoxDesc, sizeof(CPhysX_Manager::PX_BOX_DESC));
	//PxBoxDesc.pActortag = m_szCloneObjectTag;
	//PxBoxDesc.eType = BOX_DYNAMIC;
	//PxBoxDesc.vPos = vPos;
	//PxBoxDesc.vSize = vPivotScale;
	//PxBoxDesc.vRotationAxis = _float3(0.f, 0.f, 0.f);
	//PxBoxDesc.fDegree = 0.f;
	//PxBoxDesc.isGravity = false;
	//PxBoxDesc.eFilterType = PX_FILTER_TYPE::MONSTER_WEAPON;
	//PxBoxDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	//PxBoxDesc.fDensity = 0.2f;
	//PxBoxDesc.fMass = 150.f;
	//PxBoxDesc.fLinearDamping = 10.f;
	//PxBoxDesc.fAngularDamping = 5.f;
	//PxBoxDesc.bCCD = false;
	//PxBoxDesc.fDynamicFriction = 0.5f;
	//PxBoxDesc.fStaticFriction = 0.5f;
	//PxBoxDesc.fRestitution = 0.1f;
	//PxBoxDesc.isTrigger = true;

	//CPhysX_Manager::GetInstance()->Create_Box(PxBoxDesc, Create_PxUserData(m_pParent, false, COL_MONSTER_WEAPON));
	//m_pTransformCom->Add_Collider(m_szCloneObjectTag, matPivot);

// 	_tchar* pDummyTag = CUtile::Create_StringAuto(L"Warrior_FireSwipe_Dummy");
// 	PxBoxDesc.pActortag = pDummyTag;
// 	CPhysX_Manager::GetInstance()->Create_Box(PxBoxDesc, Create_PxUserData(this, false, COLLISON_DUMMY));
// 	m_pTransformCom->Add_Collider(pDummyTag, matPivot);
#pragma endregion Nouse
	m_pVIInstancingBufferCom->Set_RandomPSize(_float2(0.1f, 2.5f));
	m_pVIInstancingBufferCom->Set_RandomSpeeds(0.5f, 1.0f);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	m_pVIInstancingBufferCom->Set_Position(vPos);
	return S_OK;
}

void CE_P_ShockFrontEntended::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
	{
		m_fLife = 0.0f;
		return;
	}

	__super::Tick(fTimeDelta);
	TurnOffSystem(m_fLife, 2.f, fTimeDelta);
}

void CE_P_ShockFrontEntended::Late_Tick(_float fTimeDelta)
{
 	if (m_eEFfectDesc.bActive == false)
 		return;

	if (m_pParent != nullptr)
		Set_Matrix();

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_P_ShockFrontEntended::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CE_P_ShockFrontEntended::Imgui_RenderProperty()
{
	__super::ImGui_PhysXValueProperty();

	if (ImGui::Button("Active"))
	{
		m_eEFfectDesc.bActive = !m_eEFfectDesc.bActive;
		m_pVIInstancingBufferCom->Set_GravityTimeZero();
	}

	if (ImGui::Button("Set_Shape"))
		m_pVIInstancingBufferCom->Set_ShapePosition();

	if (ImGui::Button("Set_Speed"))
		m_pVIInstancingBufferCom->Set_RandomSpeeds(1.0f, 10.0f);

	CVIBuffer_Point_Instancing::POINTDESC* ePoint = m_pVIInstancingBufferCom->Get_PointDesc();
	CVIBuffer_Point_Instancing::INSTANCEDATA* eInstance = m_pVIInstancingBufferCom->Get_InstanceData();

	ImGui::Checkbox("UseGravity", &ePoint->bUseGravity);
	ImGui::InputFloat("fRange", &ePoint->fRange);
	ImGui::InputFloat("fCreateRange", &ePoint->fCreateRange);
}

void CE_P_ShockFrontEntended::Reset()
{
	// m_pVIInstancingBufferCom->Set_ShapePosition();
	m_pVIInstancingBufferCom->Set_GravityTimeZero();
	m_pVIInstancingBufferCom->Set_ResetOriginPos();
}

CE_P_ShockFrontEntended * CE_P_ShockFrontEntended::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_ShockFrontEntended * pInstance = new CE_P_ShockFrontEntended(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_ShockFrontEntended Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_ShockFrontEntended::Clone(void * pArg)
{
	CE_P_ShockFrontEntended * pInstance = new CE_P_ShockFrontEntended(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_ShockFrontEntended Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_ShockFrontEntended::Free()
{
	__super::Free();
}
