#include "..\public\Bone.h"

CBone::CBone()
{
}

CBone::CBone(const CBone& rhs)
{
	strcpy_s(m_szName, rhs.m_szName);
	m_OffsetMatrix = rhs.m_OffsetMatrix;
	m_TransformMatrix = rhs.m_TransformMatrix;
	m_CombindTransformMatrix = rhs.m_CombindTransformMatrix;

	m_pParent = nullptr;
	strcpy_s(m_szParentName, rhs.m_szParentName);
}

HRESULT CBone::Initialize_Prototype(HANDLE hFile)
{
	_ulong dwByte = 0;
	_uint iLen = 0;
	ReadFile(hFile, &iLen, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, m_szName, iLen + 1, &dwByte, nullptr);
	ReadFile(hFile, &m_OffsetMatrix, sizeof(_float4x4), &dwByte, nullptr);
	ReadFile(hFile, &m_TransformMatrix, sizeof(_float4x4), &dwByte, nullptr);

	_bool bParentFlag = false;
	ReadFile(hFile, &bParentFlag, sizeof(_bool), &dwByte, nullptr);
	if (bParentFlag)
	{
		ReadFile(hFile, &iLen, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, m_szParentName, iLen + 1, &dwByte, nullptr);
	}

	m_pParent = nullptr;

	return S_OK;
}

HRESULT CBone::Initialize(void* pArg)
{
	return S_OK;
}

void CBone::Compute_CombindTransformationMatrix()
{
	if (nullptr == m_pParent)
		m_CombindTransformMatrix = m_TransformMatrix;

	else
		XMStoreFloat4x4(&m_CombindTransformMatrix, XMLoadFloat4x4(&m_TransformMatrix) 
			* XMLoadFloat4x4(&m_pParent->m_CombindTransformMatrix));

}

CBone * CBone::Create(HANDLE hFile)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize_Prototype(hFile)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CBone * CBone::Clone(void * pArg)
{
	CBone* pInstance = new CBone(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBone::Free()
{
	Safe_Release(m_pParent);
}

HRESULT CBone::SetParent(CBone* pParent)
{
	if (m_pParent)
		return E_FAIL;

	m_pParent = pParent;
	Safe_AddRef(m_pParent);

	return S_OK;
}
