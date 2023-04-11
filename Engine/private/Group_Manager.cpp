#include "stdafx.h"
#include "../public/Group_Manager.h"
#include "GameInstance.h"
#include "Utile.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CGroup_Manager)

CGroup_Manager::CGroup_Manager()
{
}

void CGroup_Manager::ImGui_RenderProperty()
{
}

HRESULT CGroup_Manager::Add_Member(const wstring& wstrGroupName, CGameObject* pObject)
{
	OBJECTGROUP* pObjectGroup = Find_ObjectGroup(wstrGroupName);

	if (pObjectGroup == nullptr)
	{
		pObjectGroup = new OBJECTGROUP;
		pObjectGroup->wstrGroupName = wstrGroupName;
		m_vecGroup.push_back(pObjectGroup);
	}

	pObjectGroup->vecMember.push_back(pObject);

	return S_OK;
}

template<typename T>
inline _bool CGroup_Manager::Check_MemberState_byFunction(const wstring & wstrGroupName, _bool (T::* memFunc)(void))
{
	OBJECTGROUP* pObjectGroup = Find_ObjectGroup(wstrGroupName);
	NULL_CHECK_RETURN(pObjectGroup, false);

	_bool	bState = true;
	T*		pObj = nullptr;
	for (auto pGameObject : pObjectGroup->vecMember)
	{
		pObj = dynamic_cast<T*>(pGameObject);
		assert(pObj != nullptr && "CGroup_Manager::Downcasting FAIL.");

		bState = pObj->*memFunc();

		if (bState == false)
			break;
	}

	return bState;
}

_bool CGroup_Manager::Check_MemberState_byFunction(const wstring& wstrGroupName, const std::type_info& typeinfo, std::function<_bool(void)> CheckFunc)
{
	OBJECTGROUP* pObjectGroup = Find_ObjectGroup(wstrGroupName);
	NULL_CHECK_RETURN(pObjectGroup, false);
	
	_bool	bState = true;
	typeinfo	pObj = nullptr;
	for (auto pGameObject : pObjectGroup->vecMember)
	{
		dynamic_cast<typeinfo>(pGameObject);
		pObj = dynamic_cast<T*>(pGameObject);
		assert(pObj != nullptr && "CGroup_Manager::Downcasting FAIL.");

		bState = pObj->*memFunc();

		if (bState == false)
			break;
	}

	return bState;
}

template<typename T>
inline void CGroup_Manager::SetState_byFunction(const wstring& wstrGroupName, void(T::* memFunc)(_bool), _bool bValue)
{
	OBJECTGROUP* pObjectGroup = Find_ObjectGroup(wstrGroupName);
	NULL_CHECK_RETURN(pObjectGroup, );

	T* pObj = nullptr;
	for (auto pGameObject : pObjectGroup->vecMember)
	{
		pObj = dynamic_cast<T*>(pGameObject);
		assert(pObj != nullptr && "CGroup_Manager::Downcasting FAIL.");

		pObj->*memFunc(bValue);
	}
}

CGroup_Manager::OBJECTGROUP* CGroup_Manager::Find_ObjectGroup(const wstring& wstrGroupName)
{
	const auto	iter = find_if(m_vecGroup.begin(), m_vecGroup.end(), [wstrGroupName](OBJECTGROUP*& pObjectGroup) {
		return wstrGroupName == pObjectGroup->wstrGroupName;
		});

	if (iter == m_vecGroup.end())
		return nullptr;

	return (*iter);
}

void CGroup_Manager::Free()
{
	for (auto pObjectGroup : m_vecGroup)
	{
		pObjectGroup->vecMember.clear();
		Safe_Delete(pObjectGroup);
	}
	m_vecGroup.clear();
}