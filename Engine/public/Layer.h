#pragma once

#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent* Get_ComponentPtr(const _tchar* pComponentTag, _uint iIndex);

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	
	

private:
	list<class CGameObject*>			m_GameObjects;
	typedef list<class CGameObject*>	GAMEOBJECTS;

public:
	static CLayer* Create();
	virtual void Free() override;

	list<class CGameObject*>& GetGameObjects() { return m_GameObjects; }
};

END