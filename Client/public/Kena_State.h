#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Transform.h"
#include "Delegator.h"
#include "UI_ClientManager.h"


BEGIN(Engine)
class CGameInstance;
class CStateMachine;
class CAnimationState;
class CModel;
END

BEGIN(Client)

class CKena_State final : public CBase
{
public:
	enum MOVEOPTION { MOVEOPTION_COMMON, MOVEOPTION_ONLYTURN, MOVEOPTION_END };
	enum ANIMATION {
		PHOTO_ADVENTURE, PHOTO_ADVENTURE_LOOP,

		/* AIM */
		AIM_AIR_INTO, AIM_AIR_LOOP, AIM_AIR_REFPOSE,
		AIM_INTO, AIM_LOOK_DOWN_ADD, AIM_LOOK_LEFT_ADD, AIM_LOOK_RIGHT_ADD, AIM_LOOK_UP_ADD,
		AIM_LOOP, AIM_REFPOSE, AIM_RETURN,
		AIM_RUN_BACKWARD, AIM_RUN_FORWARD, AIM_RUN_LEFT, AIM_RUN_RIGHT,
		AIM_TURN_LEFT, AIM_TURN_LEFT_120, AIM_TURN_RIGHT, AIM_TURN_RIGHT_120, AIM_TURN_RIGHT_150,
		AIM_WALK_BACK, AIM_WALK_FORWARD, AIM_WALK_LEFT, AIM_WALK_RIGHT,
		AIM_POSE, AIM_POSE_ADD,

		/* AIR ATTACK */
		AIR_ATTACK_1_RETURN, AIR_ATTACK_LIGHT, AIR_ATTACK_LIGHT_ALT,
		AIR_ATTACK_SLAM_FINKSH, AIR_ATTACK_SLAM_INTO, AIR_ATTACK_SLAM_LOOP, 

		ANTIGRAVITY_INTO, ANTIGRAVITY_LOOP,

		/* ATTACK */
		ATTACK_1, ATTACK_1_FROM_RUN, ATTACK_1_INTO_RUN, ATTACK_1_INTO_WALK, ATTACK_1_RETURN,
		ATTACK_2, ATTACK_2_INTO_RUN, ATTACK_2_INTO_WALK, ATTACK_2_RETURN,
		ATTACK_3, ATTACK_3_INTO_RUN, ATTACK_3_RETURN,
		ATTACK_4, ATTACK_4_INTO_RUN, ATTACK_4_RETURN,

		BACKFLIP, 

		/* BOMB */
		BOMB,
		BOMB_AIR_CANCEL_ADD, BOMB_AIR_INTO_ADD, BOMB_AIR_LOOP_ADD, BOMB_AIR_RELEASE_ADD,
		BOMB_CANCEL_ADD, BOMB_INJECT_ADD, BOMB_INTO_ADD, BOMB_LOOP_ADD, BOMB_RELEASE_ADD,
		BOMB_LOOP,

		/* BOW */
		BOW,
		BOW_AIM_DOWN_ADD, BOW_AIM_DOWN_LEFT_ADD, BOW_AIM_DOWN_RIGHT_ADD, BOW_AIM_FORWARD_ADD, BOW_AIM_LEFT_ADD,
		BOW_AIM_REFPOSE,
		BOW_AIM_RIGHT_ADD, BOW_AIM_UP_ADD, BOW_AIM_UP_LEFT_ADD, BOW_AIM_UP_RIGHT_ADD,
		BOW_AIR_AIM_DOWN_ADD, BOW_AIR_AIM_FORWARD_ADD, BOW_AIR_AIM_LEFT_ADD,
		BOW_AIR_AIM_REFPOSE_ADD,
		BOW_AIR_AIM_RIGHT_ADD, BOW_AIR_AIM_UP_ADD,
		BOW_AIR_CHARGE_ADD, BOW_AIR_CHARGE_LOOP_ADD, BOW_AIR_RECHARGE_ADD, BOW_AIR_RELEASE_ADD, BOW_AIR_RETURN_ADD,
		BOW_CHARGE_ADD, BOW_CHARGE_FULL_ADD, BOW_CHARGE_LOOP_ADD,
		BOW_INJECT_ADD, BOW_INJECT_LOOP_ADD, BOW_INJECT_RELEASE_ADD,
		BOW_LAND,
		BOW_NEUTRAL_ADD, BOW_RECHARGE_ADD, BOW_RELEASE_ADD, BOW_RETURN_ADD,
		BOW_LOOP,

		CELEBRATION,

		/* COMBAT */
		COMBAT_ADDITIVE_POSE_ADD,
		COMBAT_LOOKDOWN_ADD, COMBAT_LOOKLEFT_ADD, COMBAT_LOOKRIGHT_ADD, COMBAT_LOOKUP_ADD,
		COMBAT_POSE,
		COMBAT_IDLE_INTO_RUN, COMBAT_IDLE_INTO_RUN_NOROOT, COMBAT_IDLE_POSE,
		COMBAT_POSE_ADDITIVE_ADD,
		COMBAT_RUN, COMBAT_RUN_ADDITIVE_POSE_ADD,

		/* COMMAND */
		COMMAND_REFPOSE,
		COMMAND_SLAM_ADD, COMMAND_SPIN_ADD, COMMAND_TURN_RIGHT_ADD, COMMAND_POSE_ADD,

		/* CROUCH */
		CROUCH_TO_IDLE, CROUCH_TO_RUN,

		/* DASH */
		DASH, DASH_PORTAL, DASH_COMBAT_ATTACK, DASH_COMBAT_ATTACK_3, DASH_SETTLE,

		/* DEADZONE */
		DEADZONE_COUGH_ADD, DEADZONE_DEATH, DEADZONE_IDLE, DEADZONE_REFPOSE, DEADZONE_RUN, DEADZONE_TURN_LEFT, DEADZONE_TURN_RIGHT,
		DEADZONE_WALK, DEADZONE_WALK_SLOW, DEADZONE_CLEAR,

