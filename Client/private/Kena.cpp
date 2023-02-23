#include "stdafx.h"
#include "..\public\Kena.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "Kena_Parts.h"

CKena::CKena(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CKena::CKena(const CKena & rhs)
	: CGameObject(rhs)
{
}

const _double & CKena::Get_AnimationPlayTime()
{
	return m_pModelCom->Get_PlayTime();
}

HRESULT CKena::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CKena::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GaemObjectDesc;
	ZeroMemory(&GaemObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	GaemObjectDesc.TransformDesc.fSpeedPerSec = 7.f;
	GaemObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	FAILED_CHECK_RETURN(__super::Initialize(&GaemObjectDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_State(), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Parts(), E_FAIL);

	m_pModelCom->Set_AnimIndex(0);

	return S_OK;
}

void CKena::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pModelCom->Set_AnimIndex(m_iAnimationIndex);

	for (auto& pPart : m_vecPart)
		pPart->Tick(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CKena::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (CGameInstance::GetInstance()->Key_Down(DIK_UP))
		m_iAnimationIndex++;
	if (CGameInstance::GetInstance()->Key_Down(DIK_DOWN))
		m_iAnimationIndex--;
	/* Delegator Test */
	if (CGameInstance::GetInstance()->Key_Down(DIK_P))
	{
		_int i = 0;
		m_PlayerDelegator.broadcast(i);

	}

	CUtile::Saturate<_int>(m_iAnimationIndex, 0, 35);

	if (m_pRendererCom != nullptr)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	for (auto& pPart : m_vecPart)
		pPart->Late_Tick(fTimeDelta);
}

HRESULT CKena::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_NORMALS, "g_NormalTexture");
		/********************* For. Kena PostProcess By WJ*****************/
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
		//ex
		//if(!sprint)
			m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_EMISSIVE, "g_EmissiveTexture");
		//else
		//	m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_EMISSION_COLOR, "g_EmissiveTexture");
		/******************************************************************/

		if (i == 4)	// Eye Render
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 1);
		else if (i == 0)	// EyeShadow Rneder
			continue;//m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 2);
		//else if (i == 6)
		//	continue;
		else
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices",2);
	}

	return S_OK;
}

void CKena::Imgui_RenderProperty()
{
	
}

void CKena::ImGui_AnimationProperty()
{
	m_pModelCom->Imgui_RenderProperty();
	//m_pStateMachine->Imgui_RenderProperty();
}

void CKena::Update_Child()
{
	for (auto& pPart : m_vecPart)
		pPart->Model_Synchronization();
}

HRESULT CKena::Ready_Parts()
{
	CKena_Parts*	pPart = nullptr;
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CKena_Parts::KENAPARTS_DESC	PartDesc;

	/* Staff */
	ZeroMemory(&PartDesc, sizeof(CKena_Parts::KENAPARTS_DESC));

	PartDesc.pPlayer = this;
	PartDesc.eType = CKena_Parts::KENAPARTS_STAFF;

	pPart = dynamic_cast<CKena_Parts*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Kena_Staff", L"Kena_Staff", &PartDesc));
	NULL_CHECK_RETURN(pPart, E_FAIL);

	m_vecPart.push_back(pPart);
	pGameInstance->Add_AnimObject(LEVEL_GAMEPLAY, pPart);

	/* MainOutfit */
	ZeroMemory(&PartDesc, sizeof(CKena_Parts::KENAPARTS_DESC));

	PartDesc.pPlayer = this;
	PartDesc.eType = CKena_Parts::KENAPARTS_OUTFIT;

	pPart = dynamic_cast<CKena_Parts*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Kena_MainOutfit", L"Kena_MainOutfit", &PartDesc));
	NULL_CHECK_RETURN(pPart, E_FAIL);

	m_vecPart.push_back(pPart);
	pGameInstance->Add_AnimObject(LEVEL_GAMEPLAY, pPart);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CKena::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_GAMEPLAY, L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_GAMEPLAY, L"Prototype_Component_Model_Kena", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	/********************* For. Kena PostProcess By WJ*****************/
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		// AO_R_M
		m_pModelCom->SetUp_Material(i, aiTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_props_AO_R_M.png"));
		// EMISSIVE
		m_pModelCom->SetUp_Material(i, aiTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_props_EMISSIVE.png"));
		// SPRINT_EMISSIVE
		m_pModelCom->SetUp_Material(i, aiTextureType_EMISSION_COLOR, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_props_sprint_EMISSIVE.png"));
	}
	/******************************************************************/

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(10.f, 10.f, 10.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_GAMEPLAY, L"Prototype_Component_Collider_SPHERE", L"Com_RangeCol", (CComponent**)&m_pRangeCol, &ColliderDesc, this), E_FAIL);

	CNavigation::NAVIDESC		NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIDESC));

	NaviDesc.iCurrentIndex = 0;

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_GAMEPLAY, L"Prototype_Component_Navigation", L"Com_Navigation", (CComponent**)&m_pNavigationCom, &NaviDesc, this), E_FAIL);

	return S_OK;
}

HRESULT CKena::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);

	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);

	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);

	return S_OK;
}

HRESULT CKena::SetUp_State()
{
	return S_OK;
}

CKena * CKena::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CKena*	pInstance = new CKena(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CKena");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CKena::Clone(void * pArg)
{
	CKena*	pInstance = new CKena(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CKena");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKena::Free()
{
	__super::Free();

	for (auto& pPart : m_vecPart)
		Safe_Release(pPart);
	m_vecPart.clear();

	Safe_Release(m_pStateMachine);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pRangeCol);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
