#pragma once
#include "Effect.h"
#include "Client_Defines.h"

BEGIN(Client)

class CE_KenaParryHand final : public CEffect
{
private:
	CE_KenaParryHand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_KenaParryHand(const CE_KenaParryHand& rhs);
	virtual ~CE_KenaParryHand() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Imgui_RenderProperty();

public:
	static  CE_KenaParryHand* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END