		/* DEATH */
		DEATH,
		DEATH_AIR_FINISH, DEATH_AIR_FRONT_END, DEATH_AIR_FRONT_LOOP, DEATH_AIR_FRONT_START,
		DEATH_AIR_INTRO, DEATH_AIR_LOOP,
		DEATH_BEHIND,
		DEATH_FROM_FALL,
		DEATH_INPLACE,

		/* DEFEAT */
		DEFEATED_STAND,

		/* DIALOGUE */
		DIALOGUE_1_ADD, DIALOGUE_2_ADD, DIALOGUE_3_ADD,

		/* DRIVE ROT*/
		DRIVE_ROT_END, DRIVE_ROT_START,

		/* OUTFIT */
		EQUIP_OUTFIT, EXAMINE_OUTFIT_1, EXAMINE_OUTFIT_2,

		/* FALL */
		FALL, FALL_INTO_RUN,

		/* FOCUS */
		FOCUSED, FOCUSED_LOOP,

		GREET_ROT_ADD, GUST_ADD, GUST_BIG_ADD, 

		HAND_ON_HIP, HAND_ON_HIP_END, HAND_ON_HIP_LOOP,

		HAT_CAR_EXIT,

		/* HEAVY ATTACK */
		HEAVY_ATTACK_1_CHARGE, HEAVY_ATTACK_1_INTO_RUN, HEAVY_ATTACK_1_RELEASE, HEAVY_ATTACK_1_RELEASE_PERFECT, HEAVY_ATTACK_1_RETURN,
		HEAVY_ATTACK_2_CHARGE, HEAVY_ATTACK_2_INTO_RUN, HEAVY_ATTACK_2_RELEASE, HEAVY_ATTACK_2_RELEASE_PERFECT, HEAVY_ATTACK_2_RETURN,
		HEAVY_ATTACK_3_CHARGE, HEAVY_ATTACK_3_RELEASE, HEAVY_ATTACK_3_RELEASE_PERFECT, HEAVY_ATTACK_3_RETURN,

		/* SPRINT HEAVY ATTACK */
		HEAVY_ATTACK_COMBO, HEAVY_ATTACK_COMBO_INTO_RUN, HEAVY_ATTACK_COMBO_RETURN,

		HEAVY_FINISHER, HEAVY_FINISHER_LOOP,

		HEAVY_STAGGER_INTO_RUN, HEAVY_STAGGER_AIR_LOOP, HEAVY_STAGGER_AIR_START, HEAVY_STAGGER_BACK_INTO_RUN, 

		HERO_ROT_CHEER, HERO_ROT_CHEER_ADDITIVE_ADD,

		HIT_DEFLECT,

		/* IDLE */
		IDLE, IDLE_ADDITIVE_LOOP_ADD, 
		IDLE_INTO_LOCK_ON,
		IDLE_INTO_RUN_180_LEFT, IDLE_INTO_RUN_180_RIGHT, 
		IDLE_POSE,
		
		/* INSPECT */
		INSPECT_1, INSPECT_2, INSPECT_LOW, INSPECT_MASK_GATE,

		/* INTERACT */
		INTERACT, INTERACT_STAFF, 

		INTO_HEAVY_FINISHER,

		/* PULSE */
		INTO_PULSE, INTO_PULSE_FROM_RUN,

		INTO_RUN_GRAB_STAFF,

		INTO_SHIELD,

		INTO_SPRINT,

		JOG,
		
		/* JUMP */
		JUMP, JUMP_SQUAT,
		
		PHOTO_KICK, PHOTO_KICK_LOOP,
		PHOTO_KNEEL, PHOTO_KNEEL_LOOP,

		/* LAND */
		LAND, LAND_HEAVY, LAND_WALKING,

		PHOTO_LEAN, PHOTO_LEAN_LOOP,

		/* CLIMB */
		LEDGE_CLIMB_LEFT, LEDGE_CLIMB_RIGHT, LEDGE_CLIMB_UP_SHORT, LEDGE_CLIMB_UP_STAND,
		LEDGE_DROP_DOWN, LEDGE_DROP_ONTO_LEDGE_BACK,
		LEDGE_GRAB_IDLE, LEDGE_INTO_GRAB, LEDGE_INTO_LOOK_LEFT, LEDGE_INTO_LOOK_RIGHT, 
		LEDGE_JUMP_LEFT, LEDGE_JUMP_LEFT_BIG, LEDGE_JUMP_RIGHT, LEDGE_JUMP_RIGHT_BIG,
		LEDGE_JUMP_UP, LEDGE_JUMP_UP_LONG, LEDGE_JUMP_UP_RIGHT, 
		LEDGE_LAND,
		LEDGE_LOOK_LEFT, LEDGE_LOOK_LEFT_RETURN, LEDGE_LOOK_RIGHT, LEDGE_LOOK_RIGHT_RETURN,
		LEDGE_LOOK_UP, LEDGE_LOOK_UP_LOOP, LEDGE_LOOK_UP_RETURN,
		LEDGE_SETTLE,
		LEDGE_GRAB_NOSTAFF,

		LIFE_LOOP_ADD, LIFE_LOOP_REFPOSE,

		LIPSYNC_BENI_AND_SAIYA, LIPSYNC_BENI_AND_SEIYA_LOOP,
		LIPSYNC_CREATURE_LOOP_1, LIPSYNC_CREATURE_LOOP_2, LIPSYNC_CREATURE_PT_1, LIPSYNC_CREATURE_PT_2,
		LIPSYNC_ILL_RETURN_THE_RELICS, LIPSYNC_ILL_RETURN_THE_RELICE_LOOP,
		LIPSYNC_TARO_IS_HERE, LIPSYNC_TARO_IS_HERE_LOOP,
		LIPSYNC_THANKYOU_RUSU, LIPSYNC_THANKYOU_ROSU_LOOP,
		LIPSYNC_THE_LEADER, LIPSYNC_THE_LEADER_LOOP,

		/* LISTEN */
		LISTENING_LOOP, LISTEN_LEFT_END_2,

		/* LOCK ON (COMBAT) */
		LOCK_ON_IDLE, LOCK_ON_TO_IDLE,

		LOOK_AROUND_1_ADD, LOOK_AROUND_2_ADD, LOOK_AROUND_3_ADD,

