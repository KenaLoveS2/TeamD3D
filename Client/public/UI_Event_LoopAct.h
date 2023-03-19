#pragma once
#include "Client_Defines.h"
#include "UI_Event.h"

BEGIN(Client)
class CUI_Event_LoopAct final : public CUI_Event
{
public:
	enum TYPE { TYPE_LOOP_VRT, TYPE_END };
private:
	CUI_Event_LoopAct();
	virtual ~CUI_Event_LoopAct() = default;
public:
	virtual	void		Call_Event(_bool bStart)				override;
	virtual	HRESULT		Tick(_float fTimeDelta)					override;
	virtual	HRESULT		Late_Tick(_float fTimeDelta)			override;
	virtual HRESULT		SetUp_ShaderResources(CShader* pShader)	override;
	virtual void		Imgui_RenderProperty()					override;
	virtual HRESULT		Save_Data(Json* json);
	virtual HRESULT		Load_Data(wstring fileName);
public:
	void		Initialize(CUI* pParent, TYPE eType, _float fDistance, _float fSpeed);
private:
	void		Act_Loop_Verticle(_float fTimeDelta);


private:
	TYPE		m_eType;
	_float		m_fDistance;
	_float		m_fSpeed;

public:
	static CUI_Event_LoopAct*	Create();
	static CUI_Event_LoopAct*	Create(CUI* pParent, TYPE eType, _float fDistance, _float fSpeed);
public:
	virtual void				Free();
};
END