#pragma once
#include "Base.h"

BEGIN(Engine)

class CGroup_Manager final : public CBase
{
	DECLARE_SINGLETON(CGroup_Manager);

public:
	typedef struct tagObjectGroup
	{
		wstring		wstrGroupName = L"";
		vector<class CGameObject*>	vecMember;
	} OBJECTGROUP;

private:
	CGroup_Manager();
	virtual ~CGroup_Manager() = default;

public:
	void					ImGui_RenderProperty();
	HRESULT					Save_Group(const wstring& strFilePath);
	HRESULT					Safe_Group_All(const wstring& strFilePath);
	HRESULT					Load_Group(const wstring& strFilePath);

public:
	HRESULT					Add_Member(const wstring& wstrGroupName, class CGameObject* pObject);

	template<typename T>
	_bool					Check_MemberState_byFunction(const wstring& wstrGroupName, _bool (T::* memFunc)(void));

	_bool					Check_MemberState_byFunction(const wstring& wstrGroupName, const std::type_info& typeinfo, std::function<_bool(void)> CheckFunc);

	template<typename T>
	void					SetState_byFunction(const wstring& wstrGroupName, void (T::* memFunc)(_bool), _bool bValue);

private:
	vector<OBJECTGROUP*>	m_vecGroup;

private:
	OBJECTGROUP*			Find_ObjectGroup(const wstring& wstrGroupName);

public:
	virtual void Free() override;
};

END