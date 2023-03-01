#pragma once
#include "Client_Defines.h"
#include "UI_Event.h"

BEGIN(Client)
class CUI_Event_Guage final :public CUI_Event
{
private:
	CUI_Event_Guage();
	virtual ~CUI_Event_Guage() = default;

public:
	virtual	void		Call_Event(_float fData)				override;
	_bool				Is_FullFilled() { if (m_fGuage >= 0.99f) return true; else return false; }
	_bool				Is_Zero();
	_float				Get_GuageNow() { return m_fGuageNew; }
	void				ReArrangeSettingOn() { m_bReArrangeSettingOn = true; }

public:
	virtual	HRESULT		Tick(_float fTimeDelta)					override;
	virtual	HRESULT		Late_Tick(_float fTimeDelta)			override;
	virtual HRESULT		SetUp_ShaderResources(CShader* pShader)	override;
	virtual void		Imgui_RenderProperty()					override;
public:
	virtual HRESULT		Save_Data(Json* json);
	virtual HRESULT		Load_Data(wstring fileName);

private:
	_uint				m_iRenderPass;
	_float2				m_vAcceleration;
	_float2				m_vSpeed;
	_float4				m_vMinColor;
	_float4				m_vColor;


	_float				m_fGuage;	/* Normalized Data */
	_float				m_fGuageNew;
	_float				m_fGuageSpeed;

	_bool				m_bReArrangeSettingOn;
public:
	static CUI_Event_Guage*	Create();
	static CUI_Event_Guage*	Create(wstring fileName);
	virtual void				Free();
};
END
