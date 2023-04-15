#pragma once
#include "Effect.h"

BEGIN(Client)

class CE_ShamanElectric final : public CEffect
{
private:
	CE_ShamanElectric(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_ShamanElectric(const CE_ShamanElectric& rhs);
	virtual ~CE_ShamanElectric() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg = nullptr)override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual void	Imgui_RenderProperty() override;
	virtual HRESULT Render() override;
	void			Reset();

private:
	HRESULT			SetUp_ShaderResources();
	HRESULT			SetUp_Components();

private:
	_bool			m_bFixPos = false;

public:
	static  CE_ShamanElectric* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*	   Clone(void* pArg = nullptr) override;
	virtual void               Free() override;

};

END