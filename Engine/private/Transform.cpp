#include "stdafx.h"
#include "..\public\Transform.h"
#include "Shader.h"
#include "Navigation.h"
#include "GameInstance.h"
#include "VIBuffer_Terrain.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
	
}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
	
}

void CTransform::Set_Scaled(STATE eState, _float fScale)
{
	if (eState == STATE_TRANSLATION)
		return;

	_vector		vState = Get_State(eState);

	vState = XMVector3Normalize(vState) * fScale;

	Set_State(eState, vState);
}

void CTransform::Set_Scaled(_float3 vScale)
{
	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * vScale.x);
	Set_State(CTransform::STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * vScale.y);
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * vScale.z);
}

void CTransform::Scaling(STATE eState, _float fScale)
{
	if (eState == STATE_TRANSLATION)
		return;

	Set_State(eState, Get_State(eState) * fScale);	
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));

	m_fInitSpeed = m_TransformDesc.fSpeedPerSec;

	return S_OK;
}

void CTransform::Imgui_RenderProperty()
{
	if (ImGui::CollapsingHeader("Transform"))
	{
		ImGuizmo::BeginFrame();
		ImGui::InputFloat("SpeedPerSec", &m_TransformDesc.fSpeedPerSec);
		ImGui::InputFloat("RotationPerSec", &m_TransformDesc.fRotationPerSec);

		static float snap[3] = { 1.f, 1.f, 1.f };
		static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
		if (ImGui::IsKeyPressed(90))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(69))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(82)) // r Key
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
			mCurrentGizmoOperation = ImGuizmo::SCALE;

		float matrixTranslation[3], matrixRotation[3], matrixScale[3];


		/******* Docking *******/

		_uint numViewport = 1;
		D3D11_VIEWPORT	viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
		//D3D11_VIEWPORT	viewportsInfo[] = {D3D11_VIEWPORT };
		m_pContext->RSGetViewports(&numViewport, &viewport);


		ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<float*>(&m_WorldMatrix), matrixTranslation, matrixRotation, matrixScale);

		/******* Docking *******/
		/* For the objects relative to screen. (ex.UI)") */
		ImGui::Separator();
		ImGui::Text("* Docking");
		static _bool right{ false }, left{ false }, top{ false }, bottom{ false };
		ImGui::Checkbox("right", &right); ImGui::SameLine();
		ImGui::Checkbox("left", &left); ImGui::SameLine();
		ImGui::Text(" / "); ImGui::SameLine();
		ImGui::Checkbox("top", &top); ImGui::SameLine();
		ImGui::Checkbox("bottom", &bottom);
		ImGui::Separator();

		if (left)
			matrixTranslation[0] += viewport.Width * 0.5f;
		else if (right)
			matrixTranslation[0] -= viewport.Width * 0.5f;
		if (top)
			matrixTranslation[1] -= viewport.Height * 0.5f;
		else if (bottom)
			matrixTranslation[1] += viewport.Height * 0.5f;
		/******* ~Docking *******/


		/* Before Input */
		_float fRatio = matrixScale[0] / matrixScale[1];

		ImGui::InputFloat3("Translate", matrixTranslation);
		ImGui::InputFloat3("Rotate", matrixRotation);
		ImGui::InputFloat3("Scale", matrixScale);

		/******* Docking *******/
		if (left)
			matrixTranslation[0] -= viewport.Width * 0.5f;
		else if (right)
			matrixTranslation[0] += viewport.Width * 0.5f;
		if (top)
			matrixTranslation[1] += viewport.Height * 0.5f;
		else if (bottom)
			matrixTranslation[1] -= viewport.Height * 0.5f;	
		/******* ~Docking *******/

		/* Scale Ratio(base on Width) */
		static _bool isKeepRatio{ false };
		ImGui::Checkbox(" : Keep Original Ratio", &isKeepRatio);
		if (isKeepRatio)
		{
			matrixScale[1] = fRatio * matrixScale[1];
		}

		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, reinterpret_cast<float*>(&m_WorldMatrix));

		if (mCurrentGizmoOperation != ImGuizmo::SCALE)
		{
			if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
				mCurrentGizmoMode = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
				mCurrentGizmoMode = ImGuizmo::WORLD;
		}

		static bool useSnap(false);
		if (ImGui::IsKeyPressed(83))
			useSnap = !useSnap;
		ImGui::Checkbox("##something", &useSnap);
		ImGui::SameLine();
		switch (mCurrentGizmoOperation)
		{
		case ImGuizmo::TRANSLATE:
			ImGui::InputFloat3("Snap", &snap[0]);
			break;
		case ImGuizmo::ROTATE:
			ImGui::InputFloat("Angle Snap", &snap[0]);
			break;
		case ImGuizmo::SCALE:
			ImGui::InputFloat("Scale Snap", &snap[0]);
			break;
		}

		ImGuiIO& io = ImGui::GetIO();
		RECT rt;
		GetClientRect(CGameInstance::GetInstance()->GetHWND(), &rt);
		POINT lt{ rt.left, rt.top };
		ClientToScreen(CGameInstance::GetInstance()->GetHWND(), &lt);
		ImGuizmo::SetRect((_float)lt.x, (_float)lt.y, io.DisplaySize.x, io.DisplaySize.y);

		_float4x4 matView, matProj;
		XMStoreFloat4x4(&matView, CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
		XMStoreFloat4x4(&matProj, CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

		ImGuizmo::Manipulate(
			reinterpret_cast<float*>(&matView),
			reinterpret_cast<float*>(&matProj),
			mCurrentGizmoOperation,
			mCurrentGizmoMode,
			reinterpret_cast<float*>(&m_WorldMatrix),
			nullptr, useSnap ? &snap[0] : nullptr);
	}
}

