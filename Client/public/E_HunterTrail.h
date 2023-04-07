#pragma once
#include "Effect_Trail.h"

BEGIN(Client)

class CE_HunterTrail final : public CEffect_Trail
{
private:
	CE_HunterTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_HunterTrail(const CE_HunterTrail& rhs);
	virtual ~CE_HunterTrail() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT			SetUp_ShaderResources();
	HRESULT			SetUp_Components();
	HRESULT			SetUp_MovementTrail();
	void			Update_Trail();

private:
	class CTexture*	m_pTrailflowTexture = nullptr;
	class CTexture*	m_pTrailTypeTexture = nullptr;
	_uint			m_iTrailFlowTexture = 4;
	_uint			m_iTrailTypeTexture = 2;

private:
	class CE_RectTrail* m_pMovementTrail = nullptr;

public:
	static  CE_HunterTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*	 Clone(void* pArg = nullptr) override;
	virtual void			 Free() override;

};

END