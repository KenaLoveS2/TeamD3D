#include "stdafx.h"
#include "..\public\E_BombTrail.h"
#include "GameInstance.h"
#include "Camera.h"

CE_BombTrail::CE_BombTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Trail(pDevice, pContext)
{
}

CE_BombTrail::CE_BombTrail(const CE_BombTrail & rhs)
	: CEffect_Trail(rhs)
{
}

HRESULT CE_BombTrail::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_BombTrail::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	m_iTotalDTextureComCnt = 0;
	m_iTotalMTextureComCnt = 0;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	/* Trail Texture */
	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_TrailFlow"), L"Com_flowTexture", (CComponent**)&m_pTrailflowTexture, this)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_TrailType"), L"Com_typeTexture", (CComponent**)&m_pTrailTypeTexture, this)))
		return E_FAIL;
	/* Trail Texture */

	m_pVITrailBufferCom = CVIBuffer_Trail::Create(m_pDevice, m_pContext, 300);

	/* Trail Option */
	m_eEFfectDesc.IsTrail = true;
	m_eEFfectDesc.fWidth = 0.1f;
	m_eEFfectDesc.fLife = 1.f;
	m_eEFfectDesc.bAlpha = false;
	m_eEFfectDesc.fAlpha = 1.0f;
	m_eEFfectDesc.fSegmentSize = 0.01f; // 0.5f
	m_eEFfectDesc.vColor = XMVectorSet(0.f, 0.f, 0.f, 255.f) / 255.f;
	/* ~Trail Option */

	m_iTrailFlowTexture = 8;
	m_iTrailTypeTexture = 7;

	m_eEFfectDesc.bActive = false;

	return S_OK;
}

void CE_BombTrail::Tick(_float fTimeDelta)
{
	//__super::Tick(fTimeDelta);

	if (m_eEFfectDesc.bActive == false)
	{
		m_vecPositions.clear();
		ResetInfo();
		return;
	}
	
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

void CE_BombTrail::Late_Tick(_float fTimeDelta)
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

HRESULT CE_BombTrail::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(10);
	m_pVITrailBufferCom->Render();

	return S_OK;
}

void CE_BombTrail::Imgui_RenderProperty()
{
	if (ImGui::Button("RECOMPILE"))
		m_pShaderCom->ReCompile();
}

void CE_BombTrail::Add_BezierCurve(const _float4 & vStartPos, const _float4 & vSplinePos, _float fTimeDelta)
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

// 		_float4	vNorDir;
// 		vDir.Normalize(vNorDir);
// 		_float4	vTurnPoint = vStartPos + vNorDir * 5.f;
// 
// 		m_vecPositions.push_back(vStartPos);
// 
// 		_float4	vA = vSplinePos - vTurnPoint;
// 		_float4	vNorA = XMVector3Cross(vA * -1.f, m_pParent->Get_WorldMatrix().r[0]);
// 		_float4	vEndPos = XMLoadFloat4(&vSplinePos) + XMVector3Normalize(vNorA) * XMVectorGetX(XMVector3Length(vA));
// 
// 		_float4	vPointA;
// 		_float4	vPointB;
// 		_float4	vPath;
// 		for (_float fRatio = 0.f; fRatio <= 1.f; fRatio += fTimeDelta)
// 		{
// 			vPointA = _float4::Lerp(vTurnPoint, vSplinePos, fRatio);
// 			vPointB = _float4::Lerp(vSplinePos, vEndPos, fRatio);
// 			vPath = _float4::Lerp(vPointA, vPointB, fRatio);
// 
// 			m_vecPositions.push_back(vPath);
// 		}
// 
// 		m_vecPositions.push_back(vEndPos);
// 
// 		_float4	vDropPos = XMLoadFloat4(&vEndPos) + XMVector3Normalize(vNorA) * fLength;
// 		m_vecPositions.push_back(vDropPos);
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

void CE_BombTrail::Copy_Path(list<_float4> & PathList)
{
	if (PathList.empty() == false)
		PathList.clear();

	_uint	iSize = (_uint)m_vecPositions.size();

	for (_uint i = 1; i < iSize; ++i)
		PathList.push_back(m_vecPositions[i]);
}

void CE_BombTrail::Reset()
{
// 	if (m_eEFfectDesc.bActive == false)
// 		m_vecPositions.clear();

	ResetInfo();
}

HRESULT CE_BombTrail::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	FAILED_CHECK_RETURN(m_pVITrailBufferCom->Bind_ShaderResouce(m_pShaderCom, "g_KenaInfoMatrix"),E_FAIL);
	FAILED_CHECK_RETURN(m_pTrailflowTexture->Bind_ShaderResource(m_pShaderCom, "g_FlowTexture", m_iTrailFlowTexture), E_FAIL);
	FAILED_CHECK_RETURN(m_pTrailTypeTexture->Bind_ShaderResource(m_pShaderCom, "g_TypeTexture", m_iTrailTypeTexture), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_Time", &m_fTimeDelta, sizeof(_float)), E_FAIL);

	return S_OK;
}

CE_BombTrail * CE_BombTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_BombTrail * pInstance = new CE_BombTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_BombTrail Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_BombTrail::Clone(void * pArg)
{
	CE_BombTrail * pInstance = new CE_BombTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_BombTrail Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_BombTrail::Free()
{
	__super::Free();

	Safe_Release(m_pTrailflowTexture);
	Safe_Release(m_pTrailTypeTexture);
}
