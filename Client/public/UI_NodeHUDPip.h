#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeHUDPip final : public CUI_Node
{
public:
	enum EVENT_ID	{ EVENT_GUAGE, EVENT_TEXCHANGE, EVENT_END };
	enum TEXTURE_ID { TEX_DEFAULT, TEX_FULL, TEX_END };
private:
	CUI_NodeHUDPip(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeHUDPip(const CUI_NodeHUDPip& rhs);
	virtual ~CUI_NodeHUDPip() = default;

public:
	void			Set_Guage(_float fGuage);

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();

private: 
	_bool			m_bFullFilled;

public:
	static	CUI_NodeHUDPip*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END