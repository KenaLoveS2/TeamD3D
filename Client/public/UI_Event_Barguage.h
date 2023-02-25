#pragma once
#include "Client_Defines.h"
#include "UI_Event.h"

BEGIN(Client)
class CUI_Event_Barguage final :public CUI_Event
{
private:
	CUI_Event_Barguage();
	virtual ~CUI_Event_Barguage() = default;

public:
	void			Set_Guage(_float fGauge) { m_fGuage = fGauge; }

public:
	virtual	HRESULT		Tick(_float fTimeDelta)					override;
	virtual	HRESULT		Late_Tick(_float fTimeDelta)			override;
	virtual HRESULT		SetUp_ShaderResources(CShader* pShader)	override;
	virtual void		Imgui_RenderProperty()					override;
	virtual	void		Call_Event(_float fData)				override;
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

public:
	static CUI_Event_Barguage*	Create(wstring fileName);
	virtual void				Free();
};
END
