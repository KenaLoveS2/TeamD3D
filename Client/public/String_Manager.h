#pragma once
#include "Client_Defines.h"
#include "GameInstance.h"

BEGIN(Client)
class CString_Manager :	public CBase
{
	DECLARE_SINGLETON(CString_Manager)
public:
	enum TYPE {TYPE_STATIC, };
private:


private:
	CString_Manager();
	virtual ~CString_Manager() = default;

public:
	virtual void Free();
};
END
