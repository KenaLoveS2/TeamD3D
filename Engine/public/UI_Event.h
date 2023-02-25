#pragma once
#include "Base.h"

BEGIN(Engine)
class CShader;
class CUI;
class ENGINE_DLL CUI_Event abstract : public CBase
{
public:
	typedef struct tagVariables
	{
		_float		fTimeDelta;
		_float		fTime;
		_float2		vSpeed;
		_float2		vAcceleration;
		_float4		vColor;
		_float4		vMinColor;
		_float		fData;
		_uint		iData;
	} VARIABLES;

protected:
	CUI_Event();
	virtual ~CUI_Event() = default;

public:
	void	Set_UIName(const _tchar* cloneTag);
public:
	virtual	HRESULT		Tick(_float fTimeDelta) = 0;
	virtual	HRESULT		Late_Tick(_float fTimeDelta) = 0;
	virtual HRESULT		SetUp_ShaderResources(CShader* pShader) = 0;
	virtual void		Imgui_RenderProperty() = 0;

public:
	virtual HRESULT	Save_Data(Json* json) = 0;
	virtual HRESULT Load_Data(wstring fileName) = 0;

public: /* Provided Function */
	virtual	void	Call_Event(_uint iData) {};
	virtual	void	Call_Event(_float fData) {};
	virtual	void	Call_Event(CUI* pUI, _uint iIndex = 1) {};

protected:
	const char*			m_szEventName; 
	_uint				m_iRenderPass= 0;
	const char*			m_szUIName;
	
	_float				m_fTime;
	_float				m_fTimeAcc;

public:
	virtual void Free();
};
END