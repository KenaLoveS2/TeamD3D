#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeAmmoBombFrame final : public CUI_Node
{
private:
	CUI_NodeAmmoBombFrame(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeAmmoBombFrame(const CUI_NodeAmmoBombFrame& rhs);
	virtual ~CUI_NodeAmmoBombFrame() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();

public:
	static	CUI_NodeAmmoBombFrame*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*				Clone(void* pArg = nullptr);
	virtual void						Free() override;
};
END