#include "stdafx.h"
#include "..\public\E_RotBombExplosion.h"
#include "GameInstance.h"
#include "Effect_Mesh_T.h"
#include "E_P_ExplosionGravity.h"

CE_RotBombExplosion::CE_RotBombExplosion(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_RotBombExplosion::CE_RotBombExplosion(const CE_RotBombExplosion & rhs)
	:CEffect_Mesh(rhs)
{
	
}

HRESULT CE_RotBombExplosion::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_RotBombExplosion::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	/* Component */
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(),
		TEXT("Prototype_Component_Shader_VtxEffectModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	Set_ModelCom(m_eEFfectDesc.eMeshType);
	/* ~Component */

	for (auto& pChild : m_vecChild)
		pChild->Set_Parent(this);

	Set_ShaderOption(m_eEFfectDesc.iPassCnt, 1.0f, _float2(0.0f, 0.0f), false);
	FAILED_CHECK_RETURN(SetUp_Effects(), E_FAIL);

	memcpy(&m_SaveInitWorldMatrix, &m_InitWorldMatrix, sizeof(_float4x4));
	return S_OK;
}

HRESULT CE_RotBombExplosion::Late_Initialize(void * pArg)
{	
	_float3 vPos = _float3(0.f, 0.f, 0.f);
	_float3 vPivotScale = _float3(1.f, 0.f, 1.f);
	_float3 vPivotPos = _float3(0.f, 0.f, 0.f);

	// Capsule X == radius , Y == halfHeight
	CPhysX_Manager::PX_SPHERE_DESC PxSphereDesc;
	PxSphereDesc.eType = SPHERE_DYNAMIC;
	PxSphereDesc.pActortag = m_szCloneObjectTag;
	PxSphereDesc.vPos = vPos;
	PxSphereDesc.fRadius = vPivotScale.x;
	PxSphereDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	PxSphereDesc.fDensity = 1.f;
	PxSphereDesc.fAngularDamping = 0.5f;
	PxSphereDesc.fMass = 59.f;
	PxSphereDesc.fLinearDamping = 1.f;
	PxSphereDesc.bCCD = true;
	PxSphereDesc.eFilterType = PX_FILTER_TYPE::PLAYER_WEAPON;
	PxSphereDesc.fDynamicFriction = 0.5f;
	PxSphereDesc.fStaticFriction = 0.5f;
	PxSphereDesc.fRestitution = 0.1f;

	CPhysX_Manager::GetInstance()->Create_Sphere(PxSphereDesc, Create_PxUserData(this, false, COL_PLAYER_BOMB_EXPLOSION));

	_smatrix	matPivot = XMMatrixTranslation(vPivotPos.x, vPivotPos.y, vPivotPos.z);
	m_pTransformCom->Add_Collider(PxSphereDesc.pActortag, matPivot);

	return S_OK;
}

void CE_RotBombExplosion::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//if (m_bBomb) // 사라짐
	//	TurnoffBomb(fTimeDelta);


	//if (m_pParent == nullptr)
	//{
	//	ToolOption("CE_RotBombExplosion");
	//	m_fTimeDelta += fTimeDelta;

	//	ImGui::Begin("Plus");

	//	if (ImGui::Button("retime"))
	//	{
	//		Reset();
	//		dynamic_cast<CEffect_Mesh_T*>(m_vecChild[CHILD_COVER])->Set_DissolveTime(0.0f);
	//		dynamic_cast<CEffect_Mesh_T*>(m_vecChild[CHILD_COVER])->Set_Dissolve(false);
	//	}

	//	if (ImGui::Button("Dissolve"))
	//		m_bBomb = true;

	//	ImGui::End();

	//	m_vecChild[CHILD_COVER]->ToolOption("CE_RotBombExplosion_cover");
	//	return;
	//}

	if (m_eEFfectDesc.bActive == false)
		return;

	TurnonBomb(fTimeDelta);

	if (m_bBomb)
	{
		m_fDissolveTime += fTimeDelta;
		dynamic_cast<CEffect_Mesh_T*>(m_vecChild[CHILD_COVER])->Set_DissolveTime(m_fDissolveTime);
		if (m_fDissolveTime > 1.f)
			Reset();
	}
}

