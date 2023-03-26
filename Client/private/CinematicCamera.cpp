#include "stdafx.h"
#include "..\public\CinematicCamera.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "DebugDraw.h"
#include "Utile.h"

_float fTemp = 0.f;
wstring wstrTemp = L"";

void XM_CALLCONV DrawLine(PrimitiveBatch<VertexPositionColor>* batch,
	FXMVECTOR pointA, FXMVECTOR pointB, FXMVECTOR color)
{
	VertexPositionColor verts[2];
	XMStoreFloat3(&verts[0].position, pointA);
	XMStoreFloat3(&verts[1].position, pointB);

	for (size_t j = 0; j < 2; ++j)
	{
		XMStoreFloat4(&verts[j].color, color);
	}

	batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, verts, 2);
}

CCinematicCamera::CCinematicCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCamera(pDevice, pContext)
{
}

CCinematicCamera::CCinematicCamera(const CCinematicCamera& rhs)
	:CCamera(rhs)
{
}

HRESULT CCinematicCamera::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCinematicCamera::Initialize(void* pArg)
{
	CCamera::CAMERADESC			CameraDesc;
	ZeroMemory(&CameraDesc, sizeof CameraDesc);

	if (nullptr != pArg)
		memcpy(&CameraDesc, pArg, sizeof(CAMERADESC));
	else
	{
		CameraDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
		CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
		CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
		CameraDesc.TransformDesc.fSpeedPerSec = 5.f;
		CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	}

	if (FAILED(CCamera::Initialize(&CameraDesc)))
		return E_FAIL;

	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);
	const void*		pShaderByteCode;
	size_t			iShaderByteCodeSize;
	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeSize);
	m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeSize, &m_pInputLayout);
#endif

	return S_OK;
}

void CCinematicCamera::Tick(_float fTimeDelta)
{
	ImGui::Begin("CinematicCam");
	Imgui_RenderProperty();
	ImGui::End();

	_uint keyframeSize = m_keyframes.size();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)

	if(pGameInstance->Key_Down(DIK_L))
	{
		m_pPlayerCam = pGameInstance->Find_Camera(L"PLAYER_CAM");
		if(m_pPlayerCam)
			m_pTransformCom->Set_WorldMatrix_float4x4(m_pPlayerCam->Get_TransformCom()->Get_WorldMatrixFloat4x4());
	}

	if (m_bPlay &&keyframeSize >= 4)
	{
		if (m_bInitSet)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
			m_pPlayerCam = pGameInstance->Find_Camera(L"PLAYER_CAM");
			RELEASE_INSTANCE(CGameInstance)
			m_pTransformCom->Set_WorldMatrix_float4x4(m_pPlayerCam->Get_TransformCom()->Get_WorldMatrixFloat4x4());
			m_bInitSet = false;

			/* Call CinemaUI */
			CUI_ClientManager::UI_PRESENT eLetterBox = CUI_ClientManager::BOT_LETTERBOX;
			_bool bOn = true;
			m_CinemaDelegator.broadcast(eLetterBox, bOn, fTemp, wstrTemp);
			/* ~Call CinemaUI */

		}
	
		m_fDeltaTime += fTimeDelta;
		_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_float3 InterPolatePos = _float3(vPos.x, vPos.y, vPos.z);
		_float3 InterPolateLook = _float3(vLook.x, vLook.y, vLook.z);
		Interpolate(m_fDeltaTime, InterPolatePos, InterPolateLook);
		vPos = _float4(InterPolatePos.x, InterPolatePos.y, InterPolatePos.z, 1.f);
		vLook = _float4(InterPolateLook.x, InterPolateLook.y, InterPolateLook.z, 0.f);
		m_pTransformCom->Set_Position(vPos);
		m_pTransformCom->Set_Look(vLook);
		CCamera::Tick(fTimeDelta);
	}
	pGameInstance->Set_Transform(CPipeLine::D3DTS_CINEVIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
	RELEASE_INSTANCE(CGameInstance)
}

void CCinematicCamera::Late_Tick(_float TimeDelta)
{
	CCamera::Late_Tick(TimeDelta);

	if (m_pRendererCom && CGameInstance::GetInstance()->Get_WorkCameraPtr() != this)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CCinematicCamera::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);
	_uint	 iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, nullptr, 12);

	if (m_keyframes.size() <= 4)
		return S_OK;

