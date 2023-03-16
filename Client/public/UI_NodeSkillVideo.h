#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeSkillVideo final : public CUI_Node
{
private:
	CUI_NodeSkillVideo(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeSkillVideo(const CUI_NodeSkillVideo& rhs);
	virtual ~CUI_NodeSkillVideo() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	virtual HRESULT			SetUp_Components() override;
	virtual HRESULT			SetUp_ShaderResources() override;

private:
	HRESULT		Initialize_MF();

public:
	static	CUI_NodeSkillVideo*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END