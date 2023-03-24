#include "stdafx.h"
#include "..\public\Channel.h"
#include "Model.h"
#include "Bone.h"
#include "GameObject.h"

CChannel::CChannel()
{
}

CChannel::CChannel(const CChannel& rhs)
	: m_pModel(rhs.m_pModel)
	, m_pBone(nullptr)
	, m_iNumKeyframes(rhs.m_iNumKeyframes)
	, m_KeyFrames(rhs.m_KeyFrames)
{
	strcpy_s(m_szName, rhs.m_szName);
}

HRESULT CChannel::Save_Channel(HANDLE & hFile, DWORD & dwByte)
{
	_uint			iNameLength = _uint(strlen(m_szName)) + 1;
	WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, m_szName, sizeof(char) * iNameLength, &dwByte, nullptr);

	WriteFile(hFile, &m_iNumKeyframes, sizeof(_uint), &dwByte, nullptr);

	for (auto& tKeyFrame : m_KeyFrames)
		WriteFile(hFile, &tKeyFrame, sizeof(KEYFRAME), &dwByte, nullptr);

	return S_OK;
}

HRESULT CChannel::Load_Channel(HANDLE & hFile, DWORD & dwByte)
{
	_uint			iNameLength = 0;
	ReadFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);

	char*			pName = new char[iNameLength];
	ReadFile(hFile, pName, sizeof(char) * iNameLength, &dwByte, nullptr);

	strcpy_s(m_szName, pName);

	m_pBone = m_pModel->Get_BonePtr(m_szName);
	Safe_AddRef(m_pBone);

	Safe_Delete_Array(pName);

	ReadFile(hFile, &m_iNumKeyframes, sizeof(_uint), &dwByte, nullptr);
	m_KeyFrames.reserve(m_iNumKeyframes);

	for (_uint i = 0; i < m_iNumKeyframes; ++i)
	{
		KEYFRAME		tKeyFrame;
		ZeroMemory(&tKeyFrame, sizeof(KEYFRAME));

		ReadFile(hFile, &tKeyFrame, sizeof(KEYFRAME), &dwByte, nullptr);

		m_KeyFrames.push_back(tKeyFrame);
	}

	return S_OK;
}


void CChannel::Set_BoneTranfromMatrix(_fmatrix matTransform)
{
	m_pBone->Set_TransformMatrix(matTransform);
}

_matrix CChannel::Get_BoneTransformMatrix()
{
	return m_pBone->Get_TransformMatrix();
}

_bool CChannel::Get_BoneLocked()
{
	return m_pBone->Get_BoneLocked();
}

