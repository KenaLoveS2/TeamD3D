#pragma once

#include "Npc.h"
#include "Delegator.h"
#include "UI_ClientManager.h"

BEGIN(Client)
class CUI_FocusNPC;
class CSaiya final : public CNpc
{
	struct SAIYAKEYFRAME
	{
		_float3 vPos;
		_float3 vLook;
	};

	enum NPC_CAM
	{
		NPC_CINE0 = 0,
		NPC_CINE1,

		NPC_CINE_END,
	};

	enum ANIMATION
	{
		SAIYA_APPEAR,
		SAIYA_CAUTIOUS_CROSSARMS,
		SAIYA_CAUTIOUS_SHRUGPOINT,
		SAIYA_CHASINGLOOP,
		SAIYA_CHEER,
		SAIYA_CURIOUS,
		SAIYA_DIALOGUEGORGE,
		SAIYA_DISAPPEAR,
		SAIYA_IDLE,
		SAIYA_IDLE_LOOKDOWN,
		SAIYA_IDLE_LOOKDOWNLEFT,
		SAIYA_IDLE_LOOKDOWNRIGHT,
		SAIYA_IDLE_LOOKLEFT,
		SAIYA_IDLE_LOOKRIGHT,
		SAIYA_IDLE_LOOKUP,
		SAIYA_IDLE_LOOKUPLEFT,
		SAIYA_IDLE_LOOKUPRIGHT,
		SAIYA_IDLEPOSE,
		SAIYA_IDLEPOSE_ADDITIVE,
		SAIYA_MAYBEZAJU_END,
		SAIYA_MAYBEZAJU_LOOP,
		SAIYA_MAYBEZAJU_START,
		SAIYA_MOUTHFLAP,
		SAIYA_MOUTHFLAP2,
		SAIYA_MOUTHFLAP_GAP,
		SAIYA_MOUTHFLAP_REFPOSE,
		SAIYA_MOUTHFLAPSHORT,
		SAIYA_PHOTOPOSE_CHEER,
		SAIYA_PHOTOPOSE_COOL,
		SAIYA_PHOTOPOSE_PEACE,
		SAIYA_PHOTOPOSE_SIT02,
		SAIYA_PHOTOPOSE_SIT03,
		SAIYA_PHOTOPOSE_SITTING,
		SAIYA_PULSE,
		SAIYA_PULSE_SITTING,
		SAIYA_QUICKLOOKTOBENI,
		SAIYA_ROPEBRIDGE_LOOP,
		SAIYA_ROPEBRIDGE_RUN,
		SAIYA_ROTPARADENPC_LINE1,
		SAIYA_ROTPARADENPC_LINE2,
		SAIYA_ROTPARADENPC_LINE2_LOOP,
		SAIYA_ROTPARADENPC_LINE3,
		SAIYA_ROTPARADENPC_LINE3_LOOP,
		SAIYA_ROTPARADENPC_LINE4,
		SAIYA_RUN,
		SAIYA_RUN_135L,
		SAIYA_RUN_90R,
		SAIYA_RUNFIDGET,
		SAIYA_RUNFIDGET2,
		SAIYA_RUNFIDGET_135L,
		SAIYA_SIT_DIALOGUE,
		SAIYA_SIT_DIALOGUEREACT,
		SAIYA_SIT_IDLE,
		SAIYA_SIT_SITDOWN,
		SAIYA_SITDOWN,
		SAIYA_SITLOOP,
		SAIYA_TELEPORT,
		SAIYA_TURN135L_INTORUN,
		SAIYA_TURN90R_INTORUN,
		SAIYA_W1_LOGS,
		SAIYA_WHATHAPPENED_END,
		SAIYA_WHATHAPPENED_LOOP,
		SAIYA_WHATHAPPENED_START,
		W0_POKINGEXIT_SAIYA,
		W0_POKINGLOOP_SAIYA,
		W1_NPC_FORESTPATH_COMBAT_APPEAR_SAIYA,
		W1_NPC_FORESTPATH_COMBAT_DISAPPEAR_SAIYA,
		W1_NPC_FORESTPATH_COMBAT_LOOK_SAIYA,
		W1_NPC_FORESTPATH_COMBAT_LOOP_SAIYA,
		ANIMATION_END
	};

private:
	CSaiya(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSaiya(const CNpc& rhs);
	virtual ~CSaiya() = default;

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

public:
	Delegator<CUI_ClientManager::UI_PRESENT, _bool, _float, wstring>		m_SaiyaDelegator;


protected:
	virtual void					Update_Collider(_float fTimeDelta) override;
	virtual	HRESULT			SetUp_State()override;
	virtual	HRESULT			SetUp_Components()override;
	virtual	HRESULT			SetUp_ShaderResources()override;
	virtual HRESULT			SetUp_ShadowShaderResources()override;
	virtual HRESULT			SetUp_UI()override;

private:
	virtual void					AdditiveAnim(_float fTimeDelta) override;

	_bool									IsChatEnd();
	HRESULT								Save_KeyFrame();
	HRESULT								Load_KeyFrame();
	void										Rot_WispSetPosition();

private:
	CUI_FocusNPC*							m_pFocus;
	_bool											m_bMeetPlayer = false;
	vector<wstring>							m_vecChat[10];
	_uint												m_iChatIndex;
	_uint												m_iLineIndex;
	vector<SAIYAKEYFRAME>			m_keyframes;
	_uint												m_iNumKeyFrame = 0;
	_uint												m_iKeyFrame = 0;

	_float3											m_vCamOffset;
	

	_bool											m_bCinecam[NPC_CINE_END] = { false, };
	class CCinematicCamera*			m_pCinecam[NPC_CINE_END] = { nullptr, };
	class CCameraForNpc*				m_pMainCam = nullptr;
	class CRot*									m_pRot = nullptr;

public:
	static CSaiya*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr)  override;
	virtual void						Free() override;
};
END