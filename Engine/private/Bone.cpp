#include "stdafx.h"
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

HRESULT CBone::Save_Bone(HANDLE & hFile, DWORD & dwByte)
{
	_uint			iNameLength = _uint(strlen(m_szName)) + 1;
	WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, m_szName, sizeof(char) * iNameLength, &dwByte, nullptr);

	WriteFile(hFile, &m_OffsetMatrix, sizeof(_float4x4), &dwByte, nullptr);
	WriteFile(hFile, &m_TransformMatrix, sizeof(_float4x4), &dwByte, nullptr);

	_bool bParentFlag = m_pParent != nullptr;
	WriteFile(hFile, &bParentFlag, sizeof(_bool), &dwByte, nullptr);
	if (bParentFlag)
	{
		iNameLength = _uint(strlen(m_pParent->m_szName));
		WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, m_pParent->m_szName, iNameLength + 1, &dwByte, nullptr);
	}

	return S_OK;
}

HRESULT CBone::Save_BoneName(HANDLE & hFile, DWORD & dwByte)
{
	_uint			iNameLength = _uint(strlen(m_szName)) + 1;
	WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, m_szName, sizeof(char) * iNameLength, &dwByte, nullptr);

	return S_OK;
}

HRESULT CBone::Load_Bone(HANDLE & hFile, DWORD & dwByte)
{
	_uint			iNameLength = 0;
	ReadFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);

	char*			pName = new char[iNameLength];
	ReadFile(hFile, pName, sizeof(char) * iNameLength, &dwByte, nullptr);

	strcpy_s(m_szName, pName);

	Safe_Delete_Array(pName);

	ReadFile(hFile, &m_OffsetMatrix, sizeof(_float4x4), &dwByte, nullptr);
	ReadFile(hFile, &m_TransformMatrix, sizeof(_float4x4), &dwByte, nullptr);

	_bool bParentFlag = false;
	ReadFile(hFile, &bParentFlag, sizeof(_bool), &dwByte, nullptr);
	if (bParentFlag)
	{
		ReadFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, m_szParentName, iNameLength + 1, &dwByte, nullptr);
	}

	m_pParent = nullptr;

	return S_OK;
}

HRESULT CBone::Initialize_Prototype(HANDLE hFile)
{
	if (hFile == nullptr)
		return S_OK;

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
