#include "stdafx.h"
#include "..\public\RotHat.h"
#include "GameInstance.h"
#include "Rot.h"

_tchar CRotHat::m_szModelTagTable[HAT_TYPE_END][64] = {
	L"Prototype_Component_Model_RotHat_Acorn",
	L"Prototype_Component_Model_RotHat_Clover",
	L"Prototype_Component_Model_RotHat_Egg",
	L"Prototype_Component_Model_RotHat_Mushroom",
	L"Prototype_Component_Model_RotHat_Butterfly",
	L"Prototype_Component_Model_RotHat_Dinosaur",
	L"Prototype_Component_Model_RotHat_RedBow",
	L"Prototype_Component_Model_RotHat_Cowboy",
	L"Prototype_Component_Model_RotHat_Halo",
	L"Prototype_Component_Model_RotHat_Deer",
	L"Prototype_Component_Model_RotHat_Pancake",
	L"Prototype_Component_Model_RotHat_Sunflower",
	L"Prototype_Component_Model_RotHat_Cat",
	L"Prototype_Component_Model_RotHat_Bunny",
	L"Prototype_Component_Model_RotHat_Crown",
	L"Prototype_Component_Model_RotHat_Samurai",
};

_tchar CRotHat::m_szAoTexturePathTable[HAT_TYPE_END][128] = {
	L"../Bin/Resources/NonAnim/RotHat/00_Acorn/T_StinkFruit_Acorn_Hats_AO_R_M.png",
	L"../Bin/Resources/NonAnim/RotHat/01_Clover/T_Clover_Hat_AO_R_M.png",
	L"../Bin/Resources/NonAnim/RotHat/02_Egg/T_Egg_Hat_AO_R_M.png",
	L"../Bin/Resources/NonAnim/RotHat/03_Mushroom/T_MushroomHat_Comp_AO_R_M.png",
	L"../Bin/Resources/NonAnim/RotHat/04_Butterfly/T_Buttlefly_Hat_AO_R_M.png",
	L"../Bin/Resources/NonAnim/RotHat/05_Dinosaur/T_Dinosaur_Hat_AO_R_M.png",
	L"../Bin/Resources/NonAnim/RotHat/06_RedBow/T_RedBow_Hat_AO_R_M.png",
	L"../Bin/Resources/NonAnim/RotHat/07_Cowboy/T_CowboyHat_comp_AO_R_M.png",
	L"../Bin/Resources/NonAnim/RotHat/08_Halo/T_Halo_Hat_AO_R_M.png",
	L"../Bin/Resources/NonAnim/RotHat/09_Deer/T_SM_DeerHat_AO_R_M.png",
	L"../Bin/Resources/NonAnim/RotHat/10_Pancake/T_Pancake_Hat_AO_R_M.png",
	L"../Bin/Resources/NonAnim/RotHat/11_Sunflower/T_SunflowerHat_Comp_AO_R_M.png",
	L"../Bin/Resources/NonAnim/RotHat/12_Cat/T_CatHat_AO_R_M.png",
	L"../Bin/Resources/NonAnim/RotHat/13_Bunny/T_BunnyHat_AO_R_M.png",
	L"../Bin/Resources/NonAnim/RotHat/14_Crown/T_CrownHat_AO_R_M.png",
	L"../Bin/Resources/NonAnim/RotHat/15_Samurai/T_Samurai_hat_AO_R_M.png",
};

// ACORN, CLOVER, EGG, MUSHROOM, 
// BUTTERFLY, DINOSAUR, REDBOW, COWBOY, 
// HALO, DEER, PANCAKE, SUNFLOWER, 
// CAT, BUNNY, CROWN, SAMURAI,

_float3 CRotHat::m_vPivotTrans[HAT_TYPE_END] = {
	_float3(0.f, -0.02f, -0.22f), _float3(0.f, -0.02f, -0.22f), _float3(0.f, -0.02f, -0.22f), _float3(0.f, 0.f, -0.22f),
	_float3(0.f, -0.02f, -0.22f), _float3(0.f, -0.02f, -0.22f), _float3(0.f, -0.02f, -0.22f), _float3(-0.01f, 0.01f, -0.21f),
	_float3(0.f, -0.02f, -0.22f), _float3(0.f, -0.02f, -0.22f), _float3(0.f, -0.02f, -0.22f), _float3(0.f, 0.f, -0.23f),
	_float3(0.f, -0.02f, -0.22f), _float3(0.f, -0.03f, -0.23f), _float3(0.f, -0.01f, -0.22f), _float3(0.f, -0.02f, -0.22f),
};
_float3 CRotHat::m_vPivotRot[HAT_TYPE_END] = {
	_float3(-1.f, 0.f, -3.f), _float3(-1.f, 0.f, -3.f), _float3(-1.f, 0.f, -3.f), _float3(-1.5f, 0.f, -3.f), 
	_float3(-1.f, 0.f, -3.f), _float3(-1.f, 0.f, -3.f), _float3(-1.f, 0.f, -3.f), _float3(-1.8f, 0.2f, -0.22f),
	_float3(-1.f, 0.f, -3.f), _float3(-1.f, 0.f, -3.f), _float3(-1.f, 0.f, -3.f), _float3(-1.38f, 0.08f, -3.040f),
	_float3(-1.f, 0.f, -3.f), _float3(-1.f, 0.f, -3.07f), _float3(-1.4f, 0.f, -3.09f), _float3(-1.f, 0.f, -3.f),
};


