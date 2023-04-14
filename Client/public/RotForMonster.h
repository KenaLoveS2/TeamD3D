#pragma once
#include "Rot_Base.h"

BEGIN(Client)
class CRotForMonster : public CRot_Base
{
public:
	typedef struct tagRotForMonsterDesc
	{
		_float4 vPivotPos;
	} DESC;

private:
	CRotForMonster(ID3D11Device* pDevice, ID3D11DeviceContext* p_context);
	CRotForMonster(const CRotForMonster& rhs);
	virtual ~CRotForMonster() = default;

public:
	class CMonster*		Get_Target() { return m_pTarget; }
	void				Set_Target(CMonster* pMonster) { m_pTarget = pMonster; }
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
		
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	
	virtual void Imgui_RenderProperty() override;
	virtual void ImGui_AnimationProperty() override;
	virtual void ImGui_ShaderValueProperty() override;
	virtual void ImGui_PhysXValueProperty() override;
		
	void Bind(_bool bBind, class CMonster* pGameObject);

private:
	HRESULT				SetUp_State();
	HRESULT				Set_RotTrail();
	_bool				AnimFinishChecker(_uint eAnim, _double FinishRate = 0.95);

private:
	_bool								 m_bWakeUp = false;
	_bool								 m_bCreateStart = false;
	_bool								 m_bBind = false;
	class CMonster*				  m_pTarget = nullptr;
	class CE_RotTrail*			  m_pRotTrail = nullptr;
	DESC								  m_Desc;

	_uint m_iRandTeleportAnimIndex = TELEPORT1;
	_uint m_iTeleportAnimIndexTable[7] = {
		TELEPORT1, TELEPORT2, TELEPORT3, TELEPORT4, TELEPORT5, TELEPORT6 ,TELEPORT7, 
	};

	_float m_fAttackSoundTimeCheck = 0.f;
	_float m_fAttackSoundTime = 0.f;
	_float4 m_vInvisiblePos = { -100.f, 0.f, - 100.f, 1.f };

	_bool m_bAttackSound = false;
		
	class CHealthFlower_Anim* m_pTargetFlower = nullptr;
	_bool m_bFlowerBind = false;

public:
	static CRotForMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	void Set_AttackSound(_bool bFlag) { m_bAttackSound = bFlag; }
		
	void Start_BindFlower(CHealthFlower_Anim* pFlower);
	void End_BindFlower();
};

END