		LOOK_DOWN_AT_ROT,

		MANTLE,

		/* MASK */
		MASK_LOOP, MASK_OFF, MASK_OFF_MEMORY_COLLECT, MASK_ON,

		/* MEDITETE */
		MEDITATE_EXIT, MEDITATE_INTO, MEDITATE_LOOP,

		MOUTH_FLAP_1, MOUTH_FLAP_2, MOUTH_FLAP_3, MOUTH_FLAP_GAP, MOUTH_FLAP_REFPOSE,

		/* NEUTRAL */
		NEUTRAL_LOOK_DOWN_ADD, NEUTRAL_LOOK_DOWN_LEFT_ADD, NEUTRAL_LOOK_DOWN_RIGHT_ADD,
		NEUTRAL_LOOK_LEFT_ADD, NEUTRAL_LOOK_RIGHT_ADD,
		NEUTRAL_LOOK_UP_ADD, NEUTRAL_LOOK_UP_LEFT_ADD, NEUTRAL_LOOK_UP_RIGHT_ADD,
		NEUTRAL_POSE, NEUTRAL_POSE_ADDITIVE_ADD,

		NO_ADDITIVE_ADD,

		OUTFIT_POSE_1, OUTFIT_POSE_1_LOOP,

		/* PARRY ATTACK */
		PARRY_ATTACK_LIGHT,

		PHOTO_PEACESIGN, PHOTO_PEACESIGN_LOOK_DOWN_ADD, PHOTO_PEACESIGN_LOOK_DOWN_LEFT_ADD, PHOTO_PEACESIGN_LOOK_DOWN_RIGHT_ADD,
		PHOTO_PEACESIGN_LOOK_LEFT_ADD, PHOTO_PEACESIGN_LOOK_RIGHT,
		PHOTO_PEACESIGN_LOOK_UP_ADD, PHOTO_PEACESIGN_LOOK_UP_LEFT_ADD, PHOTO_PEACESIGN_LOOK_UP_RIGHT_ADD,
		PHOTO_PEACESIGN_NEUTRAL_ADD, PHOTO_PEACESIGN_REFPOSE, PHOTO_PEACESIGN_LOOP,

		PRAY,
		PHOTO_PRAYER, PHOTO_PRAYER_LOOP,
		PRAY_LONG,

		PHOTO_PREPARED, PHOTO_PREPARED_LOOP,

		/* PULSE */
		PULSE, PULSE_INTO_COMBAT_END, PULSE_INTO_IDLE, PULSE_INTO_RUN, PULSE_LOOP,
		PULSE_WALK_BACK_LEFT, PULSE_WALK_BACK_RIGHT, PULSE_WALK_BACKWARD, 
		PULSE_WALK_FORWARD, PULSE_WALK_LEFT, PULSE_WALK_RIGHT,
		PULSE_JUMP,
		PULSE_PARRY,
		PULSE_SQUAT_SPRINT,

		PUSH_BACK,

		PHOTO_RELAX, PHOTO_RELAX_LOOP,

		/* RESPAWN */
		RESPAWN,

		PHOTO_REST, PHOTO_REST_LOOP,

		/* ROLL */
		ROLL,
		ROLL_INTO_IDLE, ROLL_INTO_RUN, ROLL_INTO_WALK, ROLL_INTO_FALL,
		ROLL_LEFT, ROLL_RIGHT,

		/* ROT */
		ROT_ACTION, ROT_ACTION_AIM_STATE,
		ROT_GOD_GRAB,
		ROT_HAMMER, ROT_HAMMER_INTO_RUN, ROT_HAMMER_RETURN,
		ROT_LEVEL_UP, ROT_PARADE_NPC_SQUAT_LOOP, ROT_PARADE_NPC_SQUAT_STAND,
		ROT_RAGE,

		/* RUN */
		RUN,
		RUN_INTO_IDLE_LEFT, RUN_INTO_SWIM,
		RUN_LOOK_DOWN_ADD, RUN_LOOK_DOWN_LEFT_ADD, RUN_LOOK_DOWN_RIGHT_ADD,
		RUN_LOOK_LEFT_ADD, RUN_LOOK_RIGHT_ADD,
		RUN_LOOK_UP_ADD, RUN_LOOK_UP_LEFT_ADD, RUN_LOOK_UP_RIGHT_ADD,
		RUN_NEUTRAL_ADD, RUN_REFPOSE,
		RUN_DECLINE_STAIR, RUN_INCLINE_STAIR,
		RUNNING_JUMP_LEFT, RUNNING_JUMP_SQUAT_LEFT, RUNNING_SHORT_HOP_LEFT,
		RUN_STOP,

		/* SHILED */
		SHILED_IMPACT, SHILED_IMPACT_BIG, SHILED_IMPACT_MEDIUM,
		SHILED_RELEASE,
		SHILED_SMALL_STAGGER,	/* DAMAGED FROM FORWARD, LEFT, RIGHT, BACKWARD */
		SHILED_BREAK_BACK, SHILED_BREAK_FRONT,
		SHIELD_EYES,
		SHIELD_STAGGER_SMALL_BACK, SHIELD_STAGGER_SMALL_LEFT, SHILED_STAGGER_SMALL_RIGHT,

		SHORT_HOP,

		/* SIT */
		SIT_EXIT, SIT_INTO,
		SIT_LOOK_DOWN_ADD, SIT_LOOK_DOWN_LEFT_ADD, SIT_LOOK_DOWN_RIGHT_ADD,
		SIT_LOOK_LEFT_ADD, SIT_LOOK_RIGHT_AD,
		SIT_LOOK_UP_ADD, SIT_LOOK_UP_LEFT_ADD, SIT_LOOK_UP_RIGHT_ADD,
		SIT_LOOP,
		SIT_NEUTRAL_ADD, SIT_REFPOSE,

