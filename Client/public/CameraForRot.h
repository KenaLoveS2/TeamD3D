#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCameraForRot : public CCamera
{
private:
	CCameraForRot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCameraForRot(const CCameraForRot& rhs);
	virtual ~CCameraForRot() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Late_Initialize(void* pArg) override;
	virtual void Tick(_float TimeDelta) override;
	virtual void Late_Tick(_float TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

	void Set_Target(class CGameObject* pTarget)
	{
		m_pTarget = pTarget;
		m_bInitSet = true;
		m_bCloseflag = false;
	}

	class CGameObject* m_pTarget = nullptr;
	_float4						m_vTargetPos;

	class CCamera*		m_pPlayerCam = nullptr;
	_bool						m_bInitSet = false;
	_bool						m_bCloseflag = false;

public:
	static CCameraForRot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	_bool IsClosed() { return m_bCloseflag; }	
	void Clear();
};

END