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

class CBombPlatform : public CEnviromentObj
{
public:
	enum STATE { STATE_SLEEP, STATE_OPEN, STATE_ACTIVATE, STATE_CLOSE, STATE_END };

public:
	CBombPlatform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBombPlatform(const CBombPlatform& rhs);
	virtual ~CBombPlatform() = default;

public:
	void						Initialize(_float4 vMovingPos, _float fReturnTime) { m_vMovingPos = vMovingPos; m_fReturnTime = fReturnTime; }

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual HRESULT				Late_Initialize(void* pArg) override;
	virtual void						Tick(_float fTimeDelta) override;
	virtual void						Late_Tick(_float fTimeDelta) override;
	virtual HRESULT				Render() override;
	virtual HRESULT				RenderShadow() override;

public:
	virtual void				Imgui_RenderProperty() override;
	virtual void				ImGui_PhysXValueProperty() override;
	virtual _int				Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex) override;
	virtual _int				Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;
	virtual _int				Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;

private:
	CModel*						m_pModelCom = nullptr;
	class CInteraction_Com*		m_pInteractionCom = nullptr;
	class CControlMove*			m_pControlMoveCom = nullptr;

private:
	class CRotBomb*				m_pDetectedBomb = nullptr;
	class CE_P_Bombplatform* m_pBombplatformEffect = nullptr;

private:
	_float4						m_vInitPos;
	_float4						m_vInitQuat;
	_float4						m_vMovingPos;
	_float4						m_vMovingQuat;
	_float						m_fReturnTime = 0.f;
	_float						m_fTimer = 0.f;

	STATE						m_eCurState = STATE_SLEEP;
	STATE						m_ePreState = STATE_SLEEP;

private:
	STATE						Check_State();
	void						Update_State(_float fTimeDelta);

private:
	HRESULT						SetUp_Components();
	HRESULT						SetUp_ShaderResources();
	HRESULT						SetUp_ShadowShaderResources() override;

	HRESULT						Ready_Effect();

public:
	static CBombPlatform*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END