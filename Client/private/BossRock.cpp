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
	ZeroMemory(&m_Desc, sizeof(DESC));
	if (pArg) memcpy(&m_Desc, pArg, sizeof(DESC));
	else {
		m_Desc.eType = (ROCK_TPYE)(rand() % ROCK_TPYE_END);
		m_Desc.vPosition = { 0.f, 0.f, 0.f, 1.f };
		m_Desc.fUpTime = 1.f;
		m_Desc.fSpeedY = 1.f;
	}

	CGameObject::GAMEOBJECTDESC GameObjDesc;
	ZeroMemory(&GameObjDesc, sizeof(CGameObject::GAMEOBJECTDESC));
	GameObjDesc.TransformDesc.fSpeedPerSec = m_Desc.fSpeedY;
	GameObjDesc.TransformDesc.fRotationPerSec = 1.f;

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	return S_OK;
}

HRESULT CBossRock::Late_Initialize(void * pArg)
{	
	FAILED_CHECK_RETURN(__super::Late_Initialize(pArg), E_FAIL);

	_float fRandScale = CUtile::Get_RandomFloat(0.1f, 0.5f);
	m_pTransformCom->Set_Scaled(_float3(fRandScale, fRandScale, fRandScale));

	CPhysX_Manager::PX_BOX_DESC PxBoxDesc;
	PxBoxDesc.eType = BOX_DYNAMIC;
	PxBoxDesc.pActortag = m_szCloneObjectTag;
	PxBoxDesc.vPos = { 0.f, 0.f, 0.f };
	PxBoxDesc.vSize = { 0.2f * fRandScale, 0.2f * fRandScale, 0.2f * fRandScale };
	PxBoxDesc.eFilterType = PX_FILTER_TYPE::FITLER_ENVIROMNT;
	PxBoxDesc.fMass = 30000.f;
	PxBoxDesc.fLinearDamping = 2.1f;
	PxBoxDesc.fAngularDamping = 2.1f;
	PxBoxDesc.fRestitution = 1.f;

	CPhysX_Manager::GetInstance()->Create_Box(PxBoxDesc, Create_PxUserData(this, true, COLLISON_DUMMY, true));
	m_pTransformCom->Connect_PxActor_Gravity(m_szCloneObjectTag, _float3(0.f, 0.f, 0.f), true);

	m_pTransformCom->Set_Position(m_Desc.vPosition);
	
	return S_OK;
}

void CBossRock::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	BossRockProc(fTimeDelta);
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
	
	for (_uint i = 0; i < m_iNumMeshes; ++i)
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

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
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

void CBossRock::BossRockProc(_float fTimeDelta)
{
	switch (m_eState)
	{
	case UP:
	{
		m_pTransformCom->Go_AxisY(fTimeDelta);
		m_fUpTimeCheck += fTimeDelta;
		m_eState = m_fUpTimeCheck >= m_Desc.fUpTime ? STATE_END : m_eState;

		break;
	}	
	case DOWN:
	{
		break;
	}	
	}
}

void CBossRock::Exectue_Up()
{
	m_fUpTimeCheck = 0.f;
	m_eState = UP;
}