#include "stdafx.h"
#include "..\public\E_LazerTrail.h"
#include "GameInstance.h"
#include "Camera.h"

CE_LazerTrail::CE_LazerTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Trail(pDevice, pContext)
{
}

CE_LazerTrail::CE_LazerTrail(const CE_LazerTrail & rhs)
	: CEffect_Trail(rhs)
{
}

HRESULT CE_LazerTrail::Initialize_Prototype(const _tchar * pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CE_LazerTrail::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	m_iTotalDTextureComCnt = 0;	m_iTotalMTextureComCnt = 0;
	//FAILED_CHECK_RETURN(, E_FAIL);
	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	/* Trail Texture */
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Texture_TrailFlow"), L"Com_flowTexture", (CComponent**)&m_pTrailflowTexture, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Texture_TrailType"), L"Com_typeTexture", (CComponent**)&m_pTrailTypeTexture, this), E_FAIL);
	/* Trail Texture */

	m_pVITrailBufferCom = CVIBuffer_Trail::Create(m_pDevice, m_pContext, 300);

	/* Trail Option */
	m_eEFfectDesc.IsTrail = true;
	m_eEFfectDesc.fWidth = 0.1f;
	m_eEFfectDesc.fLife = 0.5f;
	m_eEFfectDesc.bAlpha = false;
	m_eEFfectDesc.fAlpha = 1.0f;
	m_eEFfectDesc.fSegmentSize = 0.01f;
	m_eEFfectDesc.vColor = XMVectorSet(255.f, 255.f, 255.f, 255.f) / 255.f;
	/* ~Trail Option */

	m_iTrailFlowTexture = 0;
	m_iTrailTypeTexture = 0;

	m_eEFfectDesc.bActive = false;
	return S_OK;
}

void CE_LazerTrail::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
	{
		m_vecPositions.clear();
		ResetInfo();
		return;
	}
	
	TurnOffSystem(m_fDurationTime, 1.f, fTimeDelta, false);

	_uint	iSize = (_uint)m_vecPositions.size();
	_float	fRadian = 0.f;
	_float4	vCamPos = CGameInstance::GetInstance()->Get_CamPosition();
	_float4	vCurPos;
	_float4	vPrePos;
	_float4	vRight;
	_float4	vUp;
	_float4	vLook;
	_float4	vDir;

	for (_uint i = 0; i < iSize; ++i)
	{
		if (iSize < 2)
			return;

		vCurPos = m_vecPositions[i];

		if (i == 0)
			vPrePos = XMLoadFloat4(&vCurPos) + XMVector3Normalize(vCurPos - m_vecPositions[i + 1]) * 0.1f;
		else
			vPrePos = m_vecPositions[i - 1];

		vRight = XMVector3Normalize(vCurPos - vPrePos);
		vDir = XMVector3Normalize(vCamPos - vCurPos);

		fRadian = XMConvertToDegrees(fabs(acosf(XMVectorGetX(XMVector3Dot(vDir, vRight)))));
		if (fRadian < 5.f)
			continue;

		vUp = XMVector3Cross(vRight, vDir);
		vLook = XMVector3Cross(vRight, vUp);

		_smatrix TrailMatrix(vRight, vUp, vLook, vCurPos);
		m_pVITrailBufferCom->Add_Instance(TrailMatrix);
	}

	m_pVITrailBufferCom->Refresh_InstanceCount();
}

void CE_LazerTrail::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
	{
		m_vecPositions.clear();
		ResetInfo();
		return;
	}

	if (m_pRendererCom != nullptr)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CE_LazerTrail::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(18);
	m_pVITrailBufferCom->Render();

	return S_OK;
}

void CE_LazerTrail::Imgui_RenderProperty()
{
	if (ImGui::Button("RECOMPILE"))
		m_pShaderCom->ReCompile();
}

