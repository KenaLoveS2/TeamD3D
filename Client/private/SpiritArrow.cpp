#include "stdafx.h"
#include "..\public\SpiritArrow.h"
#include "GameInstance.h"
#include "Kena.h"
#include "Kena_Staff.h"
#include "Camera_Player.h"
#include "Bone.h"

CSpiritArrow::CSpiritArrow(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CSpiritArrow::CSpiritArrow(const CSpiritArrow & rhs)
	: CEffect_Mesh(rhs)
{
}

HRESULT CSpiritArrow::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CSpiritArrow::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GaemObjectDesc;
	ZeroMemory(&GaemObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	GaemObjectDesc.TransformDesc.fSpeedPerSec = 15.f;
	GaemObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	FAILED_CHECK_RETURN(__super::Initialize(&GaemObjectDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	m_pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena"));
	NULL_CHECK_RETURN(m_pKena, E_FAIL);

	m_pStaff = dynamic_cast<CKena_Staff*>(m_pKena->Get_KenaPart(L"Kena_Staff"));
	NULL_CHECK_RETURN(m_pStaff, E_FAIL);
	 
	m_pCamera = dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Find_Camera(L"PLAYER_CAM"));
	NULL_CHECK_RETURN(m_pCamera, E_FAIL);

	Push_EventFunctions();

	m_iObjectProperty = OP_PLAYER_WEAPON;
	m_eEFfectDesc.bActive = true;
	m_eEFfectDesc.iPassCnt = 5; // Effect_SpritArrow
	return S_OK;
}

HRESULT CSpiritArrow::Late_Initialize(void * pArg)
{
	return S_OK;
}

void CSpiritArrow::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CModel*	pModel = dynamic_cast<CModel*>(m_pKena->Find_Component(L"Com_Model"));
	CBone*	pHand = pModel->Get_BonePtr("bow_string_jnt_grip");
	CBone*	pStaffBone = pModel->Get_BonePtr("staff_skin6_jnt");

	_smatrix	matSocket = pStaffBone->Get_CombindMatrix() * pModel->Get_PivotMatrix() * m_pKena->Get_WorldMatrix();
	m_pTransformCom->Set_WorldMatrix(matSocket);
}

void CSpiritArrow::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom != nullptr)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CSpiritArrow::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (i == 0)
		{
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 5);
		}
		else if (i == 1)
		{
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 5);
		}
		else if (i == 2)
		{
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 5);
		}
	}

	return S_OK;
}

HRESULT CSpiritArrow::RenderShadow()
{
	return S_OK;
}

void CSpiritArrow::Imgui_RenderProperty()
{
}

void CSpiritArrow::ImGui_ShaderValueProperty()
{
}

void CSpiritArrow::ImGui_PhysXValueProperty()
{
}

void CSpiritArrow::Push_EventFunctions()
{
}

HRESULT CSpiritArrow::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxEffectModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_SpiritArrow", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	return S_OK;
}

HRESULT CSpiritArrow::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	return S_OK;
}

HRESULT CSpiritArrow::SetUp_ShadowShaderResources()
{
	return S_OK;
}

CSpiritArrow * CSpiritArrow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpiritArrow*	pInstance = new CSpiritArrow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CKena");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSpiritArrow::Clone(void * pArg)
{
	CSpiritArrow*	pInstance = new CSpiritArrow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CKena");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpiritArrow::Free()
{
	__super::Free();
}
