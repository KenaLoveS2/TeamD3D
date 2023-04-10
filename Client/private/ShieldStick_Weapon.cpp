#include "stdafx.h"
#include "ShieldStick_Weapon.h"
#include "Bone.h"
#include "GameInstance.h"

CShieldStick_Weapon::CShieldStick_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonsterWeapon(pDevice, pContext)
{
}

CShieldStick_Weapon::CShieldStick_Weapon(const CShieldStick_Weapon& rhs)
	: CMonsterWeapon(rhs)
{
}

HRESULT CShieldStick_Weapon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShieldStick_Weapon::Initialize(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_WeaponDesc, pArg, sizeof(m_WeaponDesc));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_vPivotPos = _float4(0.65f, 0.07f, -0.35f, 0.f);
	m_vPivotRot = _float4(0.f, 0.f, 0.f, 0.f);

	return S_OK;
}

HRESULT CShieldStick_Weapon::Late_Initialize(void* pArg)
{
	return S_OK;
}

void CShieldStick_Weapon::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bShowDissolve)
	{
		m_fDissolveTime -= fTimeDelta * 0.45f;
		if (m_fDissolveTime <= 0.f)
		{
			m_fDissolveTime = 0.f;
			m_iShaderPass = 1;
			m_bShowDissolve = false;
		}
	}
		
}

void CShieldStick_Weapon::Late_Tick(_float fTimeDelta)
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
	{
		//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CShieldStick_Weapon::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Render(m_pShaderCom, i, nullptr, m_iShaderPass);  // 1, 5
	}

	return S_OK;
}

HRESULT CShieldStick_Weapon::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, nullptr, 2);

	return S_OK;
}

void CShieldStick_Weapon::Imgui_RenderProperty()
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

void CShieldStick_Weapon::ImGui_ShaderValueProperty()
{
	CMonsterWeapon::ImGui_ShaderValueProperty();
}

void CShieldStick_Weapon::ImGui_PhysXValueProperty()
{
	CMonsterWeapon::ImGui_PhysXValueProperty();
}

void CShieldStick_Weapon::Update_Collider(_float fTimeDelta)
{
}

HRESULT CShieldStick_Weapon::SetUp_Components()
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
	if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Model_ShieldStick_Weapon"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CShieldStick_Weapon::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom) return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_SocketMatrix", &m_SocketMatrix))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4)))) return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bShowDissolve, sizeof(_bool)))) return E_FAIL;
	if (m_bShowDissolve)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float)))) return E_FAIL;
		if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture"))) return E_FAIL;
	}

	return S_OK;
}

HRESULT CShieldStick_Weapon::SetUp_ShadowShaderResources()
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

CShieldStick_Weapon* CShieldStick_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShieldStick_Weapon*		pInstance = new CShieldStick_Weapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CShieldStick_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CShieldStick_Weapon::Clone(void* pArg)
{
	CShieldStick_Weapon*		pInstance = new CShieldStick_Weapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CShieldStick_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CShieldStick_Weapon::Free()
{
	__super::Free();
}

void CShieldStick_Weapon::Show_Dissolve()
{
	m_fDissolveTime = 1.f;
	m_bShowDissolve = true;
	m_iShaderPass = 9;
}