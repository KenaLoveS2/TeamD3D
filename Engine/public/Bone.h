#pragma once

#include "Base.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	CBone(const CBone& rhs);
	virtual ~CBone() = default;

public:
	const char* Get_Name() const { return m_szName; }
	_matrix Get_CombindMatrix() { return XMLoadFloat4x4(&m_CombindTransformMatrix); }
	void Set_OffsetMatrix(_float4x4 OffsetMatrix) { m_OffsetMatrix = OffsetMatrix; }
	_matrix Get_OffsetMatrix() { return XMLoadFloat4x4(&m_OffsetMatrix); }
	void Set_TransformMatrix(_fmatrix TransformMatrix) { XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix); }
	_matrix Get_TransformMatrix() { return XMLoadFloat4x4(&m_TransformMatrix); }

	HRESULT SetParent(CBone* pParent);
	const char* Get_ParentName() const { return m_szParentName; }


public:
	HRESULT Initialize_Prototype(HANDLE hFile);
	HRESULT Initialize(void* pArg);
	void Compute_CombindTransformationMatrix();

private:
	char				m_szName[MAX_PATH];
	_float4x4			m_OffsetMatrix;
	_float4x4			m_TransformMatrix;
	_float4x4			m_CombindTransformMatrix;
	CBone*				m_pParent = nullptr;

	char				m_szParentName[MAX_PATH] = { 0, };

public:
	static CBone* Create(HANDLE hFile);
	CBone* Clone(void * pArg = nullptr);
	virtual void Free() override;
};

END