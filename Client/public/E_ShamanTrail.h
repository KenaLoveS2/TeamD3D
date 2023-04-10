#pragma once
#include "Effect_Trail.h"

BEGIN(Client)

class CE_ShamanTrail final : public CEffect_Trail
{
private:
	CE_ShamanTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_ShamanTrail(const CE_ShamanTrail& rhs);
	virtual ~CE_ShamanTrail() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void			Reset();

private:
	HRESULT			SetUp_ShaderResources();
	HRESULT			SetUp_Components();
	HRESULT			SetUp_MovementTrail();
	void			Update_Trail();
	void			ToolTrail(const char* ToolTag);
private:
	class CTexture*	m_pTrailflowTexture = nullptr;
	class CTexture*	m_pTrailTypeTexture = nullptr;
	_uint			m_iTrailFlowTexture = 4;
	_uint			m_iTrailTypeTexture = 2;

private:
	class CE_RectTrail* m_pMovementTrail = nullptr;
	_float				m_fDurationTima = 0.0f;

public:
	static  CE_ShamanTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*	 Clone(void* pArg = nullptr) override;
	virtual void			 Free() override;

};

END