#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

/* Set of Keys */

BEGIN(Client)
class CUI_NodeKey;
class CUI_NodeKeyGuide final : public CUI_Node
{
public:
	enum KEYS
	{
		KEY_R_ROTUSE,
		KEY_Q_INTERACTION,
		KEY_LB_ARROW, KEY_RB_BOMB,
		KEY_R_ROTMOVE, KEY_B_GROUND, KEY_END
	};

private:
	CUI_NodeKeyGuide(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeKeyGuide(const CUI_NodeKeyGuide& rhs);
	virtual ~CUI_NodeKeyGuide() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual HRESULT			Late_Initialize(void* pArg = nullptr) override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	virtual HRESULT			SetUp_Components() override;
	virtual HRESULT			SetUp_ShaderResources() override;

private:
	vector<CUI_NodeKey*>	m_vecKeys;


public:
	static	CUI_NodeKeyGuide*	Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END
