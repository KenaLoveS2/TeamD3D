#pragma once
#include "Base.h"

BEGIN(Engine)

/* delegate(����) ������� ������ �Լ���ü. */
/* UI�� delegator�� ���� �����ϵ��� �� ���̴�. */
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

	/* ��ü���� �� �Լ����� ���� �ڽ��� ��Ʈ�� �ϰ��� �ϴ� UI�� ������ ����. */
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

	/* ��ü�� ��ü�� ��Ʈ�� �ϰ��� �ϴ� UI�� ���� */
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

	/* ���� �Ѿ �� ����(���� ���ӵ� ��ü���� �����ɶ� ���������� ���� �ʵ��� ��.)*/
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
	/* ĵ������ �ش� ĵ���� �� Ư�� �Լ��� ���� ���ȭ */
	std::vector<std::pair<CBase*, CallBack>> m_vecBinds;
};
END 