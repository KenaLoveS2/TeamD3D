#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CEffect_Particle_Base;
class CUI_NodeReward final : public CUI_Node
{
private:
	CUI_NodeReward(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeReward(const CUI_NodeReward& rhs);
	virtual ~CUI_NodeReward() = default;

public:
	void	Open() { m_bActive = true;  m_bOpen = false; m_fTimeAcc = 0.f; m_iTextureIdx = 0; }

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

private:
	/* For. Particle */
	CEffect_Particle_Base*	m_pParticle;
	_bool					m_bOpen;
	_float					m_fTime;
	_float					m_fTimeAcc;

public:
	static	CUI_NodeReward*		Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END