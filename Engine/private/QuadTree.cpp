#include "stdafx.h"
#include "..\public\QuadTree.h"
#include "Frustum.h"
#include "PipeLine.h"

CQuadTree::CQuadTree()
{
}

HRESULT CQuadTree::Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	m_iCorner[CORNER_LT] = iLT;
	m_iCorner[CORNER_RT] = iRT;
	m_iCorner[CORNER_RB] = iRB;
	m_iCorner[CORNER_LB] = iLB;

	if (1 == iRT - iLT)
		return S_OK;

	m_iCenterIndex = (iLT + iRB) >> 1;

	_uint			iLC, iTC, iRC, iBC;

	iLC = (iLT + iLB) >> 1;
	iTC = (iLT + iRT) >> 1;
	iRC = (iRT + iRB) >> 1;
	iBC = (iLB + iRB) >> 1;
	
	m_pChild[CHILD_LT] = CQuadTree::Create(iLT, iTC, m_iCenterIndex, iLC);
	m_pChild[CHILD_RT] = CQuadTree::Create(iTC, iRT, iRC, m_iCenterIndex);
	m_pChild[CHILD_RB] = CQuadTree::Create(m_iCenterIndex, iRC, iRB, iBC);
	m_pChild[CHILD_LB] = CQuadTree::Create(iLC, m_iCenterIndex, iBC, iLB);	

	return S_OK;
}

void CQuadTree::Culling(CFrustum* pFrustum, const _float3 * pVerticesPos, FACEINDICES32* pIndices, _uint* pNumFaces)
{
	if (nullptr == m_pChild[CHILD_LT] ||
		true == LevelofDetail(pVerticesPos))
	{
		_bool			isIn[CORNER_END] = {
			pFrustum->isInFrustum_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_LT]])),
			pFrustum->isInFrustum_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_RT]])),
			pFrustum->isInFrustum_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_RB]])),
			pFrustum->isInFrustum_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_LB]])),
		};

		_bool			isDraw[NEIGHBOR_END] = { true, true, true, true };

		for (_uint i = 0; i < NEIGHBOR_END; ++i)
		{
			if (nullptr != m_pNeighbor[i])
				isDraw[i] = m_pNeighbor[i]->LevelofDetail(pVerticesPos);
		}

		if (true == isDraw[NEIGHBOR_LEFT] &&
			true == isDraw[NEIGHBOR_TOP] &&
			true == isDraw[NEIGHBOR_RIGHT] &&
			true == isDraw[NEIGHBOR_BOTTOM])
		{
			if (true == isIn[CORNER_LT] ||
				true == isIn[CORNER_RT] ||
				true == isIn[CORNER_RB])
			{
				pIndices[*pNumFaces]._0 = m_iCorner[CORNER_LT];
				pIndices[*pNumFaces]._1 = m_iCorner[CORNER_RT];
				pIndices[*pNumFaces]._2 = m_iCorner[CORNER_RB];
				++*pNumFaces;
			}

			if (true == isIn[CORNER_LT] ||
				true == isIn[CORNER_RB] ||
				true == isIn[CORNER_LB])
			{
				pIndices[*pNumFaces]._0 = m_iCorner[CORNER_LT];
				pIndices[*pNumFaces]._1 = m_iCorner[CORNER_RB];
				pIndices[*pNumFaces]._2 = m_iCorner[CORNER_LB];
				++*pNumFaces;
			}

			return;
		}

		_uint			iLC, iTC, iRC, iBC;

		iLC = (m_iCorner[CORNER_LT] + m_iCorner[CORNER_LB]) >> 1;
		iTC = (m_iCorner[CORNER_LT] + m_iCorner[CORNER_RT]) >> 1;
		iRC = (m_iCorner[CORNER_RT] + m_iCorner[CORNER_RB]) >> 1;
		iBC = (m_iCorner[CORNER_LB] + m_iCorner[CORNER_RB]) >> 1;

		if (true == isIn[CORNER_LT] ||
			true == isIn[CORNER_RB] ||
			true == isIn[CORNER_LB])
		{

			if (false == isDraw[NEIGHBOR_LEFT])
			{
				pIndices[*pNumFaces]._0 = m_iCorner[CORNER_LT];
				pIndices[*pNumFaces]._1 = m_iCenterIndex;
				pIndices[*pNumFaces]._2 = iLC;
				++*pNumFaces;

				pIndices[*pNumFaces]._0 = iLC;
				pIndices[*pNumFaces]._1 = m_iCenterIndex;
				pIndices[*pNumFaces]._2 = m_iCorner[CORNER_LB];
				++*pNumFaces;
			}
			else
			{
				pIndices[*pNumFaces]._0 = m_iCorner[CORNER_LT];
				pIndices[*pNumFaces]._1 = m_iCenterIndex;
				pIndices[*pNumFaces]._2 = m_iCorner[CORNER_LB];
				++*pNumFaces;
			}

			if (false == isDraw[NEIGHBOR_BOTTOM])
			{
				pIndices[*pNumFaces]._0 = m_iCorner[CORNER_LB];
				pIndices[*pNumFaces]._1 = m_iCenterIndex;
				pIndices[*pNumFaces]._2 = iBC;
				++*pNumFaces;

				pIndices[*pNumFaces]._0 = iBC;
				pIndices[*pNumFaces]._1 = m_iCenterIndex;
				pIndices[*pNumFaces]._2 = m_iCorner[CORNER_RB];
				++*pNumFaces;
			}
			else
			{
				pIndices[*pNumFaces]._0 = m_iCorner[CORNER_LB];
				pIndices[*pNumFaces]._1 = m_iCenterIndex;
				pIndices[*pNumFaces]._2 = m_iCorner[CORNER_RB];
				++*pNumFaces;
			}
		}

		if (true == isIn[CORNER_LT] ||
			true == isIn[CORNER_RT] ||
			true == isIn[CORNER_RB])
		{
			if (false == isDraw[NEIGHBOR_TOP])
			{
				pIndices[*pNumFaces]._0 = m_iCorner[CORNER_LT];
				pIndices[*pNumFaces]._1 = iTC;
				pIndices[*pNumFaces]._2 = m_iCenterIndex;
				++*pNumFaces;

				pIndices[*pNumFaces]._0 = m_iCenterIndex;
				pIndices[*pNumFaces]._1 = iTC;
				pIndices[*pNumFaces]._2 = m_iCorner[CORNER_RT];
				++*pNumFaces;
			}
			else
			{
				pIndices[*pNumFaces]._0 = m_iCorner[CORNER_LT];
				pIndices[*pNumFaces]._1 = m_iCorner[CORNER_RT];
				pIndices[*pNumFaces]._2 = m_iCenterIndex;
				++*pNumFaces;
			}

			if (false == isDraw[NEIGHBOR_RIGHT])
			{
				pIndices[*pNumFaces]._0 = m_iCorner[CORNER_RT];
				pIndices[*pNumFaces]._1 = iRC;
				pIndices[*pNumFaces]._2 = m_iCenterIndex;
				++*pNumFaces;

				pIndices[*pNumFaces]._0 = m_iCenterIndex;
				pIndices[*pNumFaces]._1 = iRC;
				pIndices[*pNumFaces]._2 = m_iCorner[CORNER_RB];
				++*pNumFaces;
			}
			else
			{
				pIndices[*pNumFaces]._0 = m_iCorner[CORNER_RT];
				pIndices[*pNumFaces]._1 = m_iCorner[CORNER_RB];
				pIndices[*pNumFaces]._2 = m_iCenterIndex;
				++*pNumFaces;
			}
		}

		return;		
	}

	_float		fRange = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_LT]]) - XMLoadFloat3(&pVerticesPos[m_iCenterIndex])));

	if (true == pFrustum->isInFrustum_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCenterIndex]), fRange))
	{
		for (_uint i = 0; i < CHILD_END; ++i)
		{
			if (nullptr != m_pChild[i])
				m_pChild[i]->Culling(pFrustum, pVerticesPos, pIndices, pNumFaces);
		}
	}
	return;	
}

