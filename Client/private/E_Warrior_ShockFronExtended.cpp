#include "stdafx.h"
#include "..\public\E_Warrior_ShockFronExtended.h"
#include "GameInstance.h"
#include "E_P_Warrior.h"
#include "E_P_ShockFrontEntended.h"

CE_Warrior_ShockFrontExtended::CE_Warrior_ShockFrontExtended(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_Warrior_ShockFrontExtended::CE_Warrior_ShockFrontExtended(const CE_Warrior_ShockFrontExtended & rhs)
	:CEffect_Mesh(rhs)
{
	
}

HRESULT CE_Warrior_ShockFrontExtended::Initialize_Prototype(const _tchar* pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_Warrior_ShockFrontExtended::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 4.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Child(), E_FAIL);

	m_eEFfectDesc.bActive = false;
	return S_OK;
}

HRESULT CE_Warrior_ShockFrontExtended::Late_Initialize(void * pArg)
{	
	_float3	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3	vPivotScale = _float3(4.f, 4.f, 0.15f);
	_float3	vPivotPos = _float3(0.f, 0.f, 0.f);

	_smatrix	matPivot = XMMatrixRotationX(XM_PIDIV2);

	CPhysX_Manager::PX_BOX_DESC PxBoxDesc;
	ZeroMemory(&PxBoxDesc, sizeof(CPhysX_Manager::PX_BOX_DESC));
	PxBoxDesc.pActortag = m_szCloneObjectTag;
	PxBoxDesc.eType = BOX_DYNAMIC;
	PxBoxDesc.vPos = vPos;
	PxBoxDesc.vSize = vPivotScale;
	PxBoxDesc.vRotationAxis = _float3(0.f, 0.f, 0.f);
	PxBoxDesc.fDegree = 0.f;
	PxBoxDesc.isGravity = false;
	PxBoxDesc.eFilterType = PX_FILTER_TYPE::MONSTER_WEAPON;
	PxBoxDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	PxBoxDesc.fDensity = 0.2f;
	PxBoxDesc.fMass = 150.f;
	PxBoxDesc.fLinearDamping = 10.f;
	PxBoxDesc.fAngularDamping = 5.f;
	PxBoxDesc.bCCD = false;
	PxBoxDesc.fDynamicFriction = 0.5f;
	PxBoxDesc.fStaticFriction = 0.5f;
	PxBoxDesc.fRestitution = 0.1f;

	CPhysX_Manager::GetInstance()->Create_Box(PxBoxDesc, Create_PxUserData(m_pParent, false, COL_MONSTER_WEAPON));
	m_pTransformCom->Add_Collider(m_szCloneObjectTag, matPivot);

	// 	_tchar* pDummyTag = CUtile::Create_StringAuto(L"Warrior_FireSwipe_Dummy");
	// 	PxBoxDesc.pActortag = pDummyTag;
	// 	CPhysX_Manager::GetInstance()->Create_Box(PxBoxDesc, Create_PxUserData(this, false, COLLISON_DUMMY));
	// 	m_pTransformCom->Add_Collider(pDummyTag, matPivot);

	return S_OK;
}