		PHOTO_SITBACK, PHOTO_SITBACK_LOOP,
		PHOTO_SITBACK_PEACE, PHOTO_SITBACK_PEACE_LOOP,
		PHOTO_SITTINGPOSE,
		PHOTO_SITTINGPOSE_LOOK_DOWN_ADD, PHOTO_SITTINGPOSE_LOOK_DOWN_LEFT_ADD, PHOTO_SITTINGPOSE_LOOK_DOWN_RIGHT_ADD,
		PHOTO_SITTINGPOSE_LOOK_LEFT_ADD, PHOTO_SITTINGPOSE_LOOK_RIGHT_ADD,
		PHOTO_SITTINGPOSE_LOOK_UP_ADD, PHOTO_SITTINGPOSE_LOOK_UP_LEFT_ADD, PHOTO_SITTINGPOSE_LOOK_UP_RIGHT_ADD,
		PHOTO_SITTINGPOSE_NEUTRAL_ADD, PHOTO_SITTINGPOSE_REFPOSE,
		PHOTO_SITTINGPOSE_LOOP,

		/* SIT WITH ROT (INGAME Ckey) */
		SIT_WITH_ROT, SIT_WITH_ROT_EXIT,
		SIT_WITH_ROT_FIDGET_1, SIT_WITH_ROT_FIDGET_2, SIT_WITH_ROT_FIDGET_3, SIT_WITH_ROT_FIDGET_4,
		SIT_WITH_ROT_LOOP,

		SLEAVE,

		/* SLIDE */
		SLIDE_DECLINE_30_ADD, SLIDE_DECLINE_30_LEAN_L_ADD, SLIDE_DECLINE_30_LEAN_R_ADD,
		SLIDE_DECLINE_45_ADD, SLIDE_DECLINE_45_LEAN_L_ADD, SLIDE_DECLINE_45_LEAN_R_ADD,
		SLIDE_INTO, SLIDE_JUMP, SLIDE_LAND,
		SLIDE_LEVEL_ADD, SLIDE_LEVEL_LEAN_L_ADD, SLIDE_LEVEL_LEAN_R_ADD,
		SLIDE_LOOP, SLIDE_REFPOSE,
		SLIDE_SLIP_180_L,
		SLIDE_BUMP_ADD,

		SLOW_STOP, SLOW_STOP_COMBAT,

		/* PARRY ATTACK */
		SPIN_ATTACK, SPIN_ATTACK_RETURN,

		SPIRIT_DIVE_TRANSITION,

		/* SPRINT */
		SPRINT, SPRINT_LEAN_LEFT, SPRINT_LEAN_RIGHT,
		SPRINT_TURN_180,
		SPRINT_ATTACK,
		SPRINT_STOP,
		
		/* SQUAT */
		SQUAT_END,
		SQUAT_INTO, SQUAT_INTO_SLOW, SQUAT_LOOP, SQUAT_LOOP_SLOW,
		SQUAT_RETURN,

		PHOTO_STAFF_ON_SHOULDER, PHOTO_STAFF_ON_SHOULDER_LOOP,
		PHOTO_STAFF_STANCE_RIGHT_LOOK_DOWN_ADD,
		PHOTO_STAFF_STANCE_RIGHT_LOOK_UP_ADD, PHOTO_STAFF_STANCE_RIGHT_LOOK_UP_LEFT_ADD, PHOTO_STAFF_STANCE_RIGHT_LOOK_UP_RIGHT_ADD,
		PHOTO_STAFF_STANCE_RIGHT_REFPOSE,

		STEP_BACK,

		/* SWIM */
		SWIM_FORWARD,
		SWIM_FROM_FALL,
		SWIM_IDLE,
		SWIM_LEFT, SWIM_RIGHT, SWIM_TURN_180,
		SWIM_DEATH,
		SWIM_STOP,

		/* TAKE DAMAGE */
		TAKE_DAMAGE_ADDITIVE_BACK_ADD, TAKE_DAMAGE_ADDITIVE_FRONT_ADD, TAKE_DAMAGE_ADDITIVE_LEFT_ADD, TAKE_DAMAGE_ADDITIVE_RIGHT_ADD,
		TAKE_DAMAGE_BACK, TAKE_DAMAGE_FRONT, TAKE_DAMAGE_LEFT, TAKE_DAMAGE_RIGHT, TAKE_DAMAGE_ADDITIVE,
		TAKE_DAMAGE_HEAVY_BACK, TAKE_DAMAGE_HEAVY_FRONT, TAKE_DAMAGE_HEAVY_TRIMMED_FOR_AIR_DAMAGE,

		TARO_FIGHT_GRAB_PULSE, TARO_GRAB,

		TELEPORT_FLOWER,

		/* TURN */
		TURN_LEFT, TURN_RIGHT,

		VINE_GRAB_ATTACK,

		/* WALK */
		WALK, WALK_DECLINE, WALK_INCLINE, WALK_STOP,

		WARRIOR_GRAB,

		WIND, WIND_MIRRORED, WIND_BIG_1, WIND_BIG_2, WIND_BIG_NO_SHAWL,

		DELETE_KENA_LAND_RUNNING,

		KENA_DEATH_LIQUID_DEADZONE,

		KENA_ANIMATION_END
	};

private:
	CKena_State();
	virtual ~CKena_State() = default;

public:
	static	CKena_State		NullFunc;

public:
	HRESULT					Initialize(class CKena* pKena, CStateMachine* pStateMachine, CModel* pModel, CAnimationState* pAnimation, CTransform* pTransform, class CCamera_Player* pCamera);
	void						Tick(_double dTimeDelta);
	void						Late_Tick(_double dTimeDelta);
	void						ImGui_RenderProperty();

private:
	CGameInstance*			m_pGameInstance = nullptr;

private:
	class CKena*				m_pKena = nullptr;
	CStateMachine*			m_pStateMachine = nullptr;
	CModel*					m_pModel = nullptr;
	CAnimationState*		m_pAnimationState = nullptr;
	CTransform*				m_pTransform = nullptr;
	class CCamera_Player*	m_pCamera = nullptr;

private:
	CTransform::DIRECTION	m_eDir = CTransform::DIR_END;
	CTransform::DIRECTION	m_ePreDir = CTransform::DIR_END;

