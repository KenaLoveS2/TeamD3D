#pragma once
#include "Client_Defines.h"
#include "UI_Event.h"

BEGIN(Engine)
class CUI;
END

BEGIN(Client)
class CUI_Event_Animation final :public CUI_Event
{
private:
	CUI_Event_Animation(CUI* pUI);
	virtual ~CUI_Event_Animation() = default;

public:
	_bool	Is_Finished() { return m_bFinished; }
public:
	virtual void		Call_Event(_uint iData)					override;

public:
	virtual	HRESULT		Tick(_float fTimeDelta)					override;
	virtual	HRESULT		Late_Tick(_float fTimeDelta)			override;
	virtual HRESULT		SetUp_ShaderResources(CShader* pShader)	override;
	virtual void		Imgui_RenderProperty()					override;
public:
	virtual HRESULT		Save_Data(Json* json)					override;
	virtual HRESULT		Load_Data(wstring fileName)				override;

private:
	_bool	m_bStart;
	_bool	m_bFinished;
	_bool	m_bLoop;
	//_int	m_iFramesX;
	//_int	m_iFramesY;

	_uint			m_iTextureNum;
	_uint			m_iTextureIndex;
	vector<_int>	m_vecFramesX;
	vector<_int>	m_vecFramesY;

	_int	m_iFrameXNow;
	_int	m_iFrameYNow;

public:
	static CUI_Event_Animation*		Create(CUI* pUI);
	static CUI_Event_Animation*		Create(wstring fileName, CUI* pUI);
	virtual void					Free();
};
END