void CE_Warrior_ShockFrontExtended::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_eEFfectDesc.bActive == false)
	{
		PxShape* pShape = nullptr;
		m_pTransformCom->Get_ActorList()->back().pActor->getShapes(&pShape, sizeof(PxShape));

		PxBoxGeometry	pGeometry;
		if (pShape->getBoxGeometry(pGeometry))
		{
			pGeometry.halfExtents = PxVec3(0.001f, 0.001f, 0.001f);
			pShape->setGeometry(pGeometry);
		}

		m_pTransformCom->Tick(fTimeDelta);

		return;
	}

	for (auto& pChild : m_vecChild)
		pChild->Set_Active(true);

	m_fTimeDelta += fTimeDelta;
	_float3 vScaled = m_pTransformCom->Get_Scaled();
	
	if (m_fTimeDelta > 1.5f)
	{
		for (auto& pChild : m_vecChild)
			pChild->Set_Active(false);

		m_pTransformCom->Set_Scaled(_float3(0.5f, 0.5f, 0.5f));
		m_vecChild[1]->Get_TransformCom()->Set_Scaled(_float3(0.5f, 0.5f, 0.5f));
		dynamic_cast<CE_P_ShockFrontEntended*>(m_vecChild[0])->Reset();
		m_eEFfectDesc.bActive = false;
		m_fTimeDelta = 0.0f;
	}
	else
	{
		vScaled.x += 2.0f + fTimeDelta;
		vScaled.y += 2.0f + fTimeDelta;
		vScaled.z += 2.0f + fTimeDelta;
		m_pTransformCom->Set_Scaled(vScaled);
		m_vecChild[1]->Get_TransformCom()->Set_Scaled(vScaled * 2.3f);
	}

	PxShape* pShape = nullptr;
	m_pTransformCom->Get_ActorList()->back().pActor->getShapes(&pShape, sizeof(PxShape));

	PxBoxGeometry	pGeometry;
	if (pShape->getBoxGeometry(pGeometry))
	{
		pGeometry.halfExtents = PxVec3(4.f, 4.f, 0.15f);
		pShape->setGeometry(pGeometry);
	}

	m_pTransformCom->Tick(fTimeDelta);
}

void CE_Warrior_ShockFrontExtended::Late_Tick(_float fTimeDelta)
{
   	if (m_eEFfectDesc.bActive == false)
   		return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EFFECT, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}
}

HRESULT CE_Warrior_ShockFrontExtended::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_pModelCom != nullptr)
		m_pModelCom->Render(m_pShaderCom, 0, nullptr, 18);

	return S_OK;
}

void CE_Warrior_ShockFrontExtended::Reset()
{
}

_int CE_Warrior_ShockFrontExtended::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	return 0;
}

void CE_Warrior_ShockFrontExtended::ImGui_PhysXValueProperty()
{
	__super::ImGui_PhysXValueProperty();
}

HRESULT CE_Warrior_ShockFrontExtended::SetUp_TextureComponent()
{

	return S_OK;
}

HRESULT CE_Warrior_ShockFrontExtended::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (g_bDayOrNight)	m_fHDRValue = 1.2f;

	return S_OK;
}

HRESULT CE_Warrior_ShockFrontExtended::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxEffectModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_ShockFront_Extended"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	m_eEFfectDesc.eMeshType = CEffect_Base::tagEffectDesc::MESH_ETC;

	m_eEFfectDesc.fFrame[0] = 38.f;

	m_pTransformCom->Set_Scaled(_float3(0.8f, 1.0f, 0.8f));
	return S_OK;
}

HRESULT CE_Warrior_ShockFrontExtended::SetUp_Child()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEffect_Base* pEffectBase = nullptr;

	/* Dust */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Warrior_ShockFrontEntended_P", L"Shock_Particle"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_vecChild.push_back(pEffectBase);

	/* Plane */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Warrior_ShockFronExtended_Plane", L"Shock_Plane"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_vecChild.push_back(pEffectBase);
	
	for (auto& pChild : m_vecChild)
		pChild->Set_Parent(this);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CE_Warrior_ShockFrontExtended::Imgui_RenderProperty()
{
	__super::ImGui_PhysXValueProperty();

	ImGui::InputFloat4("DiffuseTexture", (_float*)&m_eEFfectDesc.fFrame);
	ImGui::InputFloat("Pass", (_float*)&m_eEFfectDesc.iPassCnt);

	ImGui::Checkbox("Active", &m_eEFfectDesc.bActive);
}

CE_Warrior_ShockFrontExtended * CE_Warrior_ShockFrontExtended::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_Warrior_ShockFrontExtended * pInstance = new CE_Warrior_ShockFrontExtended(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_Warrior_ShockFrontExtended Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_Warrior_ShockFrontExtended::Clone(void * pArg)
{
	CE_Warrior_ShockFrontExtended * pInstance = new CE_Warrior_ShockFrontExtended(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_Warrior_ShockFrontExtended Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_Warrior_ShockFrontExtended::Free()
{
	__super::Free();
}
