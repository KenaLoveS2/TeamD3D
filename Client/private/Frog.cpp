#include "stdafx.h"
#include "Frog.h"
#include "GameInstance.h"
#include "ControlMove.h"
#include "Interaction_Com.h"
#include "RotEater.h"

CFrog::CFrog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CFrog::CFrog(const CFrog& rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CFrog::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFrog::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	_int iRandNum = rand() % 2;

	if(iRandNum ==0)
	{
		m_pModelCom->Set_AnimIndex(CRY_BIG);
	}
	else
	{
		m_pModelCom->Set_AnimIndex(LOOK_AROUND);
	}

	//CGameInstance::GetInstance()->Add_AnimObject(g_LEVEL, this);

	m_bRenderActive = true;

	


	return S_OK;
}

HRESULT CFrog::Late_Initialize(void* pArg)
{

	return S_OK;
}

void CFrog::Tick(_float fTimeDelta)
{

	__super::Tick(fTimeDelta);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
#ifdef FOR_MAP_GIMMICK
		CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL_FOR_COMPONENT, L"Layer_Player", L"Kena");

	if (pPlayer == nullptr)
		return;

	_vector vPlayerPos = pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
	const _vector	 vDir = vPlayerPos - vPos;

#else
	_float4 vCamPos;
	vCamPos = CGameInstance::GetInstance()->Get_CamPosition();
	_vector camPos = XMLoadFloat4(&vCamPos);
	const _vector	 vDir = camPos - vPos;

#endif

	_bool bPlayerCom = false;

	if (8.f >= XMVectorGetX(XMVector4Length(vDir)))
		bPlayerCom = true;

	if(bPlayerCom == true)
	{
		if(!m_bAnimActive )
		{
			CGameInstance::GetInstance()->Play_Sound(L"Frog.ogg", 1.0f, false);
			m_bAnimActive = true;
			m_pModelCom->Set_AnimIndex(CHEER);
		}
	}


	if(m_pModelCom->Get_AnimIndex() == CHEER && m_pModelCom->Get_AnimationFinish())
	{
		m_pModelCom->Set_AnimIndex(CRY_BIG);
	}



	m_pModelCom->Play_Animation(fTimeDelta);
}

void CFrog::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CFrog::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"),E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", DEFAULT),E_FAIL);
	}
	return S_OK;
}

void CFrog::ImGui_AnimationProperty()
{
	//m_pTransformCom->Imgui_RenderProperty_ForJH();
	//m_pModelCom->Imgui_RenderProperty();
}


HRESULT CFrog::SetUp_Components()
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
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_Frog", L"Com_Model",
		(CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	/* For.Com_Shader */
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(),
		L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);



	return S_OK;
}

HRESULT CFrog::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CFrog* CFrog::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFrog* pInstance = new CFrog(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFrog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CFrog::Clone(void* pArg)
{
	CFrog* pInstance = new CFrog(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFrog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFrog::Free()
{
	Safe_Release(m_pModelCom);
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);
}
