#include "stdafx.h"
#include "..\public\Gimmick_EnviObj.h"
#include "GameInstance.h"
#include "ControlMove.h"
#include "Interaction_Com.h"

#include "ControlRoom.h"
#include "BossWarrior.h"
#include "CPortalPlane.h"
#include "E_P_EnvironmentDust.h"

/* 기믹 클래스는 1개씩입니다. */
CGimmick_EnviObj::CGimmick_EnviObj(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CGimmick_EnviObj::CGimmick_EnviObj(const CGimmick_EnviObj & rhs)
	: CEnviromentObj(rhs)
{
}

void CGimmick_EnviObj::Set_Gimmick_Active(_int iRoomIndex, _bool bGimmick_Active)
{
	
	if (m_EnviromentDesc.iRoomIndex == iRoomIndex)
		m_bGimmick_Active = bGimmick_Active;

	if(bGimmick_Active == true)
	{
		/* 사운드 넣기*/
		if(iRoomIndex == 1)  // 그냥 땅 올라오기
		{
			CGameInstance::GetInstance()->Play_Sound(L"SFX_Stone_Gate_Open_End_1.ogg", 0.7f, false);
			CGameInstance::GetInstance()->Play_Sound(L"Gimmick_1_UP.mp3", 1.f, false);
			
		}
		else if(iRoomIndex ==2)  // 물땅올라오기
		{
			CGameInstance::GetInstance()->Play_Sound(L"SFX_Stone_Gate_Open_End_1.ogg", 0.8f, false);
			CGameInstance::GetInstance()->Play_Sound(L"SFX_Waterfall_Platform_Rise_Water.ogg", 0.8f, false);
		}
		else if (iRoomIndex == 3)  // 게이트 올라오기
		{
			CGameInstance::GetInstance()->Play_Sound(L"SFX_Stone_Gate_Open_End_1.ogg", 0.7f, false);
			CGameInstance::GetInstance()->Play_Sound(L"SFX_Stone_Gate_Open_LP_1.ogg", 1.f, false);
		}
		else if(iRoomIndex ==4 )  // 문 올라가는 소리
		{
			CGameInstance::GetInstance()->Play_Sound(L"SFX_Stone_Gate_Open_End_1.ogg", 0.8f, false);
			CGameInstance::GetInstance()->Play_Sound(L"SFX_Stone_Gate_Open_LP_3.ogg", 0.8f, false);
		}
	}

	if (m_pGimmickObjEffect != nullptr)
		m_pGimmickObjEffect->Set_Active(bGimmick_Active);

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

	if (FAILED(Ready_Effect()))
		return E_FAIL;

	m_bRenderActive = true;

	return S_OK;
}

HRESULT CGimmick_EnviObj::Late_Initialize(void * pArg)
{

	m_pControlRoom = dynamic_cast<CControlRoom*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_ControlRoom", L"ControlRoom"));
	assert(m_pControlRoom != nullptr  && "CPulse_Plate_Anim::Late_Initialize(void * pArg)");

	m_pControlRoom->Add_GimmickObj(m_EnviromentDesc.iRoomIndex,
		this,m_EnviromentDesc.eChapterType);

	return S_OK;
}

void CGimmick_EnviObj::Tick(_float fTimeDelta)
{
//#ifdef FOR_MAP_GIMMICK
//
//#else
//	if(!m_bTestOnce)
//	{
//		Late_Initialize();
//		m_bTestOnce = true;
//	}
//#endif

	//if(ImGui::Button("Gimmick_Init"))
	//{
	//	m_bColliderOn = false;
	//	m_pModelCom->Instaincing_GimmkicInit(m_EnviromentDesc.eChapterType);
	//	m_bGimmick_Active = false;
	//}

	__super::Tick(fTimeDelta);

	if ( m_bColliderOn == false && true == Gimmik_Start(fTimeDelta))
	{
		if (m_pModelCom->Get_UseTriangleMeshActor() && m_EnviromentDesc.iRoomIndex != 4)
			m_pModelCom->Create_Px_InstTriangle(m_pTransformCom);

		m_bColliderOn = true;

		if (m_EnviromentDesc.iRoomIndex == 3)
		{
			dynamic_cast<CPortalPlane*>(CGameInstance::GetInstance()->
				Get_GameObjectPtr(g_LEVEL, L"Layer_Enviroment",
					L"3_BossDeadPortal_1"))->Set_GimmickRender(true);

			dynamic_cast<CPortalPlane*>(CGameInstance::GetInstance()->
				Get_GameObjectPtr(g_LEVEL, L"Layer_Enviroment",
					L"3_BossDeadPortal_0"))->Set_GimmickRender(true);
		}
	}


	if (m_pGimmickObjEffect)
	{
		m_pGimmickObjEffect->Tick(fTimeDelta);
	}
	

}

void CGimmick_EnviObj::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom && m_bRenderActive)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

