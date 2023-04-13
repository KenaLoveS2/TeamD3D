#include "stdafx.h"
#include "Monster_Manager.h"
#include "Monster.h"

IMPLEMENT_SINGLETON(CMonster_Manager)

CMonster_Manager::CMonster_Manager()
{
}

inline _bool CMonster_Manager::Is_Battle()
{
	if (m_pCurActiveGroup == nullptr)
		return false;

	return true;
}

_bool CMonster_Manager::Is_CurrentGroup(const wstring& wstrGroupName)
{
	if (m_pCurActiveGroup == nullptr)
		return false;

	return m_pCurActiveGroup == Find_MonsterGroup(wstrGroupName);
}

HRESULT CMonster_Manager::Add_Member(const wstring& wstrGroupName, CMonster* pMonster)
{
	MONSTERGROUP* pMonsterGroup = Find_MonsterGroup(wstrGroupName);

	if (pMonsterGroup == nullptr)
	{
		pMonsterGroup = new MONSTERGROUP;
		pMonsterGroup->wstrGroupName = wstrGroupName;
		m_vecGroup.push_back(pMonsterGroup);
	}

	pMonsterGroup->vecMember.push_back(pMonster);

	return S_OK;
}

HRESULT CMonster_Manager::Group_Active(const wstring& wstrGroupName)
{
	MONSTERGROUP* pMonsterGroup = Find_MonsterGroup(wstrGroupName);
	NULL_CHECK_RETURN(pMonsterGroup, E_FAIL);

	if (m_pCurActiveGroup != nullptr)
	{
		if (m_pCurActiveGroup == pMonsterGroup)
			return S_FALSE;

		return E_FAIL;
	}

	m_pCurActiveGroup = pMonsterGroup;

	return S_OK;
}

_bool CMonster_Manager::Check_All_Group_Alive()
{
	_bool bState = true;

	return bState;
}

_bool CMonster_Manager::Check_All_Group_Dead()
{
	_bool bState = true;

	return bState;
}

_bool CMonster_Manager::Check_CurrentGroup_Dead()
{
	NULL_CHECK_RETURN(m_pCurActiveGroup, false);

	_bool		bState = true;

	for (auto pMonster : m_pCurActiveGroup->vecMember)
	{
		bState = pMonster->Is_Dead();

		if (bState == false)
			break;
	}

	if (bState == true)
		m_pCurActiveGroup = nullptr;

	return bState;
}

_bool CMonster_Manager::Check_Members_Spawned(const wstring& wstrGroupName)
{
	MONSTERGROUP* pMonsterGroup = Find_MonsterGroup(wstrGroupName);
	NULL_CHECK_RETURN(pMonsterGroup, false);

	_bool		bState = true;
	
	for (auto pMonster : pMonsterGroup->vecMember)
	{
		bState = pMonster->Is_Spawned();

		if (bState == false)
			break;
	}

	return bState;
}

_bool CMonster_Manager::Check_Members_Dead(const wstring& wstrGroupName)
{
	MONSTERGROUP* pMonsterGroup = Find_MonsterGroup(wstrGroupName);
	NULL_CHECK_RETURN(pMonsterGroup, false);

	_bool		bState = true;

	for (auto pMonster : pMonsterGroup->vecMember)
	{
		bState = pMonster->Is_Dead();

		if (bState == false)
			break;
	}

	if (bState == true)
		m_pCurActiveGroup = nullptr;

	return bState;
}

void CMonster_Manager::Clear_Groups()
{
	for (auto pMonsterGroup : m_vecGroup)
		Safe_Delete(pMonsterGroup);
	m_vecGroup.clear();
}

CMonster_Manager::MONSTERGROUP* CMonster_Manager::Find_MonsterGroup(const wstring& wstrGroupName)
{
	const auto	iter = find_if(m_vecGroup.begin(), m_vecGroup.end(), [wstrGroupName](MONSTERGROUP*& pMonsterGroup) {
		return wstrGroupName == pMonsterGroup->wstrGroupName;
		});

	if (iter == m_vecGroup.end())
		return nullptr;

	return (*iter);
}

void CMonster_Manager::Free()
{
	Clear_Groups();
}