/* 특정애니메이션ㄴ에서 사용되는 뼈. */
HRESULT CChannel::Initialize_Prototype(HANDLE hFile, CModel* pModel)
{
	m_pModel = pModel;

	if (hFile == nullptr)
		return S_OK;

	_ulong dwByte = 0;
	_uint iLen = 0;
	ReadFile(hFile, &iLen, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, m_szName, iLen + 1, &dwByte, nullptr);

	ReadFile(hFile, &m_iNumKeyframes, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < m_iNumKeyframes; i++)
	{
		KEYFRAME KeyFrame;
		ReadFile(hFile, &KeyFrame, sizeof(KEYFRAME), &dwByte, nullptr);
		m_KeyFrames.push_back(KeyFrame);
	}

	if (FAILED(SetUp_BonePtr(pModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CChannel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CChannel::SetUp_BonePtr(CModel* pModel)
{
	m_pBone = pModel->Get_BonePtr(m_szName);
	if (m_pBone == nullptr) return E_FAIL;
	m_pModel = pModel;
	Safe_AddRef(m_pBone);
	return S_OK;
}

HRESULT CChannel::Synchronization_BonePtr(CModel * pModel)
{
	m_pBone = pModel->Get_BonePtr(m_szName);
	if (m_pBone == nullptr) return S_FALSE;

	Safe_AddRef(m_pBone);
	return S_OK;
}

void CChannel::Update_TransformMatrix(_float PlayTime, _bool isRootBone)
{
	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition;

	_matrix			TransformMatrix;

	if (PlayTime >= m_KeyFrames.back().Time)
	{
		vScale = XMLoadFloat3(&m_KeyFrames.back().vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames.back().vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames.back().vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}
	else
	{
		while (PlayTime >= m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time)
		{
			++m_iCurrentKeyFrameIndex;
		}

		_float				Ratio = _float((PlayTime - m_KeyFrames[m_iCurrentKeyFrameIndex].Time) / 
			(m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time - m_KeyFrames[m_iCurrentKeyFrameIndex].Time));

		_vector			vSourScale, vDestScale;
		_vector			vSourRotation, vDestRotation;
		_vector			vSourPosition, vDestPosition;

		vSourScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vScale);
		vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex].vRotation);
		vSourPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vPosition);

		vDestScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vScale);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vRotation);
		vDestPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vPosition);

		vScale = XMVectorLerp(vSourScale, vDestScale, Ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, Ratio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, Ratio);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}

	if (isRootBone == false)
	{
		TransformMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

		m_pBone->Set_TransformMatrix(TransformMatrix);
	}
	else
	{
		CGameObject*	pOwner = m_pModel->Get_Owner();
		_vector			vBasePos = m_vRootBonePos;
		m_vRootBonePos = vPosition;
		_matrix			matTransform = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * pOwner->Get_WorldMatrix();
		_vector			vDisplacement = XMVector3TransformCoord(vPosition, matTransform) - XMVector3TransformCoord(vBasePos, matTransform);

		pOwner->Calc_RootBoneDisplacement(vDisplacement);

		matTransform = XMMatrixRotationX(XMConvertToRadians(-90.f));

		_vector	vSc, vRot, vTrans;
		XMMatrixDecompose(&vSc, &vRot, &vTrans, matTransform);

		TransformMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRot, XMVectorSet(0.f, 0.f, 0.f, 1.f));
		//TransformMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
		m_pBone->Set_TransformMatrix(TransformMatrix);
	}
}

void CChannel::Blend_TransformMatrix(_float PlayTime, _float fBlendRadio, _bool isRootBone)
{
	_vector vBaseScale, vBaseRot, vBasePos;
	XMMatrixDecompose(&vBaseScale, &vBaseRot, &vBasePos, m_pBone->Get_TransformMatrix());

	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition;

	if (PlayTime >= m_KeyFrames.back().Time)
	{
		vScale = XMLoadFloat3(&m_KeyFrames.back().vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames.back().vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames.back().vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}
	else
	{
		while (PlayTime >= m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time)
		{
			++m_iCurrentKeyFrameIndex;
		}

		_float				Ratio = _float((PlayTime - m_KeyFrames[m_iCurrentKeyFrameIndex].Time) /
			(m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time - m_KeyFrames[m_iCurrentKeyFrameIndex].Time));

		_vector			vSourScale, vDestScale;
		_vector			vSourRotation, vDestRotation;
		_vector			vSourPosition, vDestPosition;

		vSourScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vScale);
		vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex].vRotation);
		vSourPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vPosition);

		vDestScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vScale);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vRotation);
		vDestPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vPosition);

		vScale = XMVectorLerp(vSourScale, vDestScale, Ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, Ratio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, Ratio);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}

	vScale = XMVectorLerp(vBaseScale, vScale, fBlendRadio);
	vRotation = XMQuaternionSlerp(vBaseRot, vRotation, fBlendRadio);
	vPosition = XMVectorLerp(vBasePos, vPosition, fBlendRadio);
	vPosition = XMVectorSetW(vPosition, 1.f);

	if (isRootBone == false)
	{
		_matrix TransformMatrix = XMMatrixAffineTransformation(vScale, XMQuaternionIdentity(), vRotation, vPosition);

		m_pBone->Set_TransformMatrix(TransformMatrix);
	}
	else
	{
		CGameObject*	pOwner = m_pModel->Get_Owner();
		_matrix			matTransform = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * pOwner->Get_WorldMatrix();
		_vector			vPrePos = m_vRootBonePos;
		m_vRootBonePos = vPosition;
		_vector			vDisplacement = XMVector3TransformCoord(vPosition, matTransform) - XMVector3TransformCoord(vPrePos, matTransform);

		pOwner->Calc_RootBoneDisplacement(vDisplacement);

		_matrix TransformMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, XMVectorSet(0.f, 0.f, 0.f, 1.f));
		m_pBone->Set_TransformMatrix(TransformMatrix);
	}
}

