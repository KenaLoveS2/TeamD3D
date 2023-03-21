#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeHatCartBG final : public CUI_Node
{
private:
	CUI_NodeHatCartBG(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeHatCartBG(const CUI_NodeHatCartBG& rhs);
	virtual ~CUI_NodeHatCartBG() = default;

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
	static	CUI_NodeHatCartBG*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr);
	virtual void					Free() override;
};
END
