#include "stdafx.h"
#include "..\public\Function_Manager.h"
#include "Utile.h"

IMPLEMENT_SINGLETON(CFunction_Manager)

CFunction_Manager::CFunction_Manager()
{
}

HRESULT CFunction_Manager::Call_Function(CBase * pObj, const _tchar * pFuncName, _float fTimeDelta)
{
	const auto	iter = find_if(m_mapFunc.begin(), m_mapFunc.end(), [pObj](const pair<CBase*, vector<FUNCTION>>& Pair) {
		return pObj == Pair.first;
	});

	if (iter == m_mapFunc.end())
		return E_FAIL;

	const auto	FuncIter = find_if(iter->second.begin(), iter->second.end(), [pFuncName](const FUNCTION& Function) {
		return !lstrcmp(pFuncName, Function.FuncName.c_str());
	});

	if (FuncIter == iter->second.end())
		return E_FAIL;

	(*FuncIter).Func(false, fTimeDelta);

	return S_OK;
}

HRESULT CFunction_Manager::Get_FunctionNames(CBase * pObj, _uint & iTagsCnt, char **& ppTagsOut)
{
	const auto	iter = find_if(m_mapFunc.begin(), m_mapFunc.end(), [pObj](const pair<CBase*, vector<FUNCTION>>& Pair) {
		return pObj == Pair.first;
	});

	if (iter == m_mapFunc.end())
		return E_FAIL;

	_uint	iFuncCnt = _uint(iter->second.size());
	if (iFuncCnt == 0)
		return S_FALSE;

	iTagsCnt = iFuncCnt;
	ppTagsOut = new char*[iFuncCnt];

	_uint i = 0;
	for (auto& Function : iter->second)
		ppTagsOut[i++] = CUtile::WideCharToChar(const_cast<_tchar*>(Function.FuncName.c_str()));

	return S_OK;
}

void CFunction_Manager::Free()
{
}