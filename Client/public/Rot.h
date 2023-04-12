#pragma once
#include "Rot_Base.h"

BEGIN(Client)
class CRot : public CRot_Base
{
public:
	typedef struct tagRotDesc
	{
		_float4x4 WorldMatrix;
		_int			iRoomIndex;
	}DESC;

	DESC Get_Desc() { return m_Desc; }

private:
	static _uint m_iEveryRotCount;
	static _uint m_iKenaFindRotCount;
	static vector<CRot*> m_vecKenaConnectRot;
	static _float4 m_vKenaPos;
	_uint m_iThisRotIndex = 0;
	_uint m_iBuyHatIndex = 0;
	
	class CRotWisp* m_pRotWisp = nullptr;	
	class CRope_RotRock* m_pRopeRotRock = nullptr;
	_float4 m_vWakeUpPosition;
	
	class CRotHat* m_pRotHat = nullptr;
	class CE_P_Rot* m_pRotAcc = nullptr;

private:
	_bool m_bWakeUp = false, m_bManualWakeUp = false;
	_uint m_iCuteAnimIndex = 0;
	_float m_fKenaToRotDistance = 1.2f;
	_float m_fTeleportDistance = 7.5f;
	DESC m_Desc;

	class CCameraForRot*		m_pMyCam = nullptr;

private:
	CRot(ID3D11Device* pDevice, ID3D11DeviceContext* p_context);
	CRot(const CRot& rhs);
	virtual ~CRot() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void * pArg);
	
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	
	virtual void Imgui_RenderProperty() override;
	virtual void ImGui_AnimationProperty() override;
	virtual void ImGui_ShaderValueProperty() override;
	virtual void ImGui_PhysXValueProperty() override;
	void AlreadyRot();

private:	
	virtual HRESULT SetUp_State() override;

public:
	static CRot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
		
	virtual _int Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex) override;
	virtual _int Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;

	static void Set_RotUseKenaPos(_float4 vKenaPos) { memcpy(&m_vKenaPos, &vKenaPos, sizeof(_float4)); }
	static _float4 Get_RotUseKenaPos() { return m_vKenaPos; }
	void			Set_WispPos(_float4 vPos);
	void			Set_WakeUpPos(_float4 vPos);
	void Create_Hat(_uint iHatIndex);
	void Buy_Hat(_uint iHatIndex);
	_bool IsBuyPossible();
	_int Get_KenaConnectRotCount();
	_int Get_HaveHatRotCount();
	_int Get_NoHatRotCount();
	static void Clear();
};

END