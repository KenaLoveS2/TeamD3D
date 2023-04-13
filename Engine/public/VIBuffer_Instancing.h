#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing abstract : public CVIBuffer
{
public:
	CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs);
	virtual ~CVIBuffer_Instancing() = default;

public:
	_uint	Get_InstanceNum() { return m_iNumInstance; }
	void	Set_InstanceNum(_uint iInstanceNum) {
		m_iNumInstance = iInstanceNum;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner);
	virtual HRESULT Tick(_float fTimeDelta) = 0;
	virtual HRESULT Render();
	
protected:
	_uint						m_iNumInstance = 0; /* 몇개의 도형?!*/ 
	_uint						m_iInstanceStride = 0;
	_uint						m_iIndexCountPerInstance = 0;
	ID3D11Buffer*				m_pInstanceBuffer = nullptr; /* 행렬 집합. */

public:
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) = 0;
	virtual void Free() override;
};

END