#pragma once
#include "Effect.h"

BEGIN(Client)

class CE_KenaDashRing final : public CEffect
{
private:
	CE_KenaDashRing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_KenaDashRing(const CE_KenaDashRing& rhs);
	virtual ~CE_KenaDashRing() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void			Imgui_RenderProperty();

public:
	static  CE_KenaDashRing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END