#include "stdafx.h"
#include "..\public\FakeShaman.h"
#include "GameInstance.h"

CFakeShaman::CFakeShaman(ID3D11Device* pDevice, ID3D11DeviceContext* p_context)
	:CGameObject(pDevice, p_context)
{
}

CFakeShaman::CFakeShaman(const CFakeShaman& rhs)
	:CGameObject(rhs)
{
}

HRESULT CFakeShaman::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CFakeShaman::Initialize(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_Desc, pArg, sizeof(m_Desc));

	CGameObject::GAMEOBJECTDESC		GaemObjectDesc;
	ZeroMemory(&GaemObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));
	GaemObjectDesc.TransformDesc.fSpeedPerSec = 1.5f;
	GaemObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	FAILED_CHECK_RETURN(__super::Initialize(&GaemObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	m_vPivotPos = _float4(0.f, -5.f, 0.f, 0.f);
	m_vPivotRot = m_Desc.vPivotRot;

	return S_OK;
}

HRESULT CFakeShaman::Late_Initialize(void* pArg)
{
	return CGameObject::Late_Initialize(pArg);
}

void CFakeShaman::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_matrix SocketMatrix =
		m_Desc.pSocket->Get_OffsetMatrix() *
		m_Desc.pSocket->Get_CombindMatrix() *
		XMLoadFloat4x4(&m_Desc.PivotMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	SocketMatrix =
		XMMatrixRotationX(m_vPivotRot.x) * XMMatrixRotationY(m_vPivotRot.y) * XMMatrixRotationZ(m_vPivotRot.z)
		*XMMatrixTranslation(m_vPivotPos.x, m_vPivotPos.y, m_vPivotPos.z)
		* SocketMatrix * m_Desc.pTargetTransform->Get_WorldMatrix();

	XMStoreFloat4x4(&m_SocketMatrix, SocketMatrix);
}

void CFakeShaman::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CFakeShaman::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		// Cloth UV2
		if (i == 0)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 11), E_FAIL);
		}
		else // UV1
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture"), E_FAIL);
			//FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_ALPHA, "g_OpacityTexture"),E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 10), E_FAIL);
		}
	}
	return S_OK;
}

HRESULT CFakeShaman::RenderShadow()
{
	if (FAILED(__super::RenderShadow())) return E_FAIL;
	if (FAILED(SetUp_ShadowShaderResources())) return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i,  nullptr, 4);

	return S_OK;
}

void CFakeShaman::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();
	ImGui::Begin("CFakeShaman");
	if (ImGui::Button("Recompile"))
		m_pShaderCom->ReCompile();
	_float3 vPivot = m_vPivotPos;
	float fPos[3] = { vPivot.x, vPivot.y, vPivot.z };
	ImGui::DragFloat3("PivotPos", fPos, 0.01f, -100.f, 100.0f);
	m_vPivotPos.x = fPos[0];
	m_vPivotPos.y = fPos[1];
	m_vPivotPos.z = fPos[2];
	_float3 vRotPivot = m_vPivotRot;
	float fRotPos[3] = { vRotPivot.x, vRotPivot.y, vRotPivot.z };
	ImGui::DragFloat3("PivotRot", fRotPos, 0.01f, -100.f, 100.0f);
	m_vPivotRot.x = fRotPos[0];
	m_vPivotRot.y = fRotPos[1];
	m_vPivotRot.z = fRotPos[2];
	ImGui::End();
}

void CFakeShaman::ImGui_ShaderValueProperty()
{
	CGameObject::ImGui_ShaderValueProperty();
}

void CFakeShaman::ImGui_PhysXValueProperty()
{
	CGameObject::ImGui_PhysXValueProperty();
}

HRESULT CFakeShaman::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_FakeShaman", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Boss_Shaman/Shaman_Uv01_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Enemy/Boss_Shaman/Shaman_Uv01_EMISSIVE.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Boss_Shaman/Shaman_Uv02_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_ALPHA, TEXT("../Bin/Resources/Anim/Enemy/Boss_Shaman/Shaman_Uv02_ALPHA.png")), E_FAIL);
	return S_OK;
}

HRESULT CFakeShaman::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_SocketMatrix", &m_SocketMatrix), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_EmissiveColor", &_float4(1.f, 1.f, 1.f, 1.f), sizeof(_float4)), E_FAIL);
	float fHDRIntensity = 1.f;
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fHDRIntensity", &fHDRIntensity, sizeof(_float)), E_FAIL);
	return S_OK;
}

HRESULT CFakeShaman::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_SocketMatrix", &m_SocketMatrix), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	return S_OK;
}

CFakeShaman* CFakeShaman::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFakeShaman*	pInstance = new CFakeShaman(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CFakeShaman");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFakeShaman::Clone(void* pArg)
{
	CFakeShaman*	pInstance = new CFakeShaman(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CFakeShaman");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFakeShaman::Free()
{
	CGameObject::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	if (true == m_isCloned)
	{
		Safe_Release(m_Desc.pSocket);
		Safe_Release(m_Desc.pTargetTransform);
	}
}
