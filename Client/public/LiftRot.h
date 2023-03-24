#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "Rot.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CFSMComponent;
END

BEGIN(Client)
class CLiftRot final : public CGameObject
{
public:
	enum TYPE { CUTE, LIFT, TYPE_END };

	typedef struct tagLiftRotDesc
	{
		TYPE eType;
		_float4 vInitPos;
	} DESC;

private:	
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr; 
	CModel* m_pModelCom = nullptr;
	CFSMComponent* m_pWorkFSM = nullptr;
	CFSMComponent* m_pLiftFSM = nullptr;
	CFSMComponent* m_pCuteFSM = nullptr;

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
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;
	virtual void Imgui_RenderProperty() override;
	virtual void ImGui_AnimationProperty() override;
	virtual void ImGui_ShaderValueProperty() override;
	virtual void ImGui_PhysXValueProperty() override;
	virtual HRESULT Call_EventFunction(const string& strFuncName) override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();
	HRESULT SetUp_LiftFSM();
	HRESULT SetUp_CuteFSM();

public:
	static CLiftRot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	const _double& Get_AnimationPlayTime();
	
	virtual _int Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex) override;
	
public:
	void Execute_WakeUp(_float4& vCreatePos, _float4& vLiftPos);
	void Execute_LiftStart();
	void Execute_LiftMoveStart();
	void Execute_LiftMoveEnd();
	void Execute_LiftDownEnd();
	
	void Set_Type(TYPE eType) { m_pWorkFSM = eType == CUTE ? m_pCuteFSM : m_pLiftFSM; }

	void Execute_StartCute(_float4& vCreatePos);
	void Execute_EndCute();

public:
	_bool Get_LiftReady() { return m_bLiftReady;}	
	_bool Get_LiftEnd() { return m_bLiftEnd; }
	
	void Set_NewPosition(_float4 vNewPos);
};

END