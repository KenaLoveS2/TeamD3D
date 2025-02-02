#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeQuestReward final : public CUI_Node
{
public:
	enum EVENT { EVENT_FADE, EVENT_END };

private:
	CUI_NodeQuestReward(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeQuestReward(const CUI_NodeQuestReward& rhs);
	virtual ~CUI_NodeQuestReward() = default;

public:
	void	RewardOn(_float4 vPos);// { m_bActive = true; }
	_bool	Is_End() { return m_bEnd; }
	_float	Get_Alpha();

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
	_bool		m_bEnd;
	_float		m_fTime;
	_float		m_fTimeAcc;
	_bool		m_bStart;

public:
	static	CUI_NodeQuestReward* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void					Free() override;
};
END
