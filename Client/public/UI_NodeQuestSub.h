#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeQuestSub final : public CUI_Node
{
private:
	CUI_NodeQuestSub(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeQuestSub(const CUI_NodeQuestSub& rhs);
	virtual ~CUI_NodeQuestSub() = default;

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
	static	CUI_NodeQuestSub*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr);
	virtual void					Free() override;
};
END