#include "stdafx.h"
#include "..\public\PipeLine.h"

IMPLEMENT_SINGLETON(CPipeLine)

CPipeLine::CPipeLine()
{
}

_matrix CPipeLine::Get_TransformMatrix(TRANSFORMSTATE eState) const
{
	return XMLoadFloat4x4(&m_TransformMatrices[eState]);	
}

_float4x4 CPipeLine::Get_TransformFloat4x4(TRANSFORMSTATE eState) const
{
	return m_TransformMatrices[eState];
}

_float4x4 CPipeLine::Get_TransformFloat4x4_Inverse(TRANSFORMSTATE eState) const
{
	return m_TransformMatrices_Inverse[eState];
}

_matrix CPipeLine::Get_TransformMatrix_Inverse(TRANSFORMSTATE eState) const
{
	return XMLoadFloat4x4(&m_TransformMatrices_Inverse[eState]);
}


_float4 CPipeLine::Get_CamRight_Float4()
{
	_float4 vRight;
	memcpy(&vRight, &m_TransformMatrices_Inverse[D3DTS_VIEW].m[0][0], sizeof(_float4));
	
	return vRight;
}

_float4 CPipeLine::Get_CamUp_Float4()
{
	_float4 vUp;
	memcpy(&vUp, &m_TransformMatrices_Inverse[D3DTS_VIEW].m[1][0], sizeof(_float4));

	return vUp;
}

_float4 CPipeLine::Get_CamLook_Float4()
{
	_float4 vRight;
	memcpy(&vRight, &m_TransformMatrices_Inverse[D3DTS_VIEW].m[2][0], sizeof(_float4));

	return vRight;
}

_float3 CPipeLine::Get_CamRight_Float3()
{
	_float3 vRight;
	memcpy(&vRight, &m_TransformMatrices_Inverse[D3DTS_VIEW].m[0][0], sizeof(_float3));

	return vRight;
}

_float3 CPipeLine::Get_CamUp_Float3()
{
	_float3 vUp;
	memcpy(&vUp, &m_TransformMatrices_Inverse[D3DTS_VIEW].m[1][0], sizeof(_float3));

	return vUp;
}

_float3 CPipeLine::Get_CamLook_Float3()
{
	_float3 vLook;
	memcpy(&vLook, &m_TransformMatrices_Inverse[D3DTS_VIEW].m[2][0], sizeof(_float3));

	return vLook;
}

_float3 CPipeLine::Get_CamPosition_Float3()
{	
	return _float3(m_vCamPosition.x, m_vCamPosition.y, m_vCamPosition.z);
}


void CPipeLine::Set_Transform(TRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	XMStoreFloat4x4(&m_TransformMatrices[eState], TransformMatrix);
}

void CPipeLine::Tick()
{
	XMStoreFloat4x4(&m_TransformMatrices_Inverse[D3DTS_VIEW], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrices[D3DTS_VIEW])));
	XMStoreFloat4x4(&m_TransformMatrices_Inverse[D3DTS_PROJ], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrices[D3DTS_PROJ])));
	memcpy(&m_vCamPosition, &m_TransformMatrices_Inverse[D3DTS_VIEW].m[3][0], sizeof(_float4));
}

void CPipeLine::Free()
{
}
