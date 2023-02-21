#pragma once

#include "Base.h"

/* ���ӳ��� �ʿ��� ��ü���� �� ����(CLayer)�� ���� ������ �����Ѵ�. */
/* ��ü���� ������ �����ϰ� �ִ� CLayer���� �����ϴ� Ŭ�����̴�. */
/* ��� ��ü���� ����(Tick, Late_Tick)�� ����Ѵ�. */

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager);
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CComponent*		Get_ComponentPtr(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pCloneObjectTag, const _tchar* pComponentTag);
	class CGameObject*		Get_GameObjectPtr(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar* pCloneObjectTag);
	
	map<const _tchar*, class CGameObject*>&		Get_ProtoTypeObjects() { return m_Prototypes; }
	class CLayer*				Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);

	map<const _tchar*, class CGameObject*>*	Get_AnimObjects(_uint iLevelIndex) { return &m_mapAnimModel[iLevelIndex]; }

public:
	HRESULT					Reserve_Manager(_uint iNumLevels);
	HRESULT					Clear(_uint iLevelIndex);
public:
	HRESULT					Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT					Clone_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag,
		const _tchar* pCloneObjectTag, 
		void* pArg = nullptr, CGameObject** ppOut = nullptr);
	CGameObject*			Clone_GameObject(const _tchar* pPrototypeTag, const _tchar* pCloneObjectTag, void* pArg = nullptr);
	HRESULT					Clone_AnimObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag,
		const _tchar* pCloneObjectTag,
		void* pArg = nullptr, CGameObject** ppOut = nullptr);
	HRESULT					Add_AnimObject(_uint iLevelIndex, class CGameObject* pGameObject);

	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

private: /* ������ü������ ��Ƴ��´�. */
	map<const _tchar*, class CGameObject*>			m_Prototypes;
	typedef map<const _tchar*, class CGameObject*>	PROTOTYPES;

private: /* �纻��ü���� �����ϱ����� �����̳�. */
	map<const _tchar*, class CLayer*>*			m_pLayers = nullptr;
	typedef map<const _tchar*, class CLayer*>	LAYERS;
	_uint										m_iNumLevels = 0;

private:	/* Animation Objects, For Animation Tool */
	map<const _tchar*, class CGameObject*>*	m_mapAnimModel = nullptr;

private:
	class CGameObject* Find_Prototype(const _tchar* pPrototypeTag);


public:
	virtual void Free() override;

	// ��� ���� Object�� Imgui�� ����Ѵ�.
	// ������ ������ �̸��� szSelectedProto�� �����ش�.
	void Imgui_ProtoViewer(_uint iLevel, OUT const _tchar*& szSelectedProto);

	// iLevel�� �ִ� ��� �纻 Object�� Layer���� Imgui�� ����Ѵ�.
	// ������ Object�� pSelectedObject�� �����ش�.
	// �ش� Object�� ���ų� ������� nullptr�� �����ش�.
	void   Imgui_ObjectViewer(_uint iLevel, OUT CGameObject*& pSelectedObject);
};

END