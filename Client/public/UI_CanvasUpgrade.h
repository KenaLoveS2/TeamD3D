#pragma once
#include "Client_Defines.h"
#include "UI_Canvas.h"
#include "UI_ClientManager.h"

BEGIN(Client)
class CPlayerSkillInfo;
class CUI_CanvasUpgrade final : public CUI_Canvas
{
public:
	/* should be same with the order of m_vecNode.push_back()*/
	enum UI_ORDER {
		UI_END
	};

	enum SKILLTYPE { TYPE_STICK, TYPE_SHIELD, TYPE_BOW, TYPE_BOMB, TYPE_END };

private:
	CUI_CanvasUpgrade(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_CanvasUpgrade(const CUI_CanvasUpgrade& rhs);
	virtual ~CUI_CanvasUpgrade() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	virtual HRESULT			Bind()							override;
	virtual HRESULT			Ready_Nodes()					override;
	virtual HRESULT			SetUp_Components()				override;
	virtual HRESULT			SetUp_ShaderResources()			override;
private:
	HRESULT					Ready_PlayerSkill();
	CPlayerSkillInfo*		m_pPlayerSkills[TYPE_END];

private: /* Bind Functions */
	void	BindFunction(CUI_ClientManager::UI_PRESENT eType, CUI_ClientManager::UI_FUNCTION eFunc, _float fValue);
private:
	void	Default(CUI_ClientManager::UI_PRESENT eType, _float fValue);
	void	LevelUp(CUI_ClientManager::UI_PRESENT eType, _int iLevel);

private:

public:
	static	CUI_CanvasUpgrade*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END
