#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CFSMComponent;
class CBone;
class CGameInstance;
END

BEGIN(Client)
class CRot_Base : public CGameObject
{
protected:
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CModel* m_pModelCom = nullptr;
	CFSMComponent* m_pFSM = nullptr;
	
	class CKena* m_pKena = nullptr;
	class CKena_Status*	m_pkenaState = nullptr;
	CTransform* m_pKenaTransform = nullptr;		
	class CE_TeleportRot* m_pTeleportRot = nullptr;	
	
	_uint m_iNumMeshes = 0;

	CGameInstance* m_pGameInstance = nullptr;

	enum COPY_SOUND_KEY {
		CSK_1, CSK_2, CSK_3, CSK_4, CSK_5, CSK_6, CSK_7, CSK_8, CSK_9, CSK_10, CSK_11, CSK_12, CSK_MOVE, CSK_CROSS,
		COPY_SOUND_KEY_END,
	};

	_tchar* m_pCopySoundKey[COPY_SOUND_KEY_END] = { nullptr, };

protected:
	CRot_Base(ID3D11Device* pDevice, ID3D11DeviceContext* p_context);
	CRot_Base(const CRot_Base& rhs);
	virtual ~CRot_Base() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void * pArg);
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;
	
protected:
	virtual HRESULT SetUp_Components();
	virtual HRESULT SetUp_ShaderResources();
	virtual HRESULT SetUp_ShadowShaderResources();	
	virtual HRESULT SetUp_Effects();
	virtual HRESULT SetUp_State() { return S_OK; };

public:
	static CRot_Base* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	const _double& Get_AnimationPlayTime();
			
	virtual void Push_EventFunctions() override; 
	HRESULT Create_CopySound();
	void Play_Rot1Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Rot2Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Rot3Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Rot4Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Rot5Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Rot6Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Rot7Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Rot8Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Rot9Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Rot10Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Rot11Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Rot12Sound(_bool bIsInit, _float fTimeDelta);
	void Play_MoveSound(_bool bIsInit, _float fTimeDelta);

	void TurnOn_TeleportEffect(_float4 vTeleportPos, _uint iAnimindex);
	HRESULT Setup_KenaParamters();

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