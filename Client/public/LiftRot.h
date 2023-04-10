#pragma once
#include "Rot_Base.h"

BEGIN(Client)
class CLiftRot : public CRot_Base
{
public:
	enum TYPE { CUTE, LIFT, TYPE_END };

	typedef struct tagLiftRotDesc
	{
		TYPE eType;
		_float4 vInitPos;
	} DESC;

private:	
	CFSMComponent* m_pWorkFSM = nullptr;
	CFSMComponent* m_pLiftFSM = nullptr;
	CFSMComponent* m_pCuteFSM = nullptr;
	class CLiftRot_Master* m_pLiftRotMaster = nullptr;
	
private:
	_bool m_bWakeUp = false;
	_bool m_bCreateStart = false;		
	_bool m_bLiftReady = false;
	_bool m_bLiftStart = false;
	_bool m_bLiftEnd = false;
	_bool m_bLiftMoveStart = false;
	_bool m_bLiftMoveEnd = false;
	_bool m_bLiftDownEnd = false;

	DESC m_Desc;
	_float4 m_vLiftPos;
	
	_float m_fTimeCheck = 0.f;

private:
	_bool m_bCuteStart = false;
	_bool m_bCuteEnd = false;

private:
	CLiftRot(ID3D11Device* pDevice, ID3D11DeviceContext* p_context);
	CLiftRot(const CLiftRot& rhs);
	virtual ~CLiftRot() = default;

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
	virtual HRESULT Call_EventFunction(const string& strFuncName) override;

private:		
	virtual HRESULT SetUp_State();	
	HRESULT SetUp_LiftFSM();
	HRESULT SetUp_CuteFSM();

public:
	static CLiftRot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

public:
	void Execute_WakeUp(_float4& vCreatePos, _float4& vLiftPos);
	void Execute_LiftStart(_float4 vLookPos);
	void Execute_LiftMoveStart();
	void Execute_LiftMoveEnd();
		
	void Set_Type(CLiftRot::TYPE eType) { m_pWorkFSM = eType == CUTE ? m_pCuteFSM : m_pLiftFSM; }

	void Execute_StartCute(_float4& vCreatePos);
	void Execute_EndCute();

public:
	_bool Get_LiftReady() { return m_bLiftReady;}	
	_bool Get_LiftEnd() { return m_bLiftEnd; }
	_bool Get_LiftDownEnd() { return m_bLiftDownEnd; }	
	
	void Set_NewPosition(_float4 vNewPos, _float4 vLookPos);
	void Set_OwnerLiftRotMasterPtr(class CLiftRot_Master* pLiftRotMaster) { m_pLiftRotMaster = pLiftRotMaster; }
};

END