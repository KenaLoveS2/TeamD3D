#pragma once
#include "UI_Billboard.h"

BEGIN(Client)
class CUI_FocusMonsterParts final : public CUI_Billboard
{
private:
	CUI_FocusMonsterParts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_FocusMonsterParts(const CUI_FocusMonsterParts& rhs);
	virtual ~CUI_FocusMonsterParts() = default;


public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	virtual HRESULT			SetUp_Components() override;
	virtual HRESULT			SetUp_ShaderResources() override;

public:
	static	CUI_FocusMonsterParts*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*				Clone(void* pArg = nullptr);
	virtual void						Free() override;
};
END