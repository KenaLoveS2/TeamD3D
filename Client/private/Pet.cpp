#include "stdafx.h"
#include "Pet.h"
#include "GameInstance.h"
#include "ControlMove.h"
#include "Interaction_Com.h"
#include "RotEater.h"

CPet::CPet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CPet::CPet(const CPet& rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CPet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	//CGameInstance::GetInstance()->Add_AnimObject(g_LEVEL, this);

	m_bRenderActive = true;

	m_pModelCom->Set_AnimIndex(0);


	return S_OK;
}

HRESULT CPet::Late_Initialize(void* pArg)
{

	return S_OK;
}

void CPet::Tick(_float fTimeDelta)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

#ifdef FOR_MAP_GIMMICK
	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena");

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


	if (45.f >= XMVectorGetX(XMVector4Length(vDir)))
		m_bAnimActive = true;


	if(m_bAnimActive)
		m_pModelCom->Play_Animation(fTimeDelta);


	__super::Tick(fTimeDelta);
}

void CPet::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CPet::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"),E_FAIL);
		if (i == 0)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", DEFAULT),E_FAIL);
		}
		else	if (i == 1)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture"),E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", AO_R_M_E), E_FAIL);
		}
	}
	return S_OK;
}



void CPet::ImGui_AnimationProperty()
{
	//m_pTransformCom->Imgui_RenderProperty_ForJH();
	//m_pModelCom->Imgui_RenderProperty();
}


HRESULT CPet::SetUp_Components()
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
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Pet", L"Com_Model",
		(CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	

	/* For.Com_Shader */
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(),
		L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	m_pModelCom->SetUp_Material(1, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Pet/rh_body_AO_R_M.png"));
	m_pModelCom->SetUp_Material(1, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Pet/T_Black.png"));
	m_pModelCom->SetUp_Material(1, WJTextureType_SSS_MASK, TEXT("../Bin/Resources/Anim/Pet/rh_body_MASK_SSS_Simple.png"));

	return S_OK;
}

HRESULT CPet::SetUp_ShaderResources()
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

CPet* CPet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPet* pInstance = new CPet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPet::Clone(void* pArg)
{
	CPet* pInstance = new CPet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPet::Free()
{
	Safe_Release(m_pModelCom);
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);
}
