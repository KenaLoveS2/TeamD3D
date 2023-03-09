#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"
#include "SkillInfo.h"

BEGIN(Client)
class CUI_NodeSkillCond final : public CUI_Node
{
private:
	CUI_NodeSkillCond(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeSkillCond(const CUI_NodeSkillCond& rhs);
	virtual ~CUI_NodeSkillCond() = default;

public:
	void	Set_Condition(CSkillInfo::SKILLDESC tDesc, CSkillInfo::CHECK eCheck);

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
	CSkillInfo::CHECK		m_eCondition;
	_float4					m_vColor;
	_tchar*					m_szCondition;
	_float					m_fFontPos;

public:
	static	CUI_NodeSkillCond*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END