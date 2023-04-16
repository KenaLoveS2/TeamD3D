#pragma once
#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CGameInstance;
class CRenderer;
END

BEGIN(Client)
class CCamera_Photo : public CCamera
{
public:
	enum STATE { MOVE, WAIT, PHOTO_TIME, PHOTO_END, STATE_END};

private:	
	CRenderer* m_pRendererCom = nullptr;
	CGameInstance* m_pGameInstance = nullptr;
	
	STATE m_eState = STATE_END;

	class CKena* m_pKena = nullptr;
	class CRot* m_pFirstRot = nullptr;
	class CSaiya* m_pNpcSaiya = nullptr;

	_float4 m_vTurnDir, m_vOldAt;
	_float4 m_vTargetEye, m_vTargetAt;

	_float m_fSpeedPerSec = 0.1f;

	_bool m_bEyeComplete = false, m_bAtComplete = false;

private:
	CCamera_Photo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Photo(const CCamera_Photo& rhs);
	virtual ~CCamera_Photo() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Late_Initialize(void* pArg) override;
	virtual void Tick(_float TimeDelta) override;
	virtual void Late_Tick(_float TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

	static CCamera_Photo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
	
	HRESULT Setup_Component();

	_bool Is_Work();
	void Execute_Move(_float4 vInitEye, _float4 vInitAt);

	void Set_KenaPtr(class CKena* pKena) { m_pKena = pKena; }
	void Set_FirstRotPtr(class CRot* pFirstRot) { m_pFirstRot = pFirstRot; }
	void Set_NpcSaiyaPtr(class CSaiya* pNpcSaiya) { m_pNpcSaiya = pNpcSaiya; }
};
END