/*이 함수를 호출한 최초의 객체는 가장 큰 지형에 해당하는 쿼드트리를 의미한다. */
void CQuadTree::Make_Neighbor()
{
	if (nullptr == m_pChild[CHILD_LT]->m_pChild[CHILD_LT])
		return;

	m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChild[CHILD_RT];
	m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChild[CHILD_LB];

	m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChild[CHILD_LT];
	m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChild[CHILD_RB];

	m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChild[CHILD_LB];
	m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChild[CHILD_RT];

	m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChild[CHILD_RB];
	m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChild[CHILD_LT];

	if (nullptr != m_pNeighbor[NEIGHBOR_RIGHT])
	{
		m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChild[CHILD_LT];
		m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChild[CHILD_LB];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_BOTTOM])
	{
		m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChild[CHILD_LT];
		m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChild[CHILD_RT];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_LEFT])
	{
		m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChild[CHILD_RT];
		m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChild[CHILD_RB];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_TOP])
	{
		m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChild[CHILD_LB];
		m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChild[CHILD_RB];
	}

	for (_uint i = 0; i < CHILD_END; ++i)
	{
		if (nullptr != m_pChild[i])
			m_pChild[i]->Make_Neighbor();
	}
}

_bool CQuadTree::LevelofDetail(const _float3 * pVerticesPos)
{
	CPipeLine*			pPipeLine = GET_INSTANCE(CPipeLine);

	_float4				vCamPosition =  pPipeLine->Get_CamPosition();

	_float				fCamDistance = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vCamPosition) - XMLoadFloat3(&pVerticesPos[m_iCenterIndex])));
	_float				fWidth = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_RB]]) - XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_LB]])));

	RELEASE_INSTANCE(CPipeLine);

	if (fCamDistance * 0.2f > fWidth)
		return true;

	return false;	
}

CQuadTree * CQuadTree::Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	CQuadTree*		pInstance = new CQuadTree();

	if (FAILED(pInstance->Initialize(iLT, iRT, iRB, iLB)))
	{
		MSG_BOX("Failed to Created : CQuadTree");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQuadTree::Free()
{
	for (auto& pChild : m_pChild)
		Safe_Release(pChild);
}
