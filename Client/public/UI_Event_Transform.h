#pragma once
#include "Client_Defines.h"
#include "UI_Event.h"

BEGIN(Engine)
class CUI;
END

/* Gradually moving with Alpha */
BEGIN(Client)
class CUI_Event_Transform final : public CUI_Event
{
	enum TAG { TAG_APPEAR, TAG_DISAPPEAR, TAG_END };
private:
	CUI_Event_Transform(CUI* pUI);
	virtual ~CUI_Event_Transform() = default;

public:
	virtual void		Call_Event(_uint iTag, _float fData)	override;
	_bool				Is_Finished() { return m_bFinished; }

public:
	virtual	HRESULT		Tick(_float fTimeDelta)					override;
	virtual	HRESULT		Late_Tick(_float fTimeDelta)			override;
	virtual HRESULT		SetUp_ShaderResources(CShader* pShader)	override;
	virtual void		Imgui_RenderProperty()					override;
public:
	virtual HRESULT		Save_Data(Json* json)					override;
	virtual HRESULT		Load_Data(wstring fileName)				override;

private:
	_bool		Appear(_float fTimeDelta);
	_bool		DisAppear(_float fTimeDelta);

private:
	TAG			m_eTag;
	_bool		m_bStart;
	_bool		m_bFinished;
	_float		m_fSpeed;
	_float		m_fAlpha;

public:
	static CUI_Event_Transform*		Create(CUI* pUI);
	static CUI_Event_Transform*		Create(wstring fileName, CUI* pUI);
	virtual void					Free();
};
END