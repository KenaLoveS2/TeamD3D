#pragma once
#include "Client_Defines.h"
#include "UI_Canvas.h"
#include "UI_ClientManager.h"

BEGIN(Client)
class CSkillInfo;
class CUI_NodeSkill;
class CUI_NodeEffect;
class CKena;
class CUI_CanvasUpgrade final : public CUI_Canvas
{
public:
	/* should be same with the order of m_vecNode.push_back()*/
	enum UI_ORDER {
		UI_PLAYERSKILL_START = 0, UI_PLAYERSKILL_END = 19,
		UI_ROTSKILLS_START = 20, UI_ROTSKILL_END = 23, 
		UI_EFFECT_RING = 24, UI_EFFECT_BLUE = 25,
		UI_LOCK_START = 26, UI_LOCK_END = 45,
		UI_SKILLVIDEO = 46, UI_SKILLNAME, UI_SKILLCOND, UI_SKILLDESC,
		UI_ROTLEVEL, UI_ROTGUAGE,
		UI_END
	};

	enum SKILLTYPE { TYPE_STICK, TYPE_SHIELD, TYPE_BOW, TYPE_BOMB, TYPE_ROT, TYPE_END };
	enum EFFECT { EFFECT_RING, EFFECT_BLUE, EFFECT_END };
private:
	CUI_CanvasUpgrade(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_CanvasUpgrade(const CUI_CanvasUpgrade& rhs);
	virtual ~CUI_CanvasUpgrade() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual HRESULT			Late_Initialize(void* pArg)		override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

public:
	virtual void			Common_Function(_bool bResult)	override;
	virtual void			Set_Caller(CGameObject* pObj)   override;

private:
	virtual HRESULT			Bind()							override;
	virtual HRESULT			Ready_Nodes()					override;
	virtual HRESULT			SetUp_Components()				override;
	virtual HRESULT			SetUp_ShaderResources()			override;

private:
	HRESULT					SetUp_SkillSettings(_uint iType, _uint iSlot);

	/* Ready_Information(Real Data) */
private:
	HRESULT					Ready_SkillInfo();
	CSkillInfo*				m_pSkills[TYPE_END];

private:
	void		Picking();
	void		Spread();

private:
	CUI_NodeSkill*				m_pSelected; /* LastSelectedOne*/
	vector<CUI_NodeEffect*>		m_vecEffects;
	_int						m_iPickedIndex;
	_bool						m_bPick; /* Is Skill Icon Picked? */
	CKena*						m_pPlayer;

public:
	static	CUI_CanvasUpgrade*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END