	_bool						m_bCombat = false;
	_float						m_fCombatIdleTime = 0.f;

private:
	HRESULT					SetUp_State_Idle();
	HRESULT					SetUp_State_Run();
	HRESULT					SetUp_State_Aim();
	HRESULT					SetUp_State_Attack1();
	HRESULT					SetUp_State_Attack2();
	HRESULT					SetUp_State_Attack3();
	HRESULT					SetUp_State_Attack4();
	HRESULT					SetUp_State_Bow();
	HRESULT					SetUp_State_Combat();
	HRESULT					SetUp_State_Dodge();
	HRESULT					SetUp_State_Fall();
	HRESULT					SetUp_State_Heavy_Attack1();
	HRESULT					SetUp_State_Heavy_Attack2();
	HRESULT					SetUp_State_Heavy_Attack3();
	HRESULT					SetUp_State_Heavy_Attack_Combo();
	HRESULT					SetUp_State_Jump();
	HRESULT					SetUp_State_Land();
	HRESULT					SetUp_State_Pulse();
	HRESULT					SetUp_State_Sprint();

private:
	/* IDLE */
	void						Start_Idle(_float fTimeDelta);
	/* RUN */
	void						Start_Run(_float fTimeDelta);
	void						Start_Run_Stop(_float fTimeDelta);
	/* AIM */
	void						Start_Aim_Into(_float fTimeDelta);
	void						Start_Aim_Loop(_float fTimeDelta);
	void						Start_Aim_Return(_float fTimeDelta);
	void						Start_Aim_Run(_float fTimeDelta);
	void						Start_Aim_Run_Forward(_float fTimeDelta);
	void						Start_Aim_Run_Forward_Left(_float fTimeDelta);
	void						Start_Aim_Run_Forward_Right(_float fTimeDelta);
	void						Start_Aim_Run_Backward(_float fTimeDelta);
	void						Start_Aim_Run_Backward_Left(_float fTimeDelta);
	void						Start_Aim_Run_Backward_Right(_float fTimeDelta);
	void						Start_Aim_Run_Left(_float fTimeDelta);
	void						Start_Aim_Run_Right(_float fTimeDelta);
	/* ATTACK 1 */
	void						Start_Attack_1(_float fTimeDelta);
	void						Start_Attack_1_From_Run(_float fTimeDelta);
	void						Start_Attack_1_Into_Run(_float fTimeDelta);
	void						Start_Attack_1_Into_Walk(_float fTimeDelta);
	void						Start_Attack_1_Return(_float fTimeDelta);
	/* ATTACK 2 */
	void						Start_Attack_2(_float fTimeDelta);
	void						Start_Attack_2_Into_Run(_float fTimeDelta);
	void						Start_Attack_2_Into_Walk(_float fTimeDelta);
	void						Start_Attack_2_Return(_float fTimeDelta);
	/* ATTACK 3 */
	void						Start_Attack_3(_float fTimeDelta);
	void						Start_Attack_3_Into_Run(_float fTimeDelta);
	void						Start_Attack_3_Return(_float fTimeDelta);
	/* ATTACK 4 */
	void						Start_Attack_4(_float fTimeDelta);
	void						Start_Attack_4_Into_Run(_float fTimeDelta);
	void						Start_Attack_4_Return(_float fTimeDelta);
	/* BOW */
	void						Start_Bow_Charge(_float fTimeDelta);
	void						Start_Bow_Charge_Full(_float fTimeDelta);
	void						Start_Bow_Charge_Loop(_float fTimeDelta);
	void						Start_Bow_Release(_float fTimeDelta);
	void						Start_Bow_Recharge(_float fTimeDelta);
	void						Start_Bow_Return(_float fTimeDelta);
	/* COMBAT */
	void						Start_Idle_Into_Lock_On(_float fTimeDelta);
	void						Start_Lock_On_Idle(_float fTimeDelta);
	void						Start_Lock_On_To_Idle(_float fTimeDelta);
	void						Start_Combat_Idle_Into_Run(_float fTimeDelta);
	void						Start_Combat_Run(_float fTimeDelta);
	/* DODGE */
	void						Start_Backflip(_float fTimeDelta);
	void						Start_Roll(_float fTimeDelta);
	void						Start_Roll_Left(_float fTimeDelta);
	void						Start_Roll_Right(_float fTimeDelta);
	void						Start_Roll_Into_Idle(_float fTimeDelta);
	void						Start_Roll_Into_Run(_float fTimeDelta);
	void						Start_Roll_Into_Walk(_float fTimeDelta);
	void						Start_Roll_Into_Fall(_float fTimeDelta);
	/* HEAVY ATTACK 1 */
	void						Start_Heavy_Attack_1_Charge(_float fTimeDelta);
	void						Start_Heavy_Attack_1_Release(_float fTimeDelta);
	void						Start_Heavy_Attack_1_Release_Perfect(_float fTimeDelta);
	void						Start_Heavy_Attack_1_Return(_float fTimeDelta);
	void						Start_Heavy_Attack_1_Into_Run(_float fTimeDelta);
	/* HEAVY ATTACK 2 */
	void						Start_Heavy_Attack_2_Charge(_float fTimeDelta);
	void						Start_Heavy_Attack_2_Release(_float fTimeDelta);
	void						Start_Heavy_Attack_2_Release_Perfect(_float fTimeDelta);
	void						Start_Heavy_Attack_2_Return(_float fTimeDelta);
	void						Start_Heavy_Attack_2_Into_Run(_float fTimeDelta);
	/* HEAVY ATTACK 3 */
	void						Start_Heavy_Attack_3_Charge(_float fTimeDelta);
	void						Start_Heavy_Attack_3_Release(_float fTimeDelta);
	void						Start_Heavy_Attack_3_Release_Perfect(_float fTimeDelta);
	void						Start_Heavy_Attack_3_Return(_float fTimeDelta);
	/* HEAVY ATTACK COMBO */
	void						Start_Heavy_Attack_Combo(_float fTimeDelta);
	void						Start_Heavy_Attack_Combo_Return(_float fTimeDelta);
	void						Start_Heavy_Attack_Combo_Into_Run(_float fTimeDelta);
	/* JUMP */
	void						Start_Jump_Squat(_float fTimeDelta);
	void						Start_Jump(_float fTimeDelta);
	void						Start_Running_Jump_Squat(_float fTimeDelta);
	void						Start_Running_Jump(_float fTimeDelta);
	void						Start_Pulse_Jump(_float fTimeDelta);
	/* LAND */
	void						Start_Land(_float fTimeDelta);
	void						Start_Land_Heavy(_float fTimeDelta);
	void						Start_Land_Walking(_float fTimeDelta);
	void						Start_Bow_Land(_float fTimeDelta);
	void						Start_Ledge_Land(_float fTimeDelta);
	void						Start_Slide_Land(_float fTimeDelta);
	/* PULSE */
	void						Start_Into_Pulse(_float fTimeDelta);
	void						Start_Into_Pulse_From_Run(_float fTimeDelta);
	void						Start_Pulse(_float fTimeDelta);
	void						Start_Pulse_Loop(_float fTimeDelta);
	void						Start_Pulse_Into_Combat_End(_float fTimeDelta);
	void						Start_Pulse_Into_Idle(_float fTimeDelta);
	void						Start_Pulse_Into_Run(_float fTimeDelta);
	void						Start_Pulse_Parry(_float fTimeDelta);
	void						Start_Pulse_Walk_Forward(_float fTimeDelta);
	void						Start_Pulse_Walk_Front_Left(_float fTimeDelta);
	void						Start_Pulse_Walk_Front_Right(_float fTimeDelta);
	void						Start_Pulse_Walk_Backward(_float fTimeDelta);
	void						Start_Pulse_Walk_Back_Left(_float fTimeDelta);
	void						Start_Pulse_Walk_Back_Right(_float fTimeDelta);
	void						Start_Pulse_Walk_Left(_float fTimeDelta);
	void						Start_Pulse_Walk_Right(_float fTimeDelta);
	void						Start_Pulse_Squat_Sprint(_float fTimeDelta);
	/* SRPINT */
	void						Start_Into_Sprint(_float fTimeDelta);
	void						Start_Sprint(_float fTimeDelta);
	void						Start_Sprint_Stop(_float fTimeDelta);
	void						Start_Sprint_Lean_Left(_float fTimeDelta);
	void						Start_Sprint_Lean_Right(_float fTimeDelta);
	void						Start_Sprint_Turn_180(_float fTimeDelta);
	void						Start_Sprint_Attack(_float fTimeDelta);

private:
	/* IDLE */
	void						Tick_Idle(_float fTimeDelta);
	/* RUN */
	void						Tick_Run(_float fTimeDelta);
	void						Tick_Run_Stop(_float fTimeDelta);
	/* AIM */
	void						Tick_Aim_Into(_float fTimeDelta);
	void						Tick_Aim_Loop(_float fTimeDelta);
	void						Tick_Aim_Return(_float fTimeDelta);
	void						Tick_Aim_Run_Forward(_float fTimeDelta);
	void						Tick_Aim_Run_Forward_Left(_float fTimeDelta);
	void						Tick_Aim_Run_Forward_Right(_float fTimeDelta);
	void						Tick_Aim_Run_Backward(_float fTimeDelta);
	void						Tick_Aim_Run_Backward_Left(_float fTimeDelta);
	void						Tick_Aim_Run_Backward_Right(_float fTimeDelta);
	void						Tick_Aim_Run_Left(_float fTimeDelta);
	void						Tick_Aim_Run_Right(_float fTimeDelta);
	/* ATTACK 1 */
	void						Tick_Attack_1(_float fTimeDelta);
	void						Tick_Attack_1_From_Run(_float fTimeDelta);
	void						Tick_Attack_1_Into_Run(_float fTimeDelta);
	void						Tick_Attack_1_Into_Walk(_float fTimeDelta);
	void						Tick_Attack_1_Return(_float fTimeDelta);
	/* ATTACK 2 */
	void						Tick_Attack_2(_float fTimeDelta);
	void						Tick_Attack_2_Into_Run(_float fTimeDelta);
	void						Tick_Attack_2_Into_Walk(_float fTimeDelta);
	void						Tick_Attack_2_Return(_float fTimeDelta);
	/* ATTACK 3 */
	void						Tick_Attack_3(_float fTimeDelta);
	void						Tick_Attack_3_Into_Run(_float fTimeDelta);
	void						Tick_Attack_3_Return(_float fTimeDelta);
	/* ATTACK 4 */
	void						Tick_Attack_4(_float fTimeDelta);
	void						Tick_Attack_4_Into_Run(_float fTimeDelta);
	void						Tick_Attack_4_Return(_float fTimeDelta);
	/* BOW */
	void						Tick_Bow_Charge(_float fTimeDelta);
	void						Tick_Bow_Charge_Full(_float fTimeDelta);
	void						Tick_Bow_Charge_Loop(_float fTimeDelta);
	void						Tick_Bow_Release(_float fTimeDelta);
	void						Tick_Bow_Recharge(_float fTimeDelta);
	void						Tick_Bow_Return(_float fTimeDelta);
	/* COMBAT */
	void						Tick_Idle_Into_Lock_On(_float fTimeDelta);
	void						Tick_Lock_On_Idle(_float fTimeDelta);
	void						Tick_Lock_On_To_Idle(_float fTimeDelta);
	void						Tick_Combat_Idle_Into_Run(_float fTimeDelta);
	void						Tick_Combat_Run(_float fTimeDelta);
	/* DODGE */
	void						Tick_Backflip(_float fTimeDelta);
	void						Tick_Roll(_float fTimeDelta);
	void						Tick_Roll_Left(_float fTimeDelta);
	void						Tick_Roll_Right(_float fTimeDelta);
	void						Tick_Roll_Into_Idle(_float fTimeDelta);
	void						Tick_Roll_Into_Run(_float fTimeDelta);
	void						Tick_Roll_Into_Walk(_float fTimeDelta);
	void						Tick_Roll_Into_Fall(_float fTimeDelta);
	/* HEAVY ATTACK 1 */
	void						Tick_Heavy_Attack_1_Charge(_float fTimeDelta);
	void						Tick_Heavy_Attack_1_Release(_float fTimeDelta);
	void						Tick_Heavy_Attack_1_Release_Perfect(_float fTimeDelta);
	void						Tick_Heavy_Attack_1_Return(_float fTimeDelta);
	void						Tick_Heavy_Attack_1_Into_Run(_float fTimeDelta);
	/* HEAVY ATTACK 2 */
	void						Tick_Heavy_Attack_2_Charge(_float fTimeDelta);
	void						Tick_Heavy_Attack_2_Release(_float fTimeDelta);
	void						Tick_Heavy_Attack_2_Release_Perfect(_float fTimeDelta);
	void						Tick_Heavy_Attack_2_Return(_float fTimeDelta);
	void						Tick_Heavy_Attack_2_Into_Run(_float fTimeDelta);
	/* HEAVY ATTACK 3 */
	void						Tick_Heavy_Attack_3_Charge(_float fTimeDelta);
	void						Tick_Heavy_Attack_3_Release(_float fTimeDelta);
	void						Tick_Heavy_Attack_3_Release_Perfect(_float fTimeDelta);
	void						Tick_Heavy_Attack_3_Return(_float fTimeDelta);
	/* HEAVY ATTACK COMBO */
	void						Tick_Heavy_Attack_Combo(_float fTimeDelta);
	void						Tick_Heavy_Attack_Combo_Return(_float fTimeDelta);
	void						Tick_Heavy_Attack_Combo_Into_Run(_float fTimeDelta);
	/* JUMP */
	void						Tick_Jump_Squat(_float fTimeDelta);
	void						Tick_Jump(_float fTimeDelta);
	void						Tick_Running_Jump_Squat(_float fTimeDelta);
	void						Tick_Running_Jump(_float fTimeDelta);
	void						Tick_Pulse_Jump(_float fTimeDelta);
	/* LAND */
	void						Tick_Land(_float fTimeDelta);
	void						Tick_Land_Heavy(_float fTimeDelta);
	void						Tick_Land_Walking(_float fTimeDelta);
	void						Tick_Bow_Land(_float fTimeDelta);
	void						Tick_Ledge_Land(_float fTimeDelta);
	void						Tick_Slide_Land(_float fTimeDelta);
	/* PULSE */
	void						Tick_Into_Pulse(_float fTimeDelta);
	void						Tick_Into_Pulse_From_Run(_float fTimeDelta);
	void						Tick_Pulse(_float fTimeDelta);
	void						Tick_Pulse_Loop(_float fTimeDelta);
	void						Tick_Pulse_Into_Combat_End(_float fTimeDelta);
	void						Tick_Pulse_Into_Idle(_float fTimeDelta);
	void						Tick_Pulse_Into_Run(_float fTimeDelta);
	void						Tick_Pulse_Parry(_float fTimeDelta);
	void						Tick_Pulse_Walk(_float fTimeDelta);
	void						Tick_Pulse_Squat_Sprint(_float fTimeDelta);
	/* SRPINT */
	void						Tick_Into_Sprint(_float fTimeDelta);
	void						Tick_Sprint(_float fTimeDelta);
	void						Tick_Sprint_Stop(_float fTimeDelta);
	void						Tick_Sprint_Lean_Left(_float fTimeDelta);
	void						Tick_Sprint_Lean_Right(_float fTimeDelta);
	void						Tick_Sprint_Turn_180(_float fTimeDelta);
	void						Tick_Sprint_Attack(_float fTimeDelta);

private:
	/* IDLE */
	void						End_Idle(_float fTimeDelta);
	/* RUN */
	void						End_Run(_float fTimeDelta);
	void						End_Run_Stop(_float fTimeDelta);
	/* AIM */
	void						End_Aim_Into(_float fTimeDelta);
	void						End_Aim_Loop(_float fTimeDelta);
	void						End_Aim_Return(_float fTimeDelta);
	void						End_Aim_Run_Forward(_float fTimeDelta);
	void						End_Aim_Run_Forward_Left(_float fTimeDelta);
	void						End_Aim_Run_Forward_Right(_float fTimeDelta);
	void						End_Aim_Run_Backward(_float fTimeDelta);
	void						End_Aim_Run_Backward_Left(_float fTimeDelta);
	void						End_Aim_Run_Backward_Right(_float fTimeDelta);
	void						End_Aim_Run_Left(_float fTimeDelta);
	void						End_Aim_Run_Right(_float fTimeDelta);
	/* ATTACK 1 */
	void						End_Attack_1(_float fTimeDelta);
	void						End_Attack_1_From_Run(_float fTimeDelta);
	void						End_Attack_1_Into_Run(_float fTimeDelta);
	void						End_Attack_1_Into_Walk(_float fTimeDelta);
	void						End_Attack_1_Return(_float fTimeDelta);
	/* ATTACK 2 */
	void						End_Attack_2(_float fTimeDelta);
	void						End_Attack_2_Into_Run(_float fTimeDelta);
	void						End_Attack_2_Into_Walk(_float fTimeDelta);
	void						End_Attack_2_Return(_float fTimeDelta);
	/* ATTACK 3 */
	void						End_Attack_3(_float fTimeDelta);
	void						End_Attack_3_Into_Run(_float fTimeDelta);
	void						End_Attack_3_Return(_float fTimeDelta);
	/* ATTACK 4 */
	void						End_Attack_4(_float fTimeDelta);
	void						End_Attack_4_Into_Run(_float fTimeDelta);
	void						End_Attack_4_Return(_float fTimeDelta);
	/* BOW */
	void						End_Bow_Charge(_float fTimeDelta);
	void						End_Bow_Charge_Full(_float fTimeDelta);
	void						End_Bow_Charge_Loop(_float fTimeDelta);
	void						End_Bow_Release(_float fTimeDelta);
	void						End_Bow_Recharge(_float fTimeDelta);
	void						End_Bow_Return(_float fTimeDelta);
	/* COMBAT */
	void						End_Idle_Into_Lock_On(_float fTimeDelta);
	void						End_Lock_On_Idle(_float fTimeDelta);
	void						End_Lock_On_To_Idle(_float fTimeDelta);
	void						End_Combat_Idle_Into_Run(_float fTimeDelta);
	void						End_Combat_Run(_float fTimeDelta);
	/* DODGE */
	void						End_Backflip(_float fTimeDelta);
	void						End_Roll(_float fTimeDelta);
	void						End_Roll_Left(_float fTimeDelta);
	void						End_Roll_Right(_float fTimeDelta);
	void						End_Roll_Into_Idle(_float fTimeDelta);
	void						End_Roll_Into_Run(_float fTimeDelta);
	void						End_Roll_Into_Walk(_float fTimeDelta);
	void						End_Roll_Into_Fall(_float fTimeDelta);
	/* HEAVY ATTACK 1 */
	void						End_Heavy_Attack_1_Charge(_float fTimeDelta);
	void						End_Heavy_Attack_1_Release(_float fTimeDelta);
	void						End_Heavy_Attack_1_Release_Perfect(_float fTimeDelta);
	void						End_Heavy_Attack_1_Return(_float fTimeDelta);
	void						End_Heavy_Attack_1_Into_Run(_float fTimeDelta);
	/* HEAVY ATTACK 2 */
	void						End_Heavy_Attack_2_Charge(_float fTimeDelta);
	void						End_Heavy_Attack_2_Release(_float fTimeDelta);
	void						End_Heavy_Attack_2_Release_Perfect(_float fTimeDelta);
	void						End_Heavy_Attack_2_Return(_float fTimeDelta);
	void						End_Heavy_Attack_2_Into_Run(_float fTimeDelta);
	/* HEAVY ATTACK 3 */
	void						End_Heavy_Attack_3_Charge(_float fTimeDelta);
	void						End_Heavy_Attack_3_Release(_float fTimeDelta);
	void						End_Heavy_Attack_3_Release_Perfect(_float fTimeDelta);
	void						End_Heavy_Attack_3_Return(_float fTimeDelta);
	/* HEAVY ATTACK COMBO */
	void						End_Heavy_Attack_Combo(_float fTimeDelta);
	void						End_Heavy_Attack_Combo_Return(_float fTimeDelta);
	void						End_Heavy_Attack_Combo_Into_Run(_float fTimeDelta);
	/* JUMP */
	void						End_Jump_Squat(_float fTimeDelta);
	void						End_Jump(_float fTimeDelta);
	void						End_Running_Jump_Squat(_float fTimeDelta);
	void						End_Running_Jump(_float fTimeDelta);
	void						End_Pulse_Jump(_float fTimeDelta);
	/* LAND */
	void						End_Land(_float fTimeDelta);
	void						End_Land_Heavy(_float fTimeDelta);
	void						End_Land_Walking(_float fTimeDelta);
	void						End_Bow_Land(_float fTimeDelta);
	void						End_Ledge_Land(_float fTimeDelta);
	void						End_Slide_Land(_float fTimeDelta);
	/* PULSE */
	void						End_Into_Pulse(_float fTimeDelta);
	void						End_Into_Pulse_From_Run(_float fTimeDelta);
	void						End_Pulse(_float fTimeDelta);
	void						End_Pulse_Loop(_float fTimeDelta);
	void						End_Pulse_Into_Combat_End(_float fTimeDelta);
	void						End_Pulse_Into_Idle(_float fTimeDelta);
	void						End_Pulse_Into_Run(_float fTimeDelta);
	void						End_Pulse_Parry(_float fTimeDelta);
	void						End_Pulse_Walk(_float fTimeDelta);
	void						End_Pulse_Squat_Sprint(_float fTimeDelta);
	/* SRPINT */
	void						End_Into_Sprint(_float fTimeDelta);
	void						End_Sprint(_float fTimeDelta);
	void						End_Sprint_Stop(_float fTimeDelta);
	void						End_Sprint_Lean_Left(_float fTimeDelta);
	void						End_Sprint_Lean_Right(_float fTimeDelta);
	void						End_Sprint_Turn_180(_float fTimeDelta);
	void						End_Sprint_Attack(_float fTimeDelta);

private:	/* Changer */
	_bool	Animation_Finish();
	_bool	Animation_Progress(_float fProgress);
	_bool	Direction_Change();
	_bool	CombatTimeToIdle();
	_bool	HeavyAttack1_Perfect();
	_bool	HeavyAttack2_Perfect();
	_bool	HeavyAttack3_Perfect();

