#pragma once

#include "Npc.h"

BEGIN(Client)

class CBeni final : public CNpc
{
	enum ANIMATION
	{
		BENI_APPEAR,
		BENI_CHASINGLOOP,
		BENI_CHEER,
		BENI_CURIOUS,
		BENI_DIALOGUEGORGE,
		BENI_DISAPPEAR,
		BENI_IDLE,
		BENI_IDLE_LOOKDOWN,
		BENI_IDLE_LOOKDOWNLEFT,
		BENI_IDLE_LOOKDOWNRIGHT,
		BENI_IDLE_LOOKLEFT,
		BENI_IDLE_LOOKRIGHT,
		BENI_IDLE_LOOKUP,
		BENI_IDLE_LOOKUPLEFT,
		BENI_IDLE_LOOKUPRIGHT,
		BENI_IDLEPOSE,
		BENI_IDLEPOSE_ADDITIVE,
		BENI_LOOKTOSAIYA_END,
		BENI_LOOKTOSAIYA_LOOP,
		BENI_LOOKTOSAIYA_START,
		BENI_MOUTHFLAP,
		BENI_MOUTHFLAP2,
		BENI_MOUTHFLAP_GAP,
		BENI_MOUTHFLAP_REFPOSE,
		BENI_MOUTHFLAP_RUSUHOUSE,
		BENI_MOUTHFLAPSHORT,
		BENI_PHOTOPOSE_CHEER,
		BENI_PHOTOPOSE_SIT02,
		BENI_PHOTOPOSE_SIT03,
		BENI_PHOTOPOSE_SITTING,
		BENI_PHOTOPOSE_SLINGSHOTSHOOT,
		BENI_PHOTOPOSE_SMILING,
		BENI_PULSE,
		BENI_PULSE_SITTING,
		BENI_QUICKLOOKTOSAIYA,
		BENI_ROPEBRIDGE_LOOP,
		BENI_ROPEBRIDGE_RUN,
		BENI_ROTPARADENPC_LINE1,
		BENI_ROTPARADENPC_LINE2,
		BENI_ROTPARADENPC_LINE2_LOOP,
		BENI_ROTPARADENPC_LINE3,
		BENI_ROTPARADENPC_LINE3_LOOP,
		BENI_ROTPARADENPC_LINE4,
		BENI_RUN,
		BENI_RUN_135L,
		BENI_RUN_180,
		BENI_RUN_90R,
		BENI_SIMPLEDIALOGUE,
		BENI_SIT_DIALOGUE,
		BENI_SIT_IDLE,
		BENI_SIT_SITDOWN,
		BENI_SITDOWN,
		BENI_SITLOOP,
		BENI_SPOOKY_BEGIN,
		BENI_SPOOKY_END,
		BENI_SPOOKY_LOOP,
		BENI_SPOOKY_START,
		BENI_SPOOKYLOOP_BLINK,
		BENI_SPOOKYLOOP_GLANCE,
		BENI_TELEPORT,
		BENI_THEFORESTISHUGE,
		BENI_THEFORESTISHUGELOOP,
		BENI_TURN135L_INTORUN,
		BENI_TURN180R_INTORUN,
		BENI_TURN90R_INTORUN,
		BENI_W1_LOGS,
		W0_POKINGEXIT_BENI,
		W0_POKINGLOOP_BENI,
		W1_NPC_FORESTPATH_COMBAT_APPEAR_BENI,
		W1_NPC_FORESTPATH_COMBAT_DISAPPEAR_BENI,
		W1_NPC_FORESTPATH_COMBAT_LOOK_BENI,
		W1_NPC_FORESTPATH_COMBAT_LOOP_BENI,
		W1_NPC_ROPEBRIDGE_CHEER_BENI,
		W1_NPC_ROPEBRIDGE_CURIOUS_BENI,
		W1_NPC_ROPEBRIDGE_INTOCURIOUS_BENI,
		W1_NPC_ROPEBRIDGE_RUNOVER_BENI,
		ANIMATION_END
	};	

private:
	CBeni(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBeni(const CNpc& rhs);
	virtual ~CBeni() = default;

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

protected:
	virtual void					Update_Collider(_float fTimeDelta) override;
	virtual	HRESULT			SetUp_State()override;
	virtual  HRESULT			SetUp_StateFinal() override;
	virtual	HRESULT			SetUp_Components()override;
	virtual	HRESULT			SetUp_ShaderResources()override;
	virtual HRESULT			SetUp_ShadowShaderResources()override;
	virtual HRESULT			SetUp_UI()override;

private:
	virtual void					AdditiveAnim(_float fTimeDelta) override;
	void								SaiyaFunc(_float fTimeDelta);
	void								SaiyaPos(_float3 vOffSetPos);

private:
	_bool							m_bMeetPlayer = false;
	class CSaiya*				m_pSaiya = nullptr;
	string							m_strState;
	_float3							m_vOffsetPos = _float3(0.5f, 0.f, 0.f);
	_bool							m_bPhotoAnimEnd = false;
	class CCamera_Photo* m_pCamera_Photo = nullptr;
public:
	static CBeni*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr)  override;
	virtual void						Free() override;
};

END