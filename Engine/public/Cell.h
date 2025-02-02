#pragma once

#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum NEIGHBOR { NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END	};
	enum TYPE { TYPE_DEFAULT, TYPE_HEIGHT, TYPE_END };

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;

	_int				m_iIndex = 0;
	_float3				m_vPoints[POINT_END];
	_int				m_iNeighborIndices[NEIGHBOR_END];
	TYPE m_eType = TYPE_DEFAULT;

private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_float3 Get_Point(POINT ePoint) { 
		return m_vPoints[ePoint];	
	}
	void Set_Neighbor(NEIGHBOR eNeighbor, CCell* pNeighbor) {
		m_iNeighborIndices[eNeighbor] = pNeighbor->m_iIndex;
	}

	HRESULT Initialize(const _float3* pPoints, _int iIndex, TYPE eType);

	_bool Compare_Points(const _float3& SourPoint, const _float3& DestPoint);
	_bool isIn(_fvector vTargetPos, _int* pNeighborIndex);
	_int Get_Index() { return m_iIndex; }
	void Set_CellType(TYPE eType) { m_eType = eType; }
	TYPE Get_CellType() { return m_eType; }

#ifdef _DEBUG
private:
	class CVIBuffer_Cell*			m_pVIBuffer = nullptr;

public:
	HRESULT Render(class CShader* pShader);
#endif // _DEBUG

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex, TYPE eType = TYPE_DEFAULT);
	virtual void Free();
};

END