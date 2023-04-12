#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "EnviromentObj.h"
#include "Delegator.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)

class CControlRoom final : public CGameObject
{
private:
	CControlRoom(ID3D11Device* pDevice, ID3D11DeviceContext* p_context);
	CControlRoom(const CControlRoom& rhs);
	virtual ~CControlRoom() = default;

public:
	CGameObject*		Get_Find_TriggerObj(const _tchar * pCloneTag);
	void				Add_Gimmick_TrggerObj(const _tchar*pCloneTag, CGameObject* pTriggerObj);
	void				Add_GimmickObj(_int iRoomNumber, CGameObject* pGimmickObj,CEnviromentObj::CHAPTER GimmickType);

	void				Clear_Static_ShadowList();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void * pArg);
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void Imgui_RenderProperty() override;
	virtual void ImGui_PhysXValueProperty() override;

public:  /*For.Gimmick*/
	void				PulsePlate_Down_Active(_int iRoomIndex,_bool IsTrigger);
	void				Trigger_Active(_int iRoomIndex,CEnviromentObj::CHAPTER eChpater, _bool IsTrigger);
	void				DeadZoneObject_Change(_bool bChange);

	void				Boss_WarriorDeadGimmick();		// Door Up
	void				Boss_HunterDeadGimmick();		// Portal On

private:
	HRESULT SetUp_Components();
	
private:
	CRenderer*											  m_pRendererCom = nullptr; /*OnlyDebug*/
	
	list<pair<CEnviromentObj::CHAPTER,CGameObject*>> 			  m_GimmcikObj_List[6];		//±â¹ÍÀÌ ´Ã¾î³¯ ¼ö·Ï Ãß°¡
	map< const _tchar * ,CGameObject*>							  m_Gimmcik_Trigger_map;

	array<bool, 4>												 m_MapShadowArray;		// ÀüÃ¼¸Ê -1(Ã³À½¸Ê)
	CTransform*													m_pPlayerTransformPtr = nullptr;

private:
	list<_float4x4>		  m_RoomControlPos_List;
	_bool		m_bInitRender = false;

private:
	CGameObject*		Get_GimmickObj(_int iRoomIndex, CGameObject * pGameObj);

public:
	Delegator<_int>						m_DeadZoneChangeDelegator;

public:
	static CControlRoom*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg = nullptr)  override;
	virtual void								Free() override;
};

END