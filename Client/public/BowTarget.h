#pragma once
#include "Client_Defines.h"
#include "EnviromentObj.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CTexture;
END

BEGIN(Client)

class CBowTarget  final : public CEnviromentObj
{
public:
	enum ANIMATION { ANIMATION_END };

private:
	CBowTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBowTarget(const CBowTarget& rhs);
	virtual ~CBowTarget() = default;

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
	virtual _int					Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex) override;
	virtual _int					Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;
	virtual _int					Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;

private:
	CModel*							m_pModelCom = nullptr;
	class CInteraction_Com*			m_pInteractionCom = nullptr;
	class CControlMove*				m_pControlMoveCom = nullptr;
	_bool							m_bRenderCheck = false;

private:
	_bool							m_bKenaDetected = false;
	_bool							m_bArrowHit = false;

	ANIMATION						m_eCurState = ANIMATION_END;
	ANIMATION						m_ePreState = ANIMATION_END;

private:
	ANIMATION						Check_State();
	void							Update_State(_float fTimeDelta);

private:
	HRESULT							SetUp_Components();
	HRESULT							SetUp_ShaderResources();

public:
	static  CBowTarget*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr) override;
	virtual void					Free() override;
};

END