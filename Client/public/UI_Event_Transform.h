#pragma once
#include "Client_Defines.h"
#include "UI_Event.h"

BEGIN(Engine)
class CUI;
END

BEGIN(Client)
class CUI_Event_Transform final : public CUI_Event
{
public:
	enum TAG { TAG_SCALE_BY_PERCENT, TAG_SCALE_BY_TIME, TAG_TRANS_BY_PERCENT, TAG_TRANS_BY_TIME, TAG_END };

private:
	CUI_Event_Transform(CUI* pUI);
	virtual ~CUI_Event_Transform() = default;

public:
	virtual void		Call_Event(_uint iTag, _float fData)	override;
	_bool				Is_Finished() { m_bFinished; }

public:
	virtual	HRESULT		Tick(_float fTimeDelta)					override;
	virtual	HRESULT		Late_Tick(_float fTimeDelta)			override;
	virtual HRESULT		SetUp_ShaderResources(CShader* pShader)	override;
	virtual void		Imgui_RenderProperty()					override;
public:
	virtual HRESULT		Save_Data(Json* json)					override;
	virtual HRESULT		Load_Data(wstring fileName)				override;

private:
	TAG			m_eTag;
	_bool		m_bStart;
	_bool		m_bFinished;

	/* For. Scale */
	_float3		m_vSourScale;
	_float3		m_vDestScale;
	_float3		m_vScaleNow;
	_float		m_fScaleSpeed;

	/* For. Translation */
	_float4		m_vSourTranslation;
	_float4		m_vDestTranslation;
	_float4		m_vTranslationNow;
	_float		m_fTransSpeed;

	/* For. Rotation */
	_float3		m_vRotAxis;
	_float		m_fRotAngle;
	_float		m_fRotAngleNow;
	_float		m_fRotSpeed;

public:
	static CUI_Event_Transform*		Create(CUI* pUI);
	static CUI_Event_Transform*		Create(wstring fileName, CUI* pUI);
	virtual void					Free();
};
END