#ifdef _DEBUG
	m_pEffect->SetWorld(XMMatrixIdentity());
	CGameInstance* pInst = GET_INSTANCE(CGameInstance);
	m_pEffect->SetView(pInst->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pInst->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
	RELEASE_INSTANCE(CGameInstance);

	m_pEffect->Apply(m_pContext);
	m_pContext->IASetInputLayout(m_pInputLayout);
	m_pBatch->Begin();

	unsigned int keyFrameSize = m_keyframes.size();
	for (unsigned int   i = 1; i < keyFrameSize; ++i)
		DrawLine(m_pBatch, m_keyframes[i -1].vPos, m_keyframes[i].vPos, _float4(1.f, 0.f, 1.f, 1.f));

	m_pBatch->End();
#endif // _DEBUG

	return CCamera::Render();
}

void CCinematicCamera::Imgui_RenderProperty()
{
	CCamera::Imgui_RenderProperty();
	ImGui::InputFloat("InputTime", &m_fInputTime);
	if(ImGui::Button("Add KeyFrame"))
	{
		CAMERAKEYFRAME keyFrame;
		ZeroMemory(&keyFrame, sizeof(CAMERAKEYFRAME));
		if(m_fInputTime != 0.f)
		{
			keyFrame.fTime = m_fInputTime + m_keyframes.size() *  0.5f;
			keyFrame.vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			keyFrame.vLookAt = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			m_keyframes.push_back(keyFrame);
		}
	}
	ImGui::DragFloat("Duration", &m_fDeltaTime, 0.1f, 0.f, 1000.f);

	if(m_keyframes.size() >= 4)
	{
		ImGui::Checkbox("Play", &m_bPlay);
		if(ImGui::Button("CinematicPlay"))
			m_fDeltaTime = 0.f;
	}

	if (ImGui::Button("InitPlay"))
	{
		m_bInitSet = true;
	}

#ifdef _DEBUG
	ImGui::Checkbox("DebugRender", &m_bDebugRender);
#endif // _DEBUG

	if(!m_bPlay)
	{
		if (ImGui::Button("Clear"))
			m_keyframes.clear();
	}

	m_pTransformCom->Imgui_RenderProperty();
}

void CCinematicCamera::AddKeyFrame(CAMERAKEYFRAME keyFrame)
{
	m_keyframes.push_back(keyFrame);
}

void CCinematicCamera::Interpolate(float time, _float3& position, _float3& lookAt)
{
	int numKeyframes = m_keyframes.size();
	if (numKeyframes < 4) {
		// Not enough keyframes to interpolate, just return the first keyframe
		position = m_keyframes[0].vPos;
		lookAt = m_keyframes[0].vLookAt;
		return;
	}

	// Find the two keyframes that surround the given time
	int i = 1;
	while (i < numKeyframes - 2 && m_keyframes[i].fTime < time) {
		i++;
	}

	if (numKeyframes > i + 2)
	{
		// Calculate the parameter t for the interpolation
		float t = 0.f;
		if(m_keyframes[i].fTime - m_keyframes[i - 1].fTime != 0)
			 t = (time - m_keyframes[i - 1].fTime) / (m_keyframes[i].fTime - m_keyframes[i - 1].fTime);
		else
			t = time - m_keyframes[i - 1].fTime;

		// Calculate the interpolated position and look-at direction
		position = CatmullRomInterpolation(m_keyframes[i - 1].vPos, m_keyframes[i].vPos,
			m_keyframes[i + 1].vPos, m_keyframes[i + 1].vPos, t);
		lookAt = CatmullRomInterpolation(m_keyframes[i - 1].vLookAt, m_keyframes[i].vLookAt,
			m_keyframes[i + 1].vLookAt, m_keyframes[i + 1].vLookAt, t);
	}
}

XMFLOAT3 CCinematicCamera::CatmullRomInterpolation(_float3 p0, _float3 p1, _float3 p2, _float3 p3, float t)
{
	XMFLOAT3 v;
	v.x = 0.5f * ((2.0f * p1.x) +	(-p0.x + p2.x) * t +	(2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t * t +	(-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t * t * t);
	v.y = 0.5f * ((2.0f * p1.y) +	(-p0.y + p2.y) * t +	(2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t * t +	(-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t * t * t);
	v.z = 0.5f * ((2.0f * p1.z) +	(-p0.z + p2.z) * t +	(2.0f * p0.z - 5.0f * p1.z + 4.0f * p2.z - p3.z) * t * t +	(-p0.z + 3.0f * p1.z - 3.0f * p2.z + p3.z) * t * t * t);
	return v;
}

HRESULT CCinematicCamera::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_CineCam", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);
	return S_OK;
}

HRESULT CCinematicCamera::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	return S_OK;
}

CCinematicCamera* CCinematicCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCinematicCamera*		pInstance = new CCinematicCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCinematicCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCinematicCamera::Clone(void* pArg)
{
	CCinematicCamera*		pInstance = new CCinematicCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCinematicCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCinematicCamera::Free()
{
	CCamera::Free();
#ifdef _DEBUG
	Safe_Release(m_pInputLayout);
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
#endif // _DEBUG

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
