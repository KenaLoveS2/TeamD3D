#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CMonster_Manager final : public CBase
{
	DECLARE_SINGLETON(CMonster_Manager);

public:
	typedef struct tagMonsterGroup
	{
		wstring		wstrGroupName = L"";
		vector<class CMonster*>	vecMember;
	} MONSTERGROUP;

private:
	CMonster_Manager();
	~CMonster_Manager() = default;

public:
	inline _bool			Is_Battle();
	_bool					Is_CurrentGroup(const wstring& wstrGroupName);

public:
	HRESULT					Add_Member(const wstring& wstrGroupName, class CMonster* pMonster);
	HRESULT					Group_Active(const wstring& wstrGroupName);
	_bool					Check_All_Group_Alive();
	_bool					Check_All_Group_Dead();
	_bool					Check_CurrentGroup_Dead();
	_bool					Check_Members_Spawned(const wstring& wstrGroupName);
	_bool					Check_Members_Dead(const wstring& wstrGroupName);
	void					Clear_Groups();

private:
	vector<MONSTERGROUP*>	m_vecGroup;
	MONSTERGROUP*			m_pCurActiveGroup = nullptr;

private:
	MONSTERGROUP*			Find_MonsterGroup(const wstring& wstrGroupName);

public:
	virtual void Free() override;
};

END