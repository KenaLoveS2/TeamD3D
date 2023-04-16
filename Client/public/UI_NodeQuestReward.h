#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeQuestReward final : public CUI_Node
{
public:
	enum EVENT { EVENT_FADE, EVENT_END };

private:
	CUI_NodeQuestReward(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeQuestReward(const CUI_NodeQuestReward& rhs);
	virtual ~CUI_NodeQuestReward() = default;

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
	static	CUI_NodeQuestReward*	Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr);
	virtual void					Free() override;
};
END
