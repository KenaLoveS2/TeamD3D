#pragma once
#include "Effect_Point_Instancing.h"

BEGIN(Client)

class CE_P_Bombplatform final : public CEffect_Point_Instancing
{
private:
	CE_P_Bombplatform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_P_Bombplatform(const CE_P_Bombplatform& rhs);
	virtual ~CE_P_Bombplatform() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg = nullptr);
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();

public:
	void	Update_Particle(_float fTimeDelta);
	void	Reset();

	void	TurnOffPlatform(_bool Dissolve) {
		m_fLife = 0.0f;
		m_bDissolve = Dissolve;
	}

private:
	_bool	m_bDissolve = false;

private:
	class CE_P_CommonBox* m_pCommonBox = nullptr;

public:
	static  CE_P_Bombplatform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;
};

END