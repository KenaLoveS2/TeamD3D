#pragma once
#include "Effect.h"

BEGIN(Client)

class CE_FireBrazier final : public CEffect
{
private:
	CE_FireBrazier(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_FireBrazier(const CE_FireBrazier& rhs);
	virtual ~CE_FireBrazier() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg = nullptr);
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	Imgui_RenderProperty()override;

public:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();
	void			Reset();

public:
	static  CE_FireBrazier*     Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END