#include "stdafx.h"
#include "BossShaman_Mask.h"
#include "GameInstance.h"

CBossShaman_Mask::CBossShaman_Mask(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonsterWeapon(pDevice, pContext)
{
}

CBossShaman_Mask::CBossShaman_Mask(const CBossShaman_Mask& rhs)
	: CMonsterWeapon(rhs)
{
}

HRESULT CBossShaman_Mask::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossShaman_Mask::Initialize(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_WeaponDesc, pArg, sizeof(m_WeaponDesc));

	CGameObject::GAMEOBJECTDESC GameObjDesc;
	ZeroMemory(&GameObjDesc, sizeof(CGameObject::GAMEOBJECTDESC));
	GameObjDesc.TransformDesc.fSpeedPerSec = 10.f;

	if (FAILED(__super::Initialize(&GameObjDesc)))
		return E_FAIL;

	m_vPivotPos = _float4(0.f, -0.19f, -2.15f, 0.f);
	m_vPivotRot = _float4(-1.5f, 0.f, 3.15f, 0.f);
	
	XMStoreFloat4x4(&m_SocketPivotMatrix,
		XMMatrixRotationX(m_vPivotRot.x)
		* XMMatrixRotationY(m_vPivotRot.y)
		* XMMatrixRotationZ(m_vPivotRot.z)
		*XMMatrixTranslation(m_vPivotPos.x, m_vPivotPos.y, m_vPivotPos.z));
		
	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	return S_OK;
}

HRESULT CBossShaman_Mask::Late_Initialize(void* pArg)
{	
	return S_OK;
}

void CBossShaman_Mask::Tick(_float fTimeDelta)
{
	if (m_eState == STATE_END) return;

	__super::Tick(fTimeDelta);

	Socket_Proc(fTimeDelta);
	ShamanMask_Proc(fTimeDelta);
}

void CBossShaman_Mask::Late_Tick(_float fTimeDelta)
{
	if (m_eState == STATE_END) return;

	__super::Late_Tick(fTimeDelta);

	m_pRendererCom && m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CBossShaman_Mask::Render()
{
	if (FAILED(__super::Render())) return E_FAIL;
	if (FAILED(SetUp_ShaderResources())) return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 0, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 0, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 0, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture"), E_FAIL);		
		FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, m_iShaderPass), E_FAIL);
	}

	return S_OK;
}

HRESULT CBossShaman_Mask::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;
	
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, nullptr, 4);

	return S_OK;
}

void CBossShaman_Mask::Imgui_RenderProperty()
{
	CMonsterWeapon::Imgui_RenderProperty();

	ImGui::Begin("Monster_Weapon");
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

void CBossShaman_Mask::ImGui_ShaderValueProperty()
{
	CMonsterWeapon::ImGui_ShaderValueProperty();
}

void CBossShaman_Mask::ImGui_PhysXValueProperty()
{
	CMonsterWeapon::ImGui_PhysXValueProperty();
}

void CBossShaman_Mask::Update_Collider(_float fTimeDelta)
{
}

HRESULT CBossShaman_Mask::SetUp_Components()
{	
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom))) return E_FAIL;	
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom))) return E_FAIL;
	if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Model_ShamanMask"), TEXT("Com_Model"), (CComponent**)&m_pModelCom))) return E_FAIL;

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/NonAnim/Boss_ShamanMask/Shaman_Uv01_AO_R_M.png")), E_FAIL);
	
	return S_OK;
}

HRESULT CBossShaman_Mask::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom) return E_FAIL;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_SocketMatrix", &m_SocketMatrix))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4)))) return E_FAIL;
	
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDissolve, sizeof(_bool)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float)), E_FAIL);
	FAILED_CHECK_RETURN(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture"), E_FAIL);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CBossShaman_Mask::SetUp_ShadowShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)

		if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW))))
			return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_SocketMatrix", &m_SocketMatrix)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance)

		return S_OK;
}

CBossShaman_Mask* CBossShaman_Mask::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossShaman_Mask*		pInstance = new CBossShaman_Mask(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBossShaman_Mask");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBossShaman_Mask::Clone(void* pArg)
{
	CBossShaman_Mask*		pInstance = new CBossShaman_Mask(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBossShaman_Mask");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBossShaman_Mask::Free()
{
	__super::Free();
}

void CBossShaman_Mask::Socket_Proc(_float fTimeDelta)
{
	// Imgui_RenderProperty();

	_matrix SocketMatrix = m_WeaponDesc.pSocket->Get_OffsetMatrix() * m_WeaponDesc.pSocket->Get_CombindMatrix() * XMLoadFloat4x4(&m_WeaponDesc.PivotMatrix);
	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	//XMStoreFloat4x4(&m_SocketPivotMatrix,
	//XMMatrixRotationX(m_vPivotRot.x)
	//* XMMatrixRotationY(m_vPivotRot.y)
	//* XMMatrixRotationZ(m_vPivotRot.z)
	//*XMMatrixTranslation(m_vPivotPos.x, m_vPivotPos.y, m_vPivotPos.z));

	SocketMatrix = XMLoadFloat4x4(&m_SocketPivotMatrix) * SocketMatrix * m_WeaponDesc.pTargetTransform->Get_WorldMatrix();
	XMStoreFloat4x4(&m_SocketMatrix, SocketMatrix);
}

void CBossShaman_Mask::ShamanMask_Proc(_float fTimeDelta)
{
	switch (m_eState)
	{
	case RENDER:
	{
		if (m_bDissolve)
		{
			m_eState = DISSOLVE;
			m_fDissolveTime = 0.f;
			m_iShaderPass = 9; // 14
		}			

		break;
	}
	case DISSOLVE:
	{
		m_fDissolveTime += fTimeDelta * m_fDissolveRate;
		if (m_fDissolveTime >= 0.05f) 
			m_fDissolveRate *= 2.f;
		
		if (m_fDissolveTime >= 1.f)
		{
			m_fDissolveTime = 1.f;
			m_eState = STATE_END;
		}
		break;
	}
	}	
}

void CBossShaman_Mask::Clear()
{
	m_eState = RENDER;
	m_bDissolve = false;
	m_fDissolveTime = 0.f;
	m_iShaderPass = 10;
	m_fDissolveRate = 0.03f;
}

