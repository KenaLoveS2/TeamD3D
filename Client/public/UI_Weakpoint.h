#pragma once
#include "UI_Billboard.h"

BEGIN(Client)
class CKena;
class CMonster;
class CUI_Weakpoint final : public CUI_Billboard
{
private:
	CUI_Weakpoint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Weakpoint(const CUI_Weakpoint& rhs);
	virtual ~CUI_Weakpoint() = default;
public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual HRESULT			Late_Initialize(void* pArg)		override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	virtual HRESULT			SetUp_Components() override;
	virtual HRESULT			SetUp_ShaderResources() override;

private:
	CKena*			m_pKena;
	CMonster*		m_pTargetMonster;
	_float4			m_vColor;

public:
	static	CUI_Weakpoint*		Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END

