#include "stdafx.h"
#include "BossWarrior_Hat.h"
#include "Bone.h"
#include "GameInstance.h"

CBossWarrior_Hat::CBossWarrior_Hat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonsterWeapon(pDevice, pContext)
{
}

CBossWarrior_Hat::CBossWarrior_Hat(const CBossWarrior_Hat& rhs)
	: CMonsterWeapon(rhs)
{
}

HRESULT CBossWarrior_Hat::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossWarrior_Hat::Initialize(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_WeaponDesc, pArg, sizeof(m_WeaponDesc));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_vPivotPos = _float4(0.f, 0.f, -1.85f, 0.f);
	m_vPivotRot = _float4(-1.5f, 0.f, 3.15f, 0.f);

	return S_OK;
}

HRESULT CBossWarrior_Hat::Late_Initialize(void* pArg)
{
	return S_OK;
}

void CBossWarrior_Hat::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBossWarrior_Hat::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	_matrix			SocketMatrix =
		m_WeaponDesc.pSocket->Get_OffsetMatrix() *
		m_WeaponDesc.pSocket->Get_CombindMatrix() *
		XMLoadFloat4x4(&m_WeaponDesc.PivotMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	SocketMatrix =
		XMMatrixRotationX(m_vPivotRot.x)
		* XMMatrixRotationY(m_vPivotRot.y)
		* XMMatrixRotationZ(m_vPivotRot.z)
		*XMMatrixTranslation(m_vPivotPos.x, m_vPivotPos.y, m_vPivotPos.z)
		* SocketMatrix
		* m_WeaponDesc.pTargetTransform->Get_WorldMatrix();

	XMStoreFloat4x4(&m_SocketMatrix, SocketMatrix);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CBossWarrior_Hat::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 0, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 0, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 0, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture"), E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 0, WJTextureType_ALPHA, "g_OpacityTexture"), E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 5), E_FAIL);
	}

	return S_OK;
}

HRESULT CBossWarrior_Hat::RenderShadow()
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

void CBossWarrior_Hat::Imgui_RenderProperty()
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
	if (ImGui::Button("HatRecompile"))
	{
		m_pRendererCom->ReCompile();
		m_pShaderCom->ReCompile();
	}
	ImGui::End();
}

void CBossWarrior_Hat::ImGui_ShaderValueProperty()
{
	CMonsterWeapon::ImGui_ShaderValueProperty();
}

void CBossWarrior_Hat::ImGui_PhysXValueProperty()
{
	CMonsterWeapon::ImGui_PhysXValueProperty();
}

void CBossWarrior_Hat::Update_Collider(_float fTimeDelta)
{
}

HRESULT CBossWarrior_Hat::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Boss_Warrior_Hat"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Boss_Warrior/VillageWarrior_Uv_02_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_ALPHA, TEXT("../Bin/Resources/Anim/Enemy/Boss_Warrior/VillageWarrior_Uv_02_OPACITY.png")), E_FAIL);

	return S_OK;
}

HRESULT CBossWarrior_Hat::SetUp_ShaderResources()
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
	if (FAILED(m_pShaderCom->Set_Matrix("g_SocketMatrix", &m_SocketMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CBossWarrior_Hat::SetUp_ShadowShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance)

		if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW))))
			return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_SocketMatrix", &m_SocketMatrix)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance)

		return S_OK;
}

CBossWarrior_Hat* CBossWarrior_Hat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossWarrior_Hat*		pInstance = new CBossWarrior_Hat(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBossWarrior_Hat");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBossWarrior_Hat::Clone(void* pArg)
{
	CBossWarrior_Hat*		pInstance = new CBossWarrior_Hat(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBossWarrior_Hat");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBossWarrior_Hat::Free()
{
	__super::Free();
}
