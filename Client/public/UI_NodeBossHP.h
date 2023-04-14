#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CEffect_Particle_Base;
class CUI_NodeBossHP final : public CUI_Node
{
public:
	enum EVENT_ID { EVENT_GUAGE,EVENT_END };

private:
	CUI_NodeBossHP(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeBossHP(const CUI_NodeBossHP& rhs);
	virtual ~CUI_NodeBossHP() = default;

public: /* Events */
	void		Set_Title(wstring wstrName);
	void		Set_Guage(_float fGuage);

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
	_tchar*										m_szTitle;
	CEffect_Particle_Base*			m_pEffect;

public:
	static	CUI_NodeBossHP*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END