#pragma once
#include "Client_Defines.h"
#include "Effect_Point_Instancing.h"

BEGIN(Client)

class CE_P_Warrior final : public CEffect_Point_Instancing
{
private:
	CE_P_Warrior(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_P_Warrior(const CE_P_Warrior& rhs);
	virtual ~CE_P_Warrior() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	Imgui_RenderProperty()override;

public:
	void	Reset();

public:
	static  CE_P_Warrior* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;
};

END