CRotHat::CRotHat(ID3D11Device* pDevice, ID3D11DeviceContext* p_context)
	:CGameObject(pDevice, p_context)
{
}

CRotHat::CRotHat(const CRotHat& rhs)
	:CGameObject(rhs)
{
}

HRESULT CRotHat::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CRotHat::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	
	ZeroMemory(&m_HatDesc, sizeof(HAT_DESC));

	return S_OK;
}

HRESULT CRotHat::Late_Initialize(void * pArg)
{	
	return S_OK;
}

void CRotHat::Tick(_float fTimeDelta)
{
	// Imgui_RenderProperty();

	__super::Tick(fTimeDelta);
}

void CRotHat::Late_Tick(_float fTimeDelta)
{
	if (m_bShowFlag == false) 
		return;

	__super::Late_Tick(fTimeDelta);	
	
	_matrix SocketMatrix = 
		m_HatDesc.pSocket->Get_OffsetMatrix() * 
		m_HatDesc.pSocket->Get_CombindMatrix() *
		XMLoadFloat4x4(&m_HatDesc.PivotMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	_float3* pRot = m_vPivotRot + m_HatDesc.eHatType;
	_float3* pTrans = m_vPivotTrans + m_HatDesc.eHatType;

	_matrix PivotMatrix = XMMatrixRotationX(pRot->x) * XMMatrixRotationY(pRot->y) * XMMatrixRotationZ(pRot->z) *
		XMMatrixTranslation(pTrans->x, pTrans->y, pTrans->z);
		
	SocketMatrix = PivotMatrix * SocketMatrix * m_HatDesc.pTargetTransform->Get_WorldMatrix();

	XMStoreFloat4x4(&m_SocketMatrix, SocketMatrix);

	// m_pRendererCom && m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
	m_pRendererCom && m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CRotHat::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 10);
	}

	return S_OK;
}

HRESULT CRotHat::RenderShadow()
{
	if (FAILED(__super::RenderShadow())) return E_FAIL;
	if (FAILED(SetUp_ShadowShaderResources())) return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 3);

	return S_OK;
}

void CRotHat::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	static char szTransText[CRotHat::HAT_TYPE_END][16] = {
	"Trans Acorn", "Trans Clover", "Trans Egg", "Trans Mushroom", "Trans Butterfly",
	"Trans Dinosaur", "Trans RedBow", "Trans Cowboy", "Trans Halo", "Trans Deer",
	"Trans Pancake", "Trans Sunflower", "Trans Cat", "Trans Bunny", "Trans Crown", "Trans Samurai",
	};
	static char szRotText[CRotHat::HAT_TYPE_END][16] = {
		"Rot Acorn", "Rot Clover", "Rot Egg", "Rot Mushroom", "Rot Butterfly",
		"Rot Dinosaur", "Rot RedBow", "Rot Cowboy", "Rot Halo", "Rot Deer",
		"Rot Pancake", "Rot Sunflower", "Rot Cat", "Rot Bunny", "Rot Crown", "Rot Samurai",
	};

	float fTrans[3] = { m_vPivotTrans[m_HatDesc.eHatType].x, m_vPivotTrans[m_HatDesc.eHatType].y, m_vPivotTrans[m_HatDesc.eHatType].z };
	ImGui::DragFloat3(szTransText[m_HatDesc.eHatType], fTrans, 0.01f, -100.f, 100.0f);
	memcpy(&m_vPivotTrans[m_HatDesc.eHatType], fTrans, sizeof(_float3));

	float fRot[3] = { m_vPivotRot[m_HatDesc.eHatType].x, m_vPivotRot[m_HatDesc.eHatType].y, m_vPivotRot[m_HatDesc.eHatType].z };
	ImGui::DragFloat3(szRotText[m_HatDesc.eHatType], fRot, 0.01f, -100.f, 100.0f);
	memcpy(&m_vPivotRot[m_HatDesc.eHatType], fRot, sizeof(_float3));
}

void CRotHat::ImGui_ShaderValueProperty()
{
	if (ImGui::Button("Recompile"))
	{
		m_pShaderCom->ReCompile();
		m_pRendererCom->ReCompile();
	}
}


HRESULT CRotHat::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);
	
	return S_OK;
}

HRESULT CRotHat::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_SocketMatrix", &m_SocketMatrix), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4)), E_FAIL);
		
	return S_OK;
}

HRESULT CRotHat::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	return S_OK;
}

CRotHat* CRotHat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRotHat* pInstance = new CRotHat(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CRotHat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRotHat::Clone(void* pArg)
{
	CRotHat* pInstance = new CRotHat(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CRotHat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRotHat::Free()
{
	__super::Free();
	
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}

HRESULT CRotHat::Create_HatModel(HAT_DESC& HatDesc, _bool bShowFlag)
{
	if (HatDesc.eHatType >= HAT_TYPE_END) return E_FAIL;

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, m_szModelTagTable[HatDesc.eHatType], L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, m_szAoTexturePathTable[HatDesc.eHatType]), E_FAIL);

	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	memcpy(&m_HatDesc, &HatDesc, sizeof(HAT_DESC));

	m_bShowFlag = bShowFlag;

	return S_OK;
} 