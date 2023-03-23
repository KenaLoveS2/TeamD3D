#include "stdafx.h"
#include "BranchTosser_Tree.h"
#include "GameInstance.h"
#include "BranchTosser.h"

CBranchTosser_Tree::CBranchTosser_Tree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CBranchTosser_Tree::CBranchTosser_Tree(const CBranchTosser_Tree& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBranchTosser_Tree::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype())) return E_FAIL;

	return S_OK;
}

HRESULT CBranchTosser_Tree::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg))) return E_FAIL;		
	if (FAILED(SetUp_Components())) return E_FAIL;
		
	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	return S_OK;
}

HRESULT CBranchTosser_Tree::Late_Initialize(void* pArg)
{
	{
		CPhysX_Manager *pPhysX = CPhysX_Manager::GetInstance();
		
		CPhysX_Manager::PX_BOX_DESC PxBoxDesc;
		PxBoxDesc.eType = BOX_STATIC;
		PxBoxDesc.pActortag = m_szCloneObjectTag;
		PxBoxDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		PxBoxDesc.vSize = { 0.5f, 3.f, 0.5f };
		PxBoxDesc.vVelocity = _float3(0.f, -1.f, 0.f);
		PxBoxDesc.fDensity = 1.f;
		PxBoxDesc.fAngularDamping = 0.5f;
		PxBoxDesc.fMass = 10.f;
		PxBoxDesc.fLinearDamping = 1.f;
		PxBoxDesc.fDynamicFriction = 0.5f;
		PxBoxDesc.fStaticFriction = 0.5f;
		PxBoxDesc.fRestitution = 0.1f;
		PxBoxDesc.eFilterType = PX_FILTER_TYPE::FILTER_DEFULAT;
		
		pPhysX->Create_Box(PxBoxDesc, Create_PxUserData(this, false, COL_ENVIROMENT));
	}


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STATIC_SHADOW, this);
	return S_OK;
}

void CBranchTosser_Tree::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBranchTosser_Tree::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom && m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CBranchTosser_Tree::Render()
{
	if (FAILED(__super::Render())) return E_FAIL;
	if (FAILED(SetUp_ShaderResources())) return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Render(m_pShaderCom, i, nullptr, m_iShaderPass);
	}

	return S_OK;
}

HRESULT CBranchTosser_Tree::RenderShadow()
{
	if (FAILED(__super::RenderShadow())) return E_FAIL;
	if (FAILED(SetUp_ShadowShaderResources())) return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, nullptr, 2);

	return S_OK;
}

HRESULT CBranchTosser_Tree::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_MY_BranchTosser_Tree"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Dissolve_Texture */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_Dissolve_Texture"),
		(CComponent**)&m_pDissolveTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBranchTosser_Tree::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom) return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)))) return E_FAIL;	

	_bool bDissolve = m_iShaderPass == 5;
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &bDissolve, sizeof(_bool)), E_FAIL);
	if (bDissolve)
	{
		_float fDissolveTime = m_pBranchTosser->Get_DissolveTime();
		if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveTime", &fDissolveTime, sizeof(_float)))) return E_FAIL;
		if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture"))) return E_FAIL;
	}

	return S_OK;
}

HRESULT CBranchTosser_Tree::SetUp_ShadowShaderResources()
{
	if (nullptr == m_pShaderCom) return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(_float)))) return E_FAIL;

	_bool bDissolve = m_iShaderPass == 5;
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &bDissolve, sizeof(_bool)), E_FAIL);
	if (bDissolve)
	{
		_float fDissolveTime = m_pBranchTosser->Get_DissolveTime();
		if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveTime", &fDissolveTime, sizeof(_float)))) return E_FAIL;
		if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture"))) return E_FAIL;
	}

	return S_OK;
}

CBranchTosser_Tree* CBranchTosser_Tree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBranchTosser_Tree*		pInstance = new CBranchTosser_Tree(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBranchTosser_Tree");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBranchTosser_Tree::Clone(void* pArg)
{
	CBranchTosser_Tree*		pInstance = new CBranchTosser_Tree(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBranchTosser_Tree");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBranchTosser_Tree::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pDissolveTextureCom);
}

void CBranchTosser_Tree::Clear()
{	
	CPhysX_Manager* pPhysX = CPhysX_Manager::GetInstance();
	
	PxRigidActor* pActor = pPhysX->Find_StaticActor(m_szCloneObjectTag);
	pPhysX->Delete_Actor(pActor);
}