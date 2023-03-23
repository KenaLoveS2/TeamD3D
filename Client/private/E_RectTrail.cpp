#include "stdafx.h"
#include "..\public\E_RectTrail.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Kena.h"

CE_RectTrail::CE_RectTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Trail(pDevice, pContext)
{
}

CE_RectTrail::CE_RectTrail(const CE_RectTrail & rhs)
	: CEffect_Trail(rhs)
{
}

HRESULT CE_RectTrail::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_RectTrail::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_pVITrailBufferCom = CVIBuffer_Trail::Create(m_pDevice, m_pContext, 100);
	m_eEFfectDesc.bActive = false;

	/* Trail Option */
	m_eEFfectDesc.IsTrail = true;
	m_eEFfectDesc.fWidth = 0.5f; //5.f
	m_eEFfectDesc.fLife = 1.5f; //1.f
	m_eEFfectDesc.bAlpha = false;
	m_eEFfectDesc.fAlpha = 0.6f;
	m_eEFfectDesc.fSegmentSize = 0.03f; // 0.5f
	m_eEFfectDesc.vColor = XMVectorSet(160.f, 231.f, 255.f, 255.f) / 255.f;
	/* ~Trail Option */

	m_eEFfectDesc.iPassCnt = 11;
	return S_OK;
}

HRESULT CE_RectTrail::Late_Initialize(void * pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pKena = (CKena*)pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Player"), TEXT("Kena"));
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CE_RectTrail::Tick(_float fTimeDelta)
{
 	__super::Tick(fTimeDelta);	
	
	// m_pVITrailBufferCom->Trail_Tick(m_pTransformCom, m_pKena, fTimeDelta); 
}

void CE_RectTrail::Late_Tick(_float fTimeDelta)
{
	//if (m_eEFfectDesc.bActive == false)
	//{
	//	ResetInfo();
	//	return;
	//}

// 	__super::Late_Tick(fTimeDelta);

	/* Rect Trail */
	_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	if (m_vecProPos.empty() || m_vecProPos.back() != vPos)
		m_vecProPos.push_back(vPos);

	if (m_vecProPos.size() >= 3)
	{
		while (m_vecProPos.size() > 3)
			m_vecProPos.erase(m_vecProPos.begin());

		CCamera* pCamera = CGameInstance::GetInstance()->Get_WorkCameraPtr();
		_vector  vCamPos = pCamera->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);

		_float  fSplineLength = XMVectorGetX(XMVector3Length(m_vecProPos[1] - m_vecProPos[2]));

		_vector vPoint0 = m_vecProPos[0];
		_vector vPoint1 = m_vecProPos[1];
		_vector vPoint2 = m_vecProPos[2];
		_vector vPoint3 = m_vecProPos[2];

		_float  fPreLife = 0.0f;
		if (m_pVITrailBufferCom->Get_InstanceInfo()->size())
			fPreLife = m_pVITrailBufferCom->Get_InstanceInfo()->back().vPosition.w;

		_uint  iSegmentCnt = _uint(fSplineLength / m_eEFfectDesc.fSegmentSize);
		_vector vPrepos = vPoint1;

		_float  fWeight = 0.0f, fRadian = 0.0f;
		_vector vPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		_vector vRight = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		_vector vUp = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		_vector vLook = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		_vector vDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		vPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		for (_uint i = 0; i < iSegmentCnt; ++i)
		{
			vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			vRight = XMVector3Normalize(vPrepos - vPosition);
			vDir = XMVectorSet(0.0f, -CUtile::Get_RandomFloat(0.1f, 1.0f), 0.0f, 0.0f);

			vUp = XMVector3Cross(vRight, vDir);
			vLook = XMVector3Cross(vRight, vUp);

			_smatrix TrailMatrix(vRight, vUp, vLook, vPosition);
			m_pVITrailBufferCom->Add_Instance(TrailMatrix);
			vPrepos = vPosition;
		}
	}

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CE_RectTrail::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(11);
	m_pVITrailBufferCom->Render();

	return S_OK;
}

void CE_RectTrail::Imgui_RenderProperty()
{
	if (ImGui::Button("Recompile"))
		m_pShaderCom->ReCompile();

	ImGui::InputFloat("DiffuseTexture", &m_eEFfectDesc.fFrame[0]);
	ImGui::InputFloat("MaskTexture", &m_eEFfectDesc.fMaskFrame[0]);
	ImGui::InputFloat("pass", (_float*)&m_eEFfectDesc.iPassCnt);

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

	ImGui::ColorPicker4("CurColor##6", (float*)&vSelectColor, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color ? &ref_color_v.x : NULL);
	ImGui::ColorEdit4("Diffuse##5f", (float*)&vSelectColor, ImGuiColorEditFlags_DisplayRGB | misc_flags);
	m_eEFfectDesc.vColor = vSelectColor;
}

HRESULT CE_RectTrail::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pVITrailBufferCom->Bind_ShaderResouce(m_pShaderCom, "g_InfoMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_Time", &m_fTimeDelta, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CE_RectTrail * CE_RectTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_RectTrail * pInstance = new CE_RectTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_RectTrail Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_RectTrail::Clone(void * pArg)
{
	CE_RectTrail * pInstance = new CE_RectTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_RectTrail Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_RectTrail::Free()
{
	__super::Free();
}
