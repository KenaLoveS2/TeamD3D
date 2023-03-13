#pragma once
#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CDynamicLightCamera	: public CCamera
{
private:
	CDynamicLightCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDynamicLightCamera(const CDynamicLightCamera& rhs);
	virtual ~CDynamicLightCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Late_Initialize(void* pArg) override;
	virtual void Tick(_float TimeDelta) override;
	virtual void Late_Tick(_float TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

private:
	_float4									m_vPivotPos;
	class CGameObject*				m_pKena = nullptr;
	_float4									m_vKenaPos;

public:
	static CDynamicLightCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END