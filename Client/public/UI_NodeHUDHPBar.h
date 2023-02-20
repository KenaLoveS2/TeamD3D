#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeHUDHPBar final : public CUI_Node
{
private:
	CUI_NodeHUDHPBar(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeHUDHPBar(const CUI_NodeHUDHPBar& rhs);
	virtual ~CUI_NodeHUDHPBar() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

	virtual void			Imgui_RenderProperty();

private:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();

public:
	static	CUI_NodeHUDHPBar*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END