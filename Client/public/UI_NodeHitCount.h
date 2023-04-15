#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeHitCount final : public CUI_Node
{ 
private:
	CUI_NodeHitCount(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeHitCount(const CUI_NodeHitCount& rhs);
	virtual ~CUI_NodeHitCount() = default;

public:
	void	Set_NumTargets(_int iNumTarget) { m_iNumTargets; }
	void	Counting();
	_int	Get_NumTarget() { return m_iNumTargets; }
	_int	Get_NumHit() { return m_iNumHits; }

public:
	virtual HRESULT			Initialize_Prototype()					override;
	virtual HRESULT			Initialize(void* pArg)					override;
	virtual HRESULT			Late_Initialize(void* pArg = nullptr)	override;
	virtual void			Tick(_float fTimeDelta)					override;
	virtual void			Late_Tick(_float fTimeDelta)			override;
	virtual HRESULT			Render()								override;

private:
	virtual HRESULT			SetUp_Components()						override;
	virtual HRESULT			SetUp_ShaderResources()					override;

private:
	_tchar*					m_szTitle;
	_int					m_iNumTargets;
	_int					m_iNumHits;

	_float					m_fTime;
	_float					m_fTimeAcc;

public:
	static	CUI_NodeHitCount*	Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END