#ifdef _DEBUG
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_CINE, this);
#endif
	}

	if (m_pGimmickObjEffect) m_pGimmickObjEffect->Late_Tick(fTimeDelta);

}

HRESULT CGimmick_EnviObj::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (m_pModelCom->Get_IStancingModel())
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);

			if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_COMP_H_R_AO] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_H_R_AO, "g_HRAOTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 2), E_FAIL);
			}
			else	if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_COMP_E_R_AO] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_E_R_AO, "g_ERAOTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 5), E_FAIL);
			}
			else if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_AMBIENT_OCCLUSION] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 6), E_FAIL);
			}
			else
			{
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 1), E_FAIL);
			}
		}
	}
	else
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 15), E_FAIL);
		}
	}

	return S_OK;
}

HRESULT CGimmick_EnviObj::RenderCine()
{
	if (FAILED(__super::RenderCine()))
		return E_FAIL;

	if (FAILED(SetUp_CineShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (m_pModelCom->Get_IStancingModel())
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 12), E_FAIL);
		}
	}
	else
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 20), E_FAIL);
		}
	}

	return S_OK;
}

HRESULT CGimmick_EnviObj::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (m_pModelCom->Get_IStancingModel())
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 0), E_FAIL);
		}
	}
	else
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 21), E_FAIL);
		}
	}

	return S_OK;
}

_bool CGimmick_EnviObj::Gimmik_Start(_float fTimeDelta)
{
	if (m_bGimmick_Active == false)
		return false;

	_bool bResult = false;

	//switch (m_EnviromentDesc.eChapterType)
	//{
	//case Gimmick_TYPE_GO_UP:
	//	bResult = Gimmick_Go_up(fTimeDelta);
	//	break;
	//case Gimmick_TYPE_GO_UP:
	//	bResult = Gimmick_Go_up(fTimeDelta);
	//	break;
	//default:
	//	break;
	//}

	


	bResult = Gimmick_Go_up(fTimeDelta);


	return bResult;
}

void CGimmick_EnviObj::SetUp_GimmicKEffect_Pos()
{
#ifdef FOR_MAP_GIMMICK
	if (m_pModelCom->Get_IStancingModel() && (m_EnviromentDesc.iRoomIndex == 1))
	{
		_float4 vPos = _float4(72.097f, -1.747f, 202.843f, 1.f);
		m_pGimmickObjEffect->Get_TransformCom()->Set_State(CTransform::STATE_TRANSLATION, 
			XMLoadFloat4(&vPos));
	}
	else if(m_pModelCom->Get_IStancingModel() && (m_EnviromentDesc.iRoomIndex == 4))
	{
		_float4 vPos = _float4(61.604f, 13.330f, 901.225f, 1.f);
		m_pGimmickObjEffect->Get_TransformCom()->Set_State(CTransform::STATE_TRANSLATION,
			XMLoadFloat4(&vPos));
	}
#endif
}

_bool CGimmick_EnviObj::Gimmick_Go_up(_float fTimeDelta)
{
	return m_pModelCom->Instaincing_MoveControl(m_EnviromentDesc.eChapterType,fTimeDelta);
}

HRESULT CGimmick_EnviObj::Ready_Effect()
{
#ifdef FOR_MAP_GIMMICK
	m_pGimmickObjEffect = (CE_P_EnvironmentDust*)(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_EnvironmentDust", CUtile::Create_DummyString()));
	NULL_CHECK_RETURN(m_pGimmickObjEffect, E_FAIL);
	m_pGimmickObjEffect->Set_Parent(this);
#else

	//m_pGimmickObjEffect = (CE_P_EnvironmentDust*)(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_EnvironmentDust", CUtile::Create_DummyString()));
	//NULL_CHECK_RETURN(m_pGimmickObjEffect, E_FAIL);
	//m_pGimmickObjEffect->Set_Parent(this);
	
#endif



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
	if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, m_EnviromentDesc.szModelTag.c_str(), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModelInstance"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

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

HRESULT CGimmick_EnviObj::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(float)), E_FAIL);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CGimmick_EnviObj::SetUp_CineShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_CINEVIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
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

	Safe_Release(m_pGimmickObjEffect);
}
