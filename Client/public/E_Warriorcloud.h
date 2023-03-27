#pragma once
#include "Effect.h"
#include "Client_Defines.h"

BEGIN(Client)

class CE_Warriorcloud final : public CEffect
{
private:
	CE_Warriorcloud(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_Warriorcloud(const CE_Warriorcloud& rhs);
	virtual ~CE_Warriorcloud() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void			Imgui_RenderProperty() override;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

private:
	HRESULT			SetUp_Child();
	class CBossWarrior*		m_pWarrior = nullptr;

public:
	static CE_Warriorcloud*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath =nullptr);
	virtual CGameObject*	Clone(void* pArg = nullptr)  override;
	virtual void			Free() override;
};

END