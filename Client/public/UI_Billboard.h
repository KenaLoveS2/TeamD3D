#pragma once
#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CUI_Billboard abstract : public CUI
{
protected:
	CUI_Billboard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Billboard(const CUI_Billboard& rhs);
	virtual ~CUI_Billboard() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

public:
	virtual HRESULT			Save_Data()						override;
	virtual HRESULT			Load_Data(wstring fileName)		override;

public:
	virtual void Free() override;
};
END