void CChannel::Additive_TransformMatrix(_float PlayTime, _float fAdditiveRatio, _bool isRootBone)
{
	_vector vBaseScale, vBaseRot, vBasePos;
	XMMatrixDecompose(&vBaseScale, &vBaseRot, &vBasePos, m_pBone->Get_TransformMatrix());

	_vector			vRotation;

	if (PlayTime >= m_KeyFrames.back().Time)
		vRotation = XMLoadFloat4(&m_KeyFrames.back().vRotation);
	else
	{
		while (PlayTime >= m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time)
			++m_iCurrentKeyFrameIndex;

		_float				Ratio = _float((PlayTime - m_KeyFrames[m_iCurrentKeyFrameIndex].Time) /
			(m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time - m_KeyFrames[m_iCurrentKeyFrameIndex].Time));

		_vector			vSourRotation, vDestRotation;

		vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex].vRotation);

		vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vRotation);

		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, Ratio);
	}

	vRotation = XMQuaternionSlerp(vBaseRot, XMQuaternionMultiply(vBaseRot, vRotation), fAdditiveRatio);
	
	if (isRootBone == false)
	{
		_matrix TransformMatrix = XMMatrixAffineTransformation(vBaseScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vBasePos);

		m_pBone->Set_TransformMatrix(TransformMatrix);
	}
}

void CChannel::Additive_TransformMatrixForMonster(_float PlayTime, _float fAdditiveRatio, _bool isRootBone)
{
	_vector vBaseScale, vBaseRot, vBasePos;
	XMMatrixDecompose(&vBaseScale, &vBaseRot, &vBasePos, m_pBone->Get_TransformMatrix());

	_vector			vRotation;

	if (PlayTime >= m_KeyFrames.back().Time)
	{
		vRotation = XMLoadFloat4(&m_KeyFrames.back().vRotation);
	}
	else
	{
		while (PlayTime >= m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time)
		{
			++m_iCurrentKeyFrameIndex;
		}

		_double			Ratio = (PlayTime - m_KeyFrames[m_iCurrentKeyFrameIndex].Time) /
			(m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time - m_KeyFrames[m_iCurrentKeyFrameIndex].Time);

		_vector			vSourRotation, vDestRotation;
		vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex].vRotation);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vRotation);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (float)Ratio);
	}

	vRotation = XMQuaternionSlerp(XMQuaternionIdentity(), vRotation, fAdditiveRatio);
	vRotation = XMQuaternionMultiply(vBaseRot, vRotation);

	_matrix TransformMatrix = XMMatrixAffineTransformation(vBaseScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vBasePos);

	m_pBone->Set_TransformMatrix(TransformMatrix);
}

