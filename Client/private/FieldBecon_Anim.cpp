#include "stdafx.h"
#include "FieldBecon_Anim.h"
#include "GameInstance.h"
#include "ControlMove.h"
#include "Interaction_Com.h"

CFieldBecon_Anim::CFieldBecon_Anim(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CFieldBecon_Anim::CFieldBecon_Anim(const CFieldBecon_Anim & rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CFieldBecon_Anim::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFieldBecon_Anim::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_bRenderActive = true;

	CGameInstance::GetInstance()->Add_AnimObject(g_LEVEL, this);

	m_pModelCom->Set_AnimIndex(0);
	return S_OK;
}

HRESULT CFieldBecon_Anim::Late_Initialize(void * pArg)
{

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	CPhysX_Manager *pPhysX = CPhysX_Manager::GetInstance();

	CPhysX_Manager::PX_BOX_DESC BoxDesc;
	BoxDesc.pActortag = m_szCloneObjectTag;
	BoxDesc.eType = BOX_DYNAMIC;
	BoxDesc.vPos = CUtile::Float_4to3(vPos);
	BoxDesc.vSize = _float3(1.4f, 7.61f, 1.92f);
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

	return S_OK;
}

void CFieldBecon_Anim::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	/*Culling*/
	//_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	//_float4 vCamPos = CGameInstance::GetInstance()->Get_CamPosition();
	//_vector camPos = XMLoadFloat4(&vCamPos);
	//const _vector	 vDir = camPos - vPos;
	//
	//_float f = XMVectorGetX(XMVector4Length(vDir));
	//if (100.f <= XMVectorGetX(XMVector4Length(vDir)))
	//	m_bRenderCheck = false;
	//if(m_bRenderCheck ==true)
	//	m_bRenderCheck = CGameInstance::GetInstance()->isInFrustum_WorldSpace(vPos, 15.f);
	//if (m_bRenderCheck)
	/*~Culling*/
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CFieldBecon_Anim::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	if (m_pRendererCom)//&& m_bRenderActive && m_bRenderCheck)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CFieldBecon_Anim::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}

	return S_OK;
}

void CFieldBecon_Anim::ImGui_AnimationProperty()
{
	m_pModelCom->Imgui_RenderProperty();

}

void CFieldBecon_Anim::ImGui_PhysXValueProperty()
{
	__super::ImGui_PhysXValueProperty();

	_float3 vPxPivotScale = m_pTransformCom->Get_vPxPivotScale();
	float fScale[3] = { vPxPivotScale.x, vPxPivotScale.y, vPxPivotScale.z };
	ImGui::DragFloat3("PxScale", fScale, 0.01f, 0.1f, 100.0f);
	vPxPivotScale.x = fScale[0]; vPxPivotScale.y = fScale[1]; vPxPivotScale.z = fScale[2];
	CPhysX_Manager::GetInstance()->Set_ActorScaling(m_szCloneObjectTag, vPxPivotScale);
	m_pTransformCom->Set_PxPivotScale(vPxPivotScale);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	static _float3 FieldBecon = _float3(0.f, 0.f, 0.f);
	float fPos[3] = { FieldBecon.x, FieldBecon.y, FieldBecon.z };
	ImGui::DragFloat3("PxPivotPos", fPos, 0.01f, -100.f, 100.0f);
	FieldBecon.x = fPos[0]; FieldBecon.y = fPos[1]; FieldBecon.z = fPos[2];

	_float3 Temp;
	XMStoreFloat3(&Temp,  XMVectorSetW(XMLoadFloat3(&FieldBecon), 1.f));

	CPhysX_Manager::GetInstance()->Set_ActorPosition(
		m_szCloneObjectTag, Temp);

}

HRESULT CFieldBecon_Anim::Add_AdditionalComponent(_uint iLevelIndex, const _tchar * pComTag, COMPONENTS_OPTION eComponentOption)
{
	return S_OK;
}

HRESULT CFieldBecon_Anim::SetUp_Components()
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
	if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Model_FieldBeaconAnim"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom,nullptr,this)))
		return E_FAIL;

	/* For.Com_Shader */
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(),
		L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);
	 

	return S_OK;
}

HRESULT CFieldBecon_Anim::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CFieldBecon_Anim * CFieldBecon_Anim::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFieldBecon_Anim*		pInstance = new CFieldBecon_Anim(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFieldBecon_Anim");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFieldBecon_Anim::Clone(void * pArg)
{
	CFieldBecon_Anim*		pInstance = new CFieldBecon_Anim(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFieldBecon_Anim");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFieldBecon_Anim::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);
}
