#pragma once
#include "Client_Defines.h"
#include "GameInstance.h"
#include "MonsterWeapon.h"

#define MAX_TRAIL_EFFECTS		200
#define MAX_RING_EFFECTS		10

BEGIN(Client)
class CEffect_Base_S2;
class CHunterArrow : public CMonsterWeapon
{
public:
	enum STATE { REDAY, FIRE, FINISH, STATE_END, };
	enum FIRE_TYPE { CHARGE, RAPID, SHOCK, SINGLE, FIRE_TYPE_END };
	enum EFFECT { EFFECT_TRAIL, EFFECT_RING, EFFECT_END };

private:
	_float4 m_vInvisiblePos = { -1000.f, -1000.f , -1000.f ,1.f };
	_uint m_iNumMeshes = 0;

	STATE m_eArrowState = STATE_END;
	FIRE_TYPE m_eFireType = CHARGE;

	_float3 m_vHandPivotPos = { 0.460f, -0.10f, -0.940f };
	_float3 m_vBowPivotPos[FIRE_TYPE_END] = {
		{ 0.320f, -10.0f, -0.45f },
		{ 0.320f, -10.0f, -2.f },
		{ 0.320f, -10.0f, -0.45f },
		{ 0.320f, -10.0f, -0.45f },
	};

	_float3 m_vColliderPivotPos = { 0.f, 0.f, 1.45f };
	_float4x4 m_ColliderPivotMatrix;

	CBone* m_pBowBone = nullptr;

public:
	CHunterArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHunterArrow(const CHunterArrow& rhs);
	virtual ~CHunterArrow() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;
	virtual void Imgui_RenderProperty() override;
	virtual void ImGui_ShaderValueProperty() override;
	virtual void ImGui_PhysXValueProperty() override;

	virtual void Update_Collider(_float fTimeDelta) override;
	virtual HRESULT SetUp_Components() override;
	virtual HRESULT SetUp_ShaderResources() override;
	virtual	HRESULT	SetUp_ShadowShaderResources() override;

public:
	static CHunterArrow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	void ArrowProc(_float fTimeDelta);

	void Set_BowBonePtr(CBone* pBone) { m_pBowBone = pBone; }

	_int Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex);
	void Execute_Ready(FIRE_TYPE eFireType);
	void Execute_Fire();
	void Execute_Finish();
	_bool IsEnd() {
		return m_eArrowState == STATE_END;
	}


	/* For. Shader & Effects */
private:
	vector<CEffect_Base_S2*>	m_vecEffects[EFFECT_END];
private: /* Trail */
	_bool						m_bTrailOn;
	_float						m_fTrailTime;
	_float						m_fTrailTimeAcc;
	_int						m_iTrailIndex;
private: /* Shockwave Ring */
	_bool						m_bShockwaveOn;
	_float						m_fRingTime;
	_float						m_fRingTimeAcc;
	_int						m_iRingIndex;


private:
	void						Play_TrailEffect(_float fTimedelta);
	void						Play_RingEffect(_float fTimeDelta);

public:
	void		Set_TrailActive(_bool bActive) { m_bTrailOn = bActive; }
	_float4		Get_ArrowHeadPos();
	HRESULT		SetUp_Effects();



};
END
