#pragma once
#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CLightCamera : public CCamera
{
private:
	CLightCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLightCamera(const CLightCamera& rhs);
	virtual ~CLightCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Tick(_float TimeDelta) override;
	virtual void Late_Tick(_float TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

private:
	_bool m_bSwitch = false;

public:
	static CLightCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END