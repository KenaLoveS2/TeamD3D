#pragma once

#include "Base.h"

BEGIN(Engine)

/* Function Object in the way of "Delegate" */
/* Objects don't possess UI, just access by Delegator. */
template <typename... Args>
class Delegator
{
	using CallBack = std::function<void(Args ...)>;

public:
	~Delegator()
	{
		for (auto &Event : m_vecBinds)
			Safe_Release(Event.first);

		m_vecBinds.clear();
		m_vecBinds.shrink_to_fit();
	}

	/* Excute binded function */
	void broadcast(Args& ...args) 
	{
		for (auto iter = m_vecBinds.begin(); iter != m_vecBinds.end();)
		{
			if (iter->first->AddRef() == 0)
			{
				Safe_Release(iter->first);
				iter = m_vecBinds.erase(iter);
			}
			else
			{
				iter->second(args...);
				Safe_Release(iter->first);
				++iter;
			}
		}
	}

	/* Bind object and canvas's function */
	template <typename T>   
	void bind(T* obj, void (T::*memFunc)(Args ...))
	{
		if (nullptr == dynamic_cast<CBase*>(obj))
			return;

		CallBack callback = [obj, memFunc](Args ... args)
		{
			(obj->*memFunc)(args...);
		};

		Safe_AddRef(obj);
		m_vecBinds.emplace_back(std::make_pair(obj, callback));
	}

	/* delete the bind data for objects subordinated to scene when scene is changed. */
	template <typename T>
	void unbind(T* obj)
	{
		auto removedIter = remove_if(m_vecBinds.begin(), m_vecBinds.end(), [&obj](std::pair<void*, CallBack> e)
		{
			return e.first == obj;
		});
		m_vecBinds.erase(removedIter, m_vecBinds.end());
	}


private:
	/* pair<canvasPtr, canvasFunction> */
	std::vector<std::pair<CBase*, CallBack>> m_vecBinds;

};


END