void CChannel::Update_TransformMatrix_ReturnMat(_float PlayTime, _smatrix & matBonesTransfomation, _bool isRootBone, CChannel * pBlendChannel)
{
// 	if (m_pBone->Get_BoneLocked() == true)
// 	{
// 		if (isRootBone == true)
// 		{
// 			matBonesTransfomation = XMMatrixIdentity();
// 			m_pBone->Set_TransformMatrix(XMMatrixIdentity());
// 			return;
// 		}
// 		if (m_pBone->Get_BoneRotateLocked() == false && m_pBone->Get_BonePositioinLocked() == false)
// 			return;
// 	}

	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition;

	if (PlayTime >= m_KeyFrames.back().Time)
	{
		vScale = XMLoadFloat3(&m_KeyFrames.back().vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames.back().vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames.back().vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);

		if (pBlendChannel != nullptr)
		{
			vScale = (XMLoadFloat3(&m_KeyFrames.back().vScale) + XMLoadFloat3(&pBlendChannel->m_KeyFrames.back().vScale)) * 0.5f;
			vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames.back().vRotation), XMLoadFloat4(&pBlendChannel->m_KeyFrames.back().vRotation), 0.5f);
			vPosition = (XMLoadFloat3(&m_KeyFrames.back().vPosition) + XMLoadFloat3(&pBlendChannel->m_KeyFrames.back().vPosition)) * 0.5f;
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
	}
	else
	{
		while (PlayTime >= m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time)
		{
			++m_iCurrentKeyFrameIndex;
		}

		_float				Ratio = _float((PlayTime - m_KeyFrames[m_iCurrentKeyFrameIndex].Time) /
			(m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time - m_KeyFrames[m_iCurrentKeyFrameIndex].Time));

		_vector			vSourScale, vDestScale;
		_vector			vSourRotation, vDestRotation;
		_vector			vSourPosition, vDestPosition;

		vSourScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vScale);
		vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex].vRotation);
		vSourPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vPosition);

		vDestScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vScale);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vRotation);
		vDestPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vPosition);

		vScale = XMVectorLerp(vSourScale, vDestScale, Ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, Ratio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, Ratio);
		vPosition = XMVectorSetW(vPosition, 1.f);

		if (pBlendChannel != nullptr)
		{
			while (PlayTime >= pBlendChannel->m_KeyFrames[pBlendChannel->m_iCurrentKeyFrameIndex + 1].Time)
			{
				++pBlendChannel->m_iCurrentKeyFrameIndex;
			}

			Ratio = _float((PlayTime - pBlendChannel->m_KeyFrames[pBlendChannel->m_iCurrentKeyFrameIndex].Time) /
				(pBlendChannel->m_KeyFrames[pBlendChannel->m_iCurrentKeyFrameIndex + 1].Time - pBlendChannel->m_KeyFrames[pBlendChannel->m_iCurrentKeyFrameIndex].Time));

			vSourScale = XMLoadFloat3(&pBlendChannel->m_KeyFrames[pBlendChannel->m_iCurrentKeyFrameIndex].vScale);
			vSourRotation = XMLoadFloat4(&pBlendChannel->m_KeyFrames[pBlendChannel->m_iCurrentKeyFrameIndex].vRotation);
			vSourPosition = XMLoadFloat3(&pBlendChannel->m_KeyFrames[pBlendChannel->m_iCurrentKeyFrameIndex].vPosition);

			vDestScale = XMLoadFloat3(&pBlendChannel->m_KeyFrames[pBlendChannel->m_iCurrentKeyFrameIndex + 1].vScale);
			vDestRotation = XMLoadFloat4(&pBlendChannel->m_KeyFrames[pBlendChannel->m_iCurrentKeyFrameIndex + 1].vRotation);
			vDestPosition = XMLoadFloat3(&pBlendChannel->m_KeyFrames[pBlendChannel->m_iCurrentKeyFrameIndex + 1].vPosition);

			vScale = (vScale + XMVectorLerp(vSourScale, vDestScale, Ratio)) * 0.5f;
			vRotation = XMQuaternionSlerp(XMQuaternionSlerp(vSourRotation, vDestRotation, Ratio), vRotation, 0.5f);
			vPosition = (vPosition + XMVectorLerp(vSourPosition, vDestPosition, Ratio)) * 0.5f;
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
	}

	if (isRootBone == false)
	{
		if (m_pBone->Get_BoneRotateLocked() == false && m_pBone->Get_BonePositioinLocked() == false)
			matBonesTransfomation = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
		else if (m_pBone->Get_BoneRotateLocked() == true && m_pBone->Get_BonePositioinLocked() == false)
		{
			_vector	vSc, vRot, vTrans;
			XMMatrixDecompose(&vSc, &vRot, &vTrans, matBonesTransfomation);
			matBonesTransfomation = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRot, vPosition);
		}
		else if (m_pBone->Get_BoneRotateLocked() == false && m_pBone->Get_BonePositioinLocked() == true)
		{
			_vector	vSc, vRot, vTrans;
			XMMatrixDecompose(&vSc, &vRot, &vTrans, matBonesTransfomation);
			matBonesTransfomation = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTrans);
		}
		else
		{
			_vector	vSc, vRot, vTrans;
			XMMatrixDecompose(&vSc, &vRot, &vTrans, matBonesTransfomation);
			matBonesTransfomation = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRot, vTrans);
		}

		m_pBone->Set_TransformMatrix(matBonesTransfomation);
	}
	else
	{
		CGameObject*	pOwner = m_pModel->Get_Owner();
		_vector			vBasePos = m_vRootBonePos;
		m_vRootBonePos = vPosition;
		_matrix			matTransform = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * pOwner->Get_WorldMatrix();
		_vector			vDisplacement = XMVector3TransformCoord(vPosition, matTransform) - XMVector3TransformCoord(vBasePos, matTransform);

		pOwner->Calc_RootBoneDisplacement(vDisplacement);

		matTransform = XMMatrixRotationX(XMConvertToRadians(-90.f));
		
		_vector	vSc, vRot, vTrans;
		XMMatrixDecompose(&vSc, &vRot, &vTrans, matTransform);

		matBonesTransfomation = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, XMVectorSet(0.f, 0.f, 0.f, 1.f));
		m_pBone->Set_TransformMatrix(matBonesTransfomation);
	}
}

