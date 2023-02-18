#pragma once

#include "Base.h"

BEGIN(Engine)

class CQuadTree final : public CBase
{
public:
	enum CHILD { CHILD_LT, CHILD_RT, CHILD_RB, CHILD_LB, CHILD_END };
	enum CORNER { CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB, CORNER_END };
	enum NEIGHBOR { NEIGHBOR_LEFT, NEIGHBOR_TOP, NEIGHBOR_RIGHT, NEIGHBOR_BOTTOM, NEIGHBOR_END };
public:
	CQuadTree();
	virtual ~CQuadTree() = default;

public:
	HRESULT Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	void Culling(class CFrustum* pFrustum, const _float3* pVerticesPos, FACEINDICES32* pIndices, _uint* pNumFaces);
	void Make_Neighbor();

private:
	CQuadTree*					m_pChild[CHILD_END] = { nullptr };
	CQuadTree*					m_pNeighbor[NEIGHBOR_END] = { nullptr };
	_uint						m_iCorner[CORNER_END] = { 0 };
	


	_uint						m_iCenterIndex = 0;
	_float						m_fRange = 0.f;

private:
	_bool LevelofDetail(const _float3* pVerticesPos);

public:
	static CQuadTree* Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB);

public:
	virtual void Free() override;
};

END