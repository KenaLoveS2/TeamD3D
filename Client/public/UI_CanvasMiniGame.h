#pragma once
#include "Client_Defines.h"
#include "UI_Canvas.h"
#include "UI_ClientManager.h"

BEGIN(Client)
class CUI_CanvasMiniGame final : public CUI_Canvas
{
public:
	enum UI_ORDER { UI_TIMEATK, UI_HITCOUNT, UI_VICTORY, UI_REWARD, UI_END };
	enum STATE { STATE_VICTORY, STATE_TIME_TITLE, STATE_TIME, 
		STATE_HIT_TITLE, STATE_HIT, 
		STATE_RESULT_TITLE, STATE_CAL_TIME, STATE_CAL_TARGETS, 	/*STATE_REWARD,*/    STATE_END };

private:
	CUI_CanvasMiniGame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_CanvasMiniGame(const CUI_CanvasMiniGame& rhs);
	virtual ~CUI_CanvasMiniGame() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	virtual HRESULT			Bind()							override;
	virtual HRESULT			Ready_Nodes()					override;
	virtual HRESULT			SetUp_Components()				override;
	virtual HRESULT			SetUp_ShaderResources()			override;

private: /* Bind Functions */
	void	BindFunction(CUI_ClientManager::UI_PRESENT eType, _float fValue);
	
	void	CalculateFunction(_int iState);
private:
	_bool	m_bResultShow;
	_float	m_fTime;
	_float	m_fTimeAcc;
	_int	m_iCurState;
	
	_bool	m_bCountTime;
	_bool	m_bCountHit;
	_bool	m_bReward;

	_int	m_iTimeLeft;
	_int	m_iHitCount;
	_int	m_iResult;

	_bool	m_bGameFinish;
	//_tchar* m_szHit;
	//_tchar* m_szTime;
	//_tchar* m_szResult;
	//_tchar* m_szReward;

public:
	static	CUI_CanvasMiniGame* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END

