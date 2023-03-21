#include "stdafx.h"
#include "..\public\Gimmick_EnviObj.h"
#include "GameInstance.h"
#include "ControlMove.h"
#include "Interaction_Com.h"

#include "ControlRoom.h"

/* 기믹 클래스는 1개씩입니다. */
CGimmick_EnviObj::CGimmick_EnviObj(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CGimmick_EnviObj::CGimmick_EnviObj(const CGimmick_EnviObj & rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CGimmick_EnviObj::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGimmick_EnviObj::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_bRenderActive = true;

	return S_OK;
}

HRESULT CGimmick_EnviObj::Late_Initialize(void * pArg)
{
	m_pControlRoom = dynamic_cast<CControlRoom*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_ControlRoom", L"ControlRoom"));
	assert(m_pControlRoom != nullptr  && "CPulse_Plate_Anim::Late_Initialize(void * pArg)");

	m_pControlRoom->Add_GimmickObj(m_EnviromentDesc.iRoomIndex,this,m_EnviromentDesc.eChapterType);



	return S_OK;
}

void CGimmick_EnviObj::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bColliderOn == false && true == Gimmik_Start(fTimeDelta))
	{
		_float3 vPos = _float3(0.f,0.f,0.f), vSize;
		
		if (m_EnviromentDesc.iRoomIndex == 2)
		{
			vSize = _float3(0.8f, 0.81f, 0.8f);
			vPos = _float3(0.0f, 0.f, 0.0f);
		}
		
		if (m_pModelCom->Get_UseTriangleMeshActor())
			m_pModelCom->Create_Px_InstTriangle(m_pTransformCom);

		m_bColliderOn = true;
	}

}

void CGimmick_EnviObj::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom && m_bRenderActive)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CGimmick_EnviObj::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		//m_pE_R_AoTexCom->Bind_ShaderResource(m_pShaderCom, "g_ERAOTexture");
		m_pModelCom->Render(m_pShaderCom, i, nullptr, m_iShaderOption);
	}
	return S_OK;
}

_bool CGimmick_EnviObj::Gimmik_Start(_float fTimeDelta)
{
	if (m_bGimmick_Active == false)
		return false;

	if (m_EnviromentDesc.iRoomIndex == 2)
		_bool b = false;
	_bool bResult = false;

	switch (m_EnviromentDesc.eChapterType)
	{
	case Gimmick_TYPE_GO_UP:
		bResult = Gimmick_Go_up(fTimeDelta);
		break;
	default:
		break;
	}

	return bResult;
}

_bool CGimmick_EnviObj::Gimmick_Go_up(_float fTimeDelta)
{
	return m_pModelCom->Instaincing_MoveControl(m_EnviromentDesc.eChapterType,fTimeDelta);
}

HRESULT CGimmick_EnviObj::Add_AdditionalComponent(_uint iLevelIndex, const _tchar * pComTag, COMPONENTS_OPTION eComponentOption)
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

HRESULT CGimmick_EnviObj::SetUp_Components()
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
	if (FAILED(__super::Add_Component(g_LEVEL, m_EnviromentDesc.szModelTag.c_str(), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (m_pModelCom->Get_IStancingModel())
	{
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModelInstance"), TEXT("Com_Shader"),
			(CComponent**)&m_pShaderCom)))
			return E_FAIL;

		m_iShaderOption = 1;
	}
	else
	{
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModelTess"), TEXT("Com_Shader"),
			(CComponent**)&m_pShaderCom)))
			return E_FAIL;

		m_iShaderOption = 4;
	}

	return S_OK;
}

HRESULT CGimmick_EnviObj::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(float)), E_FAIL);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CGimmick_EnviObj * CGimmick_EnviObj::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGimmick_EnviObj*		pInstance = new CGimmick_EnviObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGimmick_EnviObj");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGimmick_EnviObj::Clone(void * pArg)
{
	CGimmick_EnviObj*		pInstance = new CGimmick_EnviObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGimmick_EnviObj");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGimmick_EnviObj::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);
}
