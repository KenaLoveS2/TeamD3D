#pragma once

#include "Monster.h"

BEGIN(Client)

class CShieldStick : public CMonster
{
private:
	enum ANIMATION
	{
		BIND,
		CHARGEATTACK,
		DEATH,
		HAEDSHOT,
		IDLE,
		INTOCHARGE,
		JUMPBACK,
		RUN,
		SHIELDDESTROY,
		SHIELDDESTORYALT,
		SHIELDHIT,
		SHOVEATTACK,
		STRAFE_L,
		STRAFE_R,
		TAUNT,
		UNARMED_BIND,
		UNARMED_IDLE,
		UNARMED_ROCKTHROW,
		UNARMED_RUN,
		UNARMED_TAKEDAMAGEBACK,
		UNARMED_TAKEDAMAGELEFT,
		UNARMED_TAKEDAMAGERIGHT,
		UNARMED_TAUNT,
		UNARMED_TWITCH,
		UNARMED_WALK,
		WALK,
		WALKBACKWARDS,
		WISPIN,
		ANIMATION_END
	};

private:
	CShieldStick(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShieldStick(const CShieldStick& rhs);
	virtual ~CShieldStick() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual HRESULT			Late_Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			RenderShadow() override;
	virtual void					Imgui_RenderProperty() override;
	virtual void					ImGui_AnimationProperty() override;
	virtual void					ImGui_ShaderValueProperty() override;
	virtual void					ImGui_PhysXValueProperty() override;
	virtual HRESULT			Call_EventFunction(const string& strFuncName) override;
	virtual void					Push_EventFunctions() override;

protected:
	virtual  HRESULT			SetUp_State() override;
	virtual	HRESULT			SetUp_Components() override;
	virtual	HRESULT			SetUp_ShaderResources() override;
	virtual  HRESULT			SetUp_ShadowShaderResources() override;

private:
	void	Update_Collider(_float fTimeDelta) override;
	void	AdditiveAnim(_float fTimeDelta) override;

private:
	_float m_fIdleToAttack = 0.f; 

	vector<wstring> m_vecColliderName;
	vector<_float3> m_vecPivot;
	vector<_float3> m_vecPivotScale;
	vector<_float3> m_vecPivotRot;
		
	class CShieldStick_Weapon* m_pWeapon = nullptr;

	enum COPY_SOUND_KEY {
		CSK_CALM, CSK_TENSE1, CSK_ATTACK, CSK_HURT, CSK_DIE,
		COPY_SOUND_KEY_END,
	};
	_tchar* m_pCopySoundKey[COPY_SOUND_KEY_END] = { nullptr, };

public:
	static CShieldStick*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg = nullptr)  override;
	virtual void								Free() override;
	virtual void Create_CopySoundKey() override;
};

END