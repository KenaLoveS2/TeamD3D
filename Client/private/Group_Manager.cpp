#include "stdafx.h"
#include "../public/Group_Manager.h"
#include "GameInstance.h"
#include "Utile.h"
#include "GameObject.h"
#include "Monster.h"
#include "BowTarget.h"

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

_bool CGroup_Manager::Check_MemberState_byFunction(const wstring & wstrGroupName, GROUPFILTER eFilter)
{
	OBJECTGROUP* pObjectGroup = Find_ObjectGroup(wstrGroupName);
	NULL_CHECK_RETURN(pObjectGroup, false);

	_bool		bState = true;
	CMonster*	pMonster = nullptr;
	CBowTarget* pBowTarget = nullptr;
	for (auto pGameObject : pObjectGroup->vecMember)
	{
		if (eFilter == CGroup_Manager::GROUP_MONSTER)
		{
			pMonster = dynamic_cast<CMonster*>(pGameObject);
			assert(pMonster != nullptr && "CGroup_Manager::Downcasting to CMonster FAILED.");

			bState = pMonster->Is_Dead();
		}
		else if (eFilter == CGroup_Manager::GROUP_BOWTARGET)
		{
			pBowTarget = dynamic_cast<CBowTarget*>(pGameObject);
			assert(pBowTarget != nullptr && "CGroup_Manager::Downcasting to CBowTarget FAILED.");

			bState = pBowTarget->Is_Dead();
		}

		if (bState == false)
			break;
	}

	return bState;
}

void CGroup_Manager::SetState_byFunction(const wstring& wstrGroupName, GROUPFILTER eFilter, _bool bValue)
{
	OBJECTGROUP* pObjectGroup = Find_ObjectGroup(wstrGroupName);
	NULL_CHECK_RETURN(pObjectGroup, );

	CMonster* pMonster = nullptr;
	CBowTarget* pBowTarget = nullptr;

	for (auto pGameObject : pObjectGroup->vecMember)
	{
		if (eFilter == CGroup_Manager::GROUP_MONSTER)
		{
			pMonster = dynamic_cast<CMonster*>(pGameObject);
			assert(pMonster != nullptr && "CGroup_Manager::Downcasting to CMonster FAILED.");

			
		}
		else if (eFilter == CGroup_Manager::GROUP_BOWTARGET)
		{
			pBowTarget = dynamic_cast<CBowTarget*>(pGameObject);
			assert(pBowTarget != nullptr && "CGroup_Manager::Downcasting to CBowTarget FAILED.");

			pBowTarget->TurnOn_Launch();
		}
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