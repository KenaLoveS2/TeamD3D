#include "stdafx.h"
#include "..\public\Enviroment_Manager.h"
#include "Utile.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CEnviroment_Manager)

_bool CEnviroment_Manager::Is_RenderIndex(_uint iObjRoomIndex)
{	
	
	if (m_iCurPlayer_RoomIndex == 0)
		return iObjRoomIndex == m_iCurPlayer_RoomIndex || iObjRoomIndex == m_iCurPlayer_RoomIndex + 1;
	else if (m_iCurPlayer_RoomIndex == 5)
		return iObjRoomIndex == m_iCurPlayer_RoomIndex;

	return	iObjRoomIndex == m_iCurPlayer_RoomIndex-1 ||  iObjRoomIndex == m_iCurPlayer_RoomIndex || iObjRoomIndex == m_iCurPlayer_RoomIndex + 1;

}

_bool CEnviroment_Manager::Is_Render_TerrainIndex(_uint iTerrainRoomIndex)
{
	if ( m_iCurPlayer_RoomIndex<=1)
	{
		return iTerrainRoomIndex == 0 || iTerrainRoomIndex == 2;
	}


	return iTerrainRoomIndex == m_iCurPlayer_RoomIndex || iTerrainRoomIndex == m_iCurPlayer_RoomIndex+1;
}

CEnviroment_Manager::CEnviroment_Manager()
{
	m_RoomCheck_Array.fill(false);
}

void CEnviroment_Manager::Free()
{
	
}

void CEnviroment_Manager::Tick(_float fTimeDelta)
{
	_float4	fPos;

	m_pPlayer = nullptr;

	// 플레이어 z 비교로 룸인덱스 계산
	if (m_pPlayer == nullptr)
	{
		fPos = CGameInstance::GetInstance()->Get_CamPosition();
	}
	else
	{
		XMStoreFloat4(&fPos, m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION));
	}
	

	if (m_RoomCheck_Array[1] ==false && fPos.z >= 78.576f)
	{
		m_iCurPlayer_RoomIndex = 1;
	}
	else if (m_RoomCheck_Array[2] == false && fPos.z >= 320.f)
	{

		//CGameInstance::GetInstance()->RoomIndex_Object_Clear(CGameInstance::GetInstance()->Get_CurLevelIndex(), L"Layer_Enviroment", 0);
		//CGameInstance::GetInstance()->RoomIndex_Object_Clear(CGameInstance::GetInstance()->Get_CurLevelIndex(), L"Layer_Enviroment", 1);

		m_iCurPlayer_RoomIndex = 2;
	}
	else if (m_RoomCheck_Array[3] == false && fPos.z >= 602.f)
	{
		// 시네마틱 때 삭제하자
		//CGameInstance::GetInstance()->RoomIndex_Object_Clear(CGameInstance::GetInstance()->Get_CurLevelIndex(), L"Layer_Enviroment", 2);

		m_iCurPlayer_RoomIndex = 3;
	}
	else if (m_RoomCheck_Array[4] == false && fPos.z >= 856.f)
	{

		//CGameInstance::GetInstance()->RoomIndex_Object_Clear(CGameInstance::GetInstance()->Get_CurLevelIndex(), L"Layer_Enviroment", 3);

		m_iCurPlayer_RoomIndex = 4;
	}
	else if (m_RoomCheck_Array[5] == false && fPos.z >= 1095.f)
	{

		//CGameInstance::GetInstance()->RoomIndex_Object_Clear(CGameInstance::GetInstance()->Get_CurLevelIndex(), L"Layer_Enviroment", 4);

		m_iCurPlayer_RoomIndex = 5;
	}
	
	m_RoomCheck_Array[m_iCurPlayer_RoomIndex] = true;
}

HRESULT CEnviroment_Manager::Reserve_Manager(_uint iStartRoomIndex)
{
	m_iCurPlayer_RoomIndex = iStartRoomIndex;

	return S_OK;
}

