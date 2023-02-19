#pragma once
#include "UI.h"

BEGIN(Engine)
class ENGINE_DLL CUI_Node abstract : public CUI
{
public:
	CUI_Node(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_Node(const CUI_Node& rhs);
	virtual ~CUI_Node() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};
END

