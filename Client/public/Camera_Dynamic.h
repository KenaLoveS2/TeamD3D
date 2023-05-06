#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Dynamic final : public CCamera
{
private:
	CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Dynamic(const CCamera_Dynamic& rhs);
	virtual ~CCamera_Dynamic() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_Components();

private:
	_bool		m_bFix = false;
	_bool		m_bAutoTurn = false;
	_float		m_fTurnSpeed = 20.f;
	_float		m_fMouseSensitivity;

public:
	static CCamera_Dynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END