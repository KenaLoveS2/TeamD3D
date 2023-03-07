#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodePlayerSkill final : public CUI_Node
{
public:
	enum STATE { STATE_BLOCKED, STATE_LOCKED, STATE_UNLOCKED, STATE_END };

private:
	CUI_NodePlayerSkill(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodePlayerSkill(const CUI_NodePlayerSkill& rhs);
	virtual ~CUI_NodePlayerSkill() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual HRESULT			Late_Initialize(void* pArg = nullptr) override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;
public:
	virtual HRESULT			Load_Data(wstring fileName);
public:
	HRESULT	Setting(_tchar* textureProtoTag, _uint iLevel);
private:
	virtual HRESULT			SetUp_Components() override;
	virtual HRESULT			SetUp_ShaderResources() override;
	virtual void			Imgui_RenderProperty() override;

private:
	_uint		m_iLevel;
	STATE		m_eState;

public:
	static	CUI_NodePlayerSkill*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr);
	virtual void					Free() override;
};
END