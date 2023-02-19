#pragma once
#include "Base.h"

BEGIN(Engine)

/* delegate(위임) 방식으로 구현한 함수객체. */
/* UI에 delegator을 통해 접근하도록 할 것이다. */
template <typename... Args>
class ENGINE_DLL CDelegator
{
	using CallBack = std::function<void(Args ...)>;

public:
	~CDelegator()
	{
		for (auto &Event : m_vecBinds)
			Safe_Release(Event.first);

		m_vecBinds.clear();
		m_vecBinds.shrink_to_fit();
	}

	/* 객체들은 이 함수들을 통해 자신이 컨트롤 하고자 하는 UI를 실행할 것임. */
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

	/* 객체와 객체가 컨트롤 하고자 하는 UI를 연동 */
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

	/* 씬이 넘어갈 때 해제(씬에 종속된 객체들이 삭제될때 연동정보가 남지 않도록 함.)*/
	template <typedef T>
	void unbind(T* obj)
	{
		auto removedIter = remove_if(m_vecBinds.begin(), m_vecBinds.end(), [&obj](std::pair<void*, CallBack> e)
		{
			return e.first == obj;
		});
		m_vecBinds.erase(removedIter, m_vecBinds.end());
	}

private:
	/* 캔버스와 해당 캔버스 내 특정 함수를 페어로 목록화 */
	std::vector<std::pair<CBase*, CallBack>> m_vecBinds;
};
END 