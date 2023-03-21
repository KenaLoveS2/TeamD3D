#pragma once
#include "Effect.h"
#include "Client_Defines.h"

BEGIN(Client)

class CE_Sapling final : public CEffect
{
private:
	CE_Sapling(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_Sapling(const CE_Sapling& rhs);
	virtual ~CE_Sapling() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty()override;

public:
	void	Set_Child();

public:
	static  CE_Sapling* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END