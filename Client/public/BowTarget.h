#pragma once
#include "Client_Defines.h"
#include "EnviromentObj.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CAnimationState;
class CTexture;
END

BEGIN(Client)

class CBowTarget  final : public CEnviromentObj
{
public:
	enum ANIMATION { FAIL_LOOP, HIT, LAUNCH, LOOP, REST, ANIMATION_END };

private:
	CBowTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBowTarget(const CBowTarget& rhs);
	virtual ~CBowTarget() = default;

public:
	void							TurnOn_Launch() { m_bLaunch = true; }
	const _bool&					Is_Dead() const { return m_bDead; }

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;
	virtual HRESULT					Late_Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

public:
	virtual void					Imgui_RenderProperty() override;
	virtual void					ImGui_AnimationProperty() override;
	virtual void					ImGui_PhysXValueProperty() override;
	virtual HRESULT					Add_AdditionalComponent(_uint iLevelIndex, const _tchar* pComTag, COMPONENTS_OPTION eComponentOption) override;
	virtual void					Calc_RootBoneDisplacement(_fvector vDisplacement) override;
	virtual _int					Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex) override;
	virtual _int					Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;
	virtual _int					Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;

private:
	CModel*							m_pModelCom = nullptr;
	CAnimationState*				m_pAnimation = nullptr;
	class CInteraction_Com*			m_pInteractionCom = nullptr;
	class CControlMove*				m_pControlMoveCom = nullptr;
	_bool							m_bRenderCheck = false;

private:
	_bool							m_bKenaDetected = false;
	_bool							m_bLaunch = false;
	_bool							m_bArrowHit = false;
	_bool							m_bDead = false;

	_float4							m_vInitPosition;
	_float							m_fFlyTime = 8.f;

	ANIMATION						m_eCurState = REST;
	ANIMATION						m_ePreState = REST;

	_float							m_fDissolveTime = 0.f;
	CTexture*						m_pDissolveTexture = nullptr;

private:
	ANIMATION						Check_State();
	void							Update_State(_float fTimeDelta);
	void							Reset();

private:
	HRESULT							SetUp_Components();
	HRESULT							SetUp_State();
	HRESULT							SetUp_ShaderResources();
	HRESULT							Bind_Dissolve(class CShader* pShader);

public:
	static  CBowTarget*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr) override;
	virtual void					Free() override;
};

END