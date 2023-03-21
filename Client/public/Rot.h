#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "PhysX_Defines.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CFSMComponent;
END

BEGIN(Client)
class CRot final : public CGameObject
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

	CRenderer*					m_pRendererCom = nullptr;
	CShader*						m_pShaderCom = nullptr;
	CModel*						m_pModelCom = nullptr;
	CFSMComponent*		m_pFSM = nullptr;
	class CKena *				m_pKena = nullptr;
	class CKena_Status*	m_pkenaState = nullptr;
	class CE_TeleportRot* m_pTeleportRot = nullptr;

	class CRotWisp*			m_pRotWisp = nullptr;
	CTransform*				m_pKenaTransform = nullptr;
	class CRope_RotRock* m_pRopeRotRock = nullptr;
	_float4 m_vWakeUpPosition;

private:
	_bool m_bWakeUp = false;	
	_uint m_iCuteAnimIndex = 0;
	_float m_fKenaToRotDistance = 2.f;
	_float m_fTeleportDistance = 6.f;

	

	DESC m_Desc;

	class CCameraForRot*		m_pMyCam = nullptr;
	PX_TRIGGER_DATA* m_pTriggerData = nullptr;

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
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;
	virtual void Imgui_RenderProperty() override;
	virtual void ImGui_AnimationProperty() override;
	virtual void ImGui_ShaderValueProperty() override;
	virtual void ImGui_PhysXValueProperty() override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();
	HRESULT SetUp_State();
	HRESULT SetUp_Effects();

public:
	static CRot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	const _double& Get_AnimationPlayTime();
	
	virtual _int Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex) override;
	virtual _int Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;

	void Set_KenaPos(_float4 vKenaPos) { memcpy(&m_vKenaPos, &vKenaPos, sizeof(_float4)); }

public:
	enum ANIMATION {
		ROT1_COLLECT, ROT1_COLLECTLOOP, ROT1_COLLECTRETURN,

		ROT2_COLLECT, ROT2_COLLECTLOOP, ROT2_COLLECTRETURN,

		ROT3_COLLECT, ROT3_COLLECTLOOP, ROT3_COLLECTRETURN,

		FLOWERROT_COLLECT_ROT, FLOWERROT_COLLECT_ROT_END,

		PUMPKINROT_ROT1_COLLECT, PUMPKINROT_ROT1_LOOP, PUMPKINROT_ROT1_START,
		PUMPKINROT_ROT2_COLLECT, PUMPKINROT_ROT2_LOOP, PUMPKINROT_ROT2_START, 
		PUMPKINROT_ROT3_COLLECT,

		APPROACH, APPROACH2, APPROACH3,

		BLINK_ADD,

		BOMB_FIDGET, BOMB_FIDGET2, BOMB_INTO, BOMB_INTO2, BOMB_LOOP, BOMB_LOOP2,

		BURST,

		CARRYBACK, CARRYFORWARD, CARRYLEFT, CARRYLOOP, CARRYREACHING_ADD, CARRYRIGHT,

		CHEER1, CHERR2, CHEER3, CHEER4, CHEER5, CHEER6, CHERR7,

		COLLECT, COLLECT2, COLLECT3, COLLECT4, COLLECT5, COLLECT6, COLLECT7, COLLECT8,

		CURIOUS1, CURIOUS3, CURIOUS4, CURIOUS5,

		EATINGSTINKYFRUIT, EATINGSTINKYFRUIT2, EATINGSTINKYFRUIT3,

		EXCITED1, EXCITED2, EXCITED3,

		EXCITED_TOIDLE, EXCITEDLOOP, EXIT,

		FALL,

		FIDGET1, FIDGET2, FIDGET3, FIDGET4,

		GETUP, GOTOSLEEP, HANGINGOUT,

		HIDE5, HIDE6, HIDE7,

		IDLE, IDLEPOSE, IDLEPOSE_ADD,

		JIGGLEDOWN, JIGGLELEFT, JIGGLERIGHT, JIGGLEUP,

		JUMP, JUMPANDTURN_L120, JUMPANDTURN_L150,
		JUMPANDTURN_L60, JUMPANDTURN_R120, JUMPANDTURN_R150,

		KICKINGLEGS,

		LAND, LAND3,

		LEVER_DOWN,

		LIFT, LIFT2, LIFT3, LIFT4,

		LIFTCATSTATUE01_V2, LIFTLOOP, LIFT_ONEARM, LIFT_ONEARM_SHORT,
		LIFT_OTHERROT2, LIFTOTHERROT2_LOOP,

		LIFTROCK, LIFTROCK2, LIFTROCK_END, LIFTROCK_LOOP, LIFTROCK_LOOP2,

		LIFTSTATUE, LIFTSTATUE3, LIFTSTATUE4,

		LOOKAT_DOWN,			 //ADD
		LOOKAT_DOWNLEFT,	 //ADD
		LOOKAT_DOWNRIGHT,	//ADD
		LOOKAT_LEFT,				//ADD
		LOOKAT_RIGHT,			//ADD
		LOOKAT_UP,					//ADD
		LOOKAT_UPLEFT,			//ADD
		LOOKAT_UPRIGHT,		//ADD

		LOSEBALANCE, LOSEBALANCE2, LOSEBALANCE2_ROOTMOTION,

		MEDITATION_LOOKFIDGET, MEDITATION_LOOP,

		NEUTRAL, OPENDRAWER, PEACESIGN,

		PHOTOPOSE1, PHOTOPOSE2, PHOTOPOSE3, PHOTOPOSE4, PHOTOPOSE5, PHOTOPOSE6, PHOTOPOSE7, PHOTOPOSE8,
		PHOTOPOSE_KICKINGLEGS1, PHOTOPOSE_SITTING1,

		PICKUP, POKE, PREVIEWHAT1, PREVIEWHAT2, PURCHASEHAT1, PURCHASEHAT3, PURCHASEHAT4,

		RELAXING, RELAXING_LOOK, ROLL,

		ROPEBRIDGE_LIFT, ROPEBRIDGE_LIFT2, ROPEBRIDGE_LIFTLOOP, ROPEBRIDGE_LIFTLOOP2,

		RUBBLE, SCAREDRUN, SCRATCHING, SHAKE, SHAKEBUSH, SHY, SIT, SIT2, SIT3, SIT4,
		SITTING, SLEEPING, SNOWANGEL, SNOWANGEL_FIDGET,

		SQUISH_D, SQUISH_DL, SQUISH_DR,
		SQUISH_L, SQUISH_L2, SQUISH_R, SQUISH_R2,
		SQUISH_U, SQUISH_UL, SQUISH_UL2,
		SQUISH_UR, SQUISH_UR2,

		STRAIGHTEN03, STRAIGHTEN04_V2,

		SWIM_A, SWIM_B, SWIM_C, SWIMFIDGET_A, SWIMFIDGET_B,
		SWIMIDLE_A, SWIMIDLE_B, SWIMIDLE_C,

		TELEPORT4, TELEPORT5, TELEPORT6, TELEPORT7, TELEPORT8,
		TELEPORT_ONTOLEDGE1, TELEPORT_ONTOLEDGE2,

		TOSS, TOUCH_ADD, TURN_L30, WALK, WARRIORBELLRING_SIDE, WARRIORBELLRING_TOP,
		WORRIED, COLLECT_FROG, COLLECT_HAT, HIDE, HIDE2, RUN,

		TELEPORT1, TELEPORT2, TELEPORT3,

		ROT_ANIMATION_END
	};
};

END