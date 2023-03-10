#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeSkillDesc final : public CUI_Node
{
private:
	CUI_NodeSkillDesc(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeSkillDesc(const CUI_NodeSkillDesc& rhs);
	virtual ~CUI_NodeSkillDesc() = default;

public:
	void	Set_String(wstring str);

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
	_tchar*					m_szDesc;

public:
	static	CUI_NodeSkillDesc*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END