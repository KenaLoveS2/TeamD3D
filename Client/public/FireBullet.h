#pragma once
#include "Effect_Mesh.h"
#include "Client_Defines.h"

BEGIN(Client)
class CFireBullet final : public CEffect_Mesh
{
public:
	enum CHILD { CHILD_COVER, CHILD_BACK, CHILD_EXPLOSION, CHILD_P_TRAIL, CHILD_P_DEAD, CHILD_END };
	enum STATE { STATE_WAIT, STATE_CREATE, STATE_CHASE, STATE_EXPLOSION_START, STATE_EXPLOSION, STATE_RESET, STATE_END };
	
private:
	class CKena* m_pKena = nullptr;
	
	STATE m_eState = STATE_WAIT;
	_float4 m_vTargetPos;

	_bool m_bCollision = false;

	/* Dissolve */
	_bool m_bDissolve = false;
	_float m_fDissolveTime = 0.0f;
	/* Dissolve */

	class CMonster* m_pOnwerMonster = nullptr;
	_float4 m_vInvisiblePos = { -1000.f, -1000.f, -1000.f , 1.f };

	enum COPY_SOUND_KEY {
		CSK_FIRE_START, CSK_FIRE_CHASE, CSK_FIRE_EXPLOSION,
		COPY_SOUND_KEY_END,
	};

	_tchar* m_pCopySoundKey[COPY_SOUND_KEY_END] = { nullptr, };

private:
	CFireBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFireBullet(const CFireBullet& rhs);
	virtual ~CFireBullet() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual HRESULT			Late_Initialize(void * pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

	_bool	IsActiveState();
	
public:
	virtual void			ImGui_AnimationProperty() override;
	virtual void			Imgui_RenderProperty() override;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();
	HRESULT					Set_ChildEffects();
	
public:
	_bool 	Get_Dissolve() { return m_bDissolve; }

public:
	static CFireBullet*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr)  override;
	virtual void			Free() override;

	void FireBullet_Proc(_float fTimeDelta);
	void Execute_Create(_float4 vCreatePos);
	_int Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex);
	void Set_OwnerMonsterPtr(class CMonster* pMonster) { m_pOnwerMonster = pMonster; }
};

END