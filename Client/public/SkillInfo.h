#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)
class CKena;
class CSkillInfo final : public CBase
{

public:
	enum LEVEL { LEVEL_0, LEVEL_1, LEVEL_2, LEVEL_3, LEVEL_4, LEVEL_END };
	enum STATE { STATE_BLOCKED, STATE_LOCKED, STATE_UNLOCKED, STATE_END };
	enum CONDITION { CONDITION_PREVSKILL, CONDITION_ROTLEVEL, CONDITION_KARMA, CONDITION_END };
	enum CHECK { CHECK_PREVSKILL, CHECK_ROTLEVEL, CHECK_KARMA, 
		CHECK_UNLOCKED_AVAILABLE, CHECK_UNLOCKED_ALREADY, CHECK_END };

	typedef struct tagSkillDesc
	{
		wstring		wstrName;
		wstring		wstrDesc;
		_int		conditions[CONDITION_END];
		STATE		eState;
		wstring		wstrVideo;
	}SKILLDESC;

private:
	CSkillInfo();
	virtual ~CSkillInfo() = default;
public:
	SKILLDESC	Get_SkillDesc(_uint iLevel) { return m_tDesc[iLevel]; }
	string		Get_TypeName() { return m_strType; }
	_tchar*		Get_TextureProtoTag() { return m_pTextureProtoTag; }
public:
	void 		UnLock(_uint iLevel) { m_tDesc[iLevel].eState = STATE_UNLOCKED; }
	CHECK		Check(_uint iLevel, CKena* pPlayer = nullptr);
private:
	HRESULT		Load_File(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext, wstring filePath);
private:
	SKILLDESC	m_tDesc[LEVEL_END];
	_tchar*		m_pTextureProtoTag;
	string		m_strType;

public:
	static CSkillInfo* Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext, wstring filePath);
	virtual void	Free() override;
};
END

