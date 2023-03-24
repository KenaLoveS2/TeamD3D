#include "stdafx.h"
#include "TelePort_Flower.h"
#include "GameInstance.h"
#include "ControlMove.h"
#include "Interaction_Com.h"

CTelePort_Flower::CTelePort_Flower(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CTelePort_Flower::CTelePort_Flower(const CTelePort_Flower & rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CTelePort_Flower::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CTelePort_Flower::Initialize(void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	m_bRenderActive = true;

	m_pModelCom->Set_AnimIndex(0);

	return S_OK;
}

void CTelePort_Flower::Tick(_float fTimeDelta)
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

void CTelePort_Flower::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	if (m_pRendererCom )//&& m_bRenderActive && m_bRenderCheck)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CTelePort_Flower::Render()
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

HRESULT CTelePort_Flower::Add_AdditionalComponent(_uint iLevelIndex, const _tchar * pComTag, COMPONENTS_OPTION eComponentOption)
{
	return S_OK;
}

HRESULT CTelePort_Flower::SetUp_Components()
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
	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_TeleportFlowerAnim"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(),
		L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	return S_OK;
}

HRESULT CTelePort_Flower::SetUp_ShaderResources()
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

CTelePort_Flower * CTelePort_Flower::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTelePort_Flower*		pInstance = new CTelePort_Flower(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTelePort_Flower");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTelePort_Flower::Clone(void * pArg)
{
	CTelePort_Flower*		pInstance = new CTelePort_Flower(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTelePort_Flower");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTelePort_Flower::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);
}