void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNaviCom)
{	
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vLook = Get_State(CTransform::STATE_LOOK);

	/* �̷��� ���� VlOOK�� Z�� �������� �����ϳ�. */
	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if(nullptr == pNaviCom)
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
	else
	{
		if(true == pNaviCom->isMove_OnNavigation(vPosition))
			Set_State(CTransform::STATE_TRANSLATION, vPosition);
	}
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vLook = Get_State(CTransform::STATE_LOOK);

	/* �̷��� ���� VlOOK�� Z�� �������� �����ϳ�. */
	vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vRight = Get_State(CTransform::STATE_RIGHT);

	/* �̷��� ���� VlOOK�� Z�� �������� �����ϳ�. */
	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vRight = Get_State(CTransform::STATE_RIGHT);

	/* �̷��� ���� VlOOK�� Z�� �������� �����ϳ�. */
	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Go_AxisY(_float fTimeDelta)
{
	_vector	vPos = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	vPos += XMVector3Normalize(vUp) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(CTransform::STATE_TRANSLATION, vPos);
}

void CTransform::Go_AxisNegY(_float fTimeDelta)
{
	_vector	vPos = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	vPos -= XMVector3Normalize(vUp) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(CTransform::STATE_TRANSLATION, vPos);
}

void CTransform::Speed_Boost(_bool bKeyState, _float fValue)
{
	if (bKeyState)
		m_TransformDesc.fSpeedPerSec = m_fInitSpeed * fValue;
	else
		m_TransformDesc.fSpeedPerSec = m_fInitSpeed;
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * fTimeDelta);

	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);	

	Set_State(CTransform::STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(CTransform::STATE_LOOK , XMVector4Transform(vLook, RotationMatrix));
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	_float3		vScale = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	Set_State(CTransform::STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));
}

void CTransform::LookAt(_fvector vTargetPos)
{
	_float3		vScale = Get_Scaled();

	_vector		vLook = XMVector3Normalize(vTargetPos - Get_State(CTransform::STATE_TRANSLATION)) * vScale.z;
	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;
	_vector		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vScale.y;

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);
}

void CTransform::Chase(_fvector vTargetPos, _float fTimeDelta, _float fLimit)
{
	_vector		vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector		vDir = vTargetPos - vPosition;

	_float		fDistance = XMVectorGetX(XMVector3Length(vDir));

	if(fDistance > fLimit)
	{
		vPosition += XMVector3Normalize(vDir) * m_TransformDesc.fSpeedPerSec * fTimeDelta;	
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
	}
}

HRESULT CTransform::Bind_ShaderResource(CShader * pShaderCom, const char * pConstantName)
{
	if (nullptr == pShaderCom)
		return E_FAIL;

	return pShaderCom->Set_Matrix(pConstantName, &m_WorldMatrix);		
}

CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTransform*		pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*		pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}

void CTransform::Set_OnTerrain(CVIBuffer_Terrain* pTerrainBuffer, _float fPlusValue)
{
	_vector vPos = Get_State(CTransform::STATE_TRANSLATION);
	_float fHeight = HeightOnTerrain(vPos, pTerrainBuffer->Get_VirticesPosPtr(), pTerrainBuffer->Get_NumVerticesX(), pTerrainBuffer->Get_NumVerticesZ());

	Set_State(CTransform::STATE_TRANSLATION, XMVectorSetY(vPos, fHeight + fPlusValue));
}

_float CTransform::HeightOnTerrain(_fvector vPos, _float3* pTerrainVtxPos, _uint iNumVerticesX, _uint iNumVerticesZ)
{
	_float4 vPosFloat;
	XMStoreFloat4(&vPosFloat, vPos);

	_uint iIndex = (_uint)vPosFloat.z * iNumVerticesX + (_uint)vPosFloat.x;
	_float fWidth = (vPosFloat.x - pTerrainVtxPos[iIndex + iNumVerticesX].x) / 1;
	_float fHeight = (pTerrainVtxPos[iIndex + iNumVerticesX].z - vPosFloat.z) / 1;

	_vector vPlane;
	if (fWidth > fHeight) // ������ �� �ﰢ��
	{
		vPlane = XMPlaneFromPoints(XMLoadFloat3(&pTerrainVtxPos[iIndex + iNumVerticesX]),
			XMLoadFloat3(&pTerrainVtxPos[iIndex + iNumVerticesX + 1]),
			XMLoadFloat3(&pTerrainVtxPos[iIndex + 1]));
	}
	else // ���� �Ʒ� �ﰢ��
	{
		vPlane = XMPlaneFromPoints(XMLoadFloat3(&pTerrainVtxPos[iIndex + iNumVerticesX]),
			XMLoadFloat3(&pTerrainVtxPos[iIndex + 1]),
			XMLoadFloat3(&pTerrainVtxPos[iIndex]));
	}

	_float4 vPlaneFloat;
	XMStoreFloat4(&vPlaneFloat, vPlane);

	// ax + by + cz + d = 0; // by = -ax - cz - d // y = (-ax - cz - d) / b
	return (-vPlaneFloat.x * vPosFloat.x - vPlaneFloat.z * vPosFloat.z - vPlaneFloat.w) / vPlaneFloat.y;
}