	_bool	KeyInput_None();
	_bool	KeyInput_Direction();
	_bool	KeyInput_W();
	_bool	KeyInput_A();
	_bool	KeyInput_S();
	_bool	KeyInput_D();
	_bool	KeyInput_WA();
	_bool	KeyInput_WD();
	_bool	KeyInput_SA();
	_bool	KeyInput_SD();
	_bool	KeyInput_E();
	_bool	KeyInput_Space();
	_bool	KeyInput_LShift();
	_bool	KeyInput_LCtrl();
	_bool	MouseInput_Left();
	_bool	MouseInput_Right();

	_bool	NoMouseInput_Right();

	_bool	KeyDown_E();
	_bool	KeyDown_Q();
	_bool	KeyDown_R();
	_bool	KeyDown_Space();
	_bool	KeyDown_LCtrl();
	_bool	MouseDown_Left();
	_bool	MouseDown_Middle();
	_bool	MouseDown_Right();

	_bool	KeyUp_E();
	_bool	KeyUp_LShift();
	_bool	MouseUp_Left();
	_bool	MouseUp_Right();

private:
	void	Move(_float TimeDelta, CTransform::DIRECTION eDir, MOVEOPTION eMoveOption = MOVEOPTION_COMMON);
	CTransform::DIRECTION		DetectDirectionInput();

public:
	Delegator<CUI_ClientManager::UI_PRESENT, CUI_ClientManager::UI_FUNCTION, _float>		m_PlayerDelegator;


public:
	static	CKena_State*	Create(class CKena* pKena, CStateMachine* pStateMachine, CModel* pModel, CAnimationState* pAnimation, CTransform* pTransform, class CCamera_Player* pCamera);
	virtual void				Free() override;
};

END