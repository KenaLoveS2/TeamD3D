#pragma once
#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CGameInstance;
class CTransform;
END

BEGIN(Client)
class CCamera_Shaman : public CCamera
{
public:
	enum STATE { FIND_SHAMAN, WAIT_SHAMAN_ACTTION, STATE_END};

private:
	CGameObject* m_pTarget = nullptr;
	CTransform* m_pTargetTransform = nullptr;

	CGameInstance* m_pGameInstance = nullptr;
	STATE m_eState = STATE_END;

	_float4 m_vTargetPos;

	_float4 m_vTargetPosOffset = {0.f, 2.f, 0.f, 0.f};
	_float m_fTargetDistance = 1.f;
	
private:
	CCamera_Shaman(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Shaman(const CCamera_Shaman& rhs);
	virtual ~CCamera_Shaman() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Late_Initialize(void* pArg) override;
	virtual void Tick(_float TimeDelta) override;
	virtual void Late_Tick(_float TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

	static CCamera_Shaman* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	void Calc_TargetPos();
	void Start_Action(CGameObject* pShamanPtr, _float4 vKenaPos);
	void Clear();

	_bool Is_WaitShamanAction() { return m_eState == WAIT_SHAMAN_ACTTION; }
	
	inline void Set_TargetPositionOffset(_float4 vOffset) { m_vTargetPosOffset = vOffset; }
	inline _float4 Get_TargetPositionOffset() { return m_vTargetPosOffset; }

	inline void Set_TargetDistance(_float fDistance) { m_fTargetDistance = fDistance; }	
	inline _float Get_TargetDistance() { return m_fTargetDistance; }
};
END