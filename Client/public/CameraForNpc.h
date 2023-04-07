#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCameraForNpc : public CCamera
{
public:
	enum NPCOFFSET
	{
		OFFSET_FRONT_LERP,
		OFFSET_FRONT,
		OFFSET_END
	};

private:
	CCameraForNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCameraForNpc(const CCameraForNpc& rhs);
	virtual ~CCameraForNpc() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Late_Initialize(void* pArg) override;
	virtual void Tick(_float TimeDelta) override;
	virtual void Late_Tick(_float TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

	void Set_Target(class CGameObject* pTarget, _uint iOffsetType, _float fOffsetY, _float fOffsetMulLook, _float fTime = 1.3f)
	{
		m_pTarget = pTarget;
		m_bInitSet = true;
		m_iOffsetType   = iOffsetType;
		m_fLerpTime = fTime;
	}

	class CGameObject* m_pTarget = nullptr;
	_float4						m_vTargetPos;

	class CCamera*		m_pPlayerCam = nullptr;
	_bool						m_bInitSet = false;

	_uint							m_iOffsetType = OFFSET_END;
	_float						m_fOffsetY = 0.4f;
	_float						m_fOffsetMulLook = 1.f;
	_float						m_fLerpTime = 1.3f;

public:
	static CCameraForNpc* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END