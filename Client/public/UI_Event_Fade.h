#pragma once
#include "Client_Defines.h"
#include "UI_Event.h"

BEGIN(Client)
class CUI_Event_Fade final :public CUI_Event
{
private:
	CUI_Event_Fade();
	virtual ~CUI_Event_Fade() = default;

public:
	virtual	void		Call_Event(_bool bStart)				override;
	void				FadeOut();
	_float				Get_Alpha() { return m_fAlpha; }
	_bool				Is_Appear() {
		if (m_bInOut && m_fSpeed > 0.0f)
			return true;
		else
			return false;
	}
	_bool				Is_DisAppear() {
		if (m_bInOut && m_fSpeed < 0.0f)
			return true;
		else
			return false;
	}

public:
	virtual	HRESULT		Tick(_float fTimeDelta)					override;
	virtual	HRESULT		Late_Tick(_float fTimeDelta)			override;
	virtual HRESULT		SetUp_ShaderResources(CShader* pShader)	override;
	virtual void		Imgui_RenderProperty()					override;
public:
	virtual HRESULT		Save_Data(Json* json);
	virtual HRESULT		Load_Data(wstring fileName);
public:
	void				Initialize(_float fSpeed, _float fDurTime);

private:
	_bool			m_bStart;
	_bool			m_bInOut;

	_float			m_fAlpha;
	_float			m_fSpeed;

public:
	static CUI_Event_Fade*	Create();
	static CUI_Event_Fade*	Create(wstring fileName);
	static CUI_Event_Fade*	Create(_float fSpeed, _float fDurTime);

	virtual void				Free();
};
END