void CE_RotBombExplosion::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EFFECT, this);
	}
}

HRESULT CE_RotBombExplosion::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_pModelCom != nullptr && m_pShaderCom != nullptr)
		m_pModelCom->Render(m_pShaderCom, 0, nullptr, m_eEFfectDesc.iPassCnt);

	return S_OK;
}

void CE_RotBombExplosion::Reset()
{
	m_pTransformCom->Set_Scaled(_float3(0.9f, 0.9f, 0.9f));
	for (auto& pChild : m_vecChild)
		m_vecChild[CHILD_COVER]->Set_Scale(XMVectorSet(0.91f, 0.91f, 0.91f, 0.f));

	dynamic_cast<CEffect_Mesh_T*>(m_vecChild[CHILD_COVER])->Set_Dissolve(false);
	dynamic_cast<CEffect_Mesh_T*>(m_vecChild[CHILD_COVER])->Set_DissolveTime(0.0f);

	m_eEFfectDesc.bActive = false;
	m_bBomb = false;
	m_bTurnOn = false;
	m_fTimeDelta = 0.f;
	m_fBombTime = 0.f;
	m_fDissolveTime = 0.0f;
}

void CE_RotBombExplosion::TurnonBomb(_float fTimeDelta)
{
	if(m_bTurnOn == false)
	{
		for (auto& pChild : m_vecChild)
			pChild->Set_Active(true);

		_vector vPos = m_pTransformCom->Get_Position();
		dynamic_cast<CE_P_ExplosionGravity*>(m_vecChild[CHILD_P])->UpdateParticle(vPos);
		m_bTurnOn = true;
	}

	m_fTimeDelta += fTimeDelta;
	_float3 vScaled = m_pTransformCom->Get_Scaled();

	if (vScaled.x > 3.f) // 내 스케일이 3보다 커지면 현재 크기 유지
	{
		m_vecChild[CHILD_COVER]->Set_Active(false); // 자식은 끄고 내 크기 유지

		m_fBombTime += fTimeDelta;
		if (m_fBombTime > 2.f)
		{
			m_bBomb = true;	   // Bomb 상태전환
			m_fBombTime = 0.0f;
			dynamic_cast<CEffect_Mesh_T*>(m_vecChild[CHILD_COVER])->Set_Dissolve(true);
		}
	}
	else
	{
		vScaled.x += fTimeDelta * 2.f;
		vScaled.y += fTimeDelta * 2.f;
		vScaled.z += fTimeDelta * 2.f;
		m_pTransformCom->Set_Scaled(vScaled);
		m_vecChild[CHILD_COVER]->Set_AddScale(fTimeDelta * 2.f);
	}
}

void CE_RotBombExplosion::TurnoffBomb(_float fTimeDelta)
{
}

HRESULT CE_RotBombExplosion::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bBomb, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CE_RotBombExplosion::Imgui_RenderProperty()
{
	if (ImGui::Button("Active"))
		m_eEFfectDesc.bActive = !m_eEFfectDesc.bActive;
}

HRESULT CE_RotBombExplosion::SetUp_Effects()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEffect_Base* pEffectBase = nullptr;

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_ExplosionGravity", L"RotBomb_P"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	dynamic_cast<CE_P_ExplosionGravity*>(pEffectBase)->Set_Option(CE_P_ExplosionGravity::TYPE_KENA_ATTACK);
	m_vecChild.push_back(pEffectBase);

	m_vecChild[CHILD_COVER]->Set_Scale(XMVectorSet(0.91f, 0.91f, 0.91f, 1.f));
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CE_RotBombExplosion * CE_RotBombExplosion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_RotBombExplosion * pInstance = new CE_RotBombExplosion(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_RotBombExplosion Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_RotBombExplosion::Clone(void * pArg)
{
	CE_RotBombExplosion * pInstance = new CE_RotBombExplosion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_RotBombExplosion Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_RotBombExplosion::Free()
{
	__super::Free();
}
