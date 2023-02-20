#include "..\public\Channel.h"
#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}

CChannel::CChannel(const CChannel& rhs)
{
	strcpy_s(m_szName, rhs.m_szName);
	m_pBone = nullptr;
	m_iNumKeyframes = rhs.m_iNumKeyframes;
	m_KeyFrames = rhs.m_KeyFrames;
}


/* 특정애니메이션ㄴ에서 사용되는 뼈. */
HRESULT CChannel::Initialize_Prototype(HANDLE hFile, CModel* pModel)
{
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

	Safe_AddRef(m_pBone);
	return S_OK;
}

/* 현재 애니메이션이 재생된 시간을 얻어온다. PlayTime */
void CChannel::Update_TransformMatrix(_double PlayTime)
{
	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition;

	_matrix			TransformMatrix;

	/* 현재 재생된 시간이 마지막 키프레임시간보다 커지며.ㄴ */
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

		_double			Ratio = (PlayTime - m_KeyFrames[m_iCurrentKeyFrameIndex].Time) / 
			(m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time - m_KeyFrames[m_iCurrentKeyFrameIndex].Time);

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

	TransformMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	m_pBone->Set_TransformMatrix(TransformMatrix);	
}

void CChannel::Blend_TransformMatrix(_float PlayTime, _float fBlendRadio)
{
	_vector vBaseScale, vBaseRot, vBasePos;
	XMMatrixDecompose(&vBaseScale, &vBaseRot, &vBasePos, m_pBone->Get_TransformMatrix());

	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition;

	/* 현재 재생된 시간이 마지막 키프레임시간보다 커지며.ㄴ */
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

		_double			Ratio = (PlayTime - m_KeyFrames[m_iCurrentKeyFrameIndex].Time) /
			(m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time - m_KeyFrames[m_iCurrentKeyFrameIndex].Time);

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


	_matrix TransformMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	m_pBone->Set_TransformMatrix(TransformMatrix);
}

void CChannel::Additive_TransformMatrix(_double PlayTime, _float fAdditiveRadio)
{
	_vector vBaseScale, vBaseRot, vBasePos;
	XMMatrixDecompose(&vBaseScale, &vBaseRot, &vBasePos, m_pBone->Get_TransformMatrix());

	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition;

	/* 현재 재생된 시간이 마지막 키프레임시간보다 커지며.ㄴ */
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

		_double			Ratio = (PlayTime - m_KeyFrames[m_iCurrentKeyFrameIndex].Time) /
			(m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time - m_KeyFrames[m_iCurrentKeyFrameIndex].Time);

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

	//vScale = XMVectorLerp(vBaseScale, vScale, fBlendRadio);
	//vRotation = XMQuaternionSlerp(vBaseRot, vRotation, fBlendRadio);
	vRotation = XMQuaternionSlerp(XMQuaternionIdentity(), vRotation, fAdditiveRadio);
	vRotation = XMQuaternionMultiply(vBaseRot, vRotation);

	//vPosition = XMVectorLerp(vBasePos, vPosition, fBlendRadio);
	//vPosition = XMVectorSetW(vPosition, 1.f);

	_matrix TransformMatrix = XMMatrixAffineTransformation(vBaseScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vBasePos);

	m_pBone->Set_TransformMatrix(TransformMatrix);
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
