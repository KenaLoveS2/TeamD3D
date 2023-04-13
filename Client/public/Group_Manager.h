#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CGroup_Manager final : public CBase
{
	DECLARE_SINGLETON(CGroup_Manager);

public:
	enum GROUPFILTER { GROUP_MONSTER, GROUP_BOWTARGET, GROUP_END };

	typedef struct tagObjectGroup
	{
		wstring		wstrGroupName = L"";
		LEVEL		eLevel = LEVEL_END;
		vector<class CGameObject*>	vecMember;
	} OBJECTGROUP;

private:
	CGroup_Manager();
	virtual ~CGroup_Manager() = default;

public:
	void					ImGui_RenderProperty();
// 	HRESULT					Save_Group(const wstring& strFilePath);
// 	HRESULT					Safe_Group_All(const wstring& strFilePath);
// 	HRESULT					Load_Group(const wstring& strFilePath);

public:
	HRESULT					Add_Member(const wstring& wstrGroupName, class CGameObject* pObject);

	_bool					Check_MemberState_byFunction(const wstring& wstrGroupName, GROUPFILTER eFilter);

	//_bool					Check_MemberState_byFunction(const wstring& wstrGroupName, const std::type_info& typeinfo, std::function<_bool(void)> CheckFunc);

	void					SetState_byFunction(const wstring& wstrGroupName, GROUPFILTER eFilter, _bool bValue);

private:
	vector<OBJECTGROUP*>	m_vecGroup;

private:
	OBJECTGROUP*			Find_ObjectGroup(const wstring& wstrGroupName);

public:
	virtual void Free() override;
};

END