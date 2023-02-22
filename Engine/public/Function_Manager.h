#pragma once
#include "Base.h"

BEGIN(Engine)

class CFunction_Manager final : public CBase
{
	DECLARE_SINGLETON(CFunction_Manager)

private:
	typedef struct tagFunc
	{
		wstring						FuncName = L"";
		std::function<void(_bool, _float)>	Func = nullptr;
	} FUNCTION;

private:
	CFunction_Manager();
	virtual ~CFunction_Manager() = default;

public:
	template<typename T>
	HRESULT		Add_Function(T* pObj, const _tchar* pFuncName, void (T::*memFunc)(_bool, _float))
	{
		const auto	iter = find_if(m_mapFunc.begin(), m_mapFunc.end(), [pObj](const pair<CBase*, vector<FUNCTION>>& Pair) {
			return (CBase*)pObj == Pair.first;
		});

		FUNCTION		Function;

		Function.FuncName = wstring(pFuncName);
		Function.Func = [pObj, memFunc](_bool bIsInit, _float fTimeDelta) { (pObj->*memFunc)(bIsInit, fTimeDelta); };

		if (iter != m_mapFunc.end())
			iter->second.push_back(Function);
		else
			m_mapFunc.emplace(pObj, vector<FUNCTION>{Function});

		return S_OK;
	}
	HRESULT		Call_Function(CBase* pObj, const _tchar* pFuncName, _float fTimeDelta);
	HRESULT		Get_FunctionNames(CBase* pObj, _uint& iTagsCnt, char**& ppTagsOut);

private:
	map<CBase*, vector<FUNCTION>>		m_mapFunc;

public:
	virtual void		Free() override;
};

END