#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)
class CPlayerSkillInfo final : public CBase
{
public:
	enum LEVEL { LEVEL_0, LEVEL_1, LEVEL_2, LEVEL_3, LEVEL_4, LEVEL_END };
	enum STATE { STATE_BLOCKED, STATE_LOCKED, STATE_UNLOCKED, STATE_END };
	enum CONDITION { CONDITION_PREVSKILL, CONDITION_ROTLEVEL, CONDITION_KARMA, CONDITION_END };
	typedef struct tagPlayerSkillDesc
	{
		wstring		wstrName;
		wstring		wstrDesc;
		_int		conditions[CONDITION_END];
		STATE		eState;
	}SKILLDESC;

private:
	CPlayerSkillInfo();
	virtual ~CPlayerSkillInfo() = default;
public:
	string		Get_TypeName() { return m_strType; }
	_tchar*		Get_TextureProtoTag() { return m_pTextureProtoTag; }

private:
	HRESULT		Load_File(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext, wstring filePath);
private:
	SKILLDESC	m_tDesc[LEVEL_END];
	_tchar*		m_pTextureProtoTag;
	string		m_strType;

public:
	static CPlayerSkillInfo* Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext, wstring filePath);
	virtual void	Free() override;
};
END

