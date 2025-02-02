#pragma once
#include "UI_Billboard.h"

/* Show Only One & Not Hidden By Other Object */
/* It's kind of Orthogonality, so need to convert perspective to Orthogonality */

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CMonster;
class CKena;
class CUI_RotIcon final : public CUI_Billboard
{
private:
	CUI_RotIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_RotIcon(const CUI_RotIcon& rhs);
	virtual ~CUI_RotIcon() = default;

public:
	void	Set_Pos(CGameObject* pTarget, _float4 vCorrect = { 0.f, 0.f, 0.f, 0.f });
	void	Off_Focus(CGameObject* pTarget);
	void	Set_Player(CKena* pKena) { m_pKena = pKena; }


public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	virtual HRESULT			SetUp_Components() override;
	virtual HRESULT			SetUp_ShaderResources() override;

private:
	CGameObject*			m_pTarget;
	CKena*					m_pKena;

public:
	static	CUI_RotIcon*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END