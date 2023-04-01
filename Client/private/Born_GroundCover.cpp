#include "stdafx.h"
#include "Born_GroundCover.h"
#include "GameInstance.h"
#include "ControlMove.h"
#include "Interaction_Com.h"

CBorn_GroundCover::CBorn_GroundCover(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CBorn_GroundCover::CBorn_GroundCover(const CBorn_GroundCover & rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CBorn_GroundCover::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBorn_GroundCover::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_bRenderActive = true;

	m_pModelCom->Set_AnimIndex(0);
	return S_OK;
}

HRESULT CBorn_GroundCover::Late_Initialize(void* pArg)
{
	m_pModelCom->Instaincing_GimmkicInit(Gimmick_TYPE_GO_UP);

	return S_OK;
}

void CBorn_GroundCover::Tick(_float fTimeDelta)
{
	if(!m_bOnceTest)
	{
		Late_Initialize();
		m_bOnceTest = true;
	}

	__super::Tick(fTimeDelta);

	Culling();

	//if(ImGui::Button("Uprise"))
	//{
		//m_pModelCom->Instaincing_MoveControl()
	//}

	if(m_bRenderCheck)
		m_pModelCom->Play_Animation(fTimeDelta);
}

void CBorn_GroundCover::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	
	if (m_pRendererCom && m_bRenderActive && m_bRenderCheck)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CBorn_GroundCover::Render()
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

HRESULT CBorn_GroundCover::Add_AdditionalComponent(_uint iLevelIndex, const _tchar * pComTag, COMPONENTS_OPTION eComponentOption)
{
	__super::Add_AdditionalComponent(iLevelIndex, pComTag, eComponentOption);

	/* For.Com_CtrlMove */
	if (eComponentOption == COMPONENTS_CONTROL_MOVE)
	{
		if (FAILED(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_ControlMove"), pComTag,
			(CComponent**)&m_pControlMoveCom)))
			return E_FAIL;
	}
	/* For.Com_Interaction */
	else if (eComponentOption == COMPONENTS_INTERACTION)
	{
		if (FAILED(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_Interaction_Com"), pComTag,
			(CComponent**)&m_pInteractionCom)))
			return E_FAIL;
	}
	else
		return S_OK;

	return S_OK;
}

HRESULT CBorn_GroundCover::SetUp_Components()
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
	if (FAILED(__super::Add_Component(m_EnviromentDesc.iCurLevel, m_EnviromentDesc.szModelTag.c_str(), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(),
		L"Prototype_Component_Shader_Vtx_Inst_AnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	return S_OK;
}

HRESULT CBorn_GroundCover::SetUp_ShaderResources()
{
	_float4x4 g_InvWorldMatrix;
	XMStoreFloat4x4(&g_InvWorldMatrix, m_pTransformCom->Get_WorldMatrix_Inverse());
	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrixInv", &g_InvWorldMatrix)))
		return E_FAIL;

	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(float)), E_FAIL);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CBorn_GroundCover::Culling()
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 vCamPos = CGameInstance::GetInstance()->Get_CamPosition();
	_vector camPos = XMLoadFloat4(&vCamPos);

	const _vector	 vDir = camPos - vPos;
	m_bRenderCheck = CGameInstance::GetInstance()->isInFrustum_WorldSpace(vPos, 20.f);

	_float f = XMVectorGetX(XMVector4Length(vDir));

	if (100.f <= XMVectorGetX(XMVector4Length(vDir)))
		m_bRenderCheck = false;
}

CBorn_GroundCover * CBorn_GroundCover::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBorn_GroundCover*		pInstance = new CBorn_GroundCover(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBorn_GroundCover");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBorn_GroundCover::Clone(void * pArg)
{
	CBorn_GroundCover*		pInstance = new CBorn_GroundCover(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBorn_GroundCover");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBorn_GroundCover::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);
}
