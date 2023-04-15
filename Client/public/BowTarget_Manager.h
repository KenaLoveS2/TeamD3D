#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CBowTarget_Manager final : public CBase
{
	DECLARE_SINGLETON(CBowTarget_Manager);

public:
	typedef struct tagBowTargetGroup
	{
		wstring		wstrGroupName = L"";
		vector<class CBowTarget*>	vecBowTarget;
		tagBowTargetGroup*			pNextGroup = nullptr;
	} BOWTARGETGROUP;

private:
	CBowTarget_Manager();
	~CBowTarget_Manager() = default;

public:
	HRESULT			Add_Member(const wstring& wstrGroupName, class CBowTarget* pBowTarget, const wstring& wstrNextGroupName = L"N/A");
	HRESULT			Group_Active(const wstring& wstrGroupName);
	void			Launch_CurrentGroup();
	void			Launch_Group(const wstring& wstrGroupName);
	void			Launch_Group(BOWTARGETGROUP* pBowTargetGroup);
	_bool			Check_CurrentGroup_Launched();
	_bool			Check_CurrentGroup_Hit();
	_bool			Check_Group_Hit(const wstring& wstrGroupName);
	void			Clear_Groups();
	_bool			Check_Group_Hit(const wstring& wstrGroupName);
private:
	vector<BOWTARGETGROUP*>	m_vecGroup;
	BOWTARGETGROUP*			m_pCurActiveGroup = nullptr;
	BOWTARGETGROUP*			m_pNextGroup = nullptr;

private:
	BOWTARGETGROUP*	Find_BowTargetGroup(const wstring& wstrGroupName);

public:
	virtual void	Free() override;
};

END