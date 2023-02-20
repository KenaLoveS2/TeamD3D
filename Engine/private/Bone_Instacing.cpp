#include "Bone_Instacing.h"

#include "Model_Instancing.h"

CBone_Instacing::CBone_Instacing()
{
}

CBone_Instacing::CBone_Instacing(const CBone_Instacing & rhs)
	: m_CombindTransformMatrix(rhs.m_CombindTransformMatrix)
	, m_OffsetMatrix(rhs.m_OffsetMatrix)
	, m_TransformMatrix(rhs.m_TransformMatrix)
{
	strcpy_s(m_szName, MAX_PATH, rhs.m_szName);
}

HRESULT CBone_Instacing::Initialize(CModel_Instancing * pModel, HANDLE hFile)
{
	DWORD   dwByte = 0;
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_CombindTransformMatrix, XMMatrixIdentity());

	ReadFile(hFile, m_szName, MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, &m_TransformMatrix, sizeof(_float4x4), &dwByte, nullptr);
	return S_OK;
}

void CBone_Instacing::Compute_CombindTransformationMatrix()
{
	if (nullptr == m_pParent)
		m_CombindTransformMatrix = m_TransformMatrix;
	else
		XMStoreFloat4x4(&m_CombindTransformMatrix, XMLoadFloat4x4(&m_TransformMatrix) * XMLoadFloat4x4(&m_pParent->m_CombindTransformMatrix));
}

CBone_Instacing * CBone_Instacing::Create(CModel_Instancing * pModel, HANDLE hFile)
{
	CBone_Instacing*		pInstance = new CBone_Instacing();

	if (FAILED(pInstance->Initialize(pModel, hFile)))
	{
		MSG_BOX("Failed to Created : CBone_Instacing");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CBone_Instacing::Free()
{
	Safe_Release(m_pParent);
}




