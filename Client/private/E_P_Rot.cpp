#include "stdafx.h"
#include "..\public\E_P_Rot.h"
#include "GameInstance.h"

CE_P_Rot::CE_P_Rot(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_Rot::CE_P_Rot(const CE_P_Rot & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_Rot::Initialize_Prototype(const _tchar * pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_P_Rot::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 6.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	m_eEFfectDesc.bActive = false;
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	return S_OK;
}

HRESULT CE_P_Rot::Late_Initialize(void* pArg)
{
	m_ePointDesc = m_pVIInstancingBufferCom->Get_PointDesc();
	m_iNumInstance = m_pVIInstancingBufferCom->Get_InstanceNum();

	Reset();
	return S_OK;
}

void CE_P_Rot::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	if(!lstrcmp(m_szCloneObjectTag, L"RotAcc_1"))
	{
		ImGui::Begin("test");
		ImGui::Checkbox("Active", &m_eEFfectDesc.bActive);

		ImGui::InputInt("m_iNumInstance", (_int*)&m_iNumInstance);
		ImGui::InputInt("iNumInstance", &m_fOverInstanceCnt);
		ImGui::InputFloat("Diffuse", &m_eEFfectDesc.fFrame[0]);
		ImGui::InputInt("Pass", &m_eEFfectDesc.iPassCnt);
		ImGui::InputFloat4("Color", (_float*)&m_eEFfectDesc.vColor);

		if (ImGui::Button("Reshape"))
			Reset();
		if (ImGui::Button("Compile"))
			m_pShaderCom->ReCompile();

		ImGui::End();
	}
#endif // _DEBUG

	if (m_eEFfectDesc.bActive == false)
		return;

	m_fOverInstanceCnt = m_pVIInstancingBufferCom->Get_InstanceNum();

	m_fMinusTime += fTimeDelta; 
	if (m_fMinusTime > 1.f)
	{
		m_fOverInstanceCnt--;
		m_pVIInstancingBufferCom->Set_InstanceNum(m_fOverInstanceCnt);

		if (m_fOverInstanceCnt < 1.f)
			Reset();
		m_fMinusTime = 0.0f;
	}
	__super::Tick(fTimeDelta);
}

void CE_P_Rot::Late_Tick(_float fTimeDelta)
{
 	if (m_eEFfectDesc.bActive == false)
 		return;

	if (m_pParent)
	{
		_float4 vPos = m_pParent->Get_TransformCom()->Get_Position();
		vPos.y += 0.4f;
		m_pTransformCom->Set_Position(vPos);
	}
	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_P_Rot::Render()
{
	FAILED_CHECK_RETURN(__super::Render(),E_FAIL);

	return S_OK;
}

void CE_P_Rot::Imgui_RenderProperty()
{
	ImGui::Checkbox("Active", &m_eEFfectDesc.bActive);

	ImGui::InputInt("m_iNumInstance", (_int*)&m_iNumInstance);
	ImGui::InputInt("iNumInstance", &m_fOverInstanceCnt);
	ImGui::InputFloat("Diffuse", &m_eEFfectDesc.fFrame[0]);
	ImGui::InputInt("Pass", &m_eEFfectDesc.iPassCnt);
	ImGui::InputFloat4("Color", (_float*)&m_eEFfectDesc.vColor);

	if (ImGui::Button("Reshape"))
	{
		m_iNumInstance = 20;

		Set_ShapePosition();
		m_ePointDesc->fConeRange = _float2(3.f, 3.f);
		m_pVIInstancingBufferCom->Set_RandomSpeeds(0.05f, 0.2f);
		m_pVIInstancingBufferCom->Set_RandomPSize(_float2(0.05f, 0.1f));
	}
}

void CE_P_Rot::Update_Rot_P()
{
	_int iRandomDiffuse = rand() % 3;
	if (iRandomDiffuse == 0)
	{
		m_eEFfectDesc.fFrame[0] = 0.0f;
		m_eEFfectDesc.vColor = XMVectorSet(CUtile::Get_RandomFloat(0.1f, 1.f), CUtile::Get_RandomFloat(0.1f, 1.f), CUtile::Get_RandomFloat(0.1f, 1.f), 0.7f);
	}
	else
	{
		if (iRandomDiffuse == 1) /* Test ÇÊ¿ä */
			m_eEFfectDesc.fFrame[0] = 58.0f;
		else
			m_eEFfectDesc.fFrame[0] = 71.0f;
		m_eEFfectDesc.vColor = XMVectorSet(CUtile::Get_RandomFloat(0.1f, 1.f), 0.f, 0.f, 0.7f);
	}
}

void CE_P_Rot::Reset()
{
	Update_Rot_P();

	m_eEFfectDesc.bActive = false;
	m_pVIInstancingBufferCom->Set_InstanceNum(m_iNumInstance);

	m_ePointDesc->fConeRange = _float2(3.f, 3.f);
	Set_ShapePosition();
	m_pVIInstancingBufferCom->Set_RandomSpeeds(0.05f, 0.1f);
	m_pVIInstancingBufferCom->Set_RandomPSize(_float2(0.05f, 0.1f));
}

CE_P_Rot * CE_P_Rot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_Rot * pInstance = new CE_P_Rot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_Rot Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_Rot::Clone(void * pArg)
{
	CE_P_Rot * pInstance = new CE_P_Rot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_Rot Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_Rot::Free()
{
	__super::Free();
}
