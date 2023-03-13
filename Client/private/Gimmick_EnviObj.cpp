#include "stdafx.h"
#include "..\public\Gimmick_EnviObj.h"
#include "GameInstance.h"
#include "ControlMove.h"
#include "Interaction_Com.h"

#include "Pulse_Plate_Anim.h"

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
#ifdef FOR_MAP_GIMMICK
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_EnviromentDesc.iRoomIndex == 1 && m_EnviromentDesc.eChapterType == CEnviromentObj::Gimmick_TYPE_GO_UP)
	{

		CPulse_Plate_Anim* pPluse_Plate = dynamic_cast<CPulse_Plate_Anim*>(pGameInstance->Get_GameObjectPtr(g_LEVEL, L"Layer_Enviroment", L"1_PulsePlate_Anim"));
		assert(pPluse_Plate != nullptr && "CGimmick_EnviObj::Late_Initialize_RoomIndex 1");
		pPluse_Plate->m_Gimmick_PulsePlateDelegate.bind(this, &CGimmick_EnviObj::Set_Gimmick_Active);

	}
	else if (m_EnviromentDesc.iRoomIndex == 1)
	{

	}

	RELEASE_INSTANCE(CGameInstance);
#endif
	return S_OK;
}

void CGimmick_EnviObj::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Gimmik_Start(fTimeDelta);

	
}

void CGimmick_EnviObj::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom)
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

void CGimmick_EnviObj::Gimmik_Start(_float fTimeDelta)
{
	if (m_bGimmick_Active == false)
		return;

	switch (m_EnviromentDesc.eChapterType)
	{
	case Gimmick_TYPE_GO_UP:
		Gimmick_Go_up(fTimeDelta);
		break;
	default:
		break;
	}

}

void CGimmick_EnviObj::Gimmick_Go_up(_float fTimeDelta)
{
	m_pModelCom->Instaincing_MoveControl(m_EnviromentDesc.eChapterType,fTimeDelta);

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
