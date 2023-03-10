#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
protected:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pHeightMapFilePath);
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner);
	HRESULT			initialize_World(class CTransform* pTransform);
public:
	void Culling(_fmatrix WorldMatrix);


public: /*Picking*/
	_bool				PickingFilter_Pixel(HWND hWnd, class CTransform * pTransform, _float3& fIndexs);
	_bool					Picking_Terrain(HWND hWnd, class CTransform * pTransform, _float4* vPickingPos=nullptr);

public:
	HRESULT					Change_HeightMap(const _tchar* pHeightMapFilePath);

private:
	_float3*		m_pVerticesPos = nullptr;
	_uint			m_iNumVerticesX = 0;
	_uint			m_iNumVerticesZ = 0;

	FACEINDICES32*	m_pIndices = nullptr;
	_int			m_iSelectHeightPixel = -1;
private:
	class CQuadTree*		m_pQuadTree = nullptr;

	PxVec3**			m_pPxTerrainVtx = nullptr;
	PxIndicies*			m_pPxTerrainIdx_Divide = nullptr;
	PxIndicies*			m_pPxTerrainIdx_Remain = nullptr;

	_uint m_iDivideCount = 0;
	const _uint m_iDivideThreshold = 10000;
	_bool m_bRemainFlag = false;

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath);
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr);
	virtual void Free() override;
		
	_float3* Get_VirticesPosPtr() {
		return m_pVerticesPos; 
	}
	FACEINDICES32* Get_InidicesPtr() { return m_pIndices; }
	_uint Get_NumVerticesX() { return m_iNumVerticesX; }
	_uint Get_NumVerticesZ() { return m_iNumVerticesZ; }	
};

END