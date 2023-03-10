#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CBone final : public CBase
{
public:
	enum LOCKTO { LOCKTO_CHILD, LOCKTO_PARENT, LOCKTO_ALONE, LOCKTO_ROTATE, UNLOCKTO_CHILD, UNLOCKTO_PARENT, UNLOCKTO_ALONE, LOCKTO_END };

private:
	CBone();
	CBone(const CBone& rhs);
	virtual ~CBone() = default;

public:
	HRESULT				Save_Bone(HANDLE& hFile, DWORD& dwByte);
	HRESULT				Save_BoneName(HANDLE& hFile, DWORD& dwByte);
	HRESULT				Load_Bone(HANDLE& hFile, DWORD& dwByte);
	void						Add_Child(CBone* pBone) { m_vecChild.push_back(pBone); }
	CBone*					Get_ParentBonePtr() { return m_pParent; }
	const char*			Get_Name() const { return m_szName; }
	_matrix					Get_CombindMatrix() { return XMLoadFloat4x4(&m_CombindTransformMatrix); }
	_matrix					Get_OffsetMatrix() { return XMLoadFloat4x4(&m_OffsetMatrix); }
	_matrix					Get_TransformMatrix() { return XMLoadFloat4x4(&m_TransformMatrix); }
	const _bool&		Get_BoneLocked() const { return m_bLock; }
	const _bool&		Get_BoneRotateLocked() const { return m_bRotateLock; }
	void						Set_CombindMatrix(_fmatrix CombindMatrix) { XMStoreFloat4x4(&m_CombindTransformMatrix, CombindMatrix); }
	void						Set_OffsetMatrix(_float4x4 OffsetMatrix) { m_OffsetMatrix = OffsetMatrix; }
	void						Set_TransformMatrix(_fmatrix TransformMatrix) { XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix); }
	void						Set_BoneLocked(LOCKTO eLockTo);
	void						Set_BoneLocked(_bool bLock) {
		m_bLock = bLock;

		if (bLock == false)
			m_bRotateLock = false;
	}

	HRESULT SetParent(CBone* pParent);
	const char* Get_ParentName() const { return m_szParentName; }

public:
	HRESULT Initialize_Prototype(HANDLE hFile);
	HRESULT Initialize(void* pArg);
	void Compute_CombindTransformationMatrix();

private:
	char					m_szName[MAX_PATH];
	_float4x4			m_OffsetMatrix;
	_float4x4			m_TransformMatrix;
	_float4x4			m_CombindTransformMatrix;

	char						m_szParentName[MAX_PATH] = { 0, };

	CBone*					m_pParent = nullptr;
	vector<CBone*>	m_vecChild;

	_bool				m_bLock = false;
	_bool				m_bRotateLock = false;

public:
	static CBone* Create(HANDLE hFile);
	CBone* Clone(void * pArg = nullptr);
	virtual void Free() override;
};

END