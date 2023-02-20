#pragma once

#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent* Get_ComponentPtr(const _tchar* pCloneObjectTag, const _tchar* pComponentTag);
	class CGameObject * Get_GameObjectPtr(const _tchar* pCloneObjectTag);

public:
	HRESULT Add_GameObject(const _tchar* pCloneObjectTag, class CGameObject* pGameObject);
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	
private:
	map <const _tchar*, class CGameObject*> m_GameObjects;	
	typedef map<const _tchar*, class CGameObject*>	GAMEOBJECTS;

public:
	static CLayer* Create();
	virtual void Free() override;

	GAMEOBJECTS& GetGameObjects() { return m_GameObjects; }

	CGameObject* Find_GameObject(const _tchar * pCloneObjectTag);
};

END