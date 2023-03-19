#pragma once
#include "Camera.h"
#include "Client_Defines.h"

BEGIN(Client)

struct CCamOffset : public CBase
{
	_float		fCamHeight = 0.f;
	_float		fCamDistance = 0.f;
	_float		fCamRightDist = 0.f;
	_float		fLerpDuration = 0.f;
	_bool		bPlayerControl = false;
	_float4	vLastPos = _float4::UnitW;

	CCamOffset() {}
	CCamOffset(_float fHeight, _float fDistance, _float fRightDist, _float fLerpDuration, _bool bPlayerControl)
		: fCamHeight{ fHeight }, fCamDistance{ fDistance }, fCamRightDist{ fRightDist }, fLerpDuration{ fLerpDuration }, bPlayerControl { bPlayerControl }
	{
	}
	~CCamOffset() = default;
	virtual void	Free() {};
};

class CCamera_Player final : public CCamera
{
public:
	enum CAMOFFSET {
		CAMOFFSET_DEFAULT, CAMOFFSET_AIM, CAMOFFSET_AIR_AIM, CAMOFFSET_INJECTBOW, CAMOFFSET_PULSE, CAMOFFSET_HEAVYATTACK, CAMOFFSET_END
	};

private:
	CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Player(const CCamera_Player& rhs);
	virtual ~CCamera_Player() = default;

public:
	void					Set_Player(class CKena* pKena, CTransform* pTransform) { m_pKena = pKena; m_pKenaTransform = pTransform; Initialize_Position(); }
	void					Set_CamOffset(CAMOFFSET eOffset);

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual void			Imgui_RenderProperty() override;

public:
	void					Initialize_Position();
	void					Camera_Shake(_float fPower, _uint iCount);	/* Random Shake */
	void					Camera_Shake(_float4 vDir, _float fPower, _float fDuration);	/* Dir Shake */
	void					TimeSleep(_float fDuration);

private:
	class CKena*			m_pKena = nullptr;
	CTransform*			m_pKenaTransform = nullptr;

	map<CAMOFFSET, CCamOffset*>	m_mapCamOffset;
	CCamOffset*			m_pCurOffset = nullptr;
	CCamOffset*			m_pPreOffset = nullptr;
	_float					m_fCurLerpTime = 0.f;

	_float					m_fTimeSleep = 0.f;
	_float					m_fShakeRatio = 0.f;
	list<_float4>			m_ShakeValueList;

	//_float					m_fInitCamHeight = 1.5f;
	//_float					m_fAimCamHeight = 1.2f;
	//_float					m_fMaxCamHeight = 2.f;
	//_float					m_fCurCamHeight = 1.5f;
	//_float					m_fInitDistance = 2.f;
	//_float					m_fAimDistance = 0.7f;
	//_float					m_fDistanceFromTarget = 2.f;
	//_bool					m_bAim = false;
	//_bool					m_bInitPlayerLook = false;
	//_float					m_fAimTime = 0.f;

	/* Mouse */
	_bool					m_bMouseFix = true;
	_float					m_fCurMouseSensitivityX = 0.f;
	_float					m_fCurMouseSensitivityY = 0.05f;
	_float					m_fInitMouseSensitivity = 0.05f;
	_long					m_LastMoveX = 0;
	_long					m_MouseMoveX = 0;
	_long					m_MouseMoveY = 0;
	_float					m_fVerticalAngle = 0.f;

private:
	_float4				Calculate_CamPosition(CAMOFFSET eOffset, _fvector vTargetPos);

public:
	static CCamera_Player*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void					Free() override;
};

END