#pragma once
#include "Client_Defines.h"
#include "UI_Event.h"

BEGIN(Engine)
class CTexture;
class CUI;
END

BEGIN(Client)
class CUI_Event_ChangeImg final : public CUI_Event
{
private:
	CUI_Event_ChangeImg();
	virtual ~CUI_Event_ChangeImg() = default;

public:
	virtual	void		Call_Event(CUI* pUI, _uint iImgIndex=1)	override;

public:
	virtual	HRESULT		Tick(_float fTimeDelta)					override;
	virtual	HRESULT		Late_Tick(_float fTimeDelta)			override;
	virtual HRESULT		SetUp_ShaderResources(CShader* pShader)	override;
	virtual void		Imgui_RenderProperty()					override;
	virtual HRESULT		Save_Data(Json* json)					override;
	virtual HRESULT		Load_Data(wstring fileName)				override;

private:
	_bool					m_bFirstCall;

	_uint					m_iLastSelected;
	_uint					m_iOriginalRenderPass;
	
	/* old */
	//CTexture*				m_pOriginal;
	//vector<CTexture*>		m_vecTextures;


public:
	static CUI_Event_ChangeImg*		Create();
	static CUI_Event_ChangeImg*		Create(wstring fileName);
	virtual void					Free();
};
END