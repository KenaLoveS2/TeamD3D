#include "stdafx.h"
#include "..\public\BossRock.h"

CBossRock::CBossRock(ID3D11Device* pDevice, ID3D11DeviceContext* p_context)
	:CGameObject(pDevice, p_context)
	, m_pGameInstance(CGameInstance::GetInstance())
{
}


CBossRock::CBossRock(const CBossRock& rhs)
	:CGameObject(rhs)
	, m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CBossRock::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
		
	return S_OK;
}

HRESULT CBossRock::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC GameObjDesc;
	ZeroMemory(&GameObjDesc, sizeof(CGameObject::GAMEOBJECTDESC));
	GameObjDesc.TransformDesc.fSpeedPerSec = 1.f;
	GameObjDesc.TransformDesc.fRotationPerSec = 1.f;

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjDesc), E_FAIL);

	ZeroMemory(&m_Desc, sizeof(DESC));

	if (pArg) memcpy(&m_Desc, pArg, sizeof(DESC));
	else {
		m_Desc.eType = (ROCK_TPYE)(rand() % ROCK_TPYE_END);
		m_Desc.vPosition = { 0.f, 0.f, 0.f, 1.f };
	}
	
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	iNumMeshes = m_pModelCom->Get_NumMeshes();

	return S_OK;
}

HRESULT CBossRock::Late_Initialize(void * pArg)
{	
	CPhysX_Manager::PX_BOX_DESC PxBoxDesc;
	PxBoxDesc.eType = BOX_DYNAMIC;
	PxBoxDesc.pActortag = m_szCloneObjectTag;
	PxBoxDesc.vPos = { 0.f, 0.f, 0.f };
	PxBoxDesc.vSize = { 0.18f, 0.18f, 0.18f };
	PxBoxDesc.eFilterType = PX_FILTER_TYPE::FITLER_ENVIROMNT;
	PxBoxDesc.fMass = 100.f;
	PxBoxDesc.fLinearDamping = 5.f;
	PxBoxDesc.fAngularDamping = 5.f;

	CPhysX_Manager::GetInstance()->Create_Box(PxBoxDesc, Create_PxUserData(this, true, COLLISON_DUMMY, true));
	m_pTransformCom->Connect_PxActor_Gravity(m_szCloneObjectTag, _float3(0.f, 0.f, 0.f), true);

	m_pTransformCom->Set_Position(m_Desc.vPosition);

	return S_OK;
}

void CBossRock::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);		
}

void CBossRock::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom && m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CBossRock::Render()
{
	if (FAILED(__super::Render())) return E_FAIL;
	if (FAILED(SetUp_ShaderResources())) return E_FAIL;
	
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 0, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 0, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
		
		FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 0), E_FAIL);
	}

	return S_OK;
}

CBossRock* CBossRock::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossRock* pInstance = new CBossRock(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CBossRock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBossRock::Clone(void* pArg)
{
	CBossRock* pInstance = new CBossRock(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBossRock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossRock::Free()
{
	__super::Free();

}

HRESULT CBossRock::SetUp_Components()
{	
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom))) return E_FAIL;	
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom))) return E_FAIL;
		
	_tchar szModelTable[ROCK_TPYE_END][64] = {
		TEXT("Prototype_Component_Model_Dy_RockSmall03"),
		TEXT("Prototype_Component_Model_Dy_RockSmall04"),
		TEXT("Prototype_Component_Model_Dy_RockSmall05"),
		TEXT("Prototype_Component_Model_Dy_RockSmall06"),
	};

	if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, szModelTable[m_Desc.eType], TEXT("Com_Model"), (CComponent**)&m_pModelCom))) return E_FAIL;
		
	return S_OK;
}

HRESULT CBossRock::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom) return E_FAIL;	

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4)))) return E_FAIL;
		
	return S_OK;
}