#pragma once
#include "UI_Billboard.h"

BEGIN(Client)
class CUI_FocusMonsterParts;
class CUI_FocusMonster final : public CUI_Billboard
{
public:
	enum PARTS { PARTS_LEFT, PARTS_RIGHT, PARTS_CENTER, PARTS_END };

private:
	CUI_FocusMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_FocusMonster(const CUI_FocusMonster& rhs);
	virtual ~CUI_FocusMonster() = default;
public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

public:
	void	Set_Pos(CGameObject* pTarget);
	void	Off_Focus(CGameObject* pTarget);
	void	Start_Animation();

private:
	virtual HRESULT			SetUp_Components() override;
	virtual HRESULT			SetUp_ShaderResources() override;
	HRESULT					SetUp_Parts();

private:
	CGameObject*					m_pTarget;
	CUI_FocusMonsterParts*			m_pParts[3];
	_bool							m_bStart;

public:
	static	CUI_FocusMonster*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr);
	virtual void					Free() override;
};
END