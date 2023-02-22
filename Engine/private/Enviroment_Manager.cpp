#include "stdafx.h"
#include "..\public\Enviroment_Manager.h"
#include "Utile.h"

IMPLEMENT_SINGLETON(CEnviroment_Manager)

CEnviroment_Manager::CEnviroment_Manager()
{
}

void CEnviroment_Manager::Free()
{
	for (auto &pRoom : m_vecAllRooms)
		Safe_Delete(pRoom);
	m_vecAllRooms.clear();
}

void CEnviroment_Manager::Tick(_float fTimeDelta)
{
	if (m_pTargetTransform == nullptr) return;

	Clear();

	for (auto &pRoom : m_vecAllRooms)		// 플레이어와의 거리 계산
	{
		pRoom->fTargetDistance = m_pTargetTransform->Calc_Distance_XZ(pRoom->vCenterPosition);
		if (pRoom->fTargetDistance < m_fAddContainerRange)
		{
			m_RenderRoomList.push_back(pRoom);
			if (pRoom->fTargetDistance < m_fRenderRange)
			{
				m_RenderRoomIndexList.push_back(pRoom->iIndex);
			}
		}			
	}

	/*
	for (auto&pRoom : m_RenderRooms)
	{	
		if(pRoom->fTargetDistance < m)
	}
	*/
}

HRESULT CEnviroment_Manager::Reserve_Manager(_uint iTickRoomCount)
{
	m_iTickRoomCount = iTickRoomCount;

	return S_OK;
}

void CEnviroment_Manager::Clear()
{
	m_RenderRoomList.clear();
	m_RenderRoomIndexList.clear();
}

void CEnviroment_Manager::Add_Room(ROOM_DESC& RoomDesc)
{
	auto iter = find_if(m_vecAllRooms.begin(), m_vecAllRooms.end(), [&](ROOM_DESC* pRoomDesc)->_bool
	{
		return pRoomDesc->iIndex == RoomDesc.iIndex;
	});

	assert(iter == m_vecAllRooms.end());

	ROOM_DESC*p = new ROOM_DESC;
	memcpy(p, &RoomDesc, sizeof(ROOM_DESC));

	m_vecAllRooms.push_back(p);
}

_bool CEnviroment_Manager::Is_RenderIndex(_uint iIndex)
{
	for (auto& iter : m_RenderRoomIndexList)
	{
		if (iter == iIndex) return true;
	}

	return false;
}

