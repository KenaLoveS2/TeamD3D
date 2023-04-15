#include "stdafx.h"
#include "..\public\E_WarriorEyeTrail.h"
#include "GameInstance.h"
#include "BossWarrior.h"

CE_WarriorEyeTrail::CE_WarriorEyeTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Trail(pDevice, pContext)
{
}

CE_WarriorEyeTrail::CE_WarriorEyeTrail(const CE_WarriorEyeTrail & rhs)
	: CEffect_Trail(rhs)
{
}

HRESULT CE_WarriorEyeTrail::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_WarriorEyeTrail::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	m_iTotalDTextureComCnt = 0;	m_iTotalMTextureComCnt = 0;
	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	/* Trail Option */
	m_eEFfectDesc.IsTrail = true;
	m_eEFfectDesc.fWidth = 0.1f; 
	m_eEFfectDesc.fLife = 2.5f; 
	m_eEFfectDesc.bAlpha = false;
	m_eEFfectDesc.fAlpha = 0.6f;
	m_eEFfectDesc.fSegmentSize = 0.001f;
	m_eEFfectDesc.vColor = XMVectorSet(255.f, 77.f, 211.f, 255.f) / 255.f;
	/* ~Trail Option */

	m_eEFfectDesc.bActive = true;
	m_eEFfectDesc.iPassCnt = 1;
	return S_OK;
}

HRESULT CE_WarriorEyeTrail::Late_Initialize(void * pArg)
{
	m_pWarriorModel = (CModel*)m_pParent->Find_Component(L"Com_Model");

	return S_OK;
}

void CE_WarriorEyeTrail::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_fTimeDelta += fTimeDelta;

	Update_Trail(); 
}

void CE_WarriorEyeTrail::Late_Tick(_float fTimeDelta)
{
	if (m_pParent && dynamic_cast<CBossWarrior*>(m_pParent)->Get_MonsterStatusPtr()->Get_HP() < 1.0f)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_WarriorEyeTrail::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	m_pShaderCom->Begin(m_eEFfectDesc.iPassCnt);
	m_pVITrailBufferCom->Render();

	return S_OK;
}

void CE_WarriorEyeTrail::Reset()
{
	m_eEFfectDesc.bActive = false;
	m_fTimeDelta = 0.0f;
	ResetInfo();
}

HRESULT CE_WarriorEyeTrail::SetUp_ShaderResources()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	FAILED_CHECK_RETURN(m_pVITrailBufferCom->Bind_ShaderResouce(m_pShaderCom, "g_BossTrailInfoMatrix"), E_FAIL);

	FAILED_CHECK_RETURN(m_pTrailflowTexture->Bind_ShaderResource(m_pShaderCom, "g_FlowTexture", m_iTrailFlowTexture), E_FAIL);
	FAILED_CHECK_RETURN(m_pTrailTypeTexture->Bind_ShaderResource(m_pShaderCom, "g_TypeTexture", m_iTrailTypeTexture), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_Time", &m_fTimeDelta, sizeof(_float)), E_FAIL);

	return S_OK;
}

HRESULT CE_WarriorEyeTrail::SetUp_Components()
{	
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Texture_TrailFlow"), L"Com_flowTexture", (CComponent**)&m_pTrailflowTexture, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Texture_TrailType"), L"Com_typeTexture", (CComponent**)&m_pTrailTypeTexture, this), E_FAIL);

	m_pVITrailBufferCom = CVIBuffer_Trail::Create(m_pDevice, m_pContext, 300);

	Delete_Component(L"Com_Shader");
	Safe_Release(m_pShaderCom);

	/* For.Com_Shader */
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxEffect_BossTrail"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom), E_FAIL);

	return S_OK;
}

void CE_WarriorEyeTrail::Update_Trail()
{
	if (m_pParent == nullptr)
		return;

	_matrix Socketmatrix = m_pWarriorBone->Get_CombindMatrix() * m_pWarriorModel->Get_PivotMatrix() * m_pParent->Get_WorldMatrix();
	_vector vRight = m_pParent->Get_TransformCom()->Get_State(CTransform::STATE_RIGHT);

	// m_fRange => 0.5f;
	m_fRange = 0.1f;

	if (m_eType == CE_WarriorEyeTrail::TYPE_L)
		Socketmatrix.r[3] -= vRight * m_fRange;
	else
		Socketmatrix.r[3] += vRight * m_fRange;

	m_pTransformCom->Set_WorldMatrix(Socketmatrix);
	Trail_InputPos(Socketmatrix.r[3]);
}

void CE_WarriorEyeTrail::ToolTrail(const char* ToolTag)
{
	ImGui::Begin(ToolTag);

	if (ImGui::Button("Recompile"))
		m_pShaderCom->ReCompile();

	ImGui::Checkbox("Active", &m_eEFfectDesc.bActive);
	ImGui::InputFloat("Width", &m_eEFfectDesc.fWidth);
	ImGui::InputFloat("Life", &m_eEFfectDesc.fLife);
	ImGui::InputFloat("Alpha", &m_eEFfectDesc.fAlpha);
	ImGui::InputFloat("SegmentSize", &m_eEFfectDesc.fSegmentSize);
	ImGui::InputInt("Flow", (_int*)&m_iTrailFlowTexture);
	ImGui::InputInt("Type", (_int*)&m_iTrailTypeTexture);
	ImGui::InputInt("Pass", &m_eEFfectDesc.iPassCnt);

	static bool alpha_preview = true;
	static bool alpha_half_preview = false;
	static bool drag_and_drop = true;
	static bool options_menu = true;
	static bool hdr = false;

	ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

	static bool   ref_color = false;
	static ImVec4 ref_color_v(1.0f, 1.0f, 1.0f, 1.0f);

	static _float4 vSelectColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	vSelectColor = m_eEFfectDesc.vColor;

	ImGui::ColorPicker4("##ShamanColor", (float*)&vSelectColor, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color ? &ref_color_v.x : NULL);
	ImGui::ColorEdit4("##ShamanDiffuse", (float*)&vSelectColor, ImGuiColorEditFlags_DisplayRGB | misc_flags);
	m_eEFfectDesc.vColor = vSelectColor;
	ImGui::End();
}

CE_WarriorEyeTrail * CE_WarriorEyeTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_WarriorEyeTrail * pInstance = new CE_WarriorEyeTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_WarriorEyeTrail Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_WarriorEyeTrail::Clone(void * pArg)
{
	CE_WarriorEyeTrail * pInstance = new CE_WarriorEyeTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_WarriorEyeTrail Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_WarriorEyeTrail::Free()
{
	__super::Free();

	Safe_Release(m_pTrailflowTexture);
	Safe_Release(m_pTrailTypeTexture);
}
