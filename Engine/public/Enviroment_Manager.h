#pragma once
#include "EnviromentObj.h"

/* ±â¹Í °ü¸® */

BEGIN(Engine)
class CEnviroment_Manager final : public CBase
{
	DECLARE_SINGLETON(CEnviroment_Manager);
public:
	_bool	Is_RenderIndex(_uint iObjRoomIndex);

	_bool	Is_Render_TerrainIndex(_uint iTerrainRoomIndex);
	void	Set_PlayerPtr(CGameObject* pPlayer) {
		assert(pPlayer != nullptr && "CEnviroment_Manager_Set_PlayerPtr");
		m_pPlayer = pPlayer;}

private:
	_uint m_iCurPlayer_RoomIndex = 0;
	_float m_fRenderRange = 0.f;
	_float m_fAddContainerRange = 0.f;

	class CTransform*	m_pTargetTransform = nullptr;
	class CGameObject*  m_pPlayer = nullptr;
	
	array<_bool, 6>		m_RoomCheck_Array;

private:
	CEnviroment_Manager();
	virtual ~CEnviroment_Manager() = default;

public:
	virtual void Free() override;
	void Tick(_float fTimeDelta);
	
	HRESULT Reserve_Manager(_uint iStartRoomIndex =0);
};
END