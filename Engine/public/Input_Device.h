#pragma once

#include "Base.h"

BEGIN(Engine)

class CInput_Device :	public CBase
{
	DECLARE_SINGLETON(CInput_Device)

private:
	CInput_Device();
	virtual ~CInput_Device() = default;

public:
	_byte			Get_DIKeyState(_ubyte byKeyID) { return m_byKeyState[byKeyID]; }
	_byte			Get_DIMouseState(MOUSEKEYSTATE byMouseID) { return m_MouseState.rgbButtons[byMouseID]; }
	_long			Get_DIMouseMove(MOUSEMOVESTATE eMoveState) { return *(((long*)&m_MouseState) + eMoveState); }
	_float			Get_KeyChargeTime(_ubyte byKeyID) { return m_fChargeTime[byKeyID]; }

public:
	_bool			Mouse_Down(MOUSEKEYSTATE MouseButton);
	_bool			Mouse_Up(MOUSEKEYSTATE MouseButton);
	_bool			Mouse_DoubleClick(MOUSEKEYSTATE MouseButton);
	_bool			Mouse_Pressing(MOUSEKEYSTATE MouseButton);
	_bool			Key_Pressing(_ubyte byKeyID);
	_bool			Key_Down(_ubyte byKeyID);
	_bool			Key_DoubleDown(_ubyte byKeyID);
	_bool			Key_Up(_ubyte byKeyID);
	_bool			Key_Charge(_ubyte byKeyID, _float fTime);
	void			Reset_EveryKey(_float fTimeDelta);

public:
	HRESULT		Ready_Input_Device(HINSTANCE hInst, HWND hWnd);
	void			Invalidate_Input_Device(_bool bWinActive);

private:
	LPDIRECTINPUTDEVICE8		m_pKeyBoard;
	LPDIRECTINPUTDEVICE8		m_pMouse;
	LPDIRECTINPUT8				m_pInputSDK;

private:
	_byte							m_byKeyState[256];
	DIMOUSESTATE				m_MouseState;
	_bool							m_bKeyState[256];
	_bool							m_bPressThisFrame[256];
	_bool							m_bUpThisFrame[256];
	_float							m_fChargeTime[256];
	_bool							m_bMouseState[3];
	_bool							m_bPressThisFrameMouse[3];
	_bool							m_bUpThisFrameMouse[3];

	_int							m_iClickedCnt = 0;

public:
	virtual void		Free(void);
};

END