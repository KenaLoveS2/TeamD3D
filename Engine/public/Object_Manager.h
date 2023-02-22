#pragma once
#include "Base.h"

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
	HRESULT Reserve_Manager(_uint iNumLevels, _uint iNumCopyPrototypes = 0);
	HRESULT Clear(_uint iLevelIndex);

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
	HRESULT Add_ClonedGameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pCloneObjectTag, CGameObject* pGameObject);

	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

	void		SwitchOnOff_Shadow(_bool bSwitch);

private:	/* Prototypes */
	map<const _tchar*, class CGameObject*>			m_Prototypes;
	typedef map<const _tchar*, class CGameObject*>	PROTOTYPES;
	
private:	/* Clone Objects */
	map<const _tchar*, class CLayer*>*			m_pLayers = nullptr;
	typedef map<const _tchar*, class CLayer*>	LAYERS;
	_uint										m_iNumLevels = 0;

private:	/* Animation Objects, For Animation Tool */
	map<const _tchar*, class CGameObject*>*	m_mapAnimModel = nullptr;

private:
	_bool									m_bShadow = true;
	_uint m_iNumCopyPrototypes = 0;
	vector<PROTOTYPES> m_CopyPrototypes;
	list<_tchar*> m_CopyTagList;

private: /*For.Imgui*/
	string							m_strComponentTag = "";
	
private:
	class CGameObject* Find_Prototype(const _tchar* pPrototypeTag);

public:	
	virtual void Free() override;
	
public: /*For.Imgui*/
	void Imgui_ProtoViewer(_uint iLevel, OUT const _tchar*& szSelectedProto);
	void Imgui_ObjectViewer(_uint iLevel, OUT class CGameObject*& pSelectedObject);
	void Imgui_DeleteComponent(class CGameObject* pSelectedObject);
	
public:	
	vector<PROTOTYPES>& Get_CopyPrototypes() { return m_CopyPrototypes; }
	_uint Get_NumCopyPrototypes() { return m_iNumCopyPrototypes; }
		
	HRESULT Delete_Object(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pCloneObjectTag);
};
END