void CChannel::Blend_TransformMatrix_ReturnMat(_float PlayTime, _float fBlendRadio, _smatrix & matBonesTransfomation, _bool isRootBone, CChannel * pBlendChannel)
{
	_vector vBaseScale, vBaseRot, vBasePos;
	XMMatrixDecompose(&vBaseScale, &vBaseRot, &vBasePos, matBonesTransfomation);

	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition;

	if (PlayTime >= m_KeyFrames.back().Time)
	{
		vScale = XMLoadFloat3(&m_KeyFrames.back().vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames.back().vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames.back().vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);

		if (pBlendChannel != nullptr)
		{
			vScale = (XMLoadFloat3(&m_KeyFrames.back().vScale) + XMLoadFloat3(&pBlendChannel->m_KeyFrames.back().vScale)) * 0.5f;
			vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames.back().vRotation), XMLoadFloat4(&pBlendChannel->m_KeyFrames.back().vRotation), 0.5f);
			vPosition = (XMLoadFloat3(&m_KeyFrames.back().vPosition) + XMLoadFloat3(&pBlendChannel->m_KeyFrames.back().vPosition)) * 0.5f;
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
	}
	else
	{
		while (PlayTime >= m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time)
		{
			++m_iCurrentKeyFrameIndex;
		}

		_float				Ratio = _float((PlayTime - m_KeyFrames[m_iCurrentKeyFrameIndex].Time) /
			(m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time - m_KeyFrames[m_iCurrentKeyFrameIndex].Time));

		_vector			vSourScale, vDestScale;
		_vector			vSourRotation, vDestRotation;
		_vector			vSourPosition, vDestPosition;

		vSourScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vScale);
		vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex].vRotation);
		vSourPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vPosition);

		vDestScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vScale);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vRotation);
		vDestPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vPosition);

		vScale = XMVectorLerp(vSourScale, vDestScale, Ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, Ratio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, Ratio);
		vPosition = XMVectorSetW(vPosition, 1.f);

		if (pBlendChannel != nullptr)
		{
			while (PlayTime >= pBlendChannel->m_KeyFrames[pBlendChannel->m_iCurrentKeyFrameIndex + 1].Time)
			{
				++pBlendChannel->m_iCurrentKeyFrameIndex;
			}

			Ratio = _float((PlayTime - pBlendChannel->m_KeyFrames[pBlendChannel->m_iCurrentKeyFrameIndex].Time) /
				(pBlendChannel->m_KeyFrames[pBlendChannel->m_iCurrentKeyFrameIndex + 1].Time - pBlendChannel->m_KeyFrames[pBlendChannel->m_iCurrentKeyFrameIndex].Time));

			vSourScale = XMLoadFloat3(&pBlendChannel->m_KeyFrames[pBlendChannel->m_iCurrentKeyFrameIndex].vScale);
			vSourRotation = XMLoadFloat4(&pBlendChannel->m_KeyFrames[pBlendChannel->m_iCurrentKeyFrameIndex].vRotation);
			vSourPosition = XMLoadFloat3(&pBlendChannel->m_KeyFrames[pBlendChannel->m_iCurrentKeyFrameIndex].vPosition);

			vDestScale = XMLoadFloat3(&pBlendChannel->m_KeyFrames[pBlendChannel->m_iCurrentKeyFrameIndex + 1].vScale);
			vDestRotation = XMLoadFloat4(&pBlendChannel->m_KeyFrames[pBlendChannel->m_iCurrentKeyFrameIndex + 1].vRotation);
			vDestPosition = XMLoadFloat3(&pBlendChannel->m_KeyFrames[pBlendChannel->m_iCurrentKeyFrameIndex + 1].vPosition);

			vScale = (vScale + XMVectorLerp(vSourScale, vDestScale, Ratio)) * 0.5f;
			vRotation = XMQuaternionSlerp(XMQuaternionSlerp(vSourRotation, vDestRotation, Ratio), vRotation, 0.5f);
			vPosition = (vPosition + XMVectorLerp(vSourPosition, vDestPosition, Ratio)) * 0.5f;
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
	}

	vScale = XMVectorLerp(vBaseScale, vScale, fBlendRadio);
	vRotation = XMQuaternionSlerp(vBaseRot, vRotation, fBlendRadio);
	vPosition = XMVectorLerp(vBasePos, vPosition, fBlendRadio);
	vPosition = XMVectorSetW(vPosition, 1.f);

	if (isRootBone == false)
	{
		matBonesTransfomation = XMMatrixAffineTransformation(vScale, XMQuaternionIdentity(), vRotation, vPosition);
		m_pBone->Set_TransformMatrix(matBonesTransfomation);
	}
	else
	{
		CGameObject*	pOwner = m_pModel->Get_Owner();
		_vector			vPrePos = m_vRootBonePos;
		m_vRootBonePos = vPosition;
		_matrix			matTransform = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * pOwner->Get_WorldMatrix();
		_vector			vDisplacement = XMVector3TransformCoord(vPosition, matTransform) - XMVector3TransformCoord(vPrePos, matTransform);

		pOwner->Calc_RootBoneDisplacement(vDisplacement);

		matTransform = XMMatrixRotationX(XMConvertToRadians(-90.f));

		_vector	vSc, vRot, vTrans;
		XMMatrixDecompose(&vSc, &vRot, &vTrans, matTransform);

		matBonesTransfomation = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, XMVectorSet(0.f, 0.f, 0.f, 1.f));
		m_pBone->Set_TransformMatrix(matBonesTransfomation);
	}
}

