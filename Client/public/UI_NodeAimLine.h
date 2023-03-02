#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeAimLine final : public CUI_Node
{
private:
	CUI_NodeAimLine(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeAimLine(const CUI_NodeAimLine& rhs);
	virtual ~CUI_NodeAimLine() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();


public:
	void			Shrink(_bool bStart) { m_bStart = bStart; }
private: 
	_bool			m_bStart;
	_bool			m_bSetInitialRatio;
	_float			m_fInitialRatioX;
	_float			m_fInitialRatioY;

public:
	static	CUI_NodeAimLine*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr);
	virtual void					Free() override;
};
END

