#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeScrollLine final : public CUI_Node
{
private:
	CUI_NodeScrollLine(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeScrollLine(const CUI_NodeScrollLine& rhs);
	virtual ~CUI_NodeScrollLine() = default;

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
	static	CUI_NodeScrollLine*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr);
	virtual void					Free() override;
};
END