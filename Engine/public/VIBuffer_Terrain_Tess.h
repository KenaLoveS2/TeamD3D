#pragma once

#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Terrain_Tess final : public CVIBuffer
{
private:
	CVIBuffer_Terrain_Tess(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain_Tess(const CVIBuffer_Terrain_Tess& rhs);
	virtual ~CVIBuffer_Terrain_Tess() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumVerticesX, _uint iNumVerticesZ);
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner);


private:
	_float3*						m_pVerticesPos = nullptr;
	_uint			m_iNumVerticesX = 0;
	_uint			m_iNumVerticesZ = 0;
	//FACEINDICES32*	m_pIndices = nullptr;


public:
	static CVIBuffer_Terrain_Tess* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumVerticesX, _uint iNumVerticesZ);
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr);
	virtual void Free();
};

END