void CE_LazerTrail::Add_BezierCurve(const _float4 & vStartPos, const _float4 & vSplinePos, _float fTimeDelta)
{
	m_vecPositions.clear();

	_float4	vDir = vSplinePos - vStartPos;
	_float		fLength = XMVectorGetX(XMVector3Length(vDir));

	if (fLength > 5.f)
	{
		_float4	vMidPoint = (vStartPos + vSplinePos) * 0.5f;
		vMidPoint.y += 2.5f;

		_float4	vPointA;
		_float4	vPointB;
		_float4	vPath;
		for (_float fRatio = 0.f; fRatio <= 1.f; fRatio += fTimeDelta)
		{
			vPointA = _float4::Lerp(vStartPos, vMidPoint, fRatio);
			vPointB = _float4::Lerp(vMidPoint, vSplinePos, fRatio);
			vPath = _float4::Lerp(vPointA, vPointB, fRatio);

			m_vecPositions.push_back(vPath);
		}
	}
	else
	{
		m_vecPositions.push_back(vStartPos);

		_float4	vMidPoint = (vStartPos + vSplinePos) * 0.5f;
		vMidPoint.y += 0.3f;

		_float4	vPointA;
		_float4	vPointB;
		_float4	vPath;
		for (_float fRatio = 0.f; fRatio <= 1.f; fRatio += fTimeDelta)
		{
			vPointA = _float4::Lerp(vStartPos, vMidPoint, fRatio);
			vPointB = _float4::Lerp(vMidPoint, vSplinePos, fRatio);
			vPath = _float4::Lerp(vPointA, vPointB, fRatio);

			m_vecPositions.push_back(vPath);
		}
	}
}

void CE_LazerTrail::Copy_Path(list<_float4> & PathList)
{
	if (PathList.empty() == false)
		PathList.clear();

	_uint	iSize = (_uint)m_vecPositions.size();

	for (_uint i = 1; i < iSize; ++i)
		PathList.push_back(m_vecPositions[i]);
}

void CE_LazerTrail::Reset()
{
	ResetInfo();
}

void CE_LazerTrail::ToolTrail(const char* ToolTag)
{
	ImGui::Begin(ToolTag);

	if (ImGui::Button("Recompile"))
		m_pShaderCom->ReCompile();

	ImGui::Checkbox("Active", &m_eEFfectDesc.bActive);
	ImGui::InputFloat("Width", &m_eEFfectDesc.fWidth);
	ImGui::InputFloat("Life", &m_eEFfectDesc.fLife);
	ImGui::InputFloat("Alpha", &m_eEFfectDesc.fAlpha);
	ImGui::InputFloat("SegmentSize", &m_eEFfectDesc.fSegmentSize);
	ImGui::InputInt("PassCnt", &m_eEFfectDesc.iPassCnt);
	ImGui::InputInt("Flow", (_int*)&m_iTrailFlowTexture);
	ImGui::InputInt("Type", (_int*)&m_iTrailTypeTexture);

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

HRESULT CE_LazerTrail::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	FAILED_CHECK_RETURN(m_pVITrailBufferCom->Bind_ShaderResouce(m_pShaderCom, "g_KenaInfoMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pTrailflowTexture->Bind_ShaderResource(m_pShaderCom, "g_FlowTexture", m_iTrailFlowTexture), E_FAIL);
	FAILED_CHECK_RETURN(m_pTrailTypeTexture->Bind_ShaderResource(m_pShaderCom, "g_TypeTexture", m_iTrailTypeTexture), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_Time", &m_fTimeDelta, sizeof(_float)), E_FAIL);

	return S_OK;
}

CE_LazerTrail * CE_LazerTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_LazerTrail * pInstance = new CE_LazerTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_LazerTrail Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_LazerTrail::Clone(void * pArg)
{
	CE_LazerTrail * pInstance = new CE_LazerTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_LazerTrail Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_LazerTrail::Free()
{
	__super::Free();

	Safe_Release(m_pTrailflowTexture);
	Safe_Release(m_pTrailTypeTexture);
}
