#pragma once
#include "EnviromentObj.h"

/* ±â¹Í °ü¸® */

BEGIN(Engine)
class CEnviroment_Manager final : public CBase
{
	DECLARE_SINGLETON(CEnviroment_Manager);

public:
	typedef struct ENGINE_DLL tagRoomDesc
	{
		_uint iIndex;
		_float4 vCenterPosition;
		_float fTargetDistance;
	} ROOM_DESC;

private:
	_uint m_iTickRoomCount = 0;

	_float m_fRenderRange = 0.f;
	_float m_fAddContainerRange = 0.f;
	
	class CTransform* m_pTargetTransform = nullptr;
	
	vector<ROOM_DESC*> m_vecAllRooms;
	list<ROOM_DESC*> m_RenderRoomList;
	list<_uint> m_RenderRoomIndexList;
	
private:
	CEnviroment_Manager();
	virtual ~CEnviroment_Manager() = default;
	
	

public:
	virtual void Free() override;
	void Tick(_float fTimeDelta);
	
	HRESULT Reserve_Manager(_uint iTickRoomCount=10);
	void Clear();

	void Add_Room(ROOM_DESC& RoomDesc);
	list<_uint>& Get_RenderRommIndexList() { return m_RenderRoomIndexList; }
	_bool Is_RenderIndex(_uint iIndex);
};
END