#pragma once
#include "Camera.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCamera_Player final : public CCamera
{
private:
	CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Player(const CCamera_Player& rhs);
	virtual ~CCamera_Player() = default;

public:
	void					Set_Player(class CKena* pKena, CTransform* pTransform) { m_pKena = pKena; m_pKenaTransform = pTransform; Initialize_Position(); }

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

public:
	void					Initialize_Position();

private:
	class CKena*			m_pKena = nullptr;
	CTransform*			m_pKenaTransform = nullptr;

	_float					m_fInitCamHeight = 1.5f;
	_float					m_fAimCamHeight = 1.2f;
	_float					m_fMaxCamHeight = 2.f;
	_float					m_fCurCamHeight = 1.5f;
	_float					m_fInitDistance = 2.f;
	_float					m_fAimDistance = 0.7f;
	_float					m_fDistanceFromTarget = 2.f;
	_bool					m_bAim = false;
	_bool					m_bInitPlayerLook = false;
	_float					m_fAimTime = 0.f;

	/* Mouse */
	_int					m_iInitMouseCursorCnt = 0;
	_bool					m_bMouseFix = true;
	_float					m_fCurMouseSensitivityX = 0.f;
	_float					m_fCurMouseSensitivityY = 0.05f;
	_float					m_fInitMouseSensitivity = 0.05f;
	_long					m_LastMoveX = 0;
	_long					m_MouseMoveX = 0;
	_long					m_MouseMoveY = 0;
	_float					m_fMouseMoveTimeX = 0.f;
	_float					m_fVerticalAngle = 0.f;

public:
	static CCamera_Player*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void					Free() override;
};

END