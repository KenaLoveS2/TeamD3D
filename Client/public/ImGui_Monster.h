#pragma once
#include "Client_Defines.h"
#include "ImguiObject.h"

BEGIN(Engine)
class CGameInstance;
class CTransform;
END

BEGIN(Client)

class CImGui_Monster final : public CImguiObject
{
private:
	CImGui_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual  HRESULT Initialize(void* pArg) override;
	virtual void	 Imgui_FreeRender() override;
	void			 MonsterList();

public :
	static void Load_MonsterObjects(_uint iLevel, string JsonFileName, _bool isDynamic = false);

private:
	void		Save();
	HRESULT		Load();

private:
	CGameInstance*				m_pGameInstance = nullptr;
	class CMonster*				m_pMonster = nullptr;
	_bool						m_bSaveWrite = false;
	string						m_strFileName;
	wstring						m_wstrSelectedProtoName;
	_int						m_iRoomIndex = 0;

public:
	static	CImGui_Monster*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext, void* pArg = nullptr);
	virtual void				Free() override;
};

END