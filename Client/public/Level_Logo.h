#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)
class CUI_NodeButton;
class CLevel_Logo final : public CLevel
{
private:
	CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Logo() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);

	HRESULT Ready_For_LevelName();
private: /*For. Imgui*/
	void		  ChoseLevel();
	void		  ChangeScene(_uint iLevel);
private:
	vector<string>	arrLevel;
	_int					iLevelIndex = -1;

private:
	vector<CUI_NodeButton*>		m_vecButtons;
	_int						m_iMouseOn;

public:
	static CLevel_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END