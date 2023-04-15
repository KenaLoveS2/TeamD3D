#include "stdafx.h"
#include "BowTarget_Manager.h"
#include "GameInstance.h"
#include "Utile.h"
#include "BowTarget.h"

IMPLEMENT_SINGLETON(CBowTarget_Manager)

CBowTarget_Manager::CBowTarget_Manager()
{
}

HRESULT CBowTarget_Manager::Add_Member(const wstring& wstrGroupName, CBowTarget* pBowTarget, const wstring& wstrNextGroupName)
{
	BOWTARGETGROUP* pBowTargetGroup = Find_BowTargetGroup(wstrGroupName);

	if (pBowTargetGroup == nullptr)
	{
		pBowTargetGroup = new BOWTARGETGROUP;
		pBowTargetGroup->wstrGroupName = wstrGroupName;
		m_vecGroup.push_back(pBowTargetGroup);
	}

	pBowTargetGroup->vecBowTarget.push_back(pBowTarget);

	if (wstrNextGroupName != L"N/A")
	{
		BOWTARGETGROUP* pNextGroup = Find_BowTargetGroup(wstrNextGroupName);

		if (pNextGroup == nullptr)
		{
			pNextGroup = new BOWTARGETGROUP;
			pNextGroup->wstrGroupName = wstrNextGroupName;
			m_vecGroup.push_back(pNextGroup);
		}

		pBowTargetGroup->pNextGroup = pNextGroup;
	}

	return S_OK;
}

HRESULT CBowTarget_Manager::Group_Active(const wstring& wstrGroupName)
{
	BOWTARGETGROUP* pBowTargetGroup = Find_BowTargetGroup(wstrGroupName);
	NULL_CHECK_RETURN(pBowTargetGroup, E_FAIL);

	if (m_pCurActiveGroup != nullptr)
	{
		if (m_pCurActiveGroup == pBowTargetGroup)
			return S_FALSE;

		return E_FAIL;
	}

	m_pCurActiveGroup = pBowTargetGroup;

	if (m_pCurActiveGroup->pNextGroup != nullptr)
		m_pNextGroup = m_pCurActiveGroup->pNextGroup;
	else
		m_pNextGroup = nullptr;

	Launch_CurrentGroup();

	return S_OK;
}

void CBowTarget_Manager::Launch_CurrentGroup()
{
	NULL_CHECK_RETURN(m_pCurActiveGroup, );

	for (auto pBowTarget : m_pCurActiveGroup->vecBowTarget)
		pBowTarget->TurnOn_Launch();
}

void CBowTarget_Manager::Launch_Group(const wstring& wstrGroupName)
{
	BOWTARGETGROUP* pBowTargetGroup = Find_BowTargetGroup(wstrGroupName);
	NULL_CHECK_RETURN(pBowTargetGroup, );

	for (auto pBowTarget : pBowTargetGroup->vecBowTarget)
		pBowTarget->TurnOn_Launch();
}

void CBowTarget_Manager::Launch_Group(BOWTARGETGROUP* pBowTargetGroup)
{
	NULL_CHECK_RETURN(pBowTargetGroup, );

	for (auto pBowTarget : pBowTargetGroup->vecBowTarget)
		pBowTarget->TurnOn_Launch();
}

_bool CBowTarget_Manager::Check_CurrentGroup_Launched()
{
	NULL_CHECK_RETURN(m_pCurActiveGroup, false);

	_bool	bState = true;

	for (auto pBowTarget : m_pCurActiveGroup->vecBowTarget)
	{
		bState = pBowTarget->Is_Launched();

		if (bState == false)
			break;
	}

	if (bState == true && m_pNextGroup != nullptr)
	{

	}

	return bState;
}

_bool CBowTarget_Manager::Check_CurrentGroup_Hit()
{
	NULL_CHECK_RETURN(m_pCurActiveGroup, false);

	_bool	bState = true;

	for (auto pBowTarget : m_pCurActiveGroup->vecBowTarget)
	{
		bState = pBowTarget->Is_Hit();

		if (bState == false)
			break;
	}

	if (bState == true)
	{
		m_pCurActiveGroup = nullptr;

		if (m_pNextGroup != nullptr)
		{
			Group_Active(m_pNextGroup->wstrGroupName);
			Launch_Group(m_pCurActiveGroup);
		}
	}

	return bState;
}

_bool CBowTarget_Manager::Check_Group_Hit(const wstring& wstrGroupName)
{
	BOWTARGETGROUP* pBowTargetGroup = Find_BowTargetGroup(wstrGroupName);
	NULL_CHECK_RETURN(pBowTargetGroup, false);

	_bool	bState = true;

	for (auto pBowTarget : pBowTargetGroup->vecBowTarget)
	{
		bState = pBowTarget->Is_Hit();

		if (bState == false)
			break;
	}

	return bState;
}

void CBowTarget_Manager::Clear_Groups()
{
	for (auto pBowTargetGroup : m_vecGroup)
		Safe_Delete(pBowTargetGroup);
	m_vecGroup.clear();
}


CBowTarget_Manager::BOWTARGETGROUP* CBowTarget_Manager::Find_BowTargetGroup(const wstring& wstrGroupName)
{
	const auto	iter = find_if(m_vecGroup.begin(), m_vecGroup.end(), [wstrGroupName](BOWTARGETGROUP*& pBowTargetGroup) {
			return wstrGroupName == pBowTargetGroup->wstrGroupName;
		});

	if (iter == m_vecGroup.end())
		return nullptr;

	return (*iter);
}

void CBowTarget_Manager::Free()
{
	Clear_Groups();
}
