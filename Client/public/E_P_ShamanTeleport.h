#pragma once
#include "Effect_Point_Instancing.h"

BEGIN(Client)

class CE_P_ShamanTeleport final : public CEffect_Point_Instancing
{
private:
	CE_P_ShamanTeleport(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_P_ShamanTeleport(const CE_P_ShamanTeleport& rhs);
	virtual ~CE_P_ShamanTeleport() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	Imgui_RenderProperty()override;

public:
	void	Reset();

private:
	_float  m_fDurationTime = 0.0f;

public:
	static  CE_P_ShamanTeleport* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;
};

END