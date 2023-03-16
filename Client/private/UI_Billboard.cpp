#include "stdafx.h"
#include "..\public\UI_Billboard.h"
#include "GameInstance.h"
#include "Monster.h"

CUI_Billboard::CUI_Billboard(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI(pDevice,pContext)
{
	m_tBBDesc.pOwner = nullptr;
	m_tBBDesc.vCorrect = { 0.f, 0.f, 0.f, 0.f };
	m_tBBDesc.fileName = L"";
}

CUI_Billboard::CUI_Billboard(const CUI_Billboard & rhs)
	:CUI(rhs)
{
	m_tBBDesc.pOwner = nullptr;
	m_tBBDesc.vCorrect = { 0.f, 0.f, 0.f, 0.f };
	m_tBBDesc.fileName = L"";
}

HRESULT CUI_Billboard::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Billboard::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg == nullptr)
		return E_FAIL;

	m_tBBDesc = *(BBDESC*)pArg;

	return Load_Data(m_tBBDesc.fileName);
}

void CUI_Billboard::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_tBBDesc.pOwner != nullptr)
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,
		(m_tBBDesc.pOwner->Get_Position() + XMLoadFloat4(&m_tBBDesc.vCorrect)));

	for (auto e : m_vecEvents)
		e->Tick(fTimeDelta);
}

void CUI_Billboard::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	CUtile::Execute_BillBoardOrtho(m_pTransformCom, m_vOriginalSettingScale, Get_CamDistance());

	for (auto e : m_vecEvents)
		e->Late_Tick(fTimeDelta);
}

HRESULT CUI_Billboard::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

bool	billboard_getter(void* data, int index, const char** output)
{
	vector<string>*	 pVec = (vector<string>*)data;
	*output = (*pVec)[index].c_str();
	return true;
}

void CUI_Billboard::Imgui_RenderProperty()
{
	ImGui::BeginTabBar("UI Setting");

	if (ImGui::BeginTabItem("UI_Setting"))
	{
		vector<string>* pPasses = CGameInstance::GetInstance()->Get_UIString(CUI_Manager::STRKEY_RENDERPASS);

		/* RenderPass */
		static int selected_Pass;// = 0;
		selected_Pass = m_iRenderPass;
		_uint iNumPasses = (_uint)pPasses->size();
		if (ImGui::ListBox(" : RenderPass", &selected_Pass, billboard_getter, pPasses, iNumPasses))
		{
			m_iRenderPass = selected_Pass;
			m_iOriginalRenderPass = selected_Pass;
		}

		/* Size */
		static float scale[3];
		_float3 vScale = m_vOriginalSettingScale;
		scale[0] = vScale.x;
		scale[1] = vScale.y;
		scale[2] = vScale.z;
		if (ImGui::DragFloat3("OriginalSettingScale", scale, 0.1f, 0.f, 500.f))
		{
			vScale.x = scale[0];
			vScale.y = scale[1];
			vScale.z = scale[2];
			Set_OriginalSettingScale(vScale);
		}
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("UI_Setting"))
	{
		Imgui_EventSetting();
		ImGui::EndTabItem();
	}


	ImGui::EndTabBar();

}

HRESULT CUI_Billboard::Save_Data()
{
	return S_OK;
}

HRESULT CUI_Billboard::Load_Data(wstring fileName)
{
	/* Orignial Scale, renderPass, ... */

	return S_OK;
}

void CUI_Billboard::Free()
{
	__super::Free();
}