void CChannel::Additive_TransformMatrix_ReturnMat(_float PlayTime, _float fAdditiveRatio, _smatrix & matBonesTransfomation, _bool isRootBone)
{
	_vector vBaseScale, vBaseRot, vBasePos;
	XMMatrixDecompose(&vBaseScale, &vBaseRot, &vBasePos, matBonesTransfomation);

	_vector			vScale;
	_vector			vPosition;
	_vector			vRotation;

	if (PlayTime >= m_KeyFrames.back().Time)
	{
		vScale = XMLoadFloat3(&m_KeyFrames.back().vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames.back().vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames.back().vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}
	else
	{
		while (PlayTime >= m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time)
			++m_iCurrentKeyFrameIndex;

		_float				Ratio = _float((PlayTime - m_KeyFrames[m_iCurrentKeyFrameIndex].Time) /
			(m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time - m_KeyFrames[m_iCurrentKeyFrameIndex].Time));

		_vector			vSourScale, vDestScale;
		_vector			vSourRotation, vDestRotation;
		_vector			vSourPosition, vDestPosition;

		vSourScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vScale);
		vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex].vRotation);
		vSourPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vPosition);

		vDestScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vScale);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vRotation);
		vDestPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex+ 1].vPosition);

		vScale = XMVectorLerp(vSourScale, vDestScale, Ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, Ratio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, Ratio);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}

	//vRotation = XMQuaternionSlerp(XMQuaternionIdentity(), vRotation, fAdditiveRatio);
	//vRotation = XMQuaternionMultiply(vBaseRot, vRotation);

	vScale = XMVectorLerp(vBaseScale, vBaseScale * vScale, fAdditiveRatio);
	vRotation = XMQuaternionSlerp(vBaseRot, XMQuaternionMultiply(vBaseRot, vRotation), fAdditiveRatio);
	vPosition = XMVectorLerp(vBasePos, vBasePos + vPosition, fAdditiveRatio);
	vPosition = XMVectorSetW(vPosition, 1.f);

	if (isRootBone == false)
	{
// 		if (m_pBone->Get_BoneRotateLocked() == false && m_pBone->Get_BonePositioinLocked() == false)
// 		{
// 			matBonesTransfomation = XMMatrixAffineTransformation(vBaseScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vBasePos);
// 		}
// 		else if (m_pBone->Get_BoneRotateLocked() == true && m_pBone->Get_BonePositioinLocked() == false)
// 		{
// 			_vector	vSc, vRot, vTrans;
// 			XMMatrixDecompose(&vSc, &vRot, &vTrans, matBonesTransfomation);
// 
// 			matBonesTransfomation = XMMatrixAffineTransformation(vBaseScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTrans);
// 		}

		if (m_pBone->Get_BoneRotateLocked() == false && m_pBone->Get_BonePositioinLocked() == false)
			matBonesTransfomation = XMMatrixAffineTransformation(vBaseScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
		else if (m_pBone->Get_BoneRotateLocked() == true && m_pBone->Get_BonePositioinLocked() == false)
			matBonesTransfomation = XMMatrixAffineTransformation(vBaseScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vBaseRot, vPosition);
		else if (m_pBone->Get_BoneRotateLocked() == false && m_pBone->Get_BonePositioinLocked() == true)
			matBonesTransfomation = XMMatrixAffineTransformation(vBaseScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vBasePos);
		else
			matBonesTransfomation = XMMatrixAffineTransformation(vBaseScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vBaseRot, vBasePos);

		m_pBone->Set_TransformMatrix(matBonesTransfomation);
	}
	else
	{
		_matrix matTransform = XMMatrixRotationX(XMConvertToRadians(-90.f));

		_vector	vSc, vRot, vTrans;
		XMMatrixDecompose(&vSc, &vRot, &vTrans, matTransform);

		matBonesTransfomation = XMMatrixAffineTransformation(vBaseScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vBaseRot, vBasePos);
		m_pBone->Set_TransformMatrix(matBonesTransfomation);
// 		CGameObject*	pOwner = m_pModel->Get_Owner();
// 		_vector			vPrePos = m_vRootBonePos;
// 		m_vRootBonePos = vPosition;
// 		_matrix			matTransform = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * pOwner->Get_WorldMatrix();
// 		_vector			vDisplacement = XMVector3TransformCoord(vPosition, matTransform) - XMVector3TransformCoord(vPrePos, matTransform);
// 
// 		pOwner->Calc_RootBoneDisplacement(vDisplacement);
// 
// 		matBonesTransfomation = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, XMVectorSet(0.f, 0.f, 0.f, 1.f));
// 		m_pBone->Set_TransformMatrix(matBonesTransfomation);
	}
}

void CChannel::Reset_KeyFrameIndex()
{
	m_iCurrentKeyFrameIndex = 0;
	m_vRootBonePos = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[0].vPosition), 1.f);
}

void CChannel::Set_KeyFrameIndex(_double dPlayTime)
{
	m_iCurrentKeyFrameIndex = 0;

	while (dPlayTime >= m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time)
		++m_iCurrentKeyFrameIndex;
}

CChannel * CChannel::Create(HANDLE hFile, CModel* pModel)
{
	CChannel*		pInstance = new CChannel();

	if (FAILED(pInstance->Initialize_Prototype(hFile, pModel)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CChannel* CChannel::Clone(void* pArg)
{
	CChannel* pInstance = new CChannel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChannel::Free()
{
	Safe_Release(m_pBone);

	m_KeyFrames.clear();
}
