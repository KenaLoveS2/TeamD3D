#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CE_KenaDamage final : public CEffect
{
private:
	CE_KenaDamage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_KenaDamage(const CE_KenaDamage& rhs);
	virtual ~CE_KenaDamage() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

private:
	_float			m_fScaleTime = 0.0f;
	_float			m_fAddValue = 0.0f;

public:
	static  